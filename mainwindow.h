#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <omp.h>
#include <QDateTime>
#include <QColor>
#include <QMessageBox>
#include <QImage>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <vector>
#include <cmath>
#include <limits>
//#include "tgaimage.h"
#include "model.h"
#include "geom.h"
#include "render.h"
#include "game.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	void keyPressEvent(QKeyEvent *event);
	~MainWindow();


private slots:
	void RenderScene();

private:
	Ui::MainWindow *ui;
	QImage im;
	QPixmap bg;
	QPainter *paint;
	QGraphicsScene *scene;
    void gamecycle();
};

#endif // MAINWINDOW_H
