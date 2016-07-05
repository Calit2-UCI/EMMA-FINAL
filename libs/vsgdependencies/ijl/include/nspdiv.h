/*M*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1998-2000 Intel Corporation. All Rights Reserved.
//
//      $Workfile: nspdiv.h $
//      $Revision: 3 $
//      $Modtime: 6/10/99 4:40p $
//
//  Purpose: NSP Vector Arithmetics
*M*/

#if !defined (_NSPDIV_H) || defined (_OWN_BLDPCS)
# define _NSPDIV_H

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*/
/*                 Vector division functions                             */
/*-------------------------------------------------------------------------*/


/*F*
//  FUNCTION
//    nsp?bDiv1
//
//  DESCRIPTION
//    Performs an element-wise division of elements
//    of vectors and value.
//
//       dst[i] /= val
//
//  PARAMETERS
//    val  value operand
//    dst  Pointer to the input/output vector.
//    n    The number of elements to be operated on.
//
//  RETURN
//    status
//
*F*/

NSPAPI(int,nspsbDiv1,(float val,float *dst,int n))
NSPAPI(int,nspdbDiv1,(double val,double *dst,int n))
NSPAPI(int,nspwbDiv1,(short val,short *dst,int n))
NSPAPI(int,nspcbDiv1,(SCplx val,SCplx *dst,int n))
NSPAPI(int,nspzbDiv1,(DCplx val,DCplx *dst,int n))
NSPAPI(int,nspvbDiv1,(WCplx val,WCplx *dst,int n))

/*F*
//  FUNCTION
//    nspsbDiv2
//
//  DESCRIPTION
//    Division the elements of two vectors and stores the results
//    in the output array.
//
//       dst[i] /= src[i]
//
//  PARAMETERS
//    src  Pointer to the vector to be Div
//    dst  Pointer to the vector dst which stores the results
//         of the Div operation src and dst.
//    n    The number of elements to be operated on.
//
//  RETURN
//    status
//
*F*/

NSPAPI(int,nspsbDiv2,(const float *src,float *dst,int n))
NSPAPI(int,nspdbDiv2,(const double *src,double *dst,int n))
NSPAPI(int,nspwbDiv2,(const short *src,short *dst,int n))
NSPAPI(int,nspcbDiv2,(const SCplx *src,SCplx *dst,int n))
NSPAPI(int,nspzbDiv2,(const DCplx *src,DCplx *dst,int n))
NSPAPI(int,nspvbDiv2,(const WCplx *src,WCplx *dst,int n))

/*F*
//  FUNCTION
//    nspsbDiv3
//
//  DESCRIPTION
//    Performs an element-wise operation of elements
//    of vectors and stores the results in a third.
//
//       dst[i] = srcA[i] / srcB[i]
//
//  PARAMETERS
//    srcA Points to the first source vector
//    srcB Pointers to the second source vector.
//    dst  Pointer to the vector dst which stores the results
//         of the Div operation srcA and srcB.
//    n    The number of elements to be operated on.
//
//  RETURN
//    status
//
*F*/

NSPAPI(int,nspsbDiv3,(const float *srcA,const float *srcB,float *dst,int n))
NSPAPI(int,nspdbDiv3,(const double *srcA,const double *srcB,double *dst,int n))
NSPAPI(int,nspwbDiv3,(const short *srcA,const short *srcB,short *dst,int n))
NSPAPI(int,nspcbDiv3,(const SCplx *srcA,const SCplx *srcB,SCplx *dst,int n))
NSPAPI(int,nspzbDiv3,(const DCplx *srcA,const DCplx *srcB,DCplx *dst,int n))
NSPAPI(int,nspvbDiv3,(const WCplx *srcA,const WCplx *srcB,WCplx *dst,int n))


#ifdef __cplusplus
}
#endif

#endif
