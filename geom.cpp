#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include "geom.h"

template <> v3d<float>::v3d(mtrx m) : x(m[0][0]/m[3][0]), y(m[1][0]/m[3][0]), z(m[2][0]/m[3][0]) {}

template <> template <> v3d<int>::v3d<>(const v3d<float> &v): x(int(v.x+.5)), y(int(v.y+.5)), z(int(v.z+.5)) {}

template <> template <> v3d<float>::v3d<>(const v3d<int> &v): x(v.x), y(v.y), z(v.z) {}

mtrx::mtrx(int r, int c): m(std::vector<std::vector<float>> (r, std::vector<float>(c, 0.f))), rows(r), cols(c) {}

mtrx::mtrx(v3df v) : m(std::vector<std::vector<float> >(4, std::vector<float>(1, 1.f))), rows(4), cols(1) {
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
}

int mtrx::nrows()
{
	return rows;
}

int mtrx::ncols()
{
	return cols;
}

mtrx mtrx::ident(int dims)
{
	mtrx e(dims, dims);
	for (int i=0; i<dims; i++)
		for (int j=0; j<dims; j++)
			e[i][j] = (i==j ? 1.f : 0.f);
	return e;
}

std::vector<float>& mtrx::operator[](const int i)
{
	assert(i>=0 && i<rows);
	return m[i];
}

mtrx mtrx::operator*(const mtrx& a)
{
	assert(cols == a.rows);
	mtrx res(rows, a.cols);
	for (int i=0; i<rows; i++)
	{
		for (int j=0; j<a.cols; j++)
		{
			res.m[i][j] = 0.f;
			for (int k=0; k<cols; k++)
			{
				res.m[i][j] += m[i][k] * a.m[k][j];
			}
		}
	}
	return res;
}


std::ostream& operator<< (std::ostream &s, mtrx &m)
{
	for (int i=0; i<m.rows; i++)
	{
		for (int j=0; j<m.cols; j++)
		{
			s << m[i][j];
			if (j<m.ncols()-1)
				s << "\t";
		}
		s << "\n";
	}
	return s;
}
