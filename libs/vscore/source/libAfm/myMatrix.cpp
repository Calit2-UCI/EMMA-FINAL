
#include "math.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <float.h>
#include "myMatrix.h"
#include "Quaternion.h"
#include "Decompose.h"


namespace VisageSDK
{

// normalize a vector
int normalize(float *v)
{
	int i;	//CHANGED BY I.K. declared iterator outside of loop
	float d=0.0;
	for(i=0;i<3;i++)
		d+=v[i]*v[i];
	d = (float)sqrt(d);
	if(d>0.0f)
	{
		for(i=0;i<3;i++)
			v[i]/=d;
		return(1);
	}
	else
		return(0);

}

void cross(myVector r,myVector v1,myVector v2)
{
	r[0] = v1[1] * v2[2] - v1[2] * v2[1];
	r[1] = v1[2] * v2[0] - v1[0] * v2[2];
	r[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void quatFromRotMat(float *quat,myMatrix mat)
{
	float T,S,Qw,Qx,Qy,Qz;
	
	T =  mat[0][0] + mat[1][1] + mat[2][2] + 1;
	
	
	//   If the trace of the matrix is greater than zero, then
	//   perform an "instant" calculation.
	if(T>0.0)
	{
		
		S = (float)(0.5 / sqrt(T));
		
		quat[3] = (float)(0.25 / S);
		quat[0] = ( mat[1][2] - mat[2][1] ) * S;
		quat[1] = ( mat[2][0] - mat[0][2] ) * S;
		quat[2] = ( mat[0][1] - mat[1][0] ) * S;
	}
	else
	{
		
		//    If the trace of the matrix is less than or equal to zero
		//    then identify which major diagonal element has the greatest
		//    value.
		
		float max = -1e10;
		int mi=0;
		for(int i=0;i<3;i++)
		{
			if(mat[i][i] >max)
			{
				max = mat[i][i];
				mi = i;
			}
		}
		
		
		//    Depending on this value, calculate the following:
		
		switch(mi)
		{
		case 0:
			S  = (float)sqrt( 1.0 + mat[0][0] - mat[1][1] - mat[2][2] ) * 2;
			
			Qx = (float)(0.5 / S);
			Qy = (mat[1][0] + mat[0][1] ) / S;
			Qz = (mat[2][0] + mat[0][2] ) / S;
			Qw = (mat[2][1] + mat[1][2] ) / S;
			break;
			
		case 1:
			S  = (float)sqrt( 1.0 + mat[1][1] - mat[0][0] - mat[2][2] ) * 2;
			
			Qx = (mat[1][0] + mat[0][1] ) / S;
			Qy = (float)(0.5 / S);
			Qz = (mat[2][1] + mat[1][2] ) / S;
			Qw = (mat[2][0] + mat[0][2] ) / S;
			break;
			
		case 2:
			S  = (float)sqrt( 1.0 + mat[2][2] - mat[0][0] - mat[1][1] ) * 2;
			
			Qx = (mat[2][0] + mat[0][2] ) / S;
			Qy = (mat[2][1] + mat[1][2] ) / S;
			Qz = (float)(0.5 / S);
			Qw = (mat[1][0] + mat[0][1] ) / S;
			break;
		}
		quat[0] = Qx;
		quat[1] = Qy;
		quat[2] = Qz;
		quat[3] = Qw;
		
		
	}
	
}


void rotMatFromQuat(myMatrix r, float *quat)
{
	float X,Y,Z,W;
	int cnt = 0;
	X = quat[cnt++];
	Y = quat[cnt++];
	Z = quat[cnt++];
	W = quat[cnt++];
	
    float xx      = X * X;
    float xy      = X * Y;
    float xz      = X * Z;
    float xw      = X * W;
	
    float yy      = Y * Y;
    float yz      = Y * Z;
    float yw      = Y * W;
	
    float zz      = Z * Z;
    float zw      = Z * W;
	
    r[0][0]  = 1 - 2 * ( yy + zz ); //0
    r[1][0]  =     2 * ( xy - zw ); //1
    r[2][0]  =     2 * ( xz + yw ); //2
	
    r[0][1]  =     2 * ( xy + zw ); //4
    r[1][1]  = 1 - 2 * ( xx + zz ); //5
    r[2][1]  =     2 * ( yz - xw ); //6
	
    r[0][2]  =     2 * ( xz - yw ); //8
    r[1][2]  =     2 * ( yz + xw ); //9
    r[2][2]  = 1 - 2 * ( xx + yy ); //10
}


// convert axis and angle to quaternion 
void aa2quat(float *q,float *axis,float angle)
{
	if(!normalize(axis))
	{
		axis[0]=1;
		axis[1]=0;
		axis[2]=0;
		angle = 0;
	}
    float sin_a = (float)sin( angle / 2 );
    float cos_a = (float)cos( angle / 2 );
	
    q[0]    = axis[0] * sin_a;
    q[1]    = axis[1] * sin_a;
    q[2]    = axis[2] * sin_a;
    q[3]    = cos_a;
}

// convert quaternion to axis and angle
float quat2aa(float *q,float *axis)
{
    float cos_angle  = q[3];
    float angle = (float)acos( cos_angle ) * 2;
    float sin_angle  = (float)sqrt( 1.0 - cos_angle * cos_angle );
	
	
    if ( myfabs( sin_angle ) < 0.0005 )
		sin_angle = 1.0;
	
    axis[0] = q[0] / sin_angle;
    axis[1] = q[1] / sin_angle;
    axis[2] = q[2] / sin_angle;
	
	if(!normalize(axis))
	{
		angle = 0.0f;
		axis[0] = 0.0;
		axis[1] = 0.0;
		axis[2] = 1.0;
	}

	return(angle);
}

void multQuat(float *qr, float *q1, float *q2)
{
	Quaternion Qqr(qr[0],qr[1],qr[2],qr[3]);
	Quaternion Qq1(q1[0],q1[1],q1[2],q1[3]);
	Quaternion Qq2(q2[0],q2[1],q2[2],q2[3]);
	Qqr = Qq1 * Qq2;
	qr[0] = Qqr.x;
	qr[1] = Qqr.y;
	qr[2] = Qqr.z;
	qr[3] = Qqr.w;
}

void multVectMat(myVector r, myVector v, myMatrix m)
{
	int i,j;
	for(i=0;i<4;i++)
	{
		r[i] = 0;
		for(j=0;j<4;j++)
			r[i]+= v[j] * m[j][i];
	}
}

void multVect3Mat3(myVector r, myVector v, myMatrix m)
{
	int i,j;
	for(i=0;i<3;i++)
	{
		r[i] = 0;
		for(j=0;j<3;j++)
			r[i]+= v[j] * m[j][i];
	}
}

void multVect3Mat(float *v, myMatrix m)
{
	int i;
	myVector v1,v2;

	for(i=0;i<3;i++)
		v1[i] = v[i];
	v1[3]= 1.0;
	multVectMat(v2,v1,m);
	for(i=0;i<3;i++)
		v[i] = v2[i];
}

void multVect3Mat(float *r, float *v, myMatrix m)
{
	int i;
	myVector v1,v2;

	for(i=0;i<3;i++)
		v1[i] = v[i];
	v1[3]= 1.0;
	multVectMat(v2,v1,m);
	for(i=0;i<3;i++)
		r[i] = v2[i];
}

int fdiff(float a, float b)
{
	if(myfabs(a-b) < 0.0001)
		return(0);
	else
		return(1);
}

void copyMat(myMatrix m1, myMatrix m2)
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			m1[j][i] = m2[j][i];
	}
}

void identityMat(myMatrix m)
{
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			m[i][j] = 0;
	for(int j = 0;j<4;j++)
		m[j][j] = 1;
}

void multMat(myMatrix r, myMatrix m1, myMatrix m2)
{
	myMatrix r1;
	int i,j;

	for(i=0;i<4;i++)
		multVectMat(r1[i],m1[i],m2);
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			r[i][j] = r1[i][j];
}

float m3_det( myMatrix mat )
{
	float det;
	
	det = mat[0][0] * ( mat[1][1]*mat[2][2] - mat[1][2]*mat[2][1] )
          - mat[1][0] * ( mat[0][1]*mat[2][2] - mat[0][2]*mat[2][1] )
          + mat[2][0] * ( mat[0][1]*mat[1][2] - mat[0][2]*mat[1][1] );
	return( det );
}

int invMat3( myMatrix mr, myMatrix ma )
    {
     float det = m3_det( ma );

     if ( myfabs( det ) < 0.0005 )
       return(0);

     mr[0][0] =  ( ma[1][1]*ma[2][2] - ma[2][1]*ma[1][2] ) / det;
     mr[1][0] = -( ma[1][0]*ma[2][2] - ma[1][2]*ma[2][0] ) / det;
     mr[2][0] =  ( ma[1][0]*ma[2][1] - ma[1][1]*ma[2][0] ) / det;

     mr[0][1] = -( ma[0][1]*ma[2][2] - ma[2][1]*ma[0][2] ) / det;
     mr[1][1] =  ( ma[0][0]*ma[2][2] - ma[0][2]*ma[2][0] ) / det;
     mr[2][1] = -( ma[0][0]*ma[2][1] - ma[0][1]*ma[2][0] ) / det;

     mr[0][2] =  ( ma[0][1]*ma[1][2] - ma[0][2]*ma[1][1] ) / det;
     mr[1][2] = -( ma[0][0]*ma[1][2] - ma[0][2]*ma[1][0] ) / det;
     mr[2][2] =  ( ma[0][0]*ma[1][1] - ma[1][0]*ma[0][1] ) / det;

	 return(1);
  }


void m4_submat( myMatrix mr, myMatrix mb, int i, int j )
{
	int ti, tj, idst, jdst;
	
	for ( ti = 0; ti < 4; ti++ )
	{
        if ( ti < i )
			idst = ti;
        else
			if ( ti > i )
				idst = ti-1;
			for ( tj = 0; tj < 4; tj++ )
			{
				if ( tj < j )
					jdst = tj;
				else
					if ( tj > j )
						jdst = tj-1;
					if ( ti != i && tj != j )
						mb[idst][jdst] = mr[ti][tj];
			}
	}
}

float m4_det( myMatrix mr )
{
	float  det, result = 0, i = 1;
	myMatrix msub3;
	int     n;
	
	for ( n = 0; n < 4; n++, i *= -1 )
	{
        m4_submat( mr, msub3, n,0 );
        det     = m3_det( msub3 );
        result += mr[n][0] * det * i;
	}
	return( result );
}

int invMat(myMatrix mr, myMatrix ma)
{
	float  mdet = m4_det( ma );
	myMatrix mtemp;
	int     i, j, sign;
	
	if ( myfabs( mdet ) < 0.000000005f )
        return( 0 );
	
	for ( i = 0; i < 4; i++ )
        for ( j = 0; j < 4; j++ )
		{
			sign = 1 - ( (i +j) % 2 ) * 2;
			m4_submat( ma, mtemp, i, j );
			mr[j][i] = ( m3_det( mtemp ) * sign ) / mdet;
		}
		return( 1 );
}

float myfabs(float f)
{
	if(f<0.0f)
		return(-f);
	else
		return(f);
}

float vect3Dist(float *v)
{
	float r = 0.0f;

	for(int i =0;i<3;i++)
		r += v[i] * v[i];

	return ((float)sqrt(r));
}

void vect3Diff(float *r, float *v1, float *v2)
{
	for(int i =0;i<3;i++)
		r[i] = v1[i] - v2[i];
}

float vect3Dist(float *v1, float *v2)
{
	float r[3];

	vect3Diff(r,v1,v2);
	return(vect3Dist(r));
}

// decompose the matrix into components:
// t - translation
// r - rotation (axis and angle)
// s - scale
// so - scale orientation (axis and angle)
// 
void getMatrixComponents(myMatrix m,float *t,float *r,float *rq,float *s,float *so,float *soq)
{
	AffineParts parts;

	decomp_affine(m,&parts);

	t[0] = parts.t.x;
	t[1] = parts.t.y;
	t[2] = parts.t.z;

	rq[0] = parts.q.x;
	rq[1] = parts.q.y;
	rq[2] = parts.q.z;
	rq[3] = parts.q.w;
	r[3] = quat2aa(rq,r);

	soq[0] = parts.u.x;
	soq[1] = parts.u.y;
	soq[2] = parts.u.z;
	soq[3] = parts.u.w;
	so[3] = quat2aa(soq,so);

	s[0] = parts.k.x;
	s[1] = parts.k.y;
	s[2] = parts.k.z;
}

// remove non-uniform scaling from a matrix
void uniformMatrix(myMatrix r, myMatrix in)
{
	AffineParts parts;
	float rq[4];


	decomp_affine(in,&parts);

	rq[0] = parts.q.x;
	rq[1] = parts.q.y;
	rq[2] = parts.q.z;
	rq[3] = parts.q.w;

	rotMatFromQuat(r,rq);

	r[3][0] = parts.t.x;
	r[3][1] = parts.t.y;
	r[3][2] = parts.t.z;
}

}
