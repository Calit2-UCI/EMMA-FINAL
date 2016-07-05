#ifndef _VEC3F_H
#define _VEC3F_H

#ifndef __MATH_H__
#include <math.h>
#endif

/**
* 3D vector. 
* \ingroup VRMLParser
*/
class Vec3f {
public:
		Vec3f()
		{ x = 0.0f; y = 0.0f; z = 0.0f; }
		Vec3f(const Vec3f &v)
		{ x = v.x; y = v.y; z = v.z; }
		Vec3f(float nx, float ny, float nz)
		{ x = nx; y = ny; z = nz; }
		Vec3f(const float *v)
		{ x = v[0]; y = v[1]; z = v[2]; }

     int	operator==(const Vec3f &v) const
		{ return x == v.x && y == v.y && z == v.z; }
     int	operator!=(const Vec3f &v) const
		{ return x != v.x || y != v.y || z != v.z; }
     Vec3f	operator+(const Vec3f &v) const
		{ return Vec3f(x + v.x, y + v.y, z + v.z); }
     Vec3f	operator-(const Vec3f &v) const
		{ return Vec3f(x - v.x, y - v.y, z - v.z); }
     Vec3f	operator-() const
		{ return Vec3f(-x, -y, -z); }
     Vec3f	operator*(float f) const
		{ return Vec3f(x * f, y * f, z * f); }
     Vec3f	operator*(const Vec3f &v) const
		{ return Vec3f(x * v.x, y * v.y, z * v.z); }
     void	zero()
		{ x = y = z = 0.0f; }
     void	scale(float s)
		{ x *= s; y *= s; z *= s; }
     float 	length() const
		{ return (float) sqrt(x * x + y * y + z * z); }
     Vec3f	cross(const Vec3f &v) const
		{ return Vec3f(y * v.z - z * v.y,
			       z * v.x - x * v.z,
			       x * v.y - y * v.x); }
     float	dot(const Vec3f &v) const
		{ return x * v.x + y * v.y + z * v.z; }
     void	normalize()
		{ float len = length();
	          if (len != 0.0f) { x /= len; y /= len; z /= len; } }
     Vec3f     &operator+=(const Vec3f &v)
		{ x += v.x;  y += v.y; z += v.z; return *this; }
public:
     float	x, y, z;
};

#endif // _VEC3F_H
