//!
//!  \file OGL_viewer.cpp
//!
//! \author Enrique Oriol <eoriol@naradarobotics.com>
//!
//! \date 03-03-2010
//!



#include "OGLViewer.h"


#include<sys/types.h>
#include<sys/timeb.h>



//void OGLViewer::createTextureArray()
//{
//	
//	//TAKE CARE with OpenGL vs. iPhone angle definitions
//	backgroundTextureArray[0] = - bgWidthX + kMinYaw/100;	//coords v0
//	backgroundTextureArray[1] = - bgWidthY - kMaxPitch/100;	//coords v0
//	backgroundTextureArray[2] = 0.0f;	//coords v0
//	backgroundTextureArray[3] = 0.0f;	//texture coord v0
//	backgroundTextureArray[4] = 0.0f;	//texture coord v0
//	
//	backgroundTextureArray[5] = bgWidthX + kMaxYaw/100;	//coords v1
//	backgroundTextureArray[6] = - bgWidthY - kMaxPitch/100;	//coords v1
//	backgroundTextureArray[7] = 0.0f;	//coords v1
//	backgroundTextureArray[8] = 1.0f;	//texture coord v1
//	backgroundTextureArray[9] = 0.0f;	//texture coord v1
//	
//	backgroundTextureArray[10] = - bgWidthX + kMinYaw/100;	//coords v2
//	backgroundTextureArray[11] = bgWidthY  - kMinPitch/100;	//coords v2
//	backgroundTextureArray[12] = 0.0f;	//coords v2
//	backgroundTextureArray[13] = 0.0f;	//texture coord v2
//	backgroundTextureArray[14] = 1.0f;	//texture coord v2
//	
//	backgroundTextureArray[15] = bgWidthX + kMaxYaw/100;	//coords v3
//	backgroundTextureArray[16] = bgWidthY - kMinPitch/100;	//coords v3
//	backgroundTextureArray[17] = 0.0f;	//coords v3
//	backgroundTextureArray[18] = 1.0f;	//texture coord v3
//	backgroundTextureArray[19] = 1.0f;	//texture coord v3
//	
//	
//	backgroundBuff = 0;
//	
//	glGenBuffers(1, &backgroundBuff);
//	glBindBuffer(GL_ARRAY_BUFFER, backgroundBuff);
//	
//	unsigned int size = 20 * sizeof(GLfloat);
//	glBufferData(GL_ARRAY_BUFFER, size, backgroundTextureArray, GL_STATIC_DRAW);
//	
//	
//}



//void OGLViewer::translateBackground(float incX, float incY)
//{
//	_bgOffsetX = incX; 
//	_bgOffsetY = incY;
//}



//bool OGLViewer::changeBackgroundImage(UIImage* uiImage)
//{
//	bgtexture[0] = loadTexture(uiImage);
//	if(bgtexture[0] == -1)
//		return false;
//	return true;
//}


bool OGLViewer::changeBackgroundImage(NSString* fileString)
{
	NSString *pathToImg = [ [ [NSBundle mainBundle] resourcePath ] stringByAppendingPathComponent:fileString ];
	
	UIImage *uiImage = [UIImage imageWithContentsOfFile:pathToImg];
    
	bgtexture[0] = loadTexture(uiImage);
	if(bgtexture[0] == -1)
		return false;
	return true;
}


bool OGLViewer::setDefaultBackground()
{
	NSString *fileString =	[NSString stringWithUTF8String:_bg_img.c_str()];
	NSString *pathToImg = [ [ [NSBundle mainBundle] resourcePath ] stringByAppendingPathComponent:fileString ];
	
	UIImage *uiImage = [UIImage imageWithContentsOfFile:pathToImg];
	
	bgtexture[0] = loadTexture(uiImage);
	if(bgtexture[0] == -1)
		return false;
	return true;
}


GLuint OGLViewer::loadTexture(UIImage* uiImage)
{
	GLuint texture[1];
	
	//IPHONE SPECIFIC CODE TO LOAD TEXTURES
	CGImageRef textureImage;
	CGContextRef textureContext;	
	
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
			 */				
			textureData = (GLubyte *) malloc(width * height * 4);
			textureContext = CGBitmapContextCreate(textureData, width, height, 8, width * 4, CGImageGetColorSpace(textureImage), kCGImageAlphaPremultipliedLast);
			CGContextTranslateCTM (textureContext, 0, height);
			CGContextScaleCTM (textureContext, 1.0, -1.0);
			CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (float)width, (float)height), textureImage);
			
			//			}
			
		}
//		glActiveTexture(GL_TEXTURE1);
		glActiveTexture(GL_TEXTURE0);        
		glGenTextures(1, &texture[0]);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		//ONLY FOR IPHONE VERSION >= 2
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 50, 50, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

		
		//ONLY FOR IPHONE VERSION >= 2
		glGenerateMipmap( GL_TEXTURE_2D );
		
		cout << "Checking error" << endl;
		int error = glGetError();
		switch (error) {
			case GL_NO_ERROR:
				cout << "No error " << endl;
				break;
			case GL_INVALID_ENUM:
				cout << "Invalid Enum" << endl;
				break;
			case GL_INVALID_VALUE:
				cout << "Invalid value" << endl;
				break;
			case GL_INVALID_OPERATION:
				cout << "Invalid operation" << endl;
				break;
			case GL_STACK_OVERFLOW:
				cout << "Stack overflow" << endl;
				break;
			case GL_STACK_UNDERFLOW:
				cout << "Stack underflow" << endl;
				break;
			case GL_OUT_OF_MEMORY:
				cout << "Out of memory"	<< endl;
				break;
			default:
				cout << "Other error: " << error << endl;
				break;
		}
        glActiveTexture(GL_TEXTURE0);
		/*
		if(glGetError())
		{
			printf("glTexImage2D failed. ");
			//return 0;
		}
		*/
		free(textureData);
		CGContextRelease(textureContext);
	}
	else {
		cout << "cannot load image" << endl;
	}	
	
	
	return texture[0];
}



void OGLViewer::setRenderingMode()
{
//		case VGLR_RENDERING_WIREFRAME:
//EN VEZ DE ESTO LO QUE HAY QUE HACER ES cambiar a la hora de pintar a glDrawArray(GL_LINES,...)


//			if(mBackfaceCull)
//				glPolygonMode(GL_FRONT, GL_FILL);
//			else
//				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// Lightning.
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);

			// Textures
			glEnable(GL_TEXTURE_2D);

			if(this->mBackfaceCull)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
			else
				glDisable(GL_CULL_FACE);
	
}






void OGLViewer::RenderToScreen(){
	
    paintGL();

}




bool OGLViewer::setPerspective(GLfloat width, GLfloat height)
{

    OGLVirtualCharacterViewer::setPerspective(width, height);
	
	return true;
}

