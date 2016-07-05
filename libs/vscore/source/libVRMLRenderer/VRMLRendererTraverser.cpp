// VRMLRendererTraverser.cpp: implementation of the VRMLRendererTraverser class.
//
//////////////////////////////////////////////////////////////////////

// Disable annyoing debug-compile error caused by STL.
// This is safe.
#pragma warning(disable : 4786)

#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

//#include "SurfaceData.h"

#ifdef WIN32
	#include <GL/glew.h>
	#define GL_GLEXT_PROTOTYPES
	#include <gl\glaux.h>
	#include <gl/glext.h>
	#include <direct.h>
	#ifdef GLEW_VERSION_1_5_IS_NOT_SUPPORTED //address the standard extensions to glew ARB extensions
		#define glGenBuffers GLEW_GET_FUN(__glewGenBuffersARB)
		#define glBindBuffer GLEW_GET_FUN(__glewBindBufferARB)
		#define glBufferData GLEW_GET_FUN(__glewBufferDataARB)
		#define glBufferSubData GLEW_GET_FUN(__glewBufferSubDataARB)
		#define glDeleteBuffers GLEW_GET_FUN(__glewDeleteBuffersARB)
		#define glGetBufferParameteriv GLEW_GET_FUN(__glewGetBufferParameterivARB)
		#define glGetBufferPointerv GLEW_GET_FUN(__glewGetBufferPointervARB)
		#define glGetBufferSubData GLEW_GET_FUN(__glewGetBufferSubDataARB)
		#define glIsBuffer GLEW_GET_FUN(__glewIsBufferARB)
		#define glMapBuffer GLEW_GET_FUN(__glewMapBufferARB)
		#define glUnmapBuffer GLEW_GET_FUN(__glewUnmapBufferARB)
	#endif
#elif defined(LINUX)
	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glx.h>
	#include <GL/glext.h>
	//#include <GL/glut.h>
	//#include <GL/freeglut_ext.h>
	#include <unistd.h>
#elif defined(MAC)
	#include <OpenGL/gl.h>
	#include <GLUT/glut.h>
	#include <OpenGL/glext.h>
	#include <unistd.h>
#elif defined(IPHONE)
	#include <OpenGLES/ES1/gl.h>
	#include <OpenGLES/ES1/glext.h>
	#import <UIKit/UIKit.h>
//	#include "glues.h"
	#include <unistd.h>
#endif


#include <assert.h>


#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <cmath>



#include "VRMLRendererTraverser.h"
#include "GLVertexBuffer.hpp"






// Added by KStankovic
/* In case your <GL/gl.h> does not advertise EXT_texture_cube_map... */
#ifndef GL_EXT_texture_cube_map
# define GL_NORMAL_MAP_EXT                   0x8511
# define GL_REFLECTION_MAP_EXT               0x8512
# define GL_TEXTURE_CUBE_MAP_EXT             0x8513
# define GL_TEXTURE_BINDING_CUBE_MAP_EXT     0x8514
# define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT  0x8515
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT  0x8516
# define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT  0x8517
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518
# define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A
# define GL_PROXY_TEXTURE_CUBE_MAP_EXT       0x851B
# define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT    0x851C
#endif
//

// VC does not define M_PI
//#define M_PI 3.141592653589793238462643383279502884197196


namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VRMLRendererTraverser::VRMLRendererTraverser()
{
	mRenderPart = false;
	isStaticDataInBuffer = false;
	nBuffersCreated = 0;
	nWorkingBuffer = 0;

	mLipsSurface = NULL;

	nLoadedTextures = 0;
	mFlashingPointSize = 6.0f;
}

VRMLRendererTraverser::~VRMLRendererTraverser()
{
	// Unload textures
//	for(map<string, GLuint>::iterator theIt = mLoadedTextures.begin(); theIt != mLoadedTextures.end(); theIt++)
//		glDeleteTextures(1, &(*theIt).second);
	int i;
	for(i = 0; i<nLoadedTextures;i++)
	{
		glDeleteTextures(1, &(loadedTexture[i]));
		free(loadedTextureName[i]);
		std::cout << "delete texture" << std::endl;
	}
	for(i = 0; i<nBuffersCreated; i++)
	{
		glDeleteBuffers(1, &(meshBuffer[i]));
		std::cout << "delete mesh" << std::endl;
	}
}

void VRMLRendererTraverser::SetRenderingIndex(int index)
{
	mRenderShape = index;
	mRenderPart = true;
}




// Changed by KStankovic
int VRMLRendererTraverser::LoadTextures(char *fullPath, char *folderPath, GLenum target)
//
{
	
	// Texture varible, return	
	GLuint texture[1];

#ifndef IPHONE

	// Returns a texture-index if successfully loaded, returns -1 if
	// there was some error.
	char curPath[200];        

	// change to the folder where the model file is, so the textures can be loaded
	if(folderPath[0])
	{
		#ifdef WIN32
			_getcwd(curPath,200); // remember current folder
			_chdir(folderPath);
		#else
			getcwd(curPath,200); // remember current folder
			chdir(folderPath);
		#endif
	}


	FILE *File=NULL;
	File=fopen(fullPath,"r");

	if(!File)
	{
		return -1;
	}
	fclose(File);




	// Changed by KStankovic
	if (target == GL_TEXTURE_2D)
	{
		glGenTextures(1, &texture[0]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		//by KaiK
		glActiveTexture( GL_TEXTURE0 );
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
	}
	else if (target == GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT)
	{
		glGenTextures(1, &texture[0]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, texture[0]);
	}
	//


	Image* my_image = new Image();
	my_image->read_image(fullPath);



	// Load texture into GL.
	int maxSize = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

	// Create a texture of nearest power 2
	int xPowSize = 1;
	int yPowSize = 1;
	while((my_image->get_size_x() / xPowSize) > 0) xPowSize <<= 1;
	while((my_image->get_size_y() / yPowSize) > 0) yPowSize <<= 1;

	// Size to nearest lower power of 2 for speed
	xPowSize >>= 1;
	yPowSize >>= 1;

	// So we're not overriding gfx-cards capabilitys
	if(xPowSize > maxSize) xPowSize = maxSize;
	if(yPowSize > maxSize) yPowSize = maxSize;

	// Allocate memory to the scaled image.
	GLubyte* tempScaleImage = new GLubyte[yPowSize * (xPowSize * my_image->get_n_channels())];


	// Scale original image to nearest lower power of 2.
	switch(my_image->get_n_channels())
	{
		cout << "N channels per image: " << my_image->get_n_channels() << endl;
			
		case 3:
			{
				gluScaleImage(GL_RGB, my_image->get_size_x(), my_image->get_size_y(), GL_UNSIGNED_BYTE, my_image->get_data(), xPowSize, yPowSize, GL_UNSIGNED_BYTE, tempScaleImage);
				// Changed by KStankovic
				glTexImage2D(target, 0, 3, xPowSize, yPowSize, 0, GL_RGB, GL_UNSIGNED_BYTE, tempScaleImage);
				//
			}
			break;
		case 4:
			{
				cout << "4 channels" << endl;
				gluScaleImage(GL_RGBA, my_image->get_size_x(), my_image->get_size_y(), GL_UNSIGNED_BYTE, my_image->get_data(), xPowSize, yPowSize, GL_UNSIGNED_BYTE, tempScaleImage);
				// Changed by KStankovic
				glTexImage2D(target, 0, 4, xPowSize, yPowSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, tempScaleImage);
				//
			}
			break;


		// Safety stuff.
		default:
			cout << "incorrect number of channels: "<< my_image->get_n_channels() << endl;
			return -1;
			break;
	}

	// Clean up
	delete [] tempScaleImage;
	delete my_image;



	// Changed by KStankovic
	if (target == GL_TEXTURE_2D)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
	} else if (target == GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT)
	{
		#ifdef IPHONE
			glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		#else
			glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT,GL_TEXTURE_WRAP_S,GL_CLAMP);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT,GL_TEXTURE_WRAP_T,GL_CLAMP);
		#endif
	}
	//



	// return to the current folder if necessary
	if(folderPath[0])
	{
		#ifdef WIN32
			_chdir(curPath);
		#else
			chdir(curPath);
		#endif
	}
	
	
#else 

	//IPHONE SPECIFIC CODE TO LOAD TEXTURES
	CGImageRef textureImage;
	CGContextRef textureContext;
	

	char buffer[2048];
	
	NSString* readPath = [[NSBundle mainBundle] resourcePath];
	[readPath getCString:buffer maxLength:2048 encoding:NSUTF8StringEncoding];
	strcat(buffer, "/");
	strcat( buffer, fullPath );
	NSString *path = [NSString stringWithUTF8String:buffer];
	

	UIImage *uiImage = [UIImage imageWithContentsOfFile:path];
	if( uiImage ) {
		textureImage = uiImage.CGImage;
		
		int width = CGImageGetWidth(textureImage);
		int height = CGImageGetHeight(textureImage);
		
		GLubyte *textureData;
		
		if(textureImage) {
			
/*			if(width%256!=0)
			{
				
				textureData = (GLubyte *) malloc(512 * 512 * 4);
				textureContext = CGBitmapContextCreate(textureData, 512, 512, 8, width * 4, CGImageGetColorSpace(textureImage), kCGImageAlphaPremultipliedLast);
				CGContextTranslateCTM (textureContext, 0, 512);
				CGContextScaleCTM (textureContext, 1.0, -1.0);
				CGContextScaleCTM(textureContext, 512/width, 512/height);
				CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (float)512, (float)512), textureImage);
				width = 512;
				height = 512;
			}
			else
			{
*/				textureData = (GLubyte *) malloc(width * height * 4);
				textureContext = CGBitmapContextCreate(textureData, width, height, 8, width * 4, CGImageGetColorSpace(textureImage), kCGImageAlphaPremultipliedLast);
				CGContextTranslateCTM (textureContext, 0, height);
				CGContextScaleCTM (textureContext, 1.0, -1.0);
				CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (float)width, (float)height), textureImage);
				
//			}

		}
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture[0]);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		//ONLY FOR IPHONE VERSION >= 2
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

		//ONLY FOR IPHONE VERSION >= 2
		glGenerateMipmap( GL_TEXTURE_2D );

		if(glGetError())
		{
			printf("glTexImage2D failed. ");
			//return 0;
		}
		free(textureData);
		CGContextRelease(textureContext);
	}
	else {
		cout << "cannot load image" << endl;
	}	
	
#endif
	
	
	
	
	// Successfully return the loaded image.
	return texture[0];
}

int VRMLRendererTraverser::traverseTransform(VRMLModel *model,    // pointer to the traversed model
										 VRMLNode *thisNode,  // the node currently being traversed
										 int index,      // index of the transform within the face model
										 float *c,       // Center of rotation expressed as translation (x,y,z)
										 float *negc,    // Negative translation of the center of rotation
										 float *r,       // Rotation expressed as axis and angle in radians (x,y,z,angle)
										 float *rq,      // Rotation expressed as quaternion (x,y,z,w)
										 float *s,       // Scale (x,y,z)
										 float *so,      // Scale orientation (acis and angle)
										 float *soq,     // Scale orientation (quaternion)
										 float *negso,   // Inverse of scale orientation (axis and angle)
										 float *negsoq,  // Inverse of scale orientation (quaternion)
										 float *t,       // Translation (x,y,z)
										 myMatrix m)  // Complete transformation matrix
{

	// Save off old matrix
	glPushMatrix();
	glMultMatrixf((GLfloat *) m);

	return 0;
}

void VRMLRendererTraverser::traverseTransformEnd(VRMLModel *model, VRMLNode *thisNode,int index)
{
	// Get back old GL_MODELVIEW matrix.

	glPopMatrix();
}

void VRMLRendererTraverser::traverseMesh(VRMLModel *model,    // pointer to the traversed model
										 VRMLNode *thisNode,  // the node currently being traversed
										 int index,           // index of the polygon mesh within the face model
										 VRMLMaterial *mat,   // material for this shape
										 float *coord,        // vertex coordinates
										 float *origCoord,    // original vertex coordinates
										 int coordLength,     // length of the vertex coordinates array (number of vertices * 3)
										 int *coordInd,       // vertex indices for triangles
										 int coordIndLength,  // length of coordInd array
										 float *norm,         // normals
										 int normLength,      // length of norm array
										 int *normInd,        // normal indices, per vertex
										 int normIndLength,   // length of normInd array
										 int NPV,              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
										 int normalsCalculated,// If 1, normals are calculated by VRMLModel; if 0, normals were read from the file
										 float creaseAngle,   // VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
										 int solid,           // VRML solid (default 1)
										 int ccw,             // VRML counter-clockwise (default 1)
										 int convex,          // VRML convex (default 1)
										 float *tcoord,       // texture coordinates, if any (else zero)
										 int tcoordLength, 
										 int *tcoordInd, 
										 int tcoordIndLength, 
										 float *cpv,          // colors per vertex, if any (else zero)
										 int cpvLength, 
										 int *cpvInd, 
										 int cpvIndLength,
										 int CPV,           // color per vertex (1) or per face (1)
										 VRMLMeshMT *mt,		// pointer to the list of morph targets for this mesh
										 VRMLMeshBones *bones)            // color per vertex (1) or per face (0)
{
	

	if(mRenderPart)
		if(index != mRenderShape) return;

	if(nBuffersCreated < model->nCoordLists)
	{
		GLVertexBuffer my_VBO_data =  GLVertexBuffer(
											   mat,   // material for this shape
											   coord,        // vertex coordinates
											   coordLength,     // length of the vertex coordinates array (number of vertices * 3)
											   coordInd,       // vertex indices for triangles
											   coordIndLength,  // length of coordInd array
											   norm,         // normals
											   normLength,      // length of norm array
											   normInd,        // normal indices, per vertex
											   normIndLength,   // length of normInd array
											   tcoord,       // texture coordinates, if any (else zero)
											   tcoordLength,
											   tcoordInd,
											   tcoordIndLength,
											   cpv,          // colors per vertex, if any (else zero)
											   cpvLength,
											   cpvInd,
											   cpvIndLength,
											   NPV,              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
											   CPV);

		//generates a VBO
		glGenBuffers(1, &meshBuffer[nBuffersCreated]);
		//link the specified buffer
		glBindBuffer(GL_ARRAY_BUFFER, meshBuffer[nBuffersCreated]);

		glBufferData(GL_ARRAY_BUFFER, my_VBO_data.get_VBO_buffer_size(), 0, GL_DYNAMIC_DRAW);

		//unlink buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		std::cout << "generated buffer number: " << nBuffersCreated << std::endl;

		nBuffersCreated++;
	}
	else if(nWorkingBuffer == nBuffersCreated)
	{
		nWorkingBuffer = 0;

		//first time when nWorkingBuffer = nBuffersCreated, all static data (colors, tex_coord) has been loaded into buffer
		if(!isStaticDataInBuffer)
			isStaticDataInBuffer = true;
	}

	// changed by Ratty
	// don't crash here, use default material instead
	// assert(mat);
	bool destroy_mat = false;
	if( !mat )
	{
		destroy_mat = true;
		mat = new VRMLMaterial();
	}
	
	assert(coord);
	assert(coordInd);
	assert(norm);
	assert(normInd);
	

	//added by Ratty
	if( coordInd == 0 || normInd == 0 )
		return;
	//

	bool is_rgba_texture = false;

	// Traverse the shape and convert VMRL to openGL calls.
	// First set the material.

	// Ambient component
	GLfloat ambient[4] = { mat->ambientIntensity, mat->ambientIntensity, mat->ambientIntensity, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);

	// Diffuse component
	GLfloat diffuseColor[4] = { mat->diffuseColor[0], mat->diffuseColor[1], mat->diffuseColor[2], 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);

	// Emissive component
	GLfloat emissiveColor[4] = { mat->emissiveColor[0], mat->emissiveColor[1], mat->emissiveColor[2], 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveColor);

	// Material shininess
	GLfloat shininess[4] = { mat->shininess, mat->shininess, mat->shininess, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	// Specular component
	GLfloat specularColor[4] = { mat->specularColor[0], mat->specularColor[1], mat->specularColor[2], 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

	// If there is a texture, load it and set up
	GLuint texture[1];
	texture[0] = 0;
	if(tcoord)
	{
		// Added by KStankovic
		bool isCubemap = false;

		if (mat->cubemapNames[0] != NULL) isCubemap = true;
		//


		// Changed by KStankovic
		char *textureURL;

		if (isCubemap) textureURL = mat->cubemapNames[0];
		else textureURL = mat->textureName;

		int i;	//CHANGED BY I.K. declared iterator outside of loop
		for(i =0; i< nLoadedTextures; i++) //identifica la textura que hay que cargar
		{
			if(!strcmp(textureURL,loadedTextureName[i]))
				break;
		}
		//


		if(i == nLoadedTextures)
		{


			if (isCubemap)
			{
				cout << "CUBEMAP TEXTURE" << endl;
				texture[0] = LoadTextures(mat->cubemapNames[0],((VRMLModel*)model)->filePath,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT);

				int j;
				for (j = 1; j < 6; j++)
					LoadTextures(mat->cubemapNames[j],((VRMLModel*)model)->filePath,
						GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + j);
			}
			else
			{
				cout << "NOT CUBEMAP TEXTURE" << endl;
				texture[0] = LoadTextures(mat->textureName,((VRMLModel*)model)->filePath,
					GL_TEXTURE_2D);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture[0]);				// Select Our Texture
			}


			loadedTexture[nLoadedTextures] = texture[0];
			loadedTextureName[nLoadedTextures] = (char*)malloc((strlen(textureURL)+1) * sizeof(char));
			strcpy(loadedTextureName[nLoadedTextures],textureURL);
			nLoadedTextures++;

			if (isCubemap)
				glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, texture[0]);				// Select Our Texture
			else
			{
				glBindTexture(GL_TEXTURE_2D, texture[0]);				// Select Our Texture
			}
			//
		}
		else
		{
			// Was the texture properly loaded?
			if(i<nLoadedTextures)
			{
				texture[0] = loadedTexture[i];
				// Changed by KStankovic
				if (isCubemap)
				{
					glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, texture[0]);				// Select Our Texture
				}
				else
				{
					//by KaiK
					glActiveTexture( GL_TEXTURE0 );
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, texture[0]);				// Select Our Texture
				}
				//
			}		
		}



		// Changed by KStankovic
#ifndef IPHONE//solo para comprobar si tiene 4 canales!!
		// figure out if this texture contains an alpha channel or not
		GLint params;
		if (isCubemap)
			glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT, 0, GL_TEXTURE_COMPONENTS, &params);
		else
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPONENTS, &params);
		//
		if (params == 4)
			is_rgba_texture = true;
#endif
		// Added by KStankovic
		if (isCubemap)
		{
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_TEXTURE_CUBE_MAP_EXT);
		}
		//
	}
	else
	{
		//glBindTexture(GL_TEXTURE_2D, 0);
	}



	// Then start feeding coordinates and normals.
	if(coordIndLength != normIndLength) assert(NULL);

	if(ccw)
		glFrontFace(GL_CCW);
	else
		glFrontFace(GL_CW);


	if (is_rgba_texture)
	{
		//glEnable(GL_CULL_FACE); // Remove Back Face

		glAlphaFunc(GL_GREATER,0.5f); // Set Alpha Testing (disable blending)
		glEnable(GL_ALPHA_TEST); // Enable Alpha Testing (disable blending)
		RenderMesh(
			mat, coord, origCoord, coordLength, coordInd, coordIndLength, norm, normLength, normInd, normIndLength,
			NPV, normalsCalculated, creaseAngle, solid, ccw, convex, tcoord, tcoordLength, tcoordInd, tcoordIndLength, 
			cpv, cpvLength, cpvInd, cpvIndLength, CPV);
		glDisable(GL_ALPHA_TEST),
		glDisable(GL_CULL_FACE); // Remove Back Face		

		glPushMatrix();
//		glScalef(1.01, 1.01, 1.01);	// avoid z-fighting

		// Enable Alpha Blending (disable alpha testing)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND); // Enable Blending (disable alpha testing)
		
		glDepthMask(GL_FALSE);
		RenderMesh(
		mat, coord, origCoord, coordLength, coordInd, coordIndLength, norm, normLength, normInd, normIndLength,
		NPV, normalsCalculated, creaseAngle, solid, ccw, convex, tcoord, tcoordLength, tcoordInd, tcoordIndLength, 
		cpv, cpvLength, cpvInd, cpvIndLength, CPV);	
	}
	
	
	glDepthMask(GL_TRUE);
	RenderMesh(
		mat, coord, origCoord, coordLength, coordInd, coordIndLength, norm, normLength, normInd, normIndLength,
		NPV, normalsCalculated, creaseAngle, solid, ccw, convex, tcoord, tcoordLength, tcoordInd, tcoordIndLength, 
		cpv, cpvLength, cpvInd, cpvIndLength, CPV);


	if (is_rgba_texture)
	{
		glEnable(GL_DEPTH_TEST);

#ifndef IPHONE
		glDisable (GL_POLYGON_STIPPLE);
#endif
		glDepthMask(GL_TRUE);
		glDisable(GL_CULL_FACE); // Remove Back Face		
		glDisable(GL_ALPHA_TEST); // Enable Alpha Testing (disable blending)
		glDisable(GL_BLEND);
		
		glPopMatrix();
	}

  // Changed by KStankovic
  if(tcoord)
  {
		glBindTexture(GL_TEXTURE_2D, 0);

		if (mat->cubemapNames[0] != NULL)
		{
			glDisable(GL_TEXTURE_CUBE_MAP_EXT);
			glEnable(GL_TEXTURE_2D);
		}
  }
  //

  // added by Ratty
  if( destroy_mat )
	  delete mat;



  nWorkingBuffer++;
	
	
	//TEST by KaiK !!!!!!!!
	//nBuffersCreated = 0;
	//nWorkingBuffer = 0;
	//isStaticDataInBuffer = false;
	//glDeleteBuffers(1, &meshBuffer[nBuffersCreated]);
}

	
	/*
	
	void VRMLRendererTraverser::traverseMesh(VRMLModel *model,    // pointer to the traversed model
											 VRMLNode *thisNode,  // the node currently being traversed
											 int index,           // index of the polygon mesh within the face model
											 VRMLMaterial *mat,   // material for this shape
											 float *coord,        // vertex coordinates
											 float *origCoord,    // original vertex coordinates
											 int coordLength,     // length of the vertex coordinates array (number of vertices * 3)
											 int *coordInd,       // vertex indices for triangles
											 int coordIndLength,  // length of coordInd array
											 float *norm,         // normals
											 int normLength,      // length of norm array
											 int *normInd,        // normal indices, per vertex
											 int normIndLength,   // length of normInd array
											 int NPV,              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
											 int normalsCalculated,// If 1, normals are calculated by VRMLModel; if 0, normals were read from the file
											 float creaseAngle,   // VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
											 int solid,           // VRML solid (default 1)
											 int ccw,             // VRML counter-clockwise (default 1)
											 int convex,          // VRML convex (default 1)
											 float *tcoord,       // texture coordinates, if any (else zero)
											 int tcoordLength, 
											 int *tcoordInd, 
											 int tcoordIndLength, 
											 float *cpv,          // colors per vertex, if any (else zero)
											 int cpvLength, 
											 int *cpvInd, 
											 int cpvIndLength,
											 int CPV,           // color per vertex (1) or per face (1)
											 VRMLMeshMT *mt,		// pointer to the list of morph targets for this mesh
											 VRMLMeshBones *bones)            // color per vertex (1) or per face (0)
	{
		
	
		if(mRenderPart)
			if(index != mRenderShape) return;
		bool destroy_mat = false;
		if( !mat )
		{
			destroy_mat = true;
			mat = new VRMLMaterial();
		}
		if( coordInd == 0 || normInd == 0 )
			return;

		
		
		// Ambient component
		GLfloat ambient[4] = { mat->ambientIntensity, mat->ambientIntensity, mat->ambientIntensity, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		
		// Diffuse component
		GLfloat diffuseColor[4] = { mat->diffuseColor[0], mat->diffuseColor[1], mat->diffuseColor[2], 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);
		
		// Emissive component
		GLfloat emissiveColor[4] = { mat->emissiveColor[0], mat->emissiveColor[1], mat->emissiveColor[2], 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveColor);
		
		// Material shininess
		GLfloat shininess[4] = { mat->shininess, mat->shininess, mat->shininess, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		
		// Specular component
		GLfloat specularColor[4] = { mat->specularColor[0], mat->specularColor[1], mat->specularColor[2], 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);
		
	
		
		// If there is a texture, load it and set up
		GLuint texture[1];
		texture[0] = 0;
		if(tcoord)
		{
			char *textureURL;
			textureURL = mat->textureName;
			
			int i;
			for(i =0; (i< nLoadedTextures); i++) //identifica la textura que hay que cargar
			{
				if(!strcmp(textureURL,loadedTextureName[i]))
					break;
			}
			
			
			if((i == nLoadedTextures)) //if texture is loaded for first time
			{
				
				texture[0] = LoadTextures(mat->textureName,((VRMLModel*)model)->filePath, GL_TEXTURE_2D);
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture[0]);				// Select Our Texture

				
				loadedTexture[nLoadedTextures] = texture[0];
				loadedTextureName[nLoadedTextures] = (char*)malloc((strlen(textureURL)+1) * sizeof(char));
				strcpy(loadedTextureName[nLoadedTextures],textureURL);
				nLoadedTextures++;

				
			}
			
			else	//if texture has been loaded previously
			{
				if(i<nLoadedTextures)
				{
					texture[0] = loadedTexture[i];

					glActiveTexture( GL_TEXTURE0 );
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, texture[0]);				// Select Our Texture

				}		
			}
			

		}
		else //if there is no texture
		{
			//by KaiK
			//		glBindTexture(GL_TEXTURE_2D, 0);
		}
		
	
	
		if(coordIndLength != normIndLength) assert(NULL);
		
		if(ccw)
			glFrontFace(GL_CCW);
		else
			glFrontFace(GL_CW);
		
		
		//by Kaik	
		//glDepthMask(GL_FALSE); // esto provoca que se vea el interior en vez de el exterior
		
		glDepthMask(GL_TRUE);
		RenderMesh(
				   mat, coord, origCoord, coordLength, coordInd, coordIndLength, norm, normLength, normInd, normIndLength,
				   NPV, normalsCalculated, creaseAngle, solid, ccw, convex, tcoord, tcoordLength, tcoordInd, tcoordIndLength, 
				   cpv, cpvLength, cpvInd, cpvIndLength, CPV);
		
		// Changed by KStankovic
		if(tcoord)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		//
		
		// added by Ratty
//		if( destroy_mat )
//			delete mat;
	}
	
	
	
	*/
	
	

void VRMLRendererTraverser::RenderMesh(	VRMLMaterial *mat,   // material for this shape
									    float *coord,        // vertex coordinates
										float *origCoord,    // original vertex coordinates
										int coordLength,     // length of the vertex coordinates array (number of vertices * 3)
										int *coordInd,       // vertex indices for triangles
										int coordIndLength,  // length of coordInd array
										float *norm,         // normals
										int normLength,      // length of norm array
										int *normInd,        // normal indices, per vertex
										int normIndLength,   // length of normInd array
										int NPV,              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
										int normalsCalculated,// If 1, normals are calculated by VRMLModel; if 0, normals were read from the file
										float creaseAngle,   // VRML creaseAngle (default 0.0; 1.2 is good value to smooth)
										int solid,           // VRML solid (default 1)
										int ccw,             // VRML counter-clockwise (default 1)
										int convex,          // VRML convex (default 1)
										float *tcoord,       // texture coordinates, if any (else zero)
										int tcoordLength, 
										int *tcoordInd, 
										int tcoordIndLength, 
										float *cpv,          // colors per vertex, if any (else zero)
										int cpvLength, 
										int *cpvInd, 
										int cpvIndLength,
										int CPV           // color per vertex (1) or per face (1)
)
{
	

#ifdef MSVC

	// Local pointers are much faster than stackvalues, therfore use locals.
	int *tempNormInd = normInd;
	float *tempNorm = norm;
	int *tempCpvInd = cpvInd;
	float *tempCpv = cpv;
	// int tempCPV = CPV;
	int *tempCoordInd = coordInd;
	int tempCoordIndLength = coordIndLength;
	float *tempCoord = coord;

	int coordIndex = 0;
	int normalIndex = 0;
	int colorIndex = 0;
	//int notCPVcntr = 0;
	int textureIndex = 0;

	glBegin(GL_TRIANGLES);
		for(int i=0; i<tempCoordIndLength; i++)
		{
			// Input normal and if any color first
			int actualNormal = tempNormInd[normalIndex] * 3;

			assert(actualNormal <= normLength);

			glNormal3fv(&tempNorm[actualNormal]);

			// Normal per vertex or per face?
			if(NPV)
				normalIndex++;
			else
				if(((coordIndex + 1) % 3) == 0) normalIndex++;

			assert(normalIndex <= normIndLength); 

			// Input color, if any
			if(tempCpv)
			{
				int actualColor = tempCpvInd[colorIndex] * 3;

				assert(actualColor <= cpvLength);

				// Is node lit (does there exist material?)
				if(mat)
				{
					// !! TODO !! Set alpha straight
					cout << "using diffuse color material per vertex" << endl; 
					GLfloat diffuseColor[4] = { tempCpv[actualColor], tempCpv[actualColor + 1], tempCpv[actualColor + 2], 1.0f };
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);
				}
				else
					glColor3fv(&tempCpv[actualColor]);

				// Is there a color per vertex?
				if(CPV)
					colorIndex++;
				else
					if(((coordIndex + 1) % 3) == 0) colorIndex++;

				assert(colorIndex <= cpvIndLength);
			}

			// Changed by KStankovic
			// Input texture, if any
			if(tcoord)
			{
				if (mat->cubemapNames[0] != NULL)
				{
					int actualTexture = tcoordInd[textureIndex] * 3;

					assert(actualTexture <= tcoordLength);
			
					glTexCoord3f(tcoord[actualTexture], tcoord[actualTexture + 1],tcoord[actualTexture + 2]);

					textureIndex++;

					assert(textureIndex <= tcoordIndLength);
				}
				else
				{
					int actualTexture = tcoordInd[textureIndex] * 2;

					assert(actualTexture <= tcoordLength);
		
					glTexCoord2f(tcoord[actualTexture], tcoord[actualTexture + 1]);

					textureIndex++;

					assert(textureIndex <= tcoordIndLength);
				}
			}
			//

			// Get actual coord
			int actualCoordinate = tempCoordInd[coordIndex] * 3;

			assert(actualCoordinate <= coordLength);

			glVertex3fv(&tempCoord[actualCoordinate]);
			
			coordIndex++;

			assert(coordIndex <= coordIndLength);
		}
	glEnd();
	
#else
	
	GLVertexBuffer my_VBO_data =  GLVertexBuffer(
										   mat,   // material for this shape
										   coord,        // vertex coordinates
										   coordLength,     // length of the vertex coordinates array (number of vertices * 3)
										   coordInd,       // vertex indices for triangles
										   coordIndLength,  // length of coordInd array
										   norm,         // normals
										   normLength,      // length of norm array
										   normInd,        // normal indices, per vertex
										   normIndLength,   // length of normInd array
										   tcoord,       // texture coordinates, if any (else zero)
										   tcoordLength, 
										   tcoordInd, 
										   tcoordIndLength, 
										   cpv,          // colors per vertex, if any (else zero)
										   cpvLength, 
										   cpvInd, 
										   cpvIndLength,
										   NPV,              // Normal Per Vertex: 1 if on, 0 if off (i.e. normal per face)
										   CPV);
	

	
//BAD OPTION, BECAUSE SPENT TOO MUCH TIME CREATING VBO EVERY ITERATION
//	GLuint dynamicVBO;
//	glGenBuffers(1, &dynamicVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, dynamicVBO);
//	glBufferData(GL_ARRAY_BUFFER, my_VBO_data.get_VBO_buffer_size(), 0, GL_DYNAMIC_DRAW);
	

	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer[nWorkingBuffer]);
	GLchar* vbo_buffer=0;

	#ifdef IPHONE
		vbo_buffer= (GLchar*)glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
	#else
		vbo_buffer = (GLchar*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY); 
	#endif
	
	
	while(my_VBO_data.has_next())
	{
		if(!isStaticDataInBuffer)
		{
			memcpy(vbo_buffer, my_VBO_data.get_coord(), my_VBO_data.get_coord_size());
			vbo_buffer += my_VBO_data.get_coord_size();

			memcpy(vbo_buffer, my_VBO_data.get_normal(), my_VBO_data.get_normal_size());
			vbo_buffer += my_VBO_data.get_normal_size();

			if(my_VBO_data.check_colors())
			{
				memcpy(vbo_buffer, my_VBO_data.get_cpv(), my_VBO_data.get_cpv_size());
				vbo_buffer += my_VBO_data.get_cpv_size();
			}
			if(my_VBO_data.check_texCoord())
			{
				memcpy(vbo_buffer, my_VBO_data.get_texCoord(), my_VBO_data.get_texCoord_size());
				vbo_buffer += my_VBO_data.get_texCoord_size();
			}
		}
		else
		{
			memcpy(vbo_buffer, my_VBO_data.get_coord(), my_VBO_data.get_coord_size());
			vbo_buffer += my_VBO_data.get_coord_size();
			
			memcpy(vbo_buffer, my_VBO_data.get_normal(), my_VBO_data.get_normal_size());
			vbo_buffer += my_VBO_data.get_normal_size();

			if(my_VBO_data.check_colors())
			{
				vbo_buffer += my_VBO_data.get_cpv_size();
			}
			if(my_VBO_data.check_texCoord())
			{
				vbo_buffer += my_VBO_data.get_texCoord_size();
			}
		}


	}
	
	if(my_VBO_data.check_colors() && !isStaticDataInBuffer)
		std::cout << "setting colors for:" << nWorkingBuffer << std::endl;
	if(my_VBO_data.check_texCoord()&& !isStaticDataInBuffer)
		std::cout << "setting tex_coords for:" << nWorkingBuffer << std::endl;

	
	#ifdef IPHONE
		glUnmapBufferOES(GL_ARRAY_BUFFER); 
	#else
		glUnmapBuffer(GL_ARRAY_BUFFER); 
	#endif
	//---End data transfers---//


	//DEFINE ACTIVE ATTRIBUTES
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(my_VBO_data.get_coord_comp(), GL_FLOAT, my_VBO_data.get_vertex_size(), my_VBO_data.get_coord_offset());

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, my_VBO_data.get_vertex_size(), my_VBO_data.get_normal_offset());
	
	if(my_VBO_data.check_colors())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(my_VBO_data.get_cpv_comp(), GL_FLOAT, my_VBO_data.get_vertex_size(), my_VBO_data.get_cpv_offset());
	}
	
	if(my_VBO_data.check_texCoord())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(my_VBO_data.get_texCoord_comp(), GL_FLOAT, my_VBO_data.get_vertex_size(), my_VBO_data.get_texCoord_offset());
	}

	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	

	//DRAW!!!! This is the actual draw command
	glDrawArrays(GL_TRIANGLES, 0, my_VBO_data.get_num_of_vertex());

	
	glDisableClientState(GL_VERTEX_ARRAY);

	glDisableClientState(GL_NORMAL_ARRAY);
	
	if(my_VBO_data.check_colors())
		glDisableClientState(GL_COLOR_ARRAY);

	if(my_VBO_data.check_texCoord())
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	
#endif
	
	

}

void VRMLRendererTraverser::EnterLipEditingMode(SurfaceData *lipsSurface)
{
	mLipsSurface = lipsSurface;
}

}
