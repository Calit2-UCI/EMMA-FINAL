
#include "FDP.h"

#include "VisageCharModel.h"

// C++
#ifdef _DEBUG
#include <assert.h>
#endif
#include <math.h>
#include <vector>
#include <fstream>
#include <sstream>

namespace VisageSDK
{

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Fresh new FDP
FDP::FDP( VisageCharModel* charModel )
{
	int tmpGrSize[13] = {0,0,14,14,6,4,4,1,10,15,10,6,47};
	normalized = 0;
	for(int i =2;i<13;i++)
	{
		grSize[i] = tmpGrSize[i];
		fp[i] = new FeaturePoint[grSize[i]];
		for(int j = 0; j < grSize[i]; j++)
		{
			fp[i][j].defined = 0;
			fp[i][j].animated = 0;
			fp[i][j].normalizing = 0;
			fp[i][j].surf = "";
			fp[i][j].vert = -1;
		}
	}
	initialized = 1;
	this->charModel = charModel;
}

FDP::FDP( const FDP& featurePoints )
{
	int tmpGrSize[13] = {0,0,14,14,6,4,4,1,10,15,10,6,47};

	for(int i=2;i<13;i++)
	{
		grSize[i] = tmpGrSize[i];
		fp[i] = new FeaturePoint[grSize[i]];
		for(int j = 0; j < grSize[i]; j++)
			fp[i][j] = featurePoints.fp[i][j];
	}

	ll = featurePoints.ll;
	ul = featurePoints.ul;

	mFilename = featurePoints.mFilename;
	normalized = featurePoints.isNormalized();
	initialized = featurePoints.isInitialized();

	this->charModel = NULL;
}

// read a new FDP from file
FDP::FDP( const char *fn, VisageCharModel* charModel )
{
	int tmpGrSize[13] = {0,0,14,14,6,4,4,1,10,15,10,6,47};
	normalized = false;
	for(int i =2;i<13;i++)
	{
		grSize[i] = tmpGrSize[i];
		fp[i] = new FeaturePoint[grSize[i]];
		for(int j = 0; j < grSize[i]; j++)
		{
			fp[i][j].defined = 0;
			fp[i][j].animated = 0;
			fp[i][j].normalizing = 0;
			fp[i][j].surf = "";
			fp[i][j].vert = -1;
		}
	}
	initialized = true;
	this->charModel = charModel;

	readFromFile(fn);
}

FDP::~FDP()
{

	//Changed by KaiK (added [] after delete key, to be consequent with the nature of fp[i])
	for(int i =2;i<13;i++)
	{
		if(fp[i])
			delete[] fp[i];
	}
	// marco: there is a memory leak causing a crash somewhere here
//	if(ll)
//		delete(ll);
//	if(ul)
//		delete(ul);

}

FDP& FDP::operator=( const FDP& featurePoints )
{
	if( this == &featurePoints )
		return *this;

	for(int i=2;i<13;i++)
	{
		for(int j = 0; j < groupSize(i); j++)
			fp[i][j] = featurePoints.fp[i][j];
	}

	ul = featurePoints.ul;
	ll = featurePoints.ll;

	mFilename = featurePoints.mFilename;
	normalized = featurePoints.isNormalized();
	initialized = featurePoints.isInitialized();

	this->charModel = NULL;

	return *this;
}

void FDP::print() const
{
	if (initialized != 1)
	{
		printf("FDP::Print: FDP structure not initialized.\n");
		return;
	}

	printf("defined FDP:\n");

	for(int i = 2; i < 13; i++)
		for(int j = 0; j < groupSize(i); j++)
		{
			if (fp[i][j].defined)
				printf("fdp: %d.%d %.3f %.3f %.3f\n", i, j+1, fp[i][j].pos[0], fp[i][j].pos[1], fp[i][j].pos[2]);
		}

	printf("\n\n");
}

const FeaturePoint& FDP::getFP(int group, int n) const
{
	#ifdef _DEBUG
	assert( FPIsValid( group, n ) );
	#endif
	
	return fp[group][n-1];
}

const FeaturePoint& FDP::getFP(const char *_name) const
{
	int g, n;
	parseFPName( _name, g, n );

	return getFP(g,n);
}

const float* FDP::getFPPos(int group, int n) const
{
	#ifdef _DEBUG
	assert( FPIsValid( group, n ) );
	#endif

	return(&fp[group][n-1].pos[0]);
}


const float* FDP::getFPPos(const char *name) const
{
	int g, n;
	parseFPName( name, g, n );

	return getFPPos(g,n);
}

void FDP::getFPSurfVert( int group, int n, string& surf, int& vert ) const
{
	#ifdef _DEBUG
	assert( FPIsValid( group, n ) );
	#endif

	surf = fp[group][n-1].surf;
	vert = fp[group][n-1].vert;
}

void FDP::getFPSurfVert( const char* name, string& surf, int& vert ) const
{
	int g, n;
	parseFPName( name, g, n );

	getFPSurfVert( g, n, surf, vert );
}

void FDP::setFPPos(int group, int n, const float *pos)
{
	#ifdef _DEBUG
	assert( FPIsValid( group, n ) );
	#endif

	fp[group][n-1].defined = 1;
	for(int i =0 ;i<3;i++)
		fp[group][n-1].pos[i] = pos[i];
}


void FDP::setFPPos(const char *name, const float *pos)
{
	int g, n;
	parseFPName( name, g, n );

	setFPPos( g, n, pos );
}

// Jonas added code.
// Sets the surface and vertex of a feature point.
void FDP::setFPSurfVert(int group, int n, const string& surf, int vert)
{
	#ifdef _DEBUG
	assert( FPIsValid( group, n ) );
	#endif

	fp[group][n-1].defined = 1;
	fp[group][n-1].surf = surf;
	fp[group][n-1].vert = vert;
}

void FDP::setFPSurfVert(const char *name, const string& surf, int vert)
{
	int g, n;
	parseFPName( name, g, n );

	setFPSurfVert( g, n, surf, vert );
}

void FDP::setFPPos(int group, int n, float x, float y, float z)
{
	#ifdef _DEBUG
	assert( FPIsValid( group, n ) );
	#endif

	float pos[3];
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
	setFPPos( group, n, &pos[0] );
}


void FDP::setFPPos(const char *name, float x, float y, float z)
{
	int g, n;
	parseFPName( name, g, n );

	setFPPos( g, n, x, y, z );
}

void FDP::setFP( int group, int n, const FeaturePoint& f )
{
	#ifdef _DEBUG
	assert( FPIsValid( group, n ) );
	#endif

	fp[group][n-1] = f;
}

void FDP::setFP( const char* name, const FeaturePoint& f )
{
	int g, n;
	parseFPName( name, g, n );

	setFP( g, n, f );
}

bool FDP::FPIsDefined( int group, int n ) const
{
	#ifdef _DEBUG
	assert( FPIsValid( group, n ) );
	#endif

	if( fp[group][n-1].defined == 0 )
		return false;

	return true;
}

bool FDP::FPIsDefined( const string& name ) const
{
	int g, n;
	parseFPName( name, g, n );
	return FPIsDefined(g,n);
}

bool FDP::isNormalized() const
{
	return normalized;
}

bool FDP::isInitialized() const
{
	return initialized;
}

void FDP::addUpperLipVertex( const FeaturePoint& vertex )
{
	ul.push_back(vertex);
}

const FeaturePoint& FDP::getUpperLipVertex( int vertexIndex ) const
{
	V_ASSERT( vertexIndex >= 0 && vertexIndex < numberOfUpperLipVertices() );

	return ul[vertexIndex];
}

void FDP::removeUpperLipVertex( int vertexIndex )
{
	V_ASSERT( vertexIndex >= 0 && vertexIndex < numberOfUpperLipVertices() );

	ul.erase( ul.begin() + vertexIndex );
}

void FDP::clearUpperLipVertices()
{
	ul.clear();
}

int FDP::numberOfUpperLipVertices() const
{
	return (int)ul.size();
}

void FDP::addLowerLipVertex( const FeaturePoint& vertex )
{
	ll.push_back(vertex);
}

const FeaturePoint& FDP::getLowerLipVertex( int vertexIndex ) const
{
	V_ASSERT( vertexIndex >= 0 && vertexIndex < numberOfLowerLipVertices() );

	return ll[vertexIndex];
}

void FDP::removeLowerLipVertex( int vertexIndex )
{
	V_ASSERT( vertexIndex >= 0 && vertexIndex < numberOfLowerLipVertices() );

	ll.erase( ll.begin() + vertexIndex );
}

void FDP::clearLowerLipVertices()
{
	ll.clear();
}

int FDP::numberOfLowerLipVertices() const
{
	return (int)ll.size();
}

char *getNextNumberString(char *in, char *next)
{
	char *r = in;
	char *out;

	while(isspace(r[0]))
		r++;
	out=r;
	while(!isspace(out[0]))
		out++;
	out[0] = 0;
	out++;
	strcpy(next,out);
	return(r);
}

float getNextNumber(char *in, char *next)
{
	return(float)(atof(getNextNumberString(in,next)));
}

void FDP::createNewFile(const char *name)
{
	printf("FDP::CreateNewFile: Creating a new fdp file\n");

	fstream f;
	f.open(name, ios::out | ios::trunc);

	f << "# FDP file, created by visage|annotator.\n";
	f << "1.0 " << name << " 0 0 0 0 0 0\n";

	f.close();

	readFromFile(name);
}

int FDP::readFromFile(const char *name)
{
	/*if( charModel == NULL )
		return 0;*/

	StringArray meshes;
	if( charModel ) charModel->_getMeshes(meshes);
	int nul = 0, nll = 0;

	// Returns 0 if file doesn't exist, 1 if successfull, and -1 if file is an old one
	// (And needs re-indexing)
	printf("FDP::readFromFile: reading fdps from %s\n", name);

	fstream f;
	f.open(name, ios::in);
	
	if(!f.is_open()) 
	{
		printf("FDP::readFromFile: Could not open the FDP file %s! (file missing?)\n", name); 
		return(0);
	}

	// Set to true if file is of the old format.
	bool oldFile = false;

	string buf;

	// Skip comments and check version number
	while(getline(f, buf))
	{
		// Skip any comments in file.
		if((buf[0] == '#') || (buf.length() < 2))
				continue;

		if(buf.find("1.0") == std::string::npos) 
		{
			printf("Error 072\nFDP file format error: wrong version!"); 
			return(0);
		}

		break;
	}
	
	// Read the rest of the file.
	while(!f.eof())
	{
		getline(f, buf);

		if (buf.empty())
			continue;

		istringstream is(buf);

		string::iterator test = buf.begin();

		// Check so that line isn't blank
		while(*test == ' ') test++;
		if(test == buf.end()) continue;

		// Skip any comments in file.
		if(buf.find('#') != std::string::npos)
			continue;

		// If first char is alpha (letter) then this is not a feature point
		if(isalpha(is.peek()))
		{
			string temp;
			is >> temp;

			if(temp == "NUL")
			{
				is >> nul;
				ul = vector<FeaturePoint>(nul);
				
				nul = 0;
			}
			else if(temp == "NLL")
			{
				is >> nll;
				ll = vector<FeaturePoint>(nll);
				
				nll = 0;
			}

			// Upper lip
			if(temp == "UL")
			{
				// Is file of old format? Count number of '.' to find out.
				string temp2 = is.str();

				// Crop string to where we are in in-stream.
				temp2.erase(0, is.tellg());
			
				// Count the number of dots in the remainder of the string.
				int noDots = 0;
				for(string::iterator it = temp2.begin(); it != temp2.end(); it++)
				{
					if(*it == '.') noDots++;
				}

				// Set this as defined.
				ul[nul].defined = 1;

				if(noDots > 1)
				{
					// Here we know that it is the new format.
					float x, y, z;
					string surf;
					int surfi;
					int vert;

					is >> x; is >> y; is >> z;

					is >> surfi;

					if( surfi >= 0 )
					{
						if( meshes.size() > 0 ) surf = meshes[surfi];
						else surf = toString<int>(surfi);
					}

					// Ignore '.' in surface.vertice 
					is.get();

					is >> vert;
	
					if( surfi >= 0 )
					{
						ul[nul].pos[0] = x; ul[nul].pos[1] = y; ul[nul].pos[2] = z;
						ul[nul].surf = surf; ul[nul].vert = vert;

						nul++;
					}
				}
				else
				{
					// The old format
					string surf;
					int surfi;
					int vert;

					is >> surfi;

					if( surfi >= 0 )
					{
						if( meshes.size() > 0 ) surf = meshes[surfi];
						else surf = toString<int>(surfi);
					}

					// Ignore '.' in surface.vertice 
					is.get();

					is >> vert;
	
					if( surfi >= 0 )
					{
						ul[nul].surf = surf; ul[nul].vert = vert;

						nul++;
					}

					oldFile = true;
				}
			}

			if(temp == "LL")
			{
				// Is file of old format? Count number of '.' to find out.
				string temp2 = is.str();

				// Crop string to where we are in in-stream.
				temp2.erase(0, is.tellg());
			
				// Count the number of dots in the remainder of the string.
				int noDots = 0;
				for(string::iterator it = temp2.begin(); it != temp2.end(); it++)
				{
					if(*it == '.') noDots++;
				}

				// Set this as defined.
				ll[nll].defined = 1;

				if(noDots > 1)
				{
					// Here we know that it is the new format.
					float x, y, z;
					string surf;
					int surfi;
					int vert;

					is >> x; is >> y; is >> z;

					is >> surfi;

					if( surfi >= 0 )
					{
						if( meshes.size() > 0 ) surf = meshes[surfi];
						else surf = toString<int>(surfi);
					}

					// Ignore '.' in surface.vertice 
					is.get();

					is >> vert;
	
					if( surfi >= 0 )
					{
						ll[nll].pos[0] = x; ll[nll].pos[1] = y; ll[nll].pos[2] = z;
						ll[nll].surf = surf; ll[nll].vert = vert;

						nll++;
					}
				}
				else
				{
					// The old format
					string surf;
					int surfi;
					int vert;

					is >> surfi;

					if( surfi >= 0 )
					{
						if( meshes.size() > 0 ) surf = meshes[surfi];
						else surf = toString<int>(surfi);
					}

					// Ignore '.' in surface.vertice 
					is.get();

					is >> vert;

					if( surfi >= 0 )
					{
						ll[nll].surf = surf; ll[nll].vert = vert;

						nll++;
					}

					oldFile = true;
				}
			}
		}
		else
		{
			// Here we know we're reading a feature point.
			int group, n;
			float x,y,z;

			is >> group;
			
			// Ignore '.' in group.n
			is.get();
			
			is >> n;

			is >> x; is >> y; is >> z;

			if(x<-9999 && y<-9999 && z<-9999)
				continue;

			// Sets defined right here.
			setFPPos(group, n, x, y, z);
			
			string temp2 = is.str();

			// Crop string to where we are in in-stream.
			temp2.erase(0, is.tellg());
			
			// Is this the old or new file format?
			if(temp2.find('.') != std::string::npos)
			{
				string surf;
				int surfi;
				int vert;

				is >> surfi;

				if( surfi >= 0 )
				{
					if( meshes.size() > 0 ) surf = meshes[surfi];
					else surf = toString<int>(surfi);
				}

				// Ignore '.' in surface.vertice 
				is.get();

				is >> vert;

				if( surfi >= 0 )
					// And set it!
					setFPSurfVert(group, n, surf, vert);
				else
					setFPSurfVert( group, n, "", -1 );
			}
			else
				oldFile = true;
		}
	}

	f.close();

	mFilename.erase();
	mFilename = name;

	if(oldFile) return -1;

	return 1;
}

// Jonas added code
void FDP::saveToFile() const
{
/*	if( charModel == NULL ) //Commented out by IP so that this works without charModel
		return;
*/
	StringArray meshes;
	if(charModel)
		charModel->_getMeshes(meshes);

	int i, j, k,
		nul = (int)ul.size(), nll = (int)ll.size();

	vector<string> tempSave;
	tempSave.clear();

	fstream f;
	f.open(mFilename.c_str(), ios::out | ios::trunc);
	
	// f.seekp(0, ios::beg);
	
	if(!tempSave.empty())
		for(vector<string>::iterator theIt = tempSave.begin(); theIt != tempSave.end(); theIt++)
		{
			f << (*theIt);
			f << "\n";
		}
	else
	{
		// !! TODO !! Use multiligual string.
		f << "# FDP file.\n";
		f << "1.0 " << mFilename << " 0 0 0 0 0 0\n";
	}

	// Write FDP points.
	for(i =2;i<13;i++)
	{
		for(j = 0;j<groupSize(i);j++)
		{
			// Is point defined yet?
			if(fp[i][j].defined)
			{
				f << "    ";
				// FP number.
				f << i << "." << (int) (j + 1);
				f << " ";
				for(k = 0;k<3 ;k++)
				{
					f << (float) fp[i][j].pos[k];
					f << " ";
				}

				// Write index to surface.vertice
				int mi = 0;
				for( StringArray::const_iterator mit = meshes.begin(); mit != meshes.end(); ++mit )
				{
					if( *mit == fp[i][j].surf )
					{
						f << mi << "." << fp[i][j].vert; 
						break;
					}
					++mi;
				}
				f << "\n";
			}
		}
	}

	// Write upper lip points.
	f << "NUL  ";
	// vertex 
	f << nul;
	f << "\n";
	for(i =0;i<nul;i++)
	{
		f << "UL  ";
	
		// 3D vertex
		for(int k = 0;k<3 ;k++)
		{
			f << (float) ul[i].pos[k];
			f << " ";
		}

		// vertex 
		int mi = 0;
		for( StringArray::const_iterator mit = meshes.begin(); mit != meshes.end(); ++mit )
		{
			if( *mit == fp[i][j].surf )
				break;
			++mi;
		}
		f << mi << "." << ul[i].vert;
		f << "\n";
	}
	
	// Write lower lip points.
	f << "NLL  ";
	// vertex 
	f << nll;
	f << "\n";
	for(i =0;i<nll;i++)
	{
		f << "LL  ";

		// 3D vertex
		for(int k = 0;k<3 ;k++)
		{
			f << (float) ll[i].pos[k];
			f << " ";
		}

		// vertex 
		int mi = 0;
		for( StringArray::const_iterator mit = meshes.begin(); mit != meshes.end(); ++mit )
		{
			if( *mit == fp[i][j].surf )
				break;
			++mi;
		}
		f << mi << "." << ll[i].vert;
		f << "\n";
	}
	// Close fdp stream.
	f.flush();

	f.close();	
}

//	float MNS0,ENS0,ES0,MW0,IRISD0;
void FDP::setNormFactors()
{
	int j;
	// first set the FAPUs
	if(getFP(9,15).defined && getFP(8,3).defined)
		MNS0 = fabs((getFPPos(9,15))[1] - (getFPPos(8,3))[1]);
	else
		printf("Not enough feature points defined (nose, mouth). Animation synthesis will be incorrect!\n");
	if(getFP(3,11).defined && getFP(9,15).defined)
		ENS0 = fabs((getFPPos(3,11))[1] - (getFPPos(9,15))[1]);
	else
		if(getFP(3,7).defined && getFP(9,15).defined)
			ENS0 = fabs((getFPPos(3,7))[1] - (getFPPos(9,15))[1]);
		else
			if(getFP(3,5).defined && getFP(9,15).defined)
				ENS0 = fabs((getFPPos(3,5))[1] - (getFPPos(9,15))[1]);
			else
				printf("Not enough feature points defined (eyes). Animation synthesis will be incorrect!\n");
	
	if(getFP(3,5).defined && getFP(3,6).defined)
		ES0 = fabs((getFPPos(3,5))[0] - (getFPPos(3,6))[0]);
	else
		if(getFP(3,1).defined && getFP(3,2).defined)
			ES0 = fabs((getFPPos(3,1))[0] - (getFPPos(3,2))[0]);
		else
			if(getFP(3,13).defined && getFP(3,14).defined)
				ES0 = fabs((getFPPos(3,13))[0] - (getFPPos(3,14))[0]);
			else
				printf("Not enough feature points defined (eyes). Animation synthesis will be incorrect!\n");

	if(getFP(8,3).defined && getFP(8,4).defined)
		MW0 = fabs((getFPPos(8,3))[0] - (getFPPos(8,4))[0]);
	else
		printf("Not enough feature points defined (mouth). Animation synthesis will be incorrect!\n");
	
	if(getFP(3,1).defined && getFP(3,3).defined)
		IRISD0 = fabs((getFPPos(3,1))[1] - (getFPPos(3,3))[1]);
	else
		printf("Not enough feature points defined (eyes). Animation synthesis will be incorrect!\n");	

	// This is a trick. Upper part of the face was normalised using ENS0. This changes it
	// to IRISD without having to do it at every occurence. A good example of lazy and ugly programming.
	ENS0=IRISD0*4.0f;

	// set the feature points that are used in animation; set their normalization factors
	int gr = 2;
	for(j = 1;j <= 14;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	gr = 3;
	for(j = 1;j <= 4;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}
	for(j = 5;j <= 6;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		setFP( gr, j, fp );
	}
	for(j = 7;j <= 8;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}
	for(j = 9;j <= 10;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		setFP( gr, j, fp );
	}
	for(j = 11;j <= 12;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}
	for(j = 13;j <= 14;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		setFP( gr, j, fp );
	}

	gr = 4;
	for(j = 1;j <= 6;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	gr = 5;
	for(j = 1;j <= 2;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}
	for(j = 3;j <= 4;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	gr = 6;
	for(j = 1;j <= 4;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		setFP( gr, j, fp );
	}

	gr = 8;
	for(j = 1;j <= 8;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}
	for(j = 9;j <= 10;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		setFP( gr, j, fp );
	}

	gr = 9;
	for(j = 1;j <= 3;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}
	for(j = 4;j <= 5;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		setFP( gr, j, fp );
	}
	for(j = 6;j <= 7;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}
	for(j = 8;j <= 11;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		setFP( gr, j, fp );
	}
	for(j = 12;j <= 14;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		fp.animated = 1;
		setFP( gr, j, fp );
	}
	for(j = 15;j <= 15;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	gr = 10;
	for(j = 1;j <= 6;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		setFP( gr, j, fp );
	}
	for(j = 7;j <= 10;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	gr = 11;
	for(j = 1;j <= 3;j++)
	{
		FeaturePoint fp = getFP(gr,j);
		if(!fp.defined)
			continue;
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	gr = 12;
	for (j = 1; j <= 2; j++)
	{
		FeaturePoint fp = getFP(gr, j);

		if (!fp.defined) continue;

		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	FeaturePoint fp = getFP(gr, 3);
	
	if (fp.defined)
	{
		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	for (j = 4; j <= 11; j++)
	{
		FeaturePoint fp = getFP(gr, j);

		if (!fp.defined) continue;

		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	for (j = 12; j <= 15; j++)
	{
		FeaturePoint fp = getFP(gr, j);

		if (!fp.defined) continue;

		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	fp = getFP(gr, 16);
	
	if (fp.defined)
	{
		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	for (j = 18; j <= 19; j++)
	{
		FeaturePoint fp = getFP(gr, j);

		if (!fp.defined) continue;

		fp.norm[0] = MW0;
		fp.norm[1] = MNS0*3;
		fp.norm[2] = MNS0*3;
		fp.animated = 1;
		fp.normalizing = 1;
		setFP( gr, j, fp );
	}

	for (j = 20; j <= 23; j++)
	{
		FeaturePoint fp = getFP(gr, j);

		if (!fp.defined) continue;

		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		setFP( gr, j, fp );
	}

	for (j = 26; j <= 27; j++)
	{
		FeaturePoint fp = getFP(gr, j);

		if (!fp.defined) continue;

		fp.norm[0] = ES0;
		fp.norm[1] = ENS0*2;
		fp.norm[2] = ENS0*2;
		setFP( gr, j, fp );
	}
}

void FDP::parseFPName( const string& name, int& group, int& n )
{
	string foo(name);					// these two lines are to avoid side effects on *_name
	char* tname = (char*)foo.c_str();

	char *gs,*ns;
	
	gs = tname;
	ns = strchr(tname,'.');
	ns[0] = 0;
	ns++;
	
	group = atoi(gs);
	n = atoi(ns);
}

string FDP::getFPName( int group, int n )
{
	ostringstream oss;
	oss << group << "." << n;
	return oss.str();
}

bool FDP::FPIsValid( int group, int n )
{
	if( group >= 2 &&
		group <= 12 &&
		n >= 1 &&
		n <= groupSize(group) )
		return true;

	return false;
}

bool FDP::FPIsValid( const string& name )
{
	int g, n;
	parseFPName( name, g, n );
	return FPIsValid( g, n );
}

const int FDP::groupSizes[FP_NUMBER_OF_GROUPS] = {14,14,6,4,4,1,10,15,10,6,47};

int FDP::groupSize( int group )
{
	#ifdef _DEBUG
	assert( group >= FP_START_GROUP_INDEX && group <= FP_END_GROUP_INDEX );
	#endif

	return groupSizes[group-FP_START_GROUP_INDEX];
}

}

