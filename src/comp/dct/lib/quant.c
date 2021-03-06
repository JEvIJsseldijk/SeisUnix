/* Copyright (c) Colorado School of Mines, 2011.*/
/* All rights reserved.                       */


/*********************** self documentation **********************/
/******************************************************************************
QUANT - QUANTization routines

uniQuant - uniform quantization with a given relative RMS error
uniDequant - uniform dequantization

*******************************************************************************
Function Prototypes:
void uniQuant(float *x, int n, float error, 
		float *ave, float *step, int *qx);
void uniDequant(float *x, int n, float ave, float step, int *qx)

*******************************************************************************
uniQuant:
Input:
x		array[] of input signal
n               length of the signal
error		relative RMS error
ave             average of the input signal
step            stepsize used in quantization
Output:
qx              array[] output integers

uniDequant:
Input:
n               length of the signal
ave             average of the signal
step            stepsize used in quantization
qx              array[] input integers
Output:
x		array[] of output signal

*******************************************************************************
Author:		Tong Chen, 07/15/94
Modifier:       Tong Chen, 05/30/95, for unified interface
******************************************************************************/

#include "comp.h"
#define ERRATIO 3.4641




void uniQuant(float *x, int n, float error, 
	      float *ave, float *step, int *qx)
/******************************************************************************
uniform quantization with a given relative RMS error
*******************************************************************************
x		array[] of input signal
n               length of the signal
error		relative RMS error
ave             average of the input signal
step            stepsize used in quantization
qx              array[] output integers
******************************************************************************/
{
   int i;
   float rn, atmp, dev, lave, lstep;
   float *g;

   /* allocate temporary space */
   g = alloc1float(n);
   
   rn = 1./n;
   
   lave = 0.;

   /* average, or mean-value */
   for(i=0; i<n; i++) lave += x[i];
   lave *= rn;
   lstep = *step;

/*
   fprintf(stderr,"average=%f\n", lave);

   for(i=0; i<n; i++)
      fprintf(stderr,"f[%d]=%f\n", i, x[i]);
*/

   /* if no deviation calculated */
   if(lstep < 0.)
   {
      
      dev = 0.;
      /* standard deviation, or RMS */
      for(i=0; i<n; i++)
      {
	 g[i] = x[i] - lave;
	 atmp = ABS(g[i]);
	 dev += atmp*atmp;
      }
   
      dev *= rn;
      dev = sqrt(dev);
   }
   
   /* else */
   else{ 
      for(i=0; i<n; i++)
	 g[i] = x[i] - lave;
      dev = lstep;
   }
   
   /* stepsize used in quantization */
   lstep = dev*error*ERRATIO;
   lstep = 1./lstep;

   fprintf(stderr,"lstep=%f\n", lstep);

   /* uniform quantization */
   for(i=0; i<n; i++)
   {
      
      atmp = g[i]*lstep;
/*
      qx[i] = NINT(atmp);
*/
      qx[i] = (atmp > 0.)? ((int) (atmp+.5)) : ((int) (atmp-.5)); 
   }

   fprintf(stderr,"after quantization\n");

   /* average and stepsize */
   *ave = lave;
   *step = lstep;
   
   /* free the workspace */
   free1float(g);
}


void uniDequant(float *x, int n, float ave, float step, int *qx)
/******************************************************************************
uniform quantization with a given relative RMS error
*******************************************************************************
x		array[] of output signal
n               length of the signal
ave             average of the signal
step            stepsize used in quantization
qx              array[] input integers
******************************************************************************/
{
   int i;
   float rstep;

   rstep = 1./step;
   
   for(i=0; i<n; i++) x[i] = ave + qx[i]*rstep;
}
