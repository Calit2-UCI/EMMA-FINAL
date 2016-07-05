#ifndef __HEADER_NEURALNETWORK_H
#define __HEADER_NEURALNETWORK_H

#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vs2005 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#ifdef __cplusplus
extern "C" {
#endif






typedef struct {                     /* A LAYER OF A NET:                     */
        int          Units;         /* - number of units in this layer       */
        double*         Output;        /* - output of ith unit                  */
        double*         Error;         /* - error term of ith unit              */
        double**        Weight;        /* - connection weights to ith unit      */
        double**        WeightSave;    /* - saved weights for stopped training  */
        double**        dWeight;       /* - last weight deltas for momentum     */
} LAYER;

typedef struct {                     /* A NET:                                */
        LAYER**       Layer;         /* - layers of this net                  */
        LAYER*        InputLayer;    /* - input layer                         */
        LAYER*        OutputLayer;   /* - output layer                        */
        double          Alpha;         /* - momentum factor                     */
        double          Eta;           /* - learning rate                       */
        double          Gain;          /* - gain of sigmoid function            */
        double          Error;         /* - total net error                     */
} NET;


void GenerateNetwork(NET* Net, int hidden);
void FreeNetwork(NET* Net);
void GetWeights(NET* Net,char *filename);


void SetInput(NET* Net, double* Input);
void GetOutput(NET* Net, double* Output);
void PropagateLayer(NET* Net, LAYER* Lower, LAYER* Upper, int l);
void PropagateNet(NET* Net);
void SimulateNet(NET* Net, double* Input, double *Output/*, INT* Target, BOOL Training, BOOL Protocoling*/);


#ifdef __cplusplus
}
#endif

#endif // __HEADER_NEURAL_NETWORK

