
#include "Quat.h"
#include "Vector3.h"
#include "Matrix4.h"

namespace VisageSDK
{

Quat::Quat()
{
	w = 1.f;
	x = y = z = 0.f;
}

Quat::Quat( float w, float x, float y, float z )
{
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
}

Quat::Quat( float ax, float ay, float az, EulerRotOrder order )
{
	Vector3 x_axis = Vector3::getXAxis(),
		y_axis = Vector3::getYAxis(),
		z_axis = Vector3::getZAxis();
	Quat qx, qy, qz;
	qx = Quat( x_axis, ax );
	qy = Quat( y_axis, ay );
	qz = Quat( z_axis, az );

	if( order == V_EULER_XYZ )
		*this = qx ^ qy ^ qz;
	else if( order == V_EULER_XZY )
		*this = qx ^ qz ^ qy;
	else if( order == V_EULER_YXZ )
		*this = qy ^ qx ^ qz;
	else if( order == V_EULER_YZX )
		*this = qy ^ qz ^ qx;
	else if( order == V_EULER_ZXY )
		*this = qz ^ qx ^ qy;
	else if( order == V_EULER_ZYX )
		*this = qz ^ qy ^ qx;
}

Quat::Quat( const Vector3& axis, float angle )
{
	Vector3 t_axis;
	float cos_a, sin_a;
	
	t_axis = axis;
	t_axis.normalize();
	sin_a = sin( angle / 2.f );
	cos_a = cos( angle / 2.f );
	w = cos_a;
	x = axis.x * sin_a;
	y = axis.y * sin_a;
	z = axis.z * sin_a;
}

Quat& Quat::conjugate()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

Quat Quat::getConjugate() const
{
	return Quat( w, -x, -y, -z );
}

float Quat::magnitude() const
{
	float t = w * w + x * x + y * y + z * z;
	return V_EQF( t, 1.f ) ? 1.f : sqrt(t);
}

Quat& Quat::normalize()
{
	float qmag = this->magnitude();
	w /= qmag;
	x /= qmag;
	y /= qmag;
	z /= qmag;

	return *this;
}

Quat Quat::getNormalized()
{
	Quat q = *this;
	return q.normalize();
}

Quat& Quat::invert()
{
	float qmag = magnitude();
	conjugate();
	w /= qmag;
	x /= qmag;
	y /= qmag;
	z /= qmag;

	return *this;
}

Quat Quat::getInverse() const
{
	Quat q = *this;
	float qmag = magnitude();
	q.w /= qmag;
	q.x /= qmag;
	q.y /= qmag;
	q.z /= qmag;

	return q.getConjugate() * 1.f / qmag;
}

void Quat::getEuler( float& ax, float& ay, float& az, EulerRotOrder order ) const
{
	Matrix4(*this).getEuler( ax, ay, az, order );
}

float Quat::getAngle() const
{
	return acos(w) * 2.f;
}

void Quat::getAxisAngle( Vector3& axis, float& angle ) const
{
	float cos_a, sin_a;
	Quat q = *this;
	
	q.normalize();
	cos_a = q.w;
	angle = acos(cos_a) * 2.f;
	sin_a = sqrt( 1.f - cos_a * cos_a );
	if( V_EQF( sin_a, 0.f ) ) sin_a = 1.f;
	axis.x = x / sin_a;
	axis.y = y / sin_a;
	axis.z = z / sin_a;
}

bool Quat::operator ==( const Quat& q ) const
{
	if( !V_EQF( x, q.x ) ||
		!V_EQF( y, q.y ) ||
		!V_EQF( z, q.z ) || 
		!V_EQF( w, q.w ) )
		return false;

	return true;
}

bool Quat::operator !=( const Quat& q ) const
{
	return !( *this == q );
}

Quat Quat::operator +() const
{
	return *this;
}

Quat Quat::operator -() const
{
	return Quat( -w, -x, -y, -z );
}

Quat Quat::operator +( const Quat& q ) const
{
	return Quat( this->w + q.w, this->x + q.x, this->y + q.y, this->z + q.z );
}

void Quat::operator +=( const Quat& q )
{
	*this = *this + q;
}

Quat Quat::operator -( const Quat& q ) const
{
	return Quat( this->w - q.w, this->x - q.x, this->y - q.y, this->z - q.z );
}

void Quat::operator -=( const Quat& q )
{
	*this = *this - q;
}

Quat Quat::operator *( float s ) const
{
	return Quat( s * w, s * x, s * y, s * z );
}

void Quat::operator *=( float s )
{
	*this = *this * s;
}

Quat Quat::operator /( float s ) const
{
	return Quat( w/s, x/s, y/s, z/s );
}

void Quat::operator /=( float s )
{
	*this = *this / s;
}

float Quat::operator *( const Quat& q ) const
{
	return w * q.w + x * q.x + y * q.y + z * q.z;
}

Quat Quat::operator ^( const Quat& q ) const
{
	return Quat( w * q.w - x * q.x - y * q.y - z * q.z,
		w * q.x + q.w * x + y * q.z - q.y * z,
		w * q.y + q.w * y - x * q.z + q.x * z,
		w * q.z + q.w * z + x * q.y - q.x * y );
}

void Quat::operator ^=( const Quat& q )
{
	*this = *this ^ q;
}

bool Quat::equals( const Quat& q ) const
{
	return *this == q;
}

Quat& Quat::negate()
{
	w = -w;
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

Quat Quat::add( const Quat& q ) const
{
	return *this + q;
}

float Quat::dot( const Quat& q ) const
{
	return *this * q;
}

Quat Quat::wedge( const Quat& q ) const
{
	return *this ^ q;
}

Quat Quat::slerp( const Quat& q, float t ) const
{
	float cos_a = *this * q;
	Quat q1;

	if( cos_a < 0.f )
	{
		cos_a = -cos_a;
		q1 = -q;
	}
	else
		q1 = q;

	if( fabs(cos_a) < 0.995f )
	{
		float sin_a = sqrt( 1.f - cos_a * cos_a );
		float a = atan2( sin_a, cos_a );
		float inv_sin_a = 1.f / sin_a;
		q1 = *this * sin( ( 1.f - t ) * a ) * inv_sin_a + q1 * sin( t * a ) * inv_sin_a;
	}
	else
	{
		q1 = *this * ( 1.f - t ) + q1 * t;
		q1.normalize();
	}

	return q1;
}

Quat Quat::lerp( const Quat& q, float t ) const
{
	Quat q1 = *this * ( 1.f - t ) + q * t;
	q1.normalize();
	return q1;
}

}
