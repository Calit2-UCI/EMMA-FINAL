
#include "Vector2.h"

// C++
#include <math.h>

namespace VisageSDK
{

Vector2::Vector2()
{
	x = y = 0;
}

Vector2::Vector2( float x, float y )
{
	this->x = x;
	this->y = y;
}

float Vector2::magnitude() const
{
	return sqrt( x*x + y*y );
}

Vector2& Vector2::normalize()
{
	return ( *this = *this * 1. / magnitude() );
}

float Vector2::angle( const Vector2& v ) const
{
	return acos( ( *this * v )
		* ( 1.f / magnitude() )
		* ( 1.f / sqrt( this->x * v.x + this->y * v.y ) ) );
}

bool Vector2::operator ==( const Vector2& v ) const
{
	if( !V_EQF( x, v.x ) ||
		!V_EQF( y, v.y ) )
		return false;

	return true;
}

bool Vector2::operator !=( const Vector2& v ) const
{
	return !( *this == v );
}

Vector2 Vector2::operator +() const
{
	return *this;
}

Vector2 Vector2::operator -() const
{
	return Vector2( -x, -y );
}

Vector2 Vector2::operator *( float s ) const
{
	return Vector2( x * s, y * s );
}

void Vector2::operator *=( float s )
{
	x *= s;
	y *= s;
}

Vector2 Vector2::operator /( float s ) const
{
	return Vector2( x/s, y/s );
}

void Vector2::operator /=( float s )
{
	x /= s;
	y /= s;
}

Vector2 Vector2::operator +( const Vector2& v ) const
{
	return Vector2( x + v.x, y + v.y );
}

void Vector2::operator +=( const Vector2& v )
{
	x += v.x;
	y += v.y;
}

Vector2 Vector2::operator -( const Vector2& v ) const
{
	return Vector2( x - v.x, y - v.y );
}

void Vector2::operator -=( const Vector2& v )
{
	x -= v.x;
	y -= v.y;
}

float Vector2::operator *( const Vector2& v ) const
{
	return x * v.x + y * v.y;
}

bool Vector2::equals( const Vector2& v ) const
{
	return *this == v;
}

Vector2& Vector2::negate()
{
	x = -x;
	y = -y;

	return *this;
}

Vector2 Vector2::mul( const float s ) const
{
	return *this * s;
}

Vector2 Vector2::div( const float s ) const
{
	return *this/s;
}

Vector2 Vector2::add( const Vector2& v ) const
{
	return *this + v;
}

Vector2 Vector2::sub( const Vector2& v ) const
{
	return *this - v;
}

float Vector2::dot( const Vector2& v ) const
{
	return *this * v;
}

Vector2 Vector2::GetXAxis()
{
	return Vector2( 1.f, 0.f );
}

Vector2 Vector2::GetYAxis()
{
	return Vector2( 0.f, 1.f );
}

}
