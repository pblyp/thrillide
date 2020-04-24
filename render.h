#ifndef RENDER_H
#define RENDER_H
#include "geom.h"
#include "model.h"
class Render
{
public:
	Render();

};

void triangle_nondiffuse(v3df t0, v3df t1, v3df t2, QImage &image, QColor color, int *zbuffer);
void triangle_diffuse(v3df t0, v3df t1, v3df t2, v2di uv0, v2di uv1, v2di uv2, QImage &image, Model &mdl, QRgb color, float intensity, int *zbuffer);
v3df m2v(mtrx m);
mtrx viewport(int x, int y, int w, int h);
mtrx lookat(v3df eye, v3df center, v3df up);
v3df viewat(v3df eye, v3df center, v3df up);
void rendsc(QImage *im, mtrx ModelView, mtrx Projection, mtrx ViewPort);

#endif // RENDER_H
