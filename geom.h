#ifndef GEOM_H
#define GEOM_H

#include <cmath>
#include <vector>
#include <iostream>

class mtrx;

//------------- Векторные структуры ---------------
template <class t>
struct v2d
{
	t x, y;
	v2d<t>(): x(t()), y(t()) {}
	v2d<t>(t _x, t _y): x(_x), y(_y) {}
	v2d<t> operator + (const v2d<t> &V) const
	{
		return v2d<t> (x+V.x, y+V.y);
	}
	v2d<t> operator - (const v2d<t> &V) const
	{
		return v2d<t> (x-V.x, y-V.y);
	}
	v2d<t> operator * (const float value) const
	{
		return v2d<t> (x*value, y*value);
	}

	t& operator[] (const int i)
	{
		return i<=0 ? x : y;
	}

	template <class>
	friend std::ostream& operator<< (std::ostream& s, v2d<t>& v);
};

template <class t>
std::ostream& operator<<(std::ostream& s, v2d<t>& v)
{
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t>
struct v3d
{
	t x, y, z;
	v3d<t>(): x(t()), y(t()), z(t()) {}
	v3d<t>(t _x, t _y, t _z): x(_x), y(_y), z(_z) {}
	v3d<t>(mtrx m);

	template <class u> v3d<t>(const v3d<u> &v);

	v3d<t> operator ^ (const v3d<t> &V) const
	{
		return v3d<t> (y*V.z - z*V.y, z*V.x-x*V.z, x*V.y-y*V.x);
	}
	v3d<t> operator + (const v3d<t> &V) const
	{
		return v3d<t> (x+V.x, y+V.y, z+V.z);
	}
	v3d<t> operator - (const v3d<t> &V) const
	{
		return v3d<t> (x-V.x, y-V.y, z-V.z);
	}
	v3d<t> operator * (const float value) const
	{
		return v3d<t> (x*value, y*value, z*value);
	}
	v3d<t> operator % (const v3d<t> &V) const
	{
		return v3d<t> (x*V.x, y*V.y, z*V.z);
	}
	t operator * (const v3d<t> &v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}

	float norm() const
	{
		return std::sqrt(x*x+y*y+z*z);
	}

	v3d<t> & normalize(t l=1)
	{
		*this = (*this) * (l/norm());
		return *this;
	}

	t& operator[] (const int i)
	{
		return i<=0 ? x : (1==i? y : z);
	}

	template <class> friend std::ostream& operator<< (std::ostream& s, v3d<t>& v);
};

template <class t>
std::ostream& operator<<(std::ostream& s, v3d<t>& v)
{
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

typedef v2d<float> v2df;
typedef v2d<int> v2di;
typedef v3d<float> v3df;
typedef v3d<int> v3di;

template <> template <> v3d<int>::v3d(const v3d<float> &v);

template <> template <> v3d<float>::v3d(const v3d<int> &v);


//----------- Матрицы -----------
class mtrx
{
	std::vector<std::vector<float>> m;
	int rows, cols;

public:
	mtrx(int r=4, int c=4);
	mtrx(v3df v);
	int nrows();
	int ncols();
	static mtrx ident(int dims);
	std::vector<float>& operator[](const int i);
	mtrx operator*(const mtrx& a);
	friend std::ostream& operator<< (std::ostream& s, mtrx& m);
};

#endif // GEOM_H
