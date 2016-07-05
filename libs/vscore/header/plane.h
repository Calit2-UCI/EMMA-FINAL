#ifndef __PLANE
#define __PLANE

#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertexplus/component.h>


namespace VisageSDK
{

//this class represent a plane in the 3d space
class Plane
{
public:
	Plane()
	{}

	Plane(Point3f p0, Point3f p1, Point3f p2)
	{
		v[0] = Point3f(p0.X(), p0.Y(), p0.Z());;
		v[1] = Point3f(p1.X(), p1.Y(), p1.Z());
		v[2] = Point3f(p2.X(), p2.Y(), p2.Z());
		calculate();
	}

	~Plane()
	{}

	Point3f getVertex(int i)
	{
		return v[i];
	}

	boolean isOnPlane(Point3f p)
	{
		return (p.X()*a + p.Y() * b + p.Z() * c + d ) <= 0.000001;
	}

	bool getPointOnPlaneByXY(float x, float y, Point3f* res)
	{
		if(c==0)
			return false;
		float z = (-a*x - b*y -d)/c;
		*res = Point3f(x, y, z);
		return true;
	}

	bool getPointOnPlaneByXZ(float x, float z, Point3f* res)
	{
		if(b==0)
			return NULL;
		float y = (-a*x - c*z -d)/b;
		*res = Point3f(x, y, z);
		return true;
	}
	
	bool getPointOnPlaneByYZ(float y, float z, Point3f* res)
	{
		if(a == 0)
			return NULL;
		float x = (-c*z- b*y -d)/a;
		*res = Point3f(x, y, z);
		return true;
	}

	float getCoeffX()     {return a;}
	float getCoeffY()     {return b;}
	float getCoeffZ()     {return c;}
	float getKnownValue() {return d;}

private:
	Point3f v[3];

	float a, b, c, d;
	
	void calculate()
	{
		
		float d21x = v[1].X() - v[0].X();
		float d21y = v[1].Y() - v[0].Y();
		float d21z = v[1].Z() - v[0].Z();
		
		float d31x = v[2].X() - v[0].X();
		float d31y = v[2].Y() - v[0].Y();
		float d31z = v[2].Z() - v[0].Z();
		
		a = d21y*d31z - d31y*d21z;
		b = d31x*d21z - d21x*d31z;
		c = d21x*d31y - d31x*d21y;
		d = -a*v[0].X() - b*v[0].Y() - c*v[0].Z();
		
	}
};

}

#endif
