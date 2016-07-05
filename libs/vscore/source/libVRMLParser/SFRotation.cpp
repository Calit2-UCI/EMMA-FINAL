#include "stdafx.h"
#include "SFRotation.h"
#include <math.h>
#include "EulerAngles.h"

SFRotation::SFRotation(float x, float y, float z, float w)
{
	_quaternion.x = x;
	_quaternion.y = y;
	_quaternion.z = z;
	_quaternion.w = w;

	_value[3] = (float) acos(w) * 2;
	float sinA = (float) sqrt(1.0 - w * w);
	if (fabs(sinA) < 0.0005) sinA = 1.0;

	_value[0] = x / sinA;
	_value[1] = y / sinA;
	_value[2] = z / sinA;

	float vLength = (float) sqrt(_value[0] * _value[0] +
		_value[1] * _value[1] + _value[2] * _value[2]);

	if (vLength == 0.0f)
	{
		_value[0] = 0;
		_value[1] = 0;
		_value[2] = 1;
		_value[3] = 0;
	}
	else
	{
		for (int i = 0; i < 3; i++) _value[i] /= vLength;
	}


	//EulerAngles &ea = Eul_FromQuat(_quaternion, EulOrdXYZs);
	EulerAngles aux = Eul_FromQuat(_quaternion, EulOrdXYZs);
	EulerAngles &ea = aux;
	_euler.x = ea.x;
	_euler.y = ea.y;
	_euler.z = ea.z;
	_eulerOrder = (int) ea.w;
}

SFRotation::SFRotation(const float *value)
{
	for (int i = 0; i < 4; i++) _value[i] = value[i];

	Vec3f axis(_value[0], _value[1], _value[2]);
	Quaternion q(axis, _value[3]);
	_quaternion.x = q.x;
	_quaternion.y = q.y;
	_quaternion.z = q.z;
	_quaternion.w = q.w;

	//EulerAngles &ea = Eul_FromQuat(_quaternion, EulOrdXYZs);
	EulerAngles aux = Eul_FromQuat(_quaternion, EulOrdXYZs);
	EulerAngles &ea = aux;
	_euler.x = ea.x;
	_euler.y = ea.y;
	_euler.z = ea.z;
	_eulerOrder = (int) ea.w;
}

SFRotation::SFRotation(Vec3f euler, int mode)
{
	setEulerAngles(euler, mode);
}

SFRotation::SFRotation(const Quaternion &q)
{
	_quaternion.x = q.x;
	_quaternion.y = q.y;
	_quaternion.z = q.z;
	_quaternion.w = q.w;

	_value[3] = (float) acos(q.w) * 2;
	float sinA = (float) sqrt(1.0 - q.w * q.w);
	if (fabs(sinA) < 0.0005) sinA = 1.0;

	_value[0] = q.x / sinA;
	_value[1] = q.y / sinA;
	_value[2] = q.z / sinA;

	float vLength = (float) sqrt(_value[0] * _value[0] +
		_value[1] * _value[1] + _value[2] * _value[2]);

	if (vLength == 0.0f)
	{
		_value[0] = 0;
		_value[1] = 0;
		_value[2] = 1;
		_value[3] = 0;
	}
	else
	{
		for (int i = 0; i < 3; i++) _value[i] /= vLength;
	}

	//EulerAngles &ea = Eul_FromQuat(_quaternion, EulOrdXYZs);
	EulerAngles aux = Eul_FromQuat(_quaternion, EulOrdXYZs);
	EulerAngles &ea = aux;
	_euler.x = ea.x;
	_euler.y = ea.y;
	_euler.z = ea.z;
	_eulerOrder = (int) ea.w;
}

SFRotation::SFRotation(const Matrix &mat)
{
	float matrix[4][4];
	mat.getValue(matrix);

	//EulerAngles &ea = Eul_FromHMatrix(matrix, EulOrdXYZs);
	EulerAngles aux = Eul_FromQuat(_quaternion, EulOrdXYZs);
	EulerAngles &ea = aux;
	_euler.x = ea.x;
	_euler.y = ea.y;
	_euler.z = ea.z;
	_eulerOrder = (int) ea.w;

	Quaternion q = Eul_ToQuat(ea);
	_quaternion.x = q.x;
	_quaternion.y = q.y;
	_quaternion.z = q.z;
	_quaternion.w = q.w;

	_value[3] = (float) acos(q.w) * 2;
	float sinA = (float) sin(_value[3] / 2);
	_value[0] = q.x / sinA;
	_value[1] = q.y / sinA;
	_value[2] = q.z / sinA;
}

void SFRotation::write(FILE *f, int indent) const
{
	for (int i = 0; i < 4; i++) fprintf(f, "%g ", _value[i]);
	fprintf(f, "\n");
}

bool SFRotation::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	SFRotation *v = (SFRotation *) value;
	for (int i = 0; i < 4; i++)
		if (v->_value[i] != _value[i]) return false;
	return true;
}

Vec3f SFRotation::getEulerAngles(int order)
{
	EulerAngles outAngs, inAngs;
	HMatrix m;

	inAngs.x = _euler.x;
	inAngs.y = _euler.y;
	inAngs.z = _euler.z;
	inAngs.w = (float) _eulerOrder;

    Eul_ToHMatrix(inAngs, m);
    outAngs = Eul_FromHMatrix(m, order);
	Vec3f v(outAngs.x, outAngs.y, outAngs.z);

	return v;
}

void SFRotation::setEulerAngles(Vec3f angles, int order)
{
	_euler.x = angles.x;
	_euler.y = angles.y;
	_euler.z = angles.z;
	_eulerOrder = order;

	//EulerAngles &ea = Eul_(angles.x, angles.y, angles.z, order);
	EulerAngles aux = Eul_FromQuat(_quaternion, EulOrdXYZs);
	EulerAngles &ea = aux;
	Quaternion q = Eul_ToQuat(ea);
	_quaternion.x = q.x;
	_quaternion.y = q.y;
	_quaternion.z = q.z;
	_quaternion.w = q.w;

	_value[3] = (float) acos(q.w) * 2;
	float sinA = (float) sqrt(1.0 - q.w * q.w);
	if (fabs(sinA) < 0.0005) sinA = 1.0;

	_value[0] = q.x / sinA;
	_value[1] = q.y / sinA;
	_value[2] = q.z / sinA;

	float vLength = (float) sqrt(_value[0] * _value[0] +
		_value[1] * _value[1] + _value[2] * _value[2]);

	if (vLength == 0.0f)
	{
		_value[0] = 0;
		_value[1] = 0;
		_value[2] = 1;
		_value[3] = 0;
	}
	else
	{
		for (int i = 0; i < 3; i++) _value[i] /= vLength;
	}
}

const Quaternion & SFRotation::getQuat() const
{
	return _quaternion;
}

Vec3f SFRotation::operator *(const Vec3f &v) const
{
	return _euler.cross(v);
}
