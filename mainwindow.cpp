#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

// Игровые переменные
#define MAXCARS 2 //не меньше 1
#define MAXTILES 7
#define MAXTREES 2
int *zbuffer = NULL;
bool collision = false;
Model *model = NULL;
Model *roadmodel = NULL;
Model *treemodel = NULL;
Model *signmodel = NULL;
actor *ent[MAXCARS+MAXTILES+MAXTREES];
QDateTime dateTime1 = QDateTime::currentDateTime();
QDateTime absTime = QDateTime::currentDateTime();
float millisecondsDiff = 0;
v3df light_dir = v3df(1,-1,1).normalize();
v3df movem(0, 0, 0);
v3df camera(0,3,-3);
v3df center(0,0.3,0);
long int score = 0;
int fpsi = 0;
float speedcoef = 1;
int overallfps = 0;
int xangle = 270, yangle = 11;

QGraphicsPixmapItem *pixitem = NULL;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	scene = new QGraphicsScene(ui->graphicsView);
	ui->graphicsView->setScene(scene);
	ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	im = QImage(ui->graphicsView->width(), ui->graphicsView->height(), QImage::Format_RGB32);
	paint = new QPainter(&im);
	paint->setBrush(Qt::white);
	paint->drawRect(0, 0, im.width(), im.height());
	//model = new Model("obj/car2.obj");
	zbuffer = new int[9*im.width()*im.height()];
	model = new Model("obj/volkslp.obj");
	//roadmodel = new Model("obj/roadchunk.obj");
	treemodel = new Model("obj/tree.obj");
	signmodel = new Model("obj/roadpaint.obj");
	bg = QPixmap("mat/background.png");
	//model = new Model("obj/road_hipoly.obj");
	for (int pnum = 0; pnum < MAXCARS+MAXTILES+MAXTREES; pnum++)
	{
        ent[pnum] = new actor(model);
		ent[pnum]->pos = v3df(-2+(rand()%3)*2,0,-32-(9.6*pnum-(MAXCARS+2*MAXTILES+MAXTREES)));
	}
	for (int tnum = MAXCARS; tnum < MAXCARS+MAXTILES; tnum++)
    {
		//ent[tnum]->mdl = roadmodel;
		ent[tnum]->mdl = signmodel;
		ent[tnum]->pos = v3df(0,0,4+((tnum-MAXCARS)*-6));
		//ent[tnum+1]->pos = ent[tnum]->pos;
    }
	for (int trnum = MAXCARS+MAXTILES; trnum < MAXCARS+MAXTILES+MAXTREES; trnum++)
	{
		ent[trnum]->mdl = treemodel;
		ent[trnum]->pos = v3df(-6+(rand()%2-1)*12,0,-12);
	}
    //ent[2]->mdl = roadmodel;
	ent[0]->pos = v3df(0,0,5);
	//ent[1]->pos.z = -10;
        //ent[2]->pos = v3df(0,0,0);
	omp_set_dynamic(0);      // запретить библиотеке openmp менять число потоков во время исполнения
	omp_set_num_threads(4);
    //omp_set_num_threads(MAXCARS+MAXCARS%2);
	QTimer *timer = new QTimer(this);
	   connect(timer, SIGNAL(timeout()), this, SLOT(RenderScene()));
	   timer->start(30);
	//connect(keyPressEvent();, SIGNAL(timeout()), this, SLOT(RenderScene()));
	scene->addPixmap(QPixmap::fromImage(im));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == 'A')
	{
		if(ent[0]->pos.x > -2)
			ent[0]->pos.x -= 1;
		//movem.x += 1;
	}
	if (event->key() == 'D')
	{
		if(ent[0]->pos.x < 2)
			ent[0]->pos.x += 1;
		//movem.x += 1;
	}
}

void rendsc(QImage *im)
{
	// Инициализация Z-buffer

	//zbuffer = new int[9*im->width()*im->height()];
	for (int i=0; i<im->width()*im->height(); i++)
	{
		zbuffer[i] = std::numeric_limits<int>::min();
	}
	{
		xangle %= 360;
		yangle %= 360;
		//center = ent[0]->pos;
		camera.x = cos(xangle * 3.14 / 180)*10;
		camera.y = tan(yangle * 3.14 / 180)*10;
		camera.z = sin(xangle * 3.14 / 180)*10;
        mtrx ModelView = lookat(camera, center, v3df(0,1,0));
		mtrx Projection = mtrx::ident(4);
		//mtrx ViewPort   = viewport(im->width()*0.3, im->height()*0.3, im->width()*0.4, im->height()*0.4);
		//mtrx ViewPort   = viewport(im->width()*0.4, im->height()*0.45, im->width()*0.2, im->height()*0.2);
		//mtrx ViewPort   = viewport(im->width()*0.46, im->height()*0.48, im->width()*0.08, im->height()*0.08);
		mtrx ViewPort   = viewport(im->width()*0.4, im->height()*0.3, im->width()*0.2, im->height()*0.3);
        Projection[3][2] = -0.5f/(camera-center).norm();
		// Цикл по каждой грани
		#pragma omp parallel for
		for (int pnum = 0; pnum < MAXCARS+MAXTILES+MAXTREES; pnum++)
		{
			//if (ent[pnum]->mdl != roadmodel)
			#pragma omp parallel for
            for (int i=0; i<ent[pnum]->mdl->nfaces(); i++)
			{
                std::vector<int> face = ent[pnum]->mdl->face(i);
				v3di screen_coords[3];
				v3df world_coords[3];
				for (int j=0; j<3; j++)
				{
                    v3df v = ent[pnum]->mdl->vert(face[j]);
                    screen_coords[j] = v3df(ViewPort*Projection*ModelView*mtrx(v-ent[pnum]->pos));
                    world_coords[j]  = v-ent[pnum]->pos;
				}
				v3df n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
				n.normalize();
				v3df viewpointlight = v3df(-500,-999,999)+camera-center;
				viewpointlight.normalize();
				float intensity = (n*viewpointlight);
				if (intensity < 0)
					intensity = fabs(intensity*0.5);
				{
					v2di uv[3];
					for (int k=0; k<3; k++)
					{
						uv[k] = model->uv(i, k);
					}
                    //triangle_nondiffuse(screen_coords[0], screen_coords[1], screen_coords[2], *im, QColor::fromRgb(intensity*255, intensity*255, intensity*255), zbuffer);
					triangle_diffuse(screen_coords[0], screen_coords[1], screen_coords[2], uv[0], uv[1], uv[2], *im, *ent[pnum]->mdl, ent[pnum]->color, intensity, zbuffer);
				}
			}
		}
	//delete [] zbuffer;
	}
}

void MainWindow::gamecycle()
{
	for (int entnum = 1; entnum<MAXCARS; entnum++)
	{
		collision = ent[0]->collisioncheck(ent[entnum]);
		if (collision)
			break;
		if (ent[entnum]->pos.z < 12)
			ent[entnum]->pos.z+=0.5*speedcoef;
		else
		{
			ent[entnum]->pos.z=-24;
			//ent[entnum]->pos.x=(float)(rand()%60)/10-3;
			ent[entnum]->pos.x=-2+(rand()%3)*2;
			ent[entnum]->color = qRgb(0,rand()%2, 0);
		}
	}
	for (int tnum = MAXCARS; tnum < MAXCARS+MAXTILES; tnum++)
	{
		ent[tnum]->pos.z += 1*speedcoef;
		if (ent[tnum]->pos.z > 8)
			ent[tnum]->pos.z = -28-(8-ent[tnum]->pos.z);
	}
	for (int trnum = MAXCARS+MAXTILES; trnum < MAXCARS+MAXTILES+MAXTREES; trnum++)
	{
		ent[trnum]->pos.z += 1*speedcoef;
		if (ent[trnum]->pos.z > 8)
		{
			ent[trnum]->pos.z = -24;
			ent[trnum]->pos.x = -6+(rand()%2)*12;
		}
	}
}

void MainWindow::RenderScene()
{
	if (!collision)
	{
		//paint->setBrush(QColor::fromRgb(0, 50, 16));
		//paint->drawRect(0, 0, im.width(), im.height());
		paint->drawPixmap(0,0,bg);
		gamecycle();
		rendsc(&im);
		QPixmap lpixmap = QPixmap::fromImage(im.mirrored(false, true));
		if (pixitem)
			delete pixitem;
		pixitem = scene->addPixmap(lpixmap);
		QDateTime dateTime2 = QDateTime::currentDateTime();
		millisecondsDiff += dateTime1.msecsTo(dateTime2);
		if (++fpsi>25)
		{
			float mediumfps = 25000/millisecondsDiff;
			millisecondsDiff = 0;
			fpsi = 0;
			score+=1;
			this->setWindowTitle("Thrillide!!! Score: " + QString::number(score) + "0   ||   " + QString::number(mediumfps) + " FPS ");
			std::cout<<mediumfps<<"\n";
			if (score % 10 == 0 && score != 0)
			{
				speedcoef+=0.1;
			}
		}
		dateTime1 = QDateTime::currentDateTime();
		if (collision)
		{
			this->setWindowTitle("Thrillide!!! GAME OVER!!! Score: " + QString::number(score) + "0");
			//paint->drawPixmap(0,0,bg);
			QMessageBox::critical(0, "Game over!", "You crashed! What a pity! \nYour score is: "
								  + QString::number(score) + "0\nRestart the game to try again.");
			exit(1);
		}
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}
