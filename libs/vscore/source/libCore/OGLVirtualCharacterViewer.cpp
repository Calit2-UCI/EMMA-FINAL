/*
 * OGLVirtualCharacterViewer.cpp
 *
 *  Created on: 16/01/2012
 *      Author: narada
 */

#include "OGLVirtualCharacterViewer.h"
#include <sys/timeb.h>

namespace VisageSDK {



/**
 *
 * @brief Constructor: Creates the OGLVirtualCharacterViewer. Initializes the data members.
 * 
 * @param width 	width of the view
 * @param height 	height of the view
 * @param background string with the path to the background image. If it's an empty string, no background is draw (black BG)
 * @param persp 	boolean that determines using perspective view (if true) or orthogonal view (if false)
 *
 * @todo TODO review usage of perspective/orthogonal view
 */
    OGLVirtualCharacterViewer::OGLVirtualCharacterViewer(int width, int height, std::string background, bool persp)
{
    


	perspective = persp;
	readyToRender = false;
	character = NULL;
	xRot = 0;
	yRot = 0;
	zRot = 0;
	zNear = 0.1f;
	zFar = 500.0f;
	backgroundBuff = 0;

	translateX = 0;
	translateY = 0;
	translateZ = 0;

	accelX = 2;
	accelY = 2;
	accelZ = 2;
	perspectFOVY = 45.0f;
	perspectTanFOVY = tan(perspectFOVY / 2.0);
    
    timeCounterInitFR = 0;
    timeCounterEndFR = 0;
    frameCounterFR = 0;
    realFrameRate = 0.0;


	myWidth = width;
	myHeight = height;
	_bg_img = background;
	if(!background.compare(""))
		drawBackground = false;
	else
		drawBackground = true;

	mBackfaceCull = false;
	bgtexture[0] = 0;

}



/**
 *
 * @brief Destructor: if there's a character defined, deletes it. In case there's a background, delete the OpenGL
 *  buffer assigned to the texture.
 *
 */
OGLVirtualCharacterViewer::~OGLVirtualCharacterViewer() {
	if(character)
		delete character;
	if(drawBackground)
	{
		glDeleteBuffers(1,&backgroundBuff);
	}
}



/**
 *
 * @brief sets the view size with the arguments
 *
 * @param w new width of the view
 * @param h new height of the view
 *
 */
void OGLVirtualCharacterViewer::setSize(int w, int h)
{
    myWidth = w;
    myHeight = h;
}



/**
 *
 * @brief loads the character specified as argument, initialize the zFar and translation data members
 *  depending on the values of the character, and prepare the basic OpenGL parameters for a good performance.
 *
 * @param pPath path to the character .AFM file
 *
 * @return a pointer to the virtualCharacter loaded
 * @see VirtualCharacter::VirtualCharacter(), InitializeGL(), BaseOGLVRMLModelRenderer::Init()
 *
 */
VirtualCharacter* OGLVirtualCharacterViewer::loadCharacter(string pPath)
{


    character = new VirtualCharacter(pPath, perspective);

    zFar = character->getDistance()*10;

    translateX = character->getCenter().x;
    translateY = character->getCenter().y;
    translateZ = character->getCenter().z;


    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    BaseOGLVRMLModelRenderer::Init(character->getVRMLModel());
    initializeGL();

    if(myWidth && myHeight)
            resizeGL(myWidth, myHeight);

    readyToRender = true;

    return character;

}



/**
 *
 * loads an image from the path in the argument, and associates it with an OpenGL texture
 *
 * @param filename string containing the path to the image file
 *
 * @return the OpenGL texture identifier asociated with the image
 *
 */
GLuint OGLVirtualCharacterViewer::loadTexture(std::string filename)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT2);
	float color2f[4] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(GL_LIGHT2, GL_AMBIENT, color2f );
    glDisable(GL_LIGHT2);

    GLuint tex_ID;

    glGenTextures(1, &tex_ID);					// Create The Texture

    // Typical Texture Generation Using Data From The Bitmap
    //by KaiK
//    glActiveTexture( GL_TEXTURE1 );
    glActiveTexture(GL_TEXTURE0);        
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_ID);


    Image* my_image = new Image();
    my_image->read_image((char *)filename.c_str());


#ifndef IPHONE
    glTexImage2D(GL_TEXTURE_2D, 0, 3, my_image->get_size_x(), my_image->get_size_y(), 0, GL_RGB, GL_UNSIGNED_BYTE, my_image->get_data());
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    
    
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    //ONLY FOR IPHONE VERSION >= 2
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, my_image->get_size_x(), my_image->get_size_y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, my_image->get_data());
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 50, 50, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

    //ONLY FOR IPHONE VERSION >= 2
    glGenerateMipmap( GL_TEXTURE_2D );
    
#endif

    delete my_image;
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glActiveTexture(GL_TEXTURE0);

    return tex_ID;

}



/**
 *
 * @brief Draws the background image. If there's some background to draw, draws it through OpenGL.
 * the first time this function is called, it loads the image as a texture.
 *
 * @return false if there's no background or cannot load its texture, true otherwise.
 * @see loadTexture()
 *
 */
bool OGLVirtualCharacterViewer::drawBackgroundImage()
{
	// Still want lighting tho..
    	if(drawBackground)
    	{

			if(!bgtexture[0])
			{
					createTextureArray();

					bgtexture[0] = loadTexture(_bg_img);
					if(bgtexture[0] == -1)
							return false;
			}

			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT2);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();

#ifndef IPHONE
			glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -10.0f, 10.0f);
#else
            glOrthof(0.0f, 1.0f, 0.0f, 1.0f, -10.0f, 10.0f);
#endif
            


			glEnable(GL_TEXTURE_2D);
//			glActiveTexture(GL_TEXTURE1);
            glActiveTexture(GL_TEXTURE0);                    
			glBindTexture(GL_TEXTURE_2D, bgtexture[0]);



			glBindBuffer(GL_ARRAY_BUFFER, backgroundBuff);

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 20, 0);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 20, (void *)12);

			glColor4f(1.0, 1.0, 1.0, 1.0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);



			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE);
			glClear(GL_DEPTH_BUFFER_BIT);

			glDisable(GL_LIGHT2);
            
            glActiveTexture(GL_TEXTURE0);
			return true;
    	}

    	return false;

}



/**
 *
 * @brief Creates an interleaved array of vertex and texture coordinates, to display the background texture.
 *
 */
void OGLVirtualCharacterViewer::createTextureArray()
{
        backgroundTextureArray[0] = 0.0f;	//coords v0
        backgroundTextureArray[1] = 0.0f;	//coords v0
        backgroundTextureArray[2] = -0.5f;	//coords v0
        backgroundTextureArray[3] = 0.0f;	//texture coord v0
        backgroundTextureArray[4] = 0.0f;	//texture coord v0

        backgroundTextureArray[5] = 1.0f;	//coords v1
        backgroundTextureArray[6] = 0.0f;	//coords v1
        backgroundTextureArray[7] = -0.5f;	//coords v1
        backgroundTextureArray[8] = 1.0f;	//texture coord v1
        backgroundTextureArray[9] = 0.0f;	//texture coord v1

        backgroundTextureArray[10] = 0.0f;	//coords v2
        backgroundTextureArray[11] = 1.0f;	//coords v2
        backgroundTextureArray[12] = -0.5f;	//coords v2
        backgroundTextureArray[13] = 0.0f;	//texture coord v2
        backgroundTextureArray[14] = 1.0f;	//texture coord v2

        backgroundTextureArray[15] = 1.0f;	//coords v3
        backgroundTextureArray[16] = 1.0f;	//coords v3
        backgroundTextureArray[17] = -0.5f;	//coords v3
        backgroundTextureArray[18] = 1.0f;	//texture coord v3
        backgroundTextureArray[19] = 1.0f;	//texture coord v3


        backgroundBuff = 0;

        glGenBuffers(1, &backgroundBuff);
        glBindBuffer(GL_ARRAY_BUFFER, backgroundBuff);

        unsigned int size = 20 * sizeof(GLfloat);
        glBufferData(GL_ARRAY_BUFFER, size, backgroundTextureArray, GL_STATIC_DRAW);


}



/**
 *
 * @brief normalize an angle specified in degrees, to belong to the range 0-360 degrees.
 *
 * @param angle angle in degrees to be normalized
 *
 */
static void normalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360;
    while (angle > 360)
        angle -= 360;
}




/**
 *
 * @brief sets the degrees of rotation on the X axis. Internally, it calls the normalizeAngle function.
 *
 * @param angle degrees to be rotated
 * @see normalizeAngle()
 *
 */
void OGLVirtualCharacterViewer::setXRotation(int angle)
{
    normalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
    }
}



/**
 *
 * @brief sets the degrees of rotation on the Y axis. Internally, it calls the normalizeAngle function.
 *
 * @param angle degrees to be rotated
 * @see normalizeAngle()
 *
 */
void OGLVirtualCharacterViewer::setYRotation(int angle)
{
    normalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
    }
}



/**
 *
 * @brief sets the degrees of rotation on the Z axis. Internally, it calls the normalizeAngle function.
 *
 * @param angle degrees to be rotated
 * @see normalizeAngle()
 *
 */
void OGLVirtualCharacterViewer::setZRotation(int angle)
{
    normalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
    }
}



/**
 *
 * @brief add some degrees of rotation on the X axis. Internally, it calls the normalizeAngle function.
 *
 * @param angle degrees to be rotated
 * @see normalizeAngle()
 *
 */
void OGLVirtualCharacterViewer::addXRotation(int angle)
{
	int aux = angle + xRot;
    setXRotation(aux);
}


/**
 *
 * @brief add some degrees of rotation on the Y axis. Internally, it calls the normalizeAngle function.
 *
 * @param angle degrees to be rotated
 * @see normalizeAngle()
 *
 */
void OGLVirtualCharacterViewer::addYRotation(int angle)
{
	int aux = angle + yRot;
    setYRotation(aux);
}


/**
 *
 * @brief add some degrees of rotation on the Z axis. Internally, it calls the normalizeAngle function.
 *
 * @param angle degrees to be rotated
 * @see normalizeAngle()
 *
 */
void OGLVirtualCharacterViewer::addZRotation(int angle)
{
	int aux = angle + zRot;
    setZRotation(aux);
}





/**
 *
 * @brief initialize some basic OpenGL parameters like smooth shading, clear color, depth testing, lights, textures, ...
 *
 */
void OGLVirtualCharacterViewer::initializeGL()
{

    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading

    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);				// Black Background
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do


    // For performance we set all of these to fastest.
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

            // Can't assume that normals aren't scaled. Therefore I need this.
    glEnable(GL_NORMALIZE);


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
	float color0f[4] = {0.2, 0.2, 0.2, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, color0f );
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);


    // Can't assume that normals aren't scaled. Therefore I need this.
    glEnable(GL_NORMALIZE);

    glEnable(GL_TEXTURE_2D);

//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);


}



/**
 *
 * @brief if there's an associated character, draws the background and the model. In case that the
 *  virtual character is playing a video, after drawing the model, it also draws the character video texture.
 *
 * @see drawBackgroundImage(), loadModelViewMatrix(), BaseOGLVRMLModelRenderer::RenderToScreen(), VirtualCharacter::drawVideo(), updateRealTimeFrameRate()
 *
 */
void OGLVirtualCharacterViewer::paintGL()
{

    if(character)
    {

//        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    	glClearColor(0.3529f, 0.7176f, 0.8627f, 1.0f);//color logo NR

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawBackgroundImage();

        loadModelViewMatrix();

        BaseOGLVRMLModelRenderer::RenderToScreen();

        //glClear(GL_DEPTH_BUFFER_BIT);

        character->drawVideo();

    }
    this->updateRealFrameRate();
}



/**
 *
 * @brief resize the OpenGL viewport, and updates the corresponding values in the character.
 *
 * @param w new width of the viewport
 * @param h new height of the viewport
 *
 */
void OGLVirtualCharacterViewer::resizeGL(int w, int h)
{
    // Reset modelview matrix before getting coords.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
	if(h == 0)
		myHeight = 1; //prevent a divide by zero
    myWidth = w;
    myHeight = h;

    if(character)
    	character->resizeWindow(w, h);

	glViewport(0, 0, myWidth, myHeight);
    setPerspective(myWidth, myHeight);

    glMatrixMode(GL_MODELVIEW);
}



/**
 *
 * @brief if the character is defined, updates it
 *
 * @return std::vector<std::string> with the names of the inmediately finished actions.
 *
 * @see VirtualCharacter::updateCharacter()
 *
 */
std::vector<std::string> OGLVirtualCharacterViewer::updateScene()
{
    std::vector<std::string>actions;
    if(character)
    {
        actions = character->updateCharacter();

//        return true;
        return actions;

    }
//    return false;
    return actions;
}
    
    
/**
 *
 * @brief updates the real frame rate data member
 * If there's enough time to make an average, updates the realFrameRate data member. This method is
 * called automatically by paintGL() method
 *
 * @see paintGL()
 *
 */
void OGLVirtualCharacterViewer::updateRealFrameRate()
{
    //get current time
    unsigned long long currentTime = 0;
    
#ifdef WIN32
    struct _timeb timebuffer;
    _ftime( &timebuffer );
    currentTime = (unsigned long long) timebuffer.time + 0.001*timebuffer.millitm;	//CHANGED BY I.K. added explicit cast
#else
    struct timeb timebuffer;
    ftime( &timebuffer );
    currentTime = (unsigned long long) timebuffer.time + 0.001*timebuffer.millitm;	//CHANGED BY I.K. added explicit cast
#endif
    
    //first time setting frame rate timer counters
    if(timeCounterEndFR == 0)
    {
        timeCounterInitFR = currentTime;
        timeCounterEndFR = currentTime;
        frameCounterFR = 0;
        realFrameRate = 0;
    }
    else
    {
        //update end timer and frame counter
        timeCounterEndFR = currentTime;
        frameCounterFR++;

        
        //update time difference from when we are counting frames
        float timeDifference = timeCounterEndFR - timeCounterInitFR;

        //if enough time, update the real frame rate
        if (timeDifference > OGLVirtualCharacterViewer::frMinAveragingTime)
        {
            realFrameRate = (float) frameCounterFR / timeDifference;
            
            //reset init time to current time
            timeCounterInitFR = currentTime;
            
            //reset number of frames
            frameCounterFR = 0;
            
        }
    }
    
    
}
    

/**
 *
 * @brief real frame rate getter
 *
 */
float OGLVirtualCharacterViewer::getRealFrameRate()
{
    return realFrameRate;
}



/**
 *
 * @brief loads the modelview matrix as the identity, and then translates it using the translateX,Y,Z
 *  data members, and rotates it using the x,y,zRot data members.
 *
 */
void OGLVirtualCharacterViewer::loadModelViewMatrix(){

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //glTranslatef(translateX, translateY, translateZ);//esto tiene sentido si hay varios modelos

        //en caso de tener un solo modelo, es mas practico usar directamente la posición del modelo
        glTranslatef(character->getCenter().x, character->getCenter().y, character->getCenter().z );
    
        //del mismo modo, en caso de un solo modelo, cogeremos las rotaciones del modelo

//        glRotatef(xRot /*/ 16.0*/, 1.0, 0.0, 0.0);
//        glRotatef(yRot /*/ 16.0*/, 0.0, 1.0, 0.0);
//        glRotatef(zRot /*/ 16.0*/, 0.0, 0.0, 1.0);
        glRotatef(character->getRotation().x, 1.0, 0.0, 0.0);
        glRotatef(character->getRotation().y, 0.0, 1.0, 0.0);
        glRotatef(character->getRotation().z, 0.0, 0.0, 1.0);

}

    
    
    
/**
 *
 * @brief set the camera perspective, based in the width and height passed as arguments,
 * and also taking into account if it should use perspective view or orthogonal view
 *
 * @param width width of the viewport
 * @param height height of the viewport
 *
 * @return true
 *
 * @todo TODO handle centering the model when using perspective view. Maybe associating
 *  the model with a file where the origin, znear and zfar, and translations are speficied, or its factor scale
 *  or something similar.
 *
 */
bool OGLVirtualCharacterViewer::setPerspective(GLfloat width, GLfloat height)
{
    double x, y;
    
    x = zNear * perspectTanFOVY;
    y = x / ( width / height );
    
    if(perspective)
    {
        
        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
        glLoadIdentity();									// Reset The Projection Matrix
        
#ifdef IPHONE            
        glFrustumf(-x, x, -y, y, zNear, zFar);	
#else
        glFrustum(-x, x, -y, y, zNear, zFar);
#endif
        
    }
    else
    {
        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
        glLoadIdentity();									// Reset The Projection Matrix
        if(character)
        {
            float aspectRatio = width/height;
            float height = character->getModelHeight()/2;
#ifdef IPHONE
            glOrthof(-height/2*aspectRatio, height/2*aspectRatio, 0, height, zNear, zFar);
#else
            glOrtho(-height/2*aspectRatio, height/2*aspectRatio, 0, height, zNear, zFar);
#endif
        }
        //glOrtho(-x, x, -y, y, zNear, zFar);
    }
    
    return true;
}

    



} /* namespace VisageSDK */

