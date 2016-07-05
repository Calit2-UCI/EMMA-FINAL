// BaseOGLVRMLModelRenderer.cpp: implementation of the BaseOGLVRMLModelRenderer class.
//
//////////////////////////////////////////////////////////////////////

// Disable annyoing debug-compile error caused by STL.
// This is safe.
#pragma warning(disable : 4786)

#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include "VRMLRendererTraverser.h"
#include "VRMLFitToScreenTraverser.h"

#include "BaseOGLVRMLModelRenderer.h"

//#include "SurfaceData.h"

#include <algorithm>

#ifdef WIN32
	#include <gl\gl.h>
	#include <gl\glu.h>
#elif defined(LINUX)
	#include <GL/gl.h>
	#include <unistd.h>
#elif defined(MAC)
	#include <OpenGL/gl.h>
	#include <unistd.h>
#elif defined(IPHONE)
	#include <OpenGLES/ES1/gl.h>
	#include <unistd.h>
#endif


#include <assert.h>
#include  <math.h>
#include  <stdlib.h>

namespace VisageSDK
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BaseOGLVRMLModelRenderer::BaseOGLVRMLModelRenderer()
{
	mNLoadedModels = 0;

	mTraversRenderer = NULL;

}

BaseOGLVRMLModelRenderer::~BaseOGLVRMLModelRenderer()
{
	printf("BaseOGLVRMLModelRenderer destructor called\n");
	delete mTraversRenderer;
}

void BaseOGLVRMLModelRenderer::Init(VRMLModel *initModel)
{
	assert(initModel);
	
	mLoadedModel[mNLoadedModels++] = initModel;
}

void BaseOGLVRMLModelRenderer::RemoveModel(VRMLModel *theModel)
{
	int found = 0;
	for(int i = 0;i < mNLoadedModels;i++)
	{
		if(mLoadedModel[i] == theModel)
		{
			found = 1;
			mNLoadedModels--;
		}
		if(found)
		{
			mLoadedModel[i] = mLoadedModel[i+1];
		}
	}
}

void BaseOGLVRMLModelRenderer::Close()
{
	if(mTraversRenderer)
		delete mTraversRenderer;
	mTraversRenderer = NULL;
}


void BaseOGLVRMLModelRenderer::RenderToScreen()
{	
	for(int i = 0; i < mNLoadedModels; i++)
		RenderToScreen(mLoadedModel[i]);
	//	else
	//		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void BaseOGLVRMLModelRenderer::RenderToScreen(VRMLModel *renderModel)
{
	while(renderModel->locked)
	{
		#ifdef WIN32
			Sleep(5);
		#else
			usleep(5000);
		#endif

	}
	renderModel->locked = 1;

	renderModel->applySkinning();
	renderModel->applyMorphing();

	glPushMatrix();

	// Keep this
	if(!mTraversRenderer) mTraversRenderer = new VRMLRendererTraverser;

	glTranslatef(renderModel->globalTranslation[0],renderModel->globalTranslation[1],renderModel->globalTranslation[2]);
	glRotatef(renderModel->globalRotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(renderModel->globalRotation[1], 0.0f, 1.0f, 0.0f);
	glRotatef(renderModel->globalRotation[2], 0.0f, 0.0f, 1.0f);
	glScalef(renderModel->globalScale[0],renderModel->globalScale[1],renderModel->globalScale[2]);
	
	renderModel->traverse(renderModel->rootNode, mTraversRenderer);

	renderModel->locked = 0;

	glPopMatrix();
}

}
