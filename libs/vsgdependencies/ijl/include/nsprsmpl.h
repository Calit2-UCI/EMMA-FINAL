/*M*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1996 Intel Corporation. All Rights Reserved.
//
//      $Workfile: nsprsmpl.h $
//      $Revision: 3 $
//      $Modtime: 20/12/99 4:39p $
//
//  Purpose: resampling functions declarations.
*M*/

/*==========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
#if !defined (_NSPRSMPL_H) || defined (_OWN_BLDPCS)

   #define _NSPRSMPL_H

/* ======================================================================== */

   #if !defined (_OWN_BLDPCS)
   typedef struct {
      int    nFactors;
      int    nTaps;
      int    sLen;
      int    dLen;
      int    isSampInit;
      float *factorRange;
      float *freq;
      void  *taps;
      NSPFirState firState;
   } NSPSampState;
   #endif
   NSPAPI(NSPStatus,nspsSampInit,(NSPSampState* sampSt, float* factorRange,
                                  float* freq, int nFactors, int nTaps ))

   NSPAPI(NSPStatus,nspdSampInit,(NSPSampState* sampSt, float* factorRange,     
                                  float* freq, int nFactors, int nTaps ))

   NSPAPI(NSPStatus,nspsSamp,(NSPSampState* sampSt,const float* src,int srcLen,   
                              float* dst,int dstLen))
   NSPAPI(NSPStatus,nspdSamp,(NSPSampState* sampSt,const double* src,int srcLen,
                              double* dst,int dstLen))
   NSPAPI(void, nspSampFree, ( NSPSampState* sampSt ))

#endif
#ifdef __cplusplus
}
#endif
