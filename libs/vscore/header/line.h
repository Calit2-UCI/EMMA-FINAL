#ifndef __LINE
#define __LINE

#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertexplus/component.h>
#include "plane.h"


namespace VisageSDK
{

//this class represent a straight line in the 3d space
//so it doesn't use a rapresentation parametric, but the intersection between two planes
class NPLine
{
public:
	NPLine()
	{
		bp = Point3<float>(0, 0, 0);
		ep = Point3<float>(0, 0, 0);
		calculate();
	}

	NPLine(Point3<float> begin, Point3<float> end)
	{
		bp = Point3<float>(begin.X(), begin.Y(), begin.Z());
		ep = Point3<float>(end.X(), end.Y(), end.Z());
		calculate();
	}

	~NPLine()
	{}
	
	void setBeginPoint(Point3<float> begin)
	{
		bp = Point3<float>(begin.X(), begin.Y(), begin.Z());
		calculate();
	}
	void setEndPoint(Point3<float> end)
	{
		ep = Point3<float>(end.X(), end.Y(), end.Z());
		calculate();
	}

	Point3f getBeginPoint()
	{
		return bp;
	}

	Point3f getEndPoint()
	{
		return ep;
	}

	void normalize()
	{
		float norm = dx*dx + dy*dy + dz*dz;
		norm = sqrt(norm);

		dx = dx / norm;
		dy = dy / norm;
		dz = dz / norm;
	}

	bool getPointOnLineByX(float qx, Point3f* res)
	{
		if(xk)
			return false;
		
		float x = qx;
		float y = bp.Y() + ryx * (x-bp.X());
		float z = bp.Z() + rzx * (x-bp.X());

		*res = Point3f(x, y, z);
		return true;
	}


	bool getPointOnLineByY(float qy, Point3f* res)
	{
		if(yk)
			return false;
		
		float y = qy;
		float x = bp.X() + rxy * (y-bp.Y());
		float z = bp.Z() + rzy * (y-bp.Y());

		*res = Point3f(x, y, z);
		return true;
	}

	bool getPointOnLineByZ(float qz, Point3f* res)
	{
		if(zk)
			return false;
		
		float z = qz;
		float x = bp.X() + rxz * (z-bp.Z());
		float y = bp.Y() + ryz * (z-bp.Z());

		*res = Point3f(x, y, z);
		return true;
	}

	bool getPointOnLineByT(float t, Point3f* res)
	{
		float z = bp.Z() + t * dz;
		float y = bp.Y() + t * dy;
		float x = bp.X() + t * dx;

		*res = Point3f(x, y, z);
		return true;
	}

	
	bool isOnLine(Point3f punto)
	{
		float x0 = bp.X();
		float y0 = bp.Y();
		float z0 = bp.Z();

		//they will store the coordinates of the point
		float x=punto.X(), y=punto.Y(), z=punto.Z();

		//the system to calculate the intersection point will be
		// 1)  ax + by + cz + d = 0
		// 2)  mx + n = y
		// 3)  qx + p = z

		//Point3f inters = Point3f();

		if(xk && yk && zk)
		{
			return x == X && y == Y && z == Z;
		}
		if(xk && yk)
		{
			return x == X && y == Y;
		}
		if(xk && zk)
		{
			return x == X && z == Z;
		}
		if(yk && zk)
		{
			return z == Z && y == Y;
		}
		if(xk)
		{
			Point3f p;
			bool res = getPointOnLineByY(y, &p);
			return (res && p.X() == x && p.Y() == y && p.Z() == z);
		}
		else
		{
				Point3f p;
				bool res = getPointOnLineByX(x, &p);
				return (res && p.X() == x && p.Y() == y && p.Z() == z);
		}
		
	}

	Point3f getByT(float t)
	{
		Point3f res = Point3f(bp.X()+t*dx, bp.Y()+t*dy, bp.Z()+t*dz);
		return res;
	}


	bool getT(Point3f q, float* t)
	{
		if(isOnLine(q))
		{
			if(!xk)
			{
				*t = (q.X()-bp.X())/dx;
				return true;
			}
			if(!yk)
			{
				*t = (q.Y()-bp.Y())/dy;
				return true;
			}
			if(!zk)
			{
				*t = (q.Z()-bp.Z())/dz;
				return true;
			}
			else
			{
				if(q.X() == X && q.Y() == Y && 	q.Z() == Z)
				{
					*t = 0;
					return true;
				}
				return false;
			}
		}
		return false;
	}

	bool intersectionPlane(Plane plane, Point3f* inters)
	{
		float m, n, q, p;
		float a = plane.getCoeffX();
		float b = plane.getCoeffY();
		float c = plane.getCoeffZ();
		float d = plane.getKnownValue();

		float x0 = bp.X();
		float y0 = bp.Y();
		float z0 = bp.Z();

		//they will store the coordinates of the point
		float x, y, z;

		//the system to calculate the intersection point will be
		// 1)  ax + by + cz + d = 0
		// 2)  mx + n = y
		// 3)  qx + p = z

		//Point3f inters = Point3f();

		if(xk && yk && zk)
		{
			//il segmento è solo un punto
			*inters = Point3f(X, Y, Z);
			if(plane.isOnPlane(*inters))
				return true;
			else
				return false;
		}

		//verifico se sia possibile che avere il punto di intersezione... 
		//se il segmento è parallelo al piano allora restituisco null
		float dx = ep.X() - bp.X();
		float dy = ep.Y() - bp.Y();
		float dz = ep.Z() - bp.Z();

		if(abs(dx*a + dy*b + dz*c) <= 0.0000001)//se vero allora il segmento è parallelo al piano e non perdo tempo con le altre cose
			return false;


		if(xk && yk)
		{
			//il segmento è parallelo all'asse z
			bool res = plane.getPointOnPlaneByXY(X, Y, inters);
			return res;
		}
		if(xk && zk)
		{
			//il segmento è parallelo all'asse y
			bool res = plane.getPointOnPlaneByXZ(X, Z, inters);
			return res;
		}
		if(yk && zk)
		{
			//il segmento è parallelo all'asse x
			bool res = plane.getPointOnPlaneByYZ(Y, Z, inters);
			return res;
		}
		if(xk)
		{
			m = 0;
			n = x0;
			q = rzy; 
			p = z0 - q*y0;

			y = -(d+c*p+a*n)/(a*m+b+c*q);
			x = m*y + n;
			z = q*y + p;
		}
		else
		{
			if(yk)
			{
				m = rxz; 
				n = x0 - m*z0;
				q = 0;
				p = y0;

				z = -(a*n + b*p + d)/(a*m + b*q + c);
				x = m*z + n;
				y = q*z + p;
			}
			else
			{
				// sia che z = cost che non lo sia è la stessa cosa
				m = ryx;
				n = y0 - m*x0;
				q = rzx;
				p = z0-q*x0;

				x = -(b*n + c*p + d)/(a + b*m + c*q);
				y = m*x + n;
				z = q*x + p;
			}
		}
		
		*inters = Point3f(x, y, z);
		return (plane.isOnPlane(*inters));
	}

	float getDirectionX()
	{
		return dx;
	}
	float getDirectionY()
	{
		return dy;
	}
	float getDirectionZ()
	{
		return dz;
	}
private:
	Point3<float> bp;  //begin Point 
	Point3<float> ep;  //end point

	//slopes
	float ryx,  rzx,  rzy,  rxy,  rxz,  ryz;
	float dx, dy, dz;
	bool xk, yk, zk;

	//these variables are just the constant values (if there are)
	float X, Y, Z;

	/*
	FIRST PLANE
	y = bp.y + ryx * (x-bp.x)

	SECOND PLANE
	z = bp.z + rzx * (x-bp.x)

	to calculate the x-value we'll use a THIRD PLANE redundant
	x = bp.x + (1/ryx)*(y-bp.y)

	WARNING!!!
	what happen if the line lie on the plane (X = cost)?
		--> dx = 0 --> ryx = rzx = NAN
	solution: i'll use a second representation... with these two planes


	FIRST PLANE (safety reserve)
	z = bp.z + rzy * (y-bp.y)

	to calculate the x-value we'll use a THIRD PLANE redundant
	x = bp.x (i know that is constant)
	
	WARNING!!!
	what happen if the line lie also on the plane (Y = cost)?
		--> dy = 0 --> rzy = NAN
	solution: now I know that the line is (bp.X, bp.Y, whatever Z).
	*/
	
	void calculate()
	{
		dx = ep.X()-bp.X();
		dy = ep.Y()-bp.Y();
		dz = ep.Z()-bp.Z();

		if(dx!=0)
		{
			ryx = dy/dx;
			rzx = dz/dx;
			xk = false;
		}
		else
		{
			xk = true;
			X = ep.X();
		}

		if(dy!=0)
		{
			rzy = dz/dy;
			rxy = dx/dy;
			yk = false;
		}
		else
		{
			yk = true;
			Y = ep.Y();
		}

		if(dz!=0)
		{
			rxz = dx/dz;
			ryz = dy/dz;
			zk = false;
		}
		else
		{
			zk = true;
			Z = ep.Z();
		}
	}


};

/*
//this class manage a Line using a parametric representation
class Line
{
public:
	Line()
	{
		bp = Point3<float>(0, 0, 0);
		ep = Point3<float>(0, 0, 0);
		calculate();
	}

	Line(Point3<float> begin, Point3<float> end)
	{
		bp = Point3<float>(begin.X(), begin.Y(), begin.Z());
		ep = Point3<float>(end.X(), end.Y(), end.Z());
		calculate();
	}

	~Line()
	{}
	
	void setBeginPoint(Point3<float> begin)
	{
		bp = Point3<float>(begin.X(), begin.Y(), begin.Z());
		calculate();
	}
	void setEndPoint(Point3<float> end)
	{
		ep = Point3<float>(end.X(), end.Y(), end.Z());
		calculate();
	}

	Point3f getBeginPoint()
	{
		return bp;
	}

	Point3f getEndPoint()
	{
		return ep;
	}

	Point3f getPointOnLineByX(float qx)
	{
		float t = (qx-bp.X()) / dx;

		float x = qx; 
		float y = bp.Y() + t*dy;
		float z = bp.Z() + t*dz;
		Point3f ret = Point3f(x, y, z);
		return ret;
	}


	Point3f getPointOnLineByY(float qy)
	{
		float t = (qy-bp.Y()) / dy;

		float y = qy; 
		float x = bp.X() + t*dx;
		float z = bp.Z() + t*dz;
		Point3f ret = Point3f(x, y, z);
		return ret;
	}

	Point3f getPointOnLineByZ(float qz)
	{
		float t = (qz-bp.Z()) / dz;

		float z = qz; 
		float y = bp.Y() + t*dy;
		float x = bp.X() + t*dx;
		Point3f ret = Point3f(x, y, z);
		return ret;
	}

	Point3f getPointOnLineByT(float t)
	{
		float y = bp.Y() + t*dy;
		float x = bp.X() + t*dx;
		float z = bp.Z() + t*dz;
		Point3f ret = Point3f(x, y, z);
		return ret;
	}
private:
	Point3<float> bp;  //begin Point 
	Point3<float> ep;  //end point

	//slopes
	float dx, dy, dz;

	void calculate()
	{
		dx = ep.X()-bp.X();
		dy = ep.Y()-bp.Y();
		dz = ep.Z()-bp.Z();
	}
};
*/

}

#endif
