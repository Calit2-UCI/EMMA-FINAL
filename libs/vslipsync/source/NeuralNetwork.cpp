#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <stdlib.h>
#include <cerrno>
//#include <memory.h>
#include <stdio.h>

#include <cmath>
#include <ctype.h>

#include <iostream>
#include <fstream>

using namespace std;

#ifdef IPHONE
	#import <UIKit/UIKit.h>
#endif


#include "NeuralNetwork.h"

#define NUM_DATA       14
#define LO            -1
#define HI            +1
#define BIAS           1
#define NUM_LAYERS    3
//#define N             30
#define N             12   //Tree layers, 2 hidden, one output, 12 and 6 neurons in
							//first and second layer

#define M             1

int					 middle;



void GenerateNetwork(NET* Net, int hidden)
{
  int l,i;

  middle = hidden;	

  int                  Units[NUM_LAYERS] = {N, middle, M};	

  Net->Layer = (LAYER**) calloc(NUM_LAYERS, sizeof(LAYER*));
   
  for (l=0; l<NUM_LAYERS; l++) {
    Net->Layer[l] = (LAYER*) malloc(sizeof(LAYER));
      
    Net->Layer[l]->Units      = Units[l];
    Net->Layer[l]->Output     = (double*)  calloc(Units[l]+1, sizeof(double));
    Net->Layer[l]->Error      = (double*)  calloc(Units[l]+1, sizeof(double));
    Net->Layer[l]->Weight     = (double**) calloc(Units[l]+1, sizeof(double*));
    Net->Layer[l]->WeightSave = (double**) calloc(Units[l]+1, sizeof(double*));
    Net->Layer[l]->dWeight    = (double**) calloc(Units[l]+1, sizeof(double*));
    Net->Layer[l]->Output[0]  = BIAS;
      
    if (l != 0) {
      for (i=1; i<=Units[l]; i++) {
        Net->Layer[l]->Weight[i]     = (double*) calloc(Units[l-1]+1, sizeof(double));
        Net->Layer[l]->WeightSave[i] = (double*) calloc(Units[l-1]+1, sizeof(double));
        Net->Layer[l]->dWeight[i]    = (double*) calloc(Units[l-1]+1, sizeof(double));
      }
    }
  }
  //bas i nije dobro, na sto pokazuje sad Net->InputLayer?? kako rijesiti?!
  Net->InputLayer  = Net->Layer[0];
  Net->OutputLayer = Net->Layer[NUM_LAYERS - 1];
  Net->Alpha       = 0.9;
  Net->Eta         = 0.05; 
  Net->Gain        = 1;

}

void FreeNetwork(NET* Net)
{
	// Changed by KStankovic
	if (!Net == 0) 
	{
		if (Net->Layer != NULL)
		{
			int l;
			for (l = 0; l < NUM_LAYERS; l++) 
			{
				//ADDED BY G.
				int i;
				for (i=1; i<=Net->Layer[l]->Units; i++) 
				{
					if (Net->Layer[l]->Weight[i] != NULL) free(Net->Layer[l]->Weight[i]);
					if (Net->Layer[l]->WeightSave[i] != NULL) free(Net->Layer[l]->WeightSave[i]);
					if (Net->Layer[l]->dWeight[i] != NULL) free(Net->Layer[l]->dWeight[i]);
				}
				free(Net->Layer[l]->Weight);
				free(Net->Layer[l]->WeightSave);
				free(Net->Layer[l]->dWeight);
				free(Net->Layer[l]->Error);
				free(Net->Layer[l]->Output);
				//
				if (Net->Layer[l] != NULL) free(Net->Layer[l]);
				//
			}
			free(Net->Layer);
			//free(Net->InputLayer);
			//free(Net->OutputLayer);
		}
	}
	//
}

void GetWeights(NET* Net,char *filename) //Ovdje uzima tezine fileova
{

		printf ("Getting weights of %s\n", filename);
	
#ifdef IPHONE
	
	NSString *fileString =	[NSString stringWithUTF8String:filename];
	NSArray *realFileName = [fileString componentsSeparatedByString:@"/"];
	fileString = (NSString *) [realFileName lastObject];
	
	NSString *path = [ [ [NSBundle mainBundle] resourcePath ] stringByAppendingPathComponent:fileString ];

	
	filename = (char*)[path cStringUsingEncoding:1];
	
	
#endif

//		FILE * inFile;
//		inFile = fopen(filename,"r");
//
//		int i, j, l;
//		double weight;
//
//		l = 1;
//		i = 1;
//		j = 0;
//
//		if(!inFile) {
//			printf ("Unable to open file at %s\n", filename);
//			printf ("ERRNO #: %d\n", errno);
//			//exit(1); // terminate with error
//			return;
//		}
//
//		int res;
//		while ((res = fscanf(inFile, "%lf", &weight)) != -1){
//			if (res == 0) {
//				std::cerr << "Early matching error" << std::endl;
//				break;
//			}
//			Net->Layer[l]->Weight[i][j] = weight;
//			j ++;
//			if (j > Net->Layer[l-1]->Units){
//				j = 0;
//				i++;
//				if (i > Net->Layer[l]->Units){
//					j = 0;
//					i = 1;
//					l++;
//				}
//			}
//		 }
//
//		fclose(inFile);

	int i, j, l;
	double weight;

	l = 1;
	i = 1;
	j = 0;

	ifstream indata; // indata is like cin
//		double num; // variable for input value
	indata.open(filename); // opens the file
	if(!indata) { // file couldn't be opened
		cerr << "Error: file could not be opened" << endl;
		exit(1);
	}
	indata >> weight;
	while ( !indata.eof() ) { // keep reading until end-of-file
//		cout << "The next number is " << weight << endl;
		Net->Layer[l]->Weight[i][j] = weight;
		j ++;
		if (j > Net->Layer[l-1]->Units){
			j = 0;
			i++;
			if (i > Net->Layer[l]->Units){
				j = 0;
				i = 1;
				l++;
			}
		}
		indata >> weight; // sets EOF flag if no value found
	}
	indata.close();
//	cout << "End-of-file reached.." << endl;
	return;

/*
		for (l=1; l<NUM_LAYERS; l++) {
		 for (i=1; i<=Net->Layer[l]->Units; i++) {
			  for (j=0; j<=Net->Layer[l-1]->Units; j++) {
				  Net->Layer[l]->WeightSave[i][j] = Net->Layer[l]->Weight[i][j];
	//	fprintf(net, "%.8f\n",Net->Layer[l]->WeightSave[i][j] );
				fout << Net->Layer[l]->Weight[i][j]<<endl;
	//	fprintf(n,"");
      }
    }
  }
		fout.close();*/
}

void SetInput(NET* Net, double* Input)
{
  int i;
   
  for (i=1; i<=Net->InputLayer->Units; i++) {
    Net->InputLayer->Output[i] = Input[i-1];
  }
}


void GetOutput(NET* Net, double* RealOutput)
{
  int i;
   
  for (i=1; i<=Net->OutputLayer->Units; i++) {
    RealOutput[i-1] = Net->OutputLayer->Output[i];
  }
}

/******************************************************************************
                     P R O P A G A T I N G   S I G N A L S
 ******************************************************************************/


void PropagateLayer(NET* Net, LAYER* Lower, LAYER* Upper, int l)
{
  int  i,j;
  double Sum;

  for (i=1; i<=Upper->Units; i++) {
    Sum = 0;
    for (j=0; j<=Lower->Units; j++) {
	//	i = Upper->Units;
      Sum += Upper->Weight[i][j] * Lower->Output[j];
    }
    Upper->Output[i] = 1 / (1 + exp(-Net->Gain *2* Sum));
	if (l == NUM_LAYERS-2)
		Upper->Output[i] = 1 / (1 + exp(-Net->Gain * Sum));
  }
}


void PropagateNet(NET* Net)
{
  int l;
   
  for (l=0; l<NUM_LAYERS-1; l++) {
    PropagateLayer(Net, Net->Layer[l], Net->Layer[l+1],l);
  }
}

/******************************************************************************
                      S I M U L A T I N G   T H E   N E T
 ******************************************************************************/


void SimulateNet(NET* Net, double* Input, double * RealOutput/*, BOOL Training, BOOL Protocoling*/)
{
 

  SetInput(Net, Input);
  PropagateNet(Net);
  GetOutput(Net, RealOutput);
 // cout<<RealOutput[0]<<endl;
   

}


