#include <stdint.h>
//#include <mach/mach_types.h>
//#include <mach/machine.h>


#include "Vector3.h"

#include "Matrix4.h"
#include "Quat.h"

namespace VisageSDK
{

Vector3::Vector3()
{
	x = y = z = 0;
	w = 1;
}

Vector3::Vector3( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
	w = 1;
}

float Vector3::magnitude() const
{
	return sqrt( x*x + y*y + z*z );
}

Vector3& Vector3::normalize()
{
	*this *= ( 1.f / magnitude() );

	return *this;
}

Vector3 Vector3::getNormalized()
{
	Vector3 v = *this;
	return v.normalize();
}

float Vector3::angle( const Vector3& v ) const
{
	return acos( *this * v );
}

bool Vector3::operator ==( const Vector3& v ) const
{
	if( !V_EQF( x, v.x ) ||
		!V_EQF( y, v.y ) ||
		!V_EQF( z, v.z ) )
		return false;

	return true;
}

bool Vector3::operator !=( const Vector3& v ) const
{
	return !( *this == v );
}

Vector3 Vector3::operator +() const
{
	return *this;
}

Vector3 Vector3::operator -() const
{
	return Vector3( -x, -y, -z );
}

Vector3 Vector3::operator *( float s ) const
{
	return Vector3( x * s, y * s, z * s );
}

void Vector3::operator *=( float s )
{
	x *= s;
	y *= s;
	z *= s;
}

Vector3 Vector3::operator /( float s ) const
{
	return Vector3( x/s, y/s, z/s );
}

void Vector3::operator /=( float s )
{
	x /= s;
	y /= s;
	z /= s;
}

Vector3 Vector3::operator +( const Vector3& v ) const
{
	return Vector3( x + v.x, y + v.y, z + v.z );
}

void Vector3::operator +=( const Vector3& v )
{
	x += v.x;
	y += v.y;
	z += v.z;
}

Vector3 Vector3::operator -( const Vector3& v ) const
{
	return Vector3( x - v.x, y - v.y, z - v.z );
}

void Vector3::operator -=( const Vector3& v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

float Vector3::operator *( const Vector3& v ) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::operator ^( const Vector3& v ) const
{
	Vector3 c = *this;
	Vector3& c_ref = c;


	//intel syntax
#ifdef INTEL_SYNTAX
	asm(
			//".intel_syntax noprefix\n"
			"mov esi, c_ref \n"
			"mov edi, v \n"
			"movups	xmm0,	[esi]\n"
			"movups	xmm1,	[edi]\n"
			"movaps	xmm2,	xmm0\n"
			"movaps	xmm3,	xmm0\n"
			"shufps	xmm0,	xmm0,	0xc9\n"
			"shufps	xmm1,	xmm1,	0xd2\n"
			"mulps	xmm0,	xmm1\n"
			"shufps	xmm2,	xmm2,	0xd2\n"
			"shufps	xmm3,	xmm3,	0xc9\n"
			"mulps	xmm2,	xmm3\n"
			"subps	xmm0,	xmm2\n"
			"movups	[esi],	xmm0\n"
			//".att_syntax prefix\n"
		);
	

/* 
		asm("mov %[cref_in], %%esi": :[cref_in] "r"(&c_ref));//it must be an address,
		asm("mov %[v_in], %%edi": : [v_in] "r"(&v));
		asm("movups (%esi), %xmm0");
		asm("movups (%edi), %xmm1");
		asm("movaps %xmm0, %xmm2");
		asm("movaps %xmm0, %xmm3");

		asm("shufps $0xc9, %xmm0, %xmm0");
		asm("shufps $0xd2, %xmm1, %xmm1");
		asm("mulps %xmm1, %xmm0");

		asm("shufps $0xd2, %xmm2, %xmm2");
		asm("shufps $0xc9, %xmm3, %xmm3");
		asm("mulps %xmm3, %xmm2");

		asm("subps %xmm2, %xmm0");
		asm("movups %xmm0, (%esi)"); */

	
#elif defined(IPHONE_DEV) || defined(_64_BITS) || defined (AVOID_ASSEMBLY_ON_VECTOR)
	
//C+ version of cross-product code
	
	Vector3 aux;

	aux.x = y * v.z - z * v.y;
	aux.y = z * v.x - x * v.z;
	aux.z = x * v.y - y * v.x;
	
	(Vector3)*this = aux;
	//x = aux.x;
	//y = aux.y;
	//z = aux.z;
	
	
	
#elif (defined(LINUX) || defined(MAC) || defined(IPHONE))
	asm(
			"mov %[cref_in], %%esi;"
			"mov %[v_in], %%edi;"
			"movups (%%esi), %%xmm0;"
			"movups (%%edi), %%xmm1;"
			"movaps %%xmm0, %%xmm2;"
			"movaps %%xmm0, %%xmm3;"

			"shufps $0xc9, %%xmm0, %%xmm0;"
			"shufps $0xd2, %%xmm1, %%xmm1;"
			"mulps %%xmm1, %%xmm0;"

			"shufps $0xd2, %%xmm2, %%xmm2;"
			"shufps $0xc9, %%xmm3, %%xmm3;"
			"mulps %%xmm3, %%xmm2;"

			"subps %%xmm2, %%xmm0;"
			"movups %%xmm0, (%%esi);"
			:
			: [cref_in] "r"(&c_ref), [v_in] "r"(&v)
		);
	
#endif


	
	return c_ref;
}

void Vector3::operator ^=( const Vector3& v )
{
	*this = *this ^ v;
}

Vector3 Vector3::operator*( const Matrix4& mat ) const
{
	Vector3 res;

	res.x = mat.getElement(0,0) * x + mat.getElement(1,0) * y + mat.getElement(2,0) * z + mat.getElement(3,0);
	res.y = mat.getElement(0,1) * x + mat.getElement(1,1) * y + mat.getElement(2,1) * z + mat.getElement(3,1);
	res.z = mat.getElement(0,2) * x + mat.getElement(1,2) * y + mat.getElement(2,2) * z + mat.getElement(3,2);
	res.w = 1.f;

	return res;
}

void Vector3::operator*=( const Matrix4& mat )
{
	*this = *this * mat;
}

bool Vector3::equals( const Vector3& v ) const
{
	return *this == v;
}

Vector3& Vector3::negate()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

Vector3 Vector3::mul( const float s ) const
{
	return *this * s;
}

Vector3 Vector3::div( const float s ) const
{
	return *this/s;
}

Vector3 Vector3::add( const Vector3& v ) const
{
	return *this + v;
}

Vector3 Vector3::sub( const Vector3& v ) const
{
	return *this - v;
}

float Vector3::dot( const Vector3& v ) const
{
	return *this * v;
}

Vector3 Vector3::cross( const Vector3& v ) const
{
	return *this ^ v;
}

Vector3 Vector3::mul( const Matrix4& mat ) const
{
	return *this * mat;
}

Vector3& Vector3::rotate( const Quat& rot )
{
	Quat vq = Quat( 0, x, y, z );
	vq = rot ^ vq;
	vq ^= rot.getConjugate();
	x = vq.x;
	y = vq.y;
	z = vq.z;

	return *this;
}

Vector3 Vector3::getXAxis()
{
	return Vector3( 1.f, 0.f, 0.f );
}

Vector3 Vector3::getYAxis()
{
	return Vector3( 0.f, 1.f, 0.f );
}

Vector3 Vector3::getZAxis()
{
	return Vector3( 0.f, 0.f, 1.f );
}

}
