/*M*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1995 Intel Corporation. All Rights Reserved.
//
//  PVCS:
//      $Workfile: nspatan.h $
//      $Revision: 3 $
//      $Modtime: 9/01/99 4:22p $
//
//  Purpose:
//          Header file for function nspAtan.
//
*M*/

#ifdef __cplusplus
extern "C" {
#endif
#if !defined (_NSPATAN_H) || (defined _OWN_BLDPCS)
   #define _NSPATAN_H

   NSPAPI(void, nspsbArctan1, ( float  *vec, int len))
   NSPAPI(void, nspsbArctan2, (const float  *src, float  *dst, int len))
   NSPAPI(void, nspdbArctan1, (double  *vec, int len))
   NSPAPI(void, nspdbArctan2, (const double *src, double *dst, int len))

#endif                                  /* _NSPLNEXP_H  */
#ifdef __cplusplus
}
#endif





