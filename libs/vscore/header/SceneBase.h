/* Generated by Together */

#ifndef SCENEBASE_H
#define SCENEBASE_H

#ifndef _STDIO_H
#include <stdio.h>
#endif

/**
* \ingroup VRMLParser
*/
class SceneBase {
public:

    virtual				~SceneBase(){};
	virtual const char*	parseVRML(FILE *f) =0;
	
};
#endif //SCENEBASE_H