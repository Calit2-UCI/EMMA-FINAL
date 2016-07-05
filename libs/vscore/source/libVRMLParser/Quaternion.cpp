#include "stdafx.h"
#include "Quaternion.h"
#include <math.h>

Quaternion::Quaternion(const Vec3f &axis, float angle)
{
	float v[3] = {axis.x, axis.y, axis.z};
	float vLength = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	if (vLength == 0.0f)
	{
		v[0] = 1;
		v[1] = 0;
		v[2] = 0;
		angle = 0;
	}
//	else
//	{
//		for (int i = 0; i < 3; i++) v[i] /= vLength;
//	}

	float sinA = (float) sin(angle / 2);
	x = v[0] * sinA;
	y = v[1] * sinA;
	z = v[2] * sinA;
	w = (float) cos(angle / 2);
}

float Quaternion::norm() const
{
	return (float) sqrt(x * x + y * y + z * z + w * w);
}

void Quaternion::normalize()
{
	float qLength = norm();

	if (qLength != 0.0f)
	{
		x /= qLength;
		y /= qLength;
		z /= qLength;
		w /= qLength;
	}
}

Quaternion Quaternion::conj() const
{
	Quaternion q;

	q.x = -x;
	q.y = -y;
	q.z = -z;
	q.w = w;

	return q;
}

Quaternion Quaternion::operator *(float f) const
{
	Quaternion q;

	q.x = x * f;
	q.y = y * f;
	q.z = z * f;
	q.w = w * f;

	return q;
}

Quaternion Quaternion::operator *(const Quaternion &q2) const
{
	Quaternion q;
	const Quaternion &q1 = *this;

	q.x = q2.w * q1.x + q2.x * q1.w + q2.y * q1.z - q2.z * q1.y;
    q.y = q2.w * q1.y + q2.y * q1.w + q2.z * q1.x - q2.x * q1.z;
    q.z = q2.w * q1.z + q2.z * q1.w + q2.x * q1.y - q2.y * q1.x;
    q.w = q2.w * q1.w - q2.x * q1.x - q2.y * q1.y - q2.z * q1.z;

	return q;
}

Vec3f Quaternion::operator *(const Vec3f &v) const
{
	Vec3f vector;
	Quaternion q2;

	q2.x = v.x;
	q2.y = v.y;
	q2.z = v.z;
	q2.w = 1;

	//Quaternion &q = (*this) * q2;

	Quaternion aux = (*this) * q2;
	Quaternion &q = aux;

	vector.x = q.x;
	vector.y = q.y;
	vector.z = q.z;

	return vector;
}
