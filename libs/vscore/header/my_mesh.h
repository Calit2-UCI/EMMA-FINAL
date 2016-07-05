#ifndef __MYMESH
#define __MYMESH
/****************************************************************************
* IDOLib                                                                    *
* Interactive Deformable Objects Library																			*
*	http://idolib.sf.net																											*	
*																																						*
* Copyright(C) 2005																												  *
* Visual Computing Lab                                                      *
* ISTI - Italian National Research Council                                  *
*                                                                           *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
  History

$Log: my_mesh.h,v $
Revision 1.6  2007/05/02 08:55:49  ganovelli
changed mesh definition to vplus tehnology

****************************************************************************/

// stuff to define the mesh
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/create/platonic.h>

#include <vcg/complex/trimesh/refine.h>
#include <vcg/complex/trimesh/base.h>

//#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/import_obj.h>
#include <wrap/io_trimesh/export_obj.h>

#include <wrap/gl/trimesh.h>

#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertexplus/component.h>
#include <vcg/simplex/faceplus/base.h>
#include <vcg/simplex/faceplus/component.h>

#include <vcg/space/point3.h>
#include "image.h"
#include <string>

using namespace vcg;
using namespace std;


namespace VisageSDK
{

class DummyEdge;
class MyFace;


class MyVertex: public vcg::VertexSimp2<float, DummyEdge, MyFace,vcg::vert::Coord3f,vcg::vert::Mark,vcg::vert::Normal3f, vcg::vert::TexCoord2f>
{
public:
	
	MyVertex()
	{
		for(int i=0; i<6; i++)
			weight[i] = 0;
	}

	~MyVertex()
	{
	
	}


	float weight[6];
	Point2f point[6];
};


class MyFace: public vcg::FaceSimp2< MyVertex, DummyEdge, MyFace,vcg::face::VertexRef, vcg::face::Mark, vcg::vert::Normal3f>
{
	private:
		int faceId;

	public:
		int getIdFace()
		{
			return faceId;
		}

		void setFaceId(int id)
		{
			faceId = id;
		}

};

class MyMesh: public vcg::tri::TriMesh<std::vector<MyVertex>,std::vector<MyFace> >
{
public:
	int texture[6];
	MyMesh(){		Init();	}

	~MyMesh(){}

	void Init()
	{
		//int loadMask;
		Clear();
		vcg::tri::Hexahedron<MyMesh>(*this);
		Update();
	}

	void Charge(string nameFile)
	{
		int loadMask;
		Clear();
		vcg::tri::io::ImporterOBJ<MeshType>::Open(*this, nameFile.c_str(), loadMask);
		Update();
	}

	void Update()
	{
		vcg::tri::UpdateBounding<MyMesh>::Box(*this);
		vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalized(*this);
	}

	void Save(string filename)
	{
		const int mask = 0;
		//vcg::tri::io::Exporter<MyMesh>::Save(*this, app.c_str());
		vcg::tri::io::ExporterOBJ<MyMesh>::Save(*this,filename.c_str(),mask);
	}


private:

	float imageX, imageY;
};





class TexCoord5f {

private:
	static const int NMAX = 5;
	Point2<float>  _t[5];	
		
public: 
    void setU(const int i, float u)      
	{ 
		//assert(i<0 || i>NMAX);
		if(i<0 || i>NMAX)
		{
			printf("\nerrore: 0 < (i= %d) < %d",i, NMAX);   
		}
		_t[i][0] = u; 
	}

	void setV(const int i, float v)      
	{ 
		//assert(i<0 || i>NMAX);
		if(i<0 || i>NMAX)
		{
			printf("\nerrore: 0 < (i= %d) < %d",i, NMAX);   
		}
		_t[i][1] = v; 
	}

	void  set(const int i, float u, float v) 
	{ 
		//assert(i<0 || i>NMAX);
		if(i<0 || i>NMAX)
		{
			printf("\nerrore: 0 < (i= %d) < %d",i, NMAX);   
		}
		_t[i][0] = u; 
		_t[i][1] = v;
	}

	inline float & U(const int i) 
	{
		//assert(i<0 || i>NMAX);
		if(i<0 || i>NMAX)
		{
			printf("\nerrore: 0 < (i= %d) < %d",i, NMAX);   
		}
		return _t[i][0]; 
	}

	inline float & V(const int i) 
	{ 
		//assert(i<0 || i>NMAX);
		if(i<0 || i>NMAX)
		{
			printf("\nerrore: 0 < (i= %d) < %d",i, NMAX);   
		}
		return _t[i][1]; 
	}
};

class TexCoord3Df {

private:
	Point3<float>  _t;	
		
public: 
    void setU(float u) {_t[0] = u; }
	void setV(float v) {_t[1] = v; }
	void setW(float w) {_t[1] = w; }
	void  set(float u, float v, float w) 
	{ 
		_t[0] = u; 
		_t[1] = v;
		_t[2] = w;
	}

	inline float & U() {	return _t[0]; }
	inline float & V() {	return _t[1]; }
};

template <class TT> class MyTexCoord2d: public vcg::vert::TexCoord<TexCoord5f, TT> {
};
template <class TT> class MyTexCoord3d: public vcg::vert::TexCoord<TexCoord3Df, TT> {
};

class MyVertexCube: public vcg::VertexSimp2<float, DummyEdge, MyFace,vcg::vert::Coord3f,vcg::vert::Mark,vcg::vert::Normal3f, MyTexCoord2d>{};
class MyFaceCube: public vcg::FaceSimp2< MyVertexCube, DummyEdge, MyFaceCube,vcg::face::VertexRef, vcg::face::WedgeTexCoord2f >
{
	private:
		int faceId;

	public:
		int getIdFace()
		{
			return faceId;
		}

		void setFaceId(int id)
		{
			faceId = id;
		}

};

class MyMeshCube: public vcg::tri::TriMesh<std::vector<MyVertexCube>,std::vector<MyFaceCube> >
{
public:
	int texture[5];
	MyMeshCube(){		Init();	}

	~MyMeshCube(){}

	void Init()
	{
		//int loadMask;
		Clear();
		vcg::tri::Hexahedron<MyMeshCube>(*this);
		Update();
		setTexCoord();
		setTextureOnFace();
	}

	void Update()
	{
		vcg::tri::UpdateBounding<MyMeshCube>::Box(*this);
		vcg::tri::UpdateNormals<MyMeshCube>::PerVertexNormalized(*this);
	}

	void Save()
	{
		string app = string("c:\\prova.obj");
		const int mask = 0;
		//vcg::tri::io::Exporter<MyMesh>::Save(*this, app.c_str());
		vcg::tri::io::ExporterOBJ<MyMeshCube>::Save(*this,app.c_str(),mask);
	}






private:
	
	
	//GlTrimesh<MyMesh> glt;
	float imageX, imageY;

	void setTexCoord()
	{
	  VertexIterator vi;



	  for(vi = vert.begin(); vi != vert.end(); ++vi) 
	  {
		  float x = vi->P().X();
		  float y = vi->P().Y();
		  float z = vi->P().Z();	
		  if(vi->P().Z() == 1)
  		  {
			if(vi->P().Y() == 1)
			{
				if(vi->P().X() == 1)
				{
					
					vi->T().set(0, 1, 0);
					vi->T().set(1, 0, 0);
					vi->T().set(4, 1, 1);
				}
				else
				{
					vi->T().set(0, 0, 0);
					vi->T().set(2, 1, 0);
					vi->T().set(4, 0, 1);
				}
			}
			else
			{
				if(vi->P().X() == 1)
				{
					vi->T().set(0, 1, 1);
					vi->T().set(1, 0, 1);
				}
				else
				{
					vi->T().set(0, 0, 1);
					vi->T().set(2, 1, 1);
				}
			}
		}
		else
		{
			if(vi->P().Y() == 1)
			{
				if(vi->P().X() == 1)
				{
					vi->T().set(1, 1, 0);
					vi->T().set(3, 0, 0);
					vi->T().set(4, 1, 0);
				}
				else
				{
					vi->T().set(2, 0, 0);
					vi->T().set(3, 1, 0);
					vi->T().set(4, 0, 0);
				}
			}
			else
			{
				if(vi->P().X() == 1)
				{
					vi->T().set(1, 1, 1);
					vi->T().set(3, 0, 1);
				}
				else
				{
					vi->T().set(2, 0, 1);
					vi->T().set(3, 1, 1);
				}
			}
		}
	  }
	}
	void setTextureOnFace()
	{
		
		//mettiamo nelle facce delle mesh(triangoli) un numero che identifica a quale 
		//faccia del cubo(quadrati) appartiene la prima.
		for(FaceIterator fi = face.begin(); fi!=face.end(); fi++)
		{
			//vedo se è una faccia frontale
			if(fi->V(0)->P().Z() == 1 && fi->V(1)->P().Z() == 1 && fi->V(2)->P().Z() == 1)
			{
				//fi->WT(0).setN(0);
				fi->setFaceId(0);
			}

			//vedo se è una faccia posteriore
			if(fi->V(0)->P().Z() == -1 && fi->V(1)->P().Z() == -1 && fi->V(2)->P().Z() == -1)
			{
				//fi->WT(0).setN(3);
				fi->setFaceId(3);
			}

			//vedo se è una faccia sinistra
			if(fi->V(0)->P().X() == 1 && fi->V(1)->P().X() == 1 && fi->V(2)->P().X() == 1)
			{
				//fi->WT(0).setN(1);
				fi->setFaceId(1);
			}

			//vedo se è una faccia destra
			if(fi->V(0)->P().X() == -1 && fi->V(1)->P().X() == -1 && fi->V(2)->P().X() == -1)
			{
				//fi->WT(0).setN(2);
				fi->setFaceId(2);
			}

			//vedo se è una faccia superiore
			if(fi->V(0)->P().Y() == 1 && fi->V(1)->P().Y() == 1 && fi->V(2)->P().Y() == 1)
			{
				//fi->WT(0).setN(4);
				fi->setFaceId(4);
			}
			//se è una faccia inferiore non me ne curo
			if(fi->V(0)->P().Y() == -1 && fi->V(1)->P().Y() == -1 && fi->V(2)->P().Y() == -1)
			{
				//fi->WT(0).setN(-1);
				fi->setFaceId(-1);
			}
		}
	}

};

}

#endif