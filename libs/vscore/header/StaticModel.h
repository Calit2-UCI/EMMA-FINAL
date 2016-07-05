
#ifndef __StaticModel_h__
#define __StaticModel_h__

#include "AppModel.h"
#include "cv.h"

namespace VisageSDK
{

/**
	Static Appearance %Model class. 

	The class inherits from the AppModel class and builds on it the functionality more related to the facial features detection. 
	

*/
class StaticModel : public AppModel
{
public:

	/*
	*	Constructor
	*/
	StaticModel(void);

	/*
	*	Destructor
	*/
	~StaticModel(void);

	/*
	*	Load a model from file(s).

		The model is comprised of two files: a txt file containing the shape information
		and a binary file containing the appearance information (images).
		Currently, the two files have extension .shape and .app respectively.
		The filename passed as parameter should have no extension. If the files are not in the working directory
		the full path must be given.
		For example, if we want to load the model comprised of the files model1.shape and model1.app in C:\Data\models\
		we should pass "C:\Data\models\model1" as argument to the function.
		NOTE: the haarcascade file facecascade.xml must be present in the same folder as the model files

		@param aamFileName the name of appearance model files
		@return true if successful
	*/
	bool ReadFromFile(const std::string& fName);

	bool WriteAppearanceFile(const std::string& fName);


	/*
	*	Adapt the appearance model to an image. Proper initialization must be provided, i.e., the model should
		be placed roughly on the face.
		The algorithm is the Simultaneous Inverse Compositional Algorithm described by Baker and al.
		the paper "Lucas-Kanade 20 Years On: A Unifying Framework: Part 3"

		@param input the image
		@param maxIter max number of allowed iterations 
		@param epsilon criterion to stop the iterations
		@param show if true a set of debug information (mostly images) are shown

		@returns the measure of the fit, i.e., the image difference between the input image warped onto the mean shape
		and the current instance of the appearance model
	*/
	double AdaptToFrame(IplImage* input, int maxIter = 40, double epsilon = 0.1, bool show = false);

	/*
	*	Initialize the features detectoin by first calling a face detector and then trying to locate the pupils and the mouth corners based on color.
		This only works for color images.

		@param input the image
		@returns true is a face was found in the image, false otherwise
	*/
	bool FindFaceAndInitFeaturesDetection(IplImage* input);

	/*
	*	Given an image of a face (i.e., output of a face detector), finds the two pupils

		@param faceImage the image
		@param leye left eye
		@param reye right eye
		@returns nothing
	*/
	void FindEyes(IplImage* faceImage, CvPoint* leye, CvPoint* reye);

	/*
	*	iven an image of a face (i.e., output of a face detector), finds the mouth corners, only works for BGR color images

		@param faceImage the image
		@param p1 left mouth corner
		@param p2 right mouth corner
		@returns true if the retrieved mouth corner positions make sense
	*/
	bool FindMouth(IplImage* faceImage, CvPoint* p1, CvPoint* p2);

	void ComputeSteepestDescentImages(bool show = false);

private:

	void UpdateAppearance(bool show);
	void InitAppearance(uint w, uint h, uint numAppParameters);
	bool ReadAppFileCompact(const std::string& fName, bool show = false);
	void ComputeInverseUpdates(bool show = false);
	void ComputeHessian();
	
	void PreComputeSteepestDescentImages(bool show = false);
	void InvertWarp();

    
	CvMat* mParamsUpdates;

};

}

#endif // __StaticModel_h__
