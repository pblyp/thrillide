#include <QColor>
#include <QImage>
#include "render.h"

const int depth = 64000;

v3df m2v(mtrx m)
{
	return v3df(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

mtrx viewport(int x, int y, int w, int h) {
	mtrx m = mtrx::ident(4);
	m[0][3] = x+w/2.f;
	m[1][3] = y+h/2.f;
	m[2][3] = depth/2.f;

	m[0][0] = w/2.f;
	m[1][1] = h/2.f;
	m[2][2] = depth/2.f;
	return m;
}

mtrx lookat(v3df eye, v3df center, v3df up) {
	v3df z = (eye-center).normalize();
	v3df x = (up^z).normalize();
	v3df y = (z^x).normalize();
	mtrx res = mtrx::ident(4);
	for (int i=0; i<3; i++)
	{
		res[0][i] = x[i];
		res[1][i] = y[i];
		res[2][i] = z[i];
		res[i][3] = -center[i];
	}
	return res;
}

v3df viewat(v3df eye, v3df center, v3df up) {
	v3df k;
	v3df z = (eye-center).normalize();
	v3df x = (up^z).normalize();
	v3df y = (z^x).normalize();
	mtrx res = mtrx::ident(4);
	for (int i=0; i<3; i++)
	{
		res[0][i] = x[i];
		res[1][i] = y[i];
		res[2][i] = z[i];
		res[i][3] = -center[i];
	}
	return res;
}

void triangle_nondiffuse(v3df t0, v3df t1, v3df t2, QImage &image, QColor color, int *zbuffer)
{
	if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
	if (t0.y>t1.y) std::swap(t0, t1);
	if (t0.y>t2.y) std::swap(t0, t2);
	if (t1.y>t2.y) std::swap(t1, t2);
	int total_height = t2.y-t0.y;
	for (int i=0; i<total_height; i++)
	{
		bool second_half = i>t1.y-t0.y || t1.y==t0.y;
		float segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
		float alpha = (float)i/total_height;
		float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
		v3di A =               v3di(t0) + v3df(t2-t0)*alpha;
		v3di B = second_half ? v3di(t1) + v3df(t2-t1)*beta : v3di(t0) + v3df(t1-t0)*beta;
		if (A.x>B.x) std::swap(A, B);
		for (int j=A.x; j<=B.x; j++)
		{
			float phi = B.x==A.x ? 1. : (float)(j-A.x)/(float)(B.x-A.x);
			v3di P = v3df(A) + v3df(B-A)*phi;
			if (P.x > 0 && P.x < image.width() && P.y > 0 && P.y < image.height())
			{
				unsigned long int idx = P.y+P.x*image.height();
				//int idx = P.x+P.y*image.width();
				if (idx >= 0 && idx <= image.width()*image.height())
				{
					int curzbf = zbuffer[idx];
					if ((curzbf<P.z))
					{
						zbuffer[idx] = P.z;
						{

							image.setPixelColor(P.x, P.y, color);
						}
					}
				}
			}
			//else (printf("idx out of bounds\n"));
		}
	}
}

void triangle_diffuse(v3df t0, v3df t1, v3df t2, v2di uv0, v2di uv1, v2di uv2, QImage &image, Model &mdl, QRgb color, float intensity, int *zbuffer)
{
	if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
	if (t0.y>t1.y) std::swap(t0, t1);
	if (t0.y>t2.y) std::swap(t0, t2);
	if (t1.y>t2.y) std::swap(t1, t2);
	int total_height = t2.y-t0.y;
	for (int i=0; i<total_height; i++)
	{
		bool second_half = i>t1.y-t0.y || t1.y==t0.y;
		float segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
		float alpha = (float)i/total_height;
		float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
		v3df A =               v3di(t0) + v3df(t2-t0)*alpha;
		v3df B = second_half ? v3di(t1) + v3df(t2-t1)*beta : v3di(t0) + v3df(t1-t0)*beta;
		v2di uvA =               uv0 +      (uv2-uv0)*alpha;
		v2di uvB = second_half ? uv1 + (uv2-uv1)*beta : uv0 + (uv1-uv0)*beta;
		if (A.x>B.x)
		{
			std::swap(A, B);
			std::swap(uvA, uvB);
		}
		for (int j=A.x; j<=B.x; j++)
		{
			float phi = B.x==A.x ? 1. : (float)(j-A.x)/(float)(B.x-A.x);
			v3di P = v3df(A) + v3df(B-A)*phi;
			v2di uvP = uvA + (uvB-uvA)*phi;
			if (P.x > 0 && P.x < image.width() && P.y > 0 && P.y < image.height())
			{
				unsigned long int idx = P.y+P.x*image.height();
				//int idx = P.x+P.y*image.width();
				if (idx >= 0 && idx <= image.width()*image.height())
				{
					//int curzbf = zbuffer[idx];
					if ((zbuffer[idx]<P.z))
					{
						zbuffer[idx] = P.z;
						//QRgb tcolor = qRgb(255,255,255);
						QRgb tcolor = mdl.diffuse(uvP);
						QRgb tc = qRgb(((qRed(tcolor)-qRed(color))*intensity),
									   ((qGreen(tcolor)-qGreen(color))*intensity),
									   ((qBlue(tcolor)-qBlue(color))*intensity));
						QRgb *rowData = (QRgb*)image.scanLine(P.y);
						QRgb *pixelData = &rowData[P.x];
						rowData+=P.x;
						*rowData = tc;
					}
				}
			}
			//else (printf("idx out of bounds\n"));
		}
	}
}
