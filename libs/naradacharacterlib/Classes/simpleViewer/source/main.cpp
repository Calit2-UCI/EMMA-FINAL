
#include "OGL_viewer.hpp"
#include <iostream>



int main ( int argc, char** argv )   // Create Main Function For Bringing It All Together
{


	std::cout << "going to start" << endl;
	OGLViewer my_viewer(600, 600, "prueba", "background");

	//OpenGL ES must be initialized at this point
	
	my_viewer.LoadVCModelFile("../data/models/reana.afm");
	std::cout << "model loaded" << endl;

	my_viewer.startRenderingLoop();

	return 1;



}
