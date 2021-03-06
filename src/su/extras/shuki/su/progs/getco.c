#include <stdio.h>
#include "../include/su.h"
#define BLOCK 1000000
extern bool verbose;
extern int nalloc,dalloc;

getco(infd,acoff,anewoffset)
Section *acoff;
int infd,*anewoffset;
{
	int nx,nread=0,offset,cmp0,cmp,rgettr,newoffset;
	static nt4,idx;
	static bool first=true;
	float ratio;
	int iratio;
	static Sutrace tr;

	if(first) {

		tr.data = (float*)malloc(acoff->n1*sizeof(float));

		/* READ THE FIRST TRACE */
		nread = gettr(infd,&tr);
		if(!nread) err(__FILE__,__LINE__,"getco: can't get first trace\n");

		nt4 = acoff->n1*sizeof(float);

		/* (FIRST) MEMORY ALLOCATION */
		nalloc = MAX(BLOCK,nt4);
		dalloc = nalloc;
		acoff->data = (float*) malloc(nalloc);
		if(acoff->data==NULL) err(__FILE__,__LINE__,"Can't malloc %d bytes",nalloc);
	}

	offset = tr.offset;
	cmp0 = tr.cdp;
	acoff->o2 = cmp0;

	bcopy(tr.data,acoff->data,nt4);

	/* READ THE REST OF THE CO (+ MAYBE FIRST TRACE OF NEXT ONE) */
	
	for(nx=1,cmp=cmp0,newoffset=offset;(rgettr=gettr(infd,&tr))>0;nx++,cmp=tr.cdp) {

		/* IS IT A NEW OFFSET */
		if( tr.offset != offset) {
			newoffset = tr.offset;
			if(verbose) fprintf(stderr,"getco: New offset=%d Current offset=%d\n",tr.offset,offset);
			break;
		}

		nread += rgettr;

		if(nx==1) {
			if(first) { /* FIND THE INPUT MIDPOINT INTERVAL */

				idx = tr.cdp - cmp0;
				if(idx == 0) err(__FILE__,__LINE__,"idx=%d , Is input a CO section? \n",idx);
			}

		}

		/* CHECK FOR MISSING TRACES MIDPOINT */

		ratio = (float)(tr.cdp-cmp)/(float)idx;
/* 		ratio = fabs(ratio); */

		iratio = ratio - 0.5;

		if(iratio<0)
			err(__FILE__,__LINE__,"Zero or negative gap in input (tracl=%d)",tr.tracl);

		if (iratio)
		warn(__FILE__,__LINE__,"Gap in input. Old cdp = %d, new cdp = %d. Missing %d traces",
							cmp,tr.cdp,iratio);

		/* MEMORY REALLOCATION */
		if( (nx+1+iratio)*nt4 >= nalloc) {
			nalloc += dalloc;
			acoff->data = (float*) realloc(acoff->data,nalloc);
			if(acoff->data==NULL)
				err(__FILE__,__LINE__,"Can't realloc %d bytes",nalloc);
		}

		if(iratio) bzero(acoff->data+nx*acoff->n1,nt4*iratio);

		nx += iratio;

		bcopy(tr.data,acoff->data+nx*acoff->n1,nt4);

	}

	first = false;

	acoff->d2 = idx;
	acoff->n2 = nx;
	acoff->val.i = offset;
/* 	acoff->d1 = 0.000001*tr.dt; */
	*anewoffset = newoffset;

	return(nread);
}
