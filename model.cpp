#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <QColor>
#include <QMessageBox>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_()
{
	std::ifstream in;
	in.open (filename, std::ifstream::in);
	if (in.fail())
	{
		QMessageBox::critical(0, "Error", "Couldn't load model!");
		exit(1);
	}
	std::string line;
	while (!in.eof())
	{
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		//std::cout << iss << '\n';
		char trash;
		if (!line.compare(0, 2, "v "))
		{
			iss >> trash;
			std::cout << trash << "E\n";
			v3df v;
			for (int i=0;i<3;i++)
			{
				iss >> v[i];
				std::cout << v[i] << "E\n";
			}
			verts_.push_back(v);
		}
		else if (!line.compare(0, 3, "vn "))
		{
			iss >> trash >> trash;
			v3df n;
			for (int i=0;i<3;i++) iss >> n[i];
			norms_.push_back(n);
		}
		else if (!line.compare(0, 3, "vt "))
		{
			iss >> trash >> trash;
			v2df uv;
			for (int i=0;i<2;i++) iss >> uv[i];
			uv_.push_back(uv);
		}
		else if (!line.compare(0, 2, "f "))
		{
			std::vector<v3di> f;
			v3di tmp;
			iss >> trash;
			while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2])
			{
				for (int i=0; i<3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
				f.push_back(tmp);
			}
			faces_.push_back(f);
		}
	}
	load_texture(filename, "_diffuse.tga", diffusemap_);
}

Model::~Model() {}

int Model::nverts()
{
	return (int)verts_.size();
}

int Model::nfaces()
{
	return (int)faces_.size();
}

std::vector<int> Model::face(int idx)
{
	//return faces_[idx];
	std::vector<int> face;
	for (int i=0; i<(int)faces_[idx].size(); i++)
		face.push_back(faces_[idx][i][0]);
	return face;
}

v3df Model::vert(int i)
{
	return verts_[i];
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage &img)
{
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot!=std::string::npos)
	{
		texfile = texfile.substr(0,dot) + std::string(suffix);
		std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
		img.flip_vertically();
	}
}

QRgb Model::diffuse(v2di uv)
{
	TGAColor getdiffuse = diffusemap_.get(uv.x, uv.y);
	//
	return qRgb(getdiffuse.r, getdiffuse.g, getdiffuse.b);
}

v2di Model::uv(int iface, int nvert) {
	int idx = faces_[iface][nvert][1];
	return v2di(uv_[idx].x*diffusemap_.get_width(), uv_[idx].y*diffusemap_.get_height());
}
