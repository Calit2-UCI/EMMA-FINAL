;//!
//!
//! File: GLVertexBuffer.cpp
//! Desc: Class that simplifies managing OpenGL VBO
//! Author: Enrique Oriol
//! E-mail: enriqueoriol@naradarobotics.com
//!
//! Owner: Narada Robotics
//!
//! Data: 13/09/2010
//!
//!


#include "GLVertexBuffer.hpp"





namespace VisageSDK
{
		
	
	GLVertexBuffer::GLVertexBuffer(
					VRMLMaterial *material,
				   float *coord,
				   int coordLength,
				   int *coordInd,
				   int coordIndLength,
				   float *norm,
				   int normLength,
				   int *normInd,
				   int normIndLength,
				   float *tcoord,
				   int tcoordLength, 
				   int *tcoordInd, 
				   int tcoordIndLength, 
				   float *cpv,
				   int cpvLength, 
				   int *cpvInd, 
				   int cpvIndLength,
				   int NPV,
				   int CPV
				   )
	{
		if(material)
		{
			mat = true;

			if (material->cubemapNames[0] != NULL)
				cubeMap = true;
			else
				cubeMap = false;

		}
		else
		{
			mat = false;
			cubeMap = false;
		}
		
		
		
		coordPtr = coord;
		normalPtr = norm;
		cpvPtr = cpv;
		texCoordPtr = tcoord;
		
		coordIndPtr = coordInd;
		normalIndPtr = normInd;
		cpvIndPtr = cpvInd;
		texCoordIndPtr = tcoordInd;
		
		coordPtrLength = coordLength;
		normalPtrLength = normLength;
		cpvPtrLength = cpvLength;
		texCoordPtrLength = tcoordLength;
		
		coordIndPtrLength = coordIndLength;
		normalIndPtrLength = normIndLength;
		cpvIndPtrLength = cpvIndLength;
		texCoordIndPtrLength = tcoordIndLength;
		

		coordIndex = 0;
		normalIndex = 0;
		colorIndex = 0;
		textureIndex = 0;
		
		
		this->NPV = NPV;
		this->CPV = CPV;
		

		//coordinates data size
		vertex_data_size = 3* sizeof(float);
		buffer_data_size =  coordIndLength * 3 * sizeof(float);
		
		//normals data size
		vertex_data_size += 3* sizeof(float);
		buffer_data_size +=  coordIndLength * 3 * sizeof(float);
	
		//color data size
		if(cpv)
		{
			vertex_data_size += 3* sizeof(float);
			buffer_data_size += cpvIndLength*3*sizeof(float);
		}
		//texture data size
		if(tcoord)
		{
			if (cubeMap)
			{
				vertex_data_size += 3* sizeof(float);
				buffer_data_size += tcoordIndLength*3*sizeof(float);
			}
			else
			{
				vertex_data_size += 2* sizeof(float);
				buffer_data_size += tcoordIndLength*2*sizeof(float);
			}
		}

		
	}
	

	GLsizeiptr GLVertexBuffer::get_VBO_buffer_size()
	{
		return buffer_data_size;
	}
	
	

	bool GLVertexBuffer::has_next()
	{
		if(coordIndex < coordIndPtrLength)
			return true;
		else
			return false;
	}
	
	

	bool GLVertexBuffer::check_colors()
	{
		if(cpvPtr)
			return true;
		else 
			return false;

	}
	
	

	bool GLVertexBuffer::check_texCoord()
	{
		if(texCoordPtr)
			return true;
		else 
			return false;
		
	}
	
	

	
	float* GLVertexBuffer::get_coord()
	{
		int actualCoordinate = coordIndPtr[coordIndex] * 3;
		assert(actualCoordinate <= coordPtrLength);
	
		coordIndex++;
		assert(coordIndex <= coordIndPtrLength);
		

		return &coordPtr[actualCoordinate];

	}
		
	
	

	float* GLVertexBuffer::get_normal()
	{
		int actualNormal = normalIndPtr[normalIndex] * 3;
		assert(actualNormal <= normalPtrLength);
		
		//Normalize normals!!!
/*		float normalLength = sqrt(normalPtr[actualNormal]*normalPtr[actualNormal] + normalPtr[actualNormal+1]*normalPtr[actualNormal+1] + normalPtr[actualNormal+2]*normalPtr[actualNormal+2]);
		float normalizedNormal[3];
		normalizedNormal[0] = normalPtr[actualNormal] / normalLength;
		normalizedNormal[1] = normalPtr[actualNormal+1] / normalLength;
		normalizedNormal[2] = normalPtr[actualNormal+2] / normalLength;
*/		
		// Normal per vertex or per face?
		if(NPV)
			normalIndex++;
		else
			if(((coordIndex) % 3) == 0) normalIndex++;
		assert(normalIndex <= normalIndPtrLength);
		
//		return normalizedNormal;
		return &normalPtr[actualNormal];
	}
	
	
	

	float* GLVertexBuffer::get_texCoord()
	{
		// Input texture, if any
		if(texCoordPtr)
		{
			int actualTexture;
			if (cubeMap)
				actualTexture = texCoordIndPtr[textureIndex] * 3;

			else
				actualTexture = texCoordIndPtr[textureIndex] * 2;

			assert(actualTexture <= texCoordPtrLength);			
			
			textureIndex++;
			assert(textureIndex <= texCoordIndPtrLength);
			
			return &texCoordPtr[actualTexture];
		}
		else
			return texCoordPtr;
	  return 0;
	}
	
	
	
	float* GLVertexBuffer::get_cpv()
	{
		// Input color
		if(cpvPtr)
		{
			int actualColor = cpvIndPtr[colorIndex] * 3;
			assert(actualColor <= cpvPtrLength);
			
			// glColorMaterial seems only option to use per-vertex material with VBO, but does not exist in OGL_ES
			//if(mat)
			//{
			//	glEnable(GL_COLOR_MATERIAL);
			//	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
			//}
			
			
			// Is there a color per vertex?
			if(CPV)
				colorIndex++;
			else
				if(((coordIndex) % 3) == 0) colorIndex++;
			
//			assert(colorIndex <= cpvIndPtrLength);
			if(colorIndex > cpvIndPtrLength)
				std::cout << "color Index= " << colorIndex << " VS cpvIndPtrLength " << cpvIndPtrLength << std::endl;
			
			return &cpvPtr[actualColor];
		}
		else
			
			return cpvPtr;
		
	}
	

	
	int GLVertexBuffer::get_num_of_vertex()
	{
		return coordIndPtrLength;
	}
	

	int GLVertexBuffer::get_coord_size()
	{
		return 3* sizeof(float);
	}
	
	

	int GLVertexBuffer::get_normal_size()
	{
		return 3*sizeof(float);
	}
	
	

	int GLVertexBuffer::get_texCoord_size()
	{
		
		if(texCoordPtr)
		{
			if (cubeMap)
				return 3*sizeof(float);
			
			else
				return 2*sizeof(float);
		}
		return 0;
	}
	
	

	int GLVertexBuffer::get_cpv_size()
	{
		if(cpvPtr)
			return 3*sizeof(float);
		else
			return 0;
		
	}
	

	
	int GLVertexBuffer::get_vertex_size()
	{
		return vertex_data_size;
	}
	
	
	

	GLvoid * GLVertexBuffer::get_coord_offset()
	{
		return (GLvoid*)((char*)NULL);
		
	}
	

	
	GLvoid * GLVertexBuffer::get_normal_offset()
	{
		return (GLvoid*)((char*)NULL+3*sizeof(float));
	}
	
	

	GLvoid * GLVertexBuffer::get_cpv_offset()
	{
		return (GLvoid*)((char*)NULL+3*sizeof(float)+3*sizeof(float));
		return (GLvoid*)((char*)NULL+3*sizeof(float));
	}
	
	

	GLvoid * GLVertexBuffer::get_texCoord_offset()
	{
		if(cpvPtr)
			return (GLvoid*)((char*)NULL+3*sizeof(float)+3*sizeof(float)+3*sizeof(float));
		else 
			return (GLvoid*)((char*)NULL+3*sizeof(float)+3*sizeof(float));
	
	}

	
	

	unsigned char GLVertexBuffer::get_coord_comp()
	{
		return 3;
	}
	
	
	/*not used because glNormalPointer don't need it*/
	unsigned char GLVertexBuffer::get_normal_comp()
	{
		return 3;
	}
	

	
	unsigned char GLVertexBuffer::get_texCoord_comp()
	{
		if(texCoordPtr)
		{
			if (cubeMap)
				return 3;
			
			else
				return 2;
		}
		else
			return 0;
	}
	
	

	unsigned char GLVertexBuffer::get_cpv_comp()
	{
		if(cpvPtr)
			return 3;
		else 
			return 0;

	}
	
	

	//NOT USED!!!
	
	int GLVertexBuffer::get_kindof_coord()
	{
		return GL_FLOAT;
	}
	
	

	int GLVertexBuffer::get_kindof_normal()
	{
		return GL_FLOAT;
	}
	
	

	int GLVertexBuffer::get_kindof_texCoord()
	{
		return GL_FLOAT;
	}
	
	

	int GLVertexBuffer::get_kindof_cpv()
	{
		return GL_FLOAT;
	}

	
	
	
}
