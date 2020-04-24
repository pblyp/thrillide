#ifndef MODEL_H
#define MODEL_H

#include <QColor>
#include <vector>
#include "geom.h"
#include "tga.h"

class Model
{
private:
	std::vector<v3df> verts_;
	std::vector<std::vector<v3di> > faces_;
	std::vector<v3df> norms_;
	std::vector<v2df> uv_;
	TGAImage diffusemap_;
	void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	v3df vert(int i);
	v2di uv(int iface, int nvert);
	QRgb diffuse(v2di uv);
	std::vector<int> face(int idx);
};

#endif // MODEL_H
