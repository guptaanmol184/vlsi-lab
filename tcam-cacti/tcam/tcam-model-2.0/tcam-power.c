/*------------------------------------------------------------
 * UC Santa Barbara ArchLab
 * Ternary CAM (TCAM) Power and Delay Model - Release 2.0
 *
 * Copyright (c) 2006 The Regents of the University of California.
 * All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for educational, research and non-profit purposes,
 * without fee, and without a written agreement is hereby granted,
 * provided that the above copyright notice, this paragraph and the
 * following three paragraphs appear in all copies.
 *
 * Permission to incorporate this software into commercial products may
 * be obtained by contacting the University of California. For
 * information about obtaining such a license contact:
 * Banit Agrawal <banit@cs.ucsb.edu>
 * Tim Sherwood  <sherwood@cs.ucsb.edu>
 *
 * This software program and documentation are copyrighted by The Regents
 * of the University of California. The software program and
 * documentation are supplied "as is", without any accompanying services
 * from The Regents. The Regents does not warrant that the operation of
 * the program will be uninterrupted or error-free. The end-user
 * understands that the program was developed for research purposes and
 * is advised not to rely exclusively on the program for any reason.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND
 * ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. THE UNIVERSITY OF
 * CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *------------------------------------------------------------*/
/*------------------------------------------------------------
 *                              CACTI 3.0
 *               Copyright 2002 Compaq Computer Corporation
 *                         All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein,
 * and hereby grant back to Compaq a non-exclusive, unrestricted, royalty-
 * free right and license under any changes, enhancements or extensions
 * made to the core functions of the software, including but not limited to
 * those affording compatibility with other hardware or software
 * environments, but excluding applications which incorporate this software.
 * Users further agree to use their best efforts to return to Compaq any
 * such changes, enhancements or extensions that they make and inform Compaq
 * of noteworthy uses of this software.  Correspondence should be provided
 * to Compaq at:
 *
 *                       Director of Licensing
 *                       Western Research Laboratory
 *                       Compaq Computer Corporation
 *                       250 University Avenue
 *                       Palo Alto, California  94301
 *
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND COMPAQ COMPUTER CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL COMPAQ COMPUTER
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *------------------------------------------------------------*/

#include <math.h>
#include "stdio.h"

#include "tcam-power.h"
#include "itrs_interconnect.h"
#include "leakage.h"


float VddPow=4.5;


int powers (int base, int n) {
int i,p;

  p = 1;
  for (i = 1; i <= n; ++i)
     p *= base;
  return p;
}

/*----------------------------------------------------------------------*/

double logtwo(x)
double x;
{
if (x<=0) printf("%e\n",x);
	return( (double) (log(x)/log(2.0)) );
}
/*----------------------------------------------------------------------*/

double gatecap(width,wirelength) /* returns gate capacitance in Farads */
double width;		/* gate width in um (length is Leff) */
double wirelength;	/* poly wire length going to gate in lambda */
{
	return(width*Leff*Cgate+wirelength*Cpolywire*Leff);
}

double gatecappass(width,wirelength) /* returns gate capacitance in Farads */
double width;           /* gate width in um (length is Leff) */
double wirelength;      /* poly wire length going to gate in lambda */
{
        return(width*Leff*Cgatepass+wirelength*Cpolywire*Leff);
}


/*----------------------------------------------------------------------*/

/* Routine for calculating drain capacitances.  The draincap routine
 * folds transistors larger than 10um */

double draincap(width,nchannel,stack)  /* returns drain cap in Farads */
double width;		/* um */
int nchannel;		/* whether n or p-channel (boolean) */
int stack;		/* number of transistors in series that are on */
{
	double Cdiffside,Cdiffarea,Coverlap,cap;

	Cdiffside = (nchannel) ? Cndiffside : Cpdiffside;
	Cdiffarea = (nchannel) ? Cndiffarea : Cpdiffarea;
	Coverlap = (nchannel) ? (Cndiffovlp+Cnoxideovlp) :
				(Cpdiffovlp+Cpoxideovlp);
	/* calculate directly-connected (non-stacked) capacitance */
	/* then add in capacitance due to stacking */
	if (width >= 10) {
	    cap = 3.0*Leff*width/2.0*Cdiffarea + 6.0*Leff*Cdiffside +
		width*Coverlap;
	    cap += (double)(stack-1)*(Leff*width*Cdiffarea +
		4.0*Leff*Cdiffside + 2.0*width*Coverlap);
	} else {
	    cap = 3.0*Leff*width*Cdiffarea + (6.0*Leff+width)*Cdiffside +
		width*Coverlap;
	    cap += (double)(stack-1)*(Leff*width*Cdiffarea +
		2.0*Leff*Cdiffside + 2.0*width*Coverlap);
	}
	return(cap);
}

/*----------------------------------------------------------------------*/

/* The following routines estimate the effective resistance of an
   on transistor as described in the tech report.  The first routine
   gives the "switching" resistance, and the second gives the 
   "full-on" resistance */

double transresswitch(width,nchannel,stack)  /* returns resistance in ohms */
double width;		/* um */
int nchannel;		/* whether n or p-channel (boolean) */
int stack;		/* number of transistors in series */
{
	double restrans;
        restrans = (nchannel) ? (Rnchannelstatic):
                                (Rpchannelstatic);
	/* calculate resistance of stack - assume all but switching trans
	   have 0.8X the resistance since they are on throughout switching */
	return((1.0+((stack-1.0)*0.8))*restrans/width);	
}

/*----------------------------------------------------------------------*/

double transreson(width,nchannel,stack)  /* returns resistance in ohms */
double width;           /* um */
int nchannel;           /* whether n or p-channel (boolean) */
int stack;              /* number of transistors in series */
{
        double restrans;
        restrans = (nchannel) ? Rnchannelon : Rpchannelon;

      /* calculate resistance of stack.  Unlike transres, we don't
           multiply the stacked transistors by 0.8 */
        return(stack*restrans/width);

}

/*----------------------------------------------------------------------*/

/* This routine operates in reverse: given a resistance, it finds
 * the transistor width that would have this R.  It is used in the
 * data wordline to estimate the wordline driver size. */

double restowidth(res,nchannel)  /* returns width in um */
double res;            /* resistance in ohms */
int nchannel;          /* whether N-channel or P-channel */
{
   double restrans;

        restrans = (nchannel) ? Rnchannelon : Rpchannelon;

   return(restrans/res);

}

/*----------------------------------------------------------------------*/

double horowitz(inputramptime,tf,vs1,vs2,rise)
double inputramptime,    /* input rise time */
       tf,               /* time constant of gate */
       vs1,vs2;          /* threshold voltages */
int rise;                /* whether INPUT rise or fall (boolean) */
{
    double a,b,td;

    a = inputramptime/tf;
    if (rise==RISE) {
       b = 0.5;
       td = tf*sqrt( log(vs1)*log(vs1)+2*a*b*(1.0-vs1)) +
            tf*(log(vs1)-log(vs2));
    } else {
       b = 0.4;
       td = tf*sqrt( log(1.0-vs1)*log(1.0-vs1)+2*a*b*(vs1)) +
            tf*(log(1.0-vs1)-log(1.0-vs2));
    }
    return(td);
}


/*----------------------------------------------------------------------*/

void
tcam_subbank_routing_length (struct parameters tcam_parameters, double *subbank_v, double *subbank_h)
{
    double htree ;
    int htree_int ;
    uint32_t Nrd ;
    double sub_h, sub_v;
    double inter_h, inter_v;
    uint32_t rows, columns ;
    uint32_t NSubbanks, inter_subbanks;

    Nrd = tcam_parameters.Nrd ;
    rows = tcam_parameters.number_of_rows / (tcam_parameters.number_of_subbanks * Nrd ) ;
    columns = tcam_parameters.number_of_column_bits ;
    NSubbanks = tcam_parameters.number_of_subbanks ;

    if(Nrd==1) {
	   sub_v= rows;
	   sub_h= columns;
    }
    if(Nrd==2) {
	   sub_v= rows;
	   sub_h= 2*columns;
    }

    if(Nrd>2) {
	htree=logtwo((double)(Nrd));
	htree_int = (int) htree;
	if (htree_int % 2 ==0) {
	  sub_v = sqrt(Nrd)*rows;
	  sub_h = sqrt(Nrd)*columns;
	}
	else {
	  sub_v = sqrt(Nrd/2)*rows;
	  sub_h = 2*sqrt(Nrd/2)*columns;
	}
    }
	
    inter_v=sub_v;
    inter_h=sub_h;

    sub_v=0;
    sub_h=0;

    if(NSubbanks==1.0 || NSubbanks==2.0 ) {
     	sub_h = 0;
     	sub_v = 0;
    }
    if(NSubbanks==4.0) {
     	sub_h = 0;
     	sub_v = inter_v;
    }

    inter_subbanks=NSubbanks;

    while((inter_subbanks > 2.0) && (NSubbanks > 4.0))
     {
	sub_v+=inter_v;
	sub_h+=inter_h;

	inter_v=2*inter_v;
	inter_h=2*inter_h;
	inter_subbanks=inter_subbanks/4.0;

	if(inter_subbanks==4.0) {
		inter_h = 0; 
        }
     }

     *subbank_v=sub_v;
     *subbank_h=sub_h;
}


void 
tcam_subbank_dimension(struct parameters tcam_parameters, double *subbank_h, double *subbank_v)
{
    double htree ;
    int htree_int ;
    uint32_t Nrd ;
    double sub_h, sub_v;
    uint32_t rows, columns ;

    Nrd = tcam_parameters.Nrd ;
    rows = tcam_parameters.number_of_rows / (tcam_parameters.number_of_subbanks * Nrd ) ;
    columns = tcam_parameters.number_of_column_bits ;

    if(Nrd==1) {
	   sub_v= rows;
	   sub_h= columns;
    }
    if(Nrd==2) {
	   sub_v= rows;
	   sub_h= 2*columns;
    }

    if(Nrd>2) {
	htree=logtwo((double)(Nrd));
	htree_int = (int) htree;
	if (htree_int % 2 ==0) {
	  sub_v = sqrt(Nrd)*rows;
	  sub_h = sqrt(Nrd)*columns;
	}
	else {
	  sub_v = sqrt(Nrd/2)*rows;
	  sub_h = 2*sqrt(Nrd/2)*columns;
	}
    }

   *subbank_v=sub_v;
   *subbank_h=sub_h;

   return;
}


double
tcam_subbanks_routing_power(struct parameters tcam_parameters)
{
       double power;
       double Ceq,Ceq_outdrv;
       int i,blocks,htree_int,subbank_mod;
       double htree, wire_cap, wire_cap_h, start_h,start_v,line_h,line_v;
       double subbank_v, subbank_h;

       uint32_t number_of_column_bits = tcam_parameters.number_of_column_bits;
       uint32_t NSubbanks = tcam_parameters.number_of_subbanks ;
	
       power=0.0;
       tcam_subbank_dimension ( tcam_parameters, &subbank_h, &subbank_v );

       if(NSubbanks==4.0) {
		/* calculation of address routing power */
		wire_cap = Cmetal * (subbank_v) * (TCAMBitHeight + 2 * WIRESPACING );
		Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
		      gatecap(Wdecdrivep+Wdecdriven,0.0);
		power+=2*number_of_column_bits*Ceq*.5*VddPow*VddPow;
		Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
		      wire_cap;
		power+=2*number_of_column_bits*Ceq*.5*VddPow*VddPow;
	}
	
	if(NSubbanks==8.0) {
		wire_cap = Cmetal * (subbank_v) * (TCAMBitHeight + 2 * WIRESPACING );
		wire_cap_h = Cmetal * (subbank_h) * (TCAMBitWidth + 2 * WIRESPACING );
		wire_cap += wire_cap_h ;

		/* buffer stage */
		Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
		      gatecap(Wdecdrivep+Wdecdriven,0.0);
		power+=6*number_of_column_bits*Ceq*.5*VddPow*VddPow;
		Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
		      wire_cap;
		power+=4*number_of_column_bits*Ceq*.5*VddPow*VddPow;
		Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
		      wire_cap_h ;
		power+=2*number_of_column_bits*Ceq*.5*VddPow*VddPow;
	}
 
	if(NSubbanks > 8.0) {
		 blocks=(int) (NSubbanks/8.0);
		 htree=logtwo((double)(blocks));
		 htree_int = (int) htree;
		 if (htree_int % 2 ==0) { 
			  subbank_mod = htree_int; 
			  start_h=(subbank_h*(powers(2,((int)(logtwo(htree)))+1)-1));
			  start_v=subbank_v; 
		 }
		 else {
			  subbank_mod = htree_int -1;
			  start_h=(subbank_h*(powers(2,(htree_int+1)/2)-1));
			  start_v=subbank_v; 
	 	 }


		 if(subbank_mod==0) {
		   	subbank_mod=1;
		 }

		 line_v= start_v;
		 line_h= start_h;

		 for(i=1;i<=blocks;i++) {
			wire_cap = Cmetal * (line_v) * (TCAMBitHeight + 2 * WIRESPACING );
			wire_cap_h = Cmetal * (line_h) * (TCAMBitWidth + 2 * WIRESPACING );
			wire_cap += wire_cap_h ;
		 
			 Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
			      gatecap(Wdecdrivep+Wdecdriven,0.0);
			 power+=6*number_of_column_bits*Ceq*.5*VddPow*VddPow;
			 Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
			      wire_cap;
			 power+=4*number_of_column_bits*Ceq*.5*VddPow*VddPow;
			 Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
			      (wire_cap_h);
			 power+=2*number_of_column_bits*Ceq*.5*VddPow*VddPow;

			if(i % subbank_mod ==0) {
				line_v+=2*(subbank_v);
			}
		}
       }

       return power ;
}

double 
tcam_subbank_routing_delay(struct parameters tcam_parameters, double *nextinputtime)
{
        double Ceq,Req,Rwire,tf;
	double delay_stage1,delay_stage2;
        double addr_h,addr_v;
        double wire_cap, wire_res;
	double NSubbanks = tcam_parameters.number_of_subbanks ;

        /* Calculate rise time.  Consider two inverters */

        Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
              gatecap(Wdecdrivep+Wdecdriven,0.0);
        tf = Ceq*transreson(Wdecdriven,NCH,1);
        *nextinputtime = horowitz(0.0,tf, VTHINV, VTHINV, FALL)/ (VTHINV);

        Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
              gatecap(Wdecdrivep+Wdecdriven,0.0);
        tf = Ceq*transreson(Wdecdriven,NCH,1);
        *nextinputtime = horowitz(*nextinputtime,tf,VTHINV,VTHINV, RISE)/ (1.0-VTHINV);

	/* calculate routing wire resistance and capacitance based on TCAM subbank length and width */
	addr_h=0; addr_v=0;
	tcam_subbank_dimension(tcam_parameters, &addr_h,&addr_v);
     	wire_cap = Cmetal * (addr_v) * (TCAMBitHeight + 2 * WIRESPACING ) 
		+ Cmetal * (addr_h) * (TCAMBitWidth + 2 * WIRESPACING );
	wire_res = addr_v * 0.5 * ((TCAMBitHeight + 2 * WIREPITCH ) / WIREWIDTH)  * Rmetal 
		+  addr_h * 0.5 * ((TCAMBitWidth + 2 * WIREPITCH ) / WIREWIDTH)  * Rmetal ;

	/* buffer stage */

        Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
              gatecap(Wdecdrivep+Wdecdriven,0.0);
        tf = Ceq*transreson(Wdecdriven,NCH,1);
	delay_stage1 = horowitz(*nextinputtime,tf,VTHINV,VTHINV,FALL);
        *nextinputtime = delay_stage1 / (VTHINV);

        Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +  wire_cap;
        tf = Ceq*(transreson(Wdecdriven,NCH,1)+ wire_res);
	delay_stage2=horowitz(*nextinputtime,tf,VTHINV,VTHINV,RISE);
        *nextinputtime = delay_stage2/(1.0-VTHINV);

	return (delay_stage1+delay_stage2);
}

double
tcam_intra_subbank_delay ( struct parameters tcam_parameters, double *nextinputtime )
{
  	uint32_t Nrd = tcam_parameters.Nrd ;
	uint32_t rows = tcam_parameters.number_of_rows ;
	double Ceq, tf, Rwire;       
	double tagdriveA=0.0, TagdriveB=0.0, TagdriveA1=0.0, TagdriveB1=0.0, TagdriveA2=0.0, TagdriveB2=0.0;
	double total_delay;

	if (Nrd>1)
	  {
	    Ceq = draincap(Wcamdrivep,PCH,1)+draincap(Wcamdriven,NCH,1) +
	      gatecap(Wcamdrive2p+Wcamdrive2n,0.0);
	    tf = Ceq*transreson(Wcamdriven,NCH,1);
	    tagdriveA = horowitz(*nextinputtime,tf,VSINV,VSINV,FALL);
	    *nextinputtime = tagdriveA / (VSINV);
	    
	    if (Nrd<=4)
	      {
		Ceq = draincap(Wcamdrive2p,PCH,1)+draincap(Wcamdrive2n,NCH,1) +
		  gatecap(Wcamdecdrive1p+Wcamdecdrive1n,10.0)*2+
		  + (Cmetal * (TCAMBitWidth + 2 * WIRESPACING )) *sqrt((rows+1)*Nrd)/2
		  + (Cmetal * (TCAMBitHeight + 2 * WIRESPACING)) *sqrt((rows+1)*Nrd)/2;
		Rwire = (Rmetal * 0.5 * ((TCAMBitWidth + 2 * WIREPITCH ) / WIREWIDTH)) *sqrt((rows+1)*Nrd)*.5/2+
		  (Rmetal * 0.5 * ((TCAMBitHeight + 2 * WIREPITCH ) / WIREWIDTH))*sqrt((rows+1)*Nrd)*.5/2;
		tf = Ceq*(transreson(Wcamdrive2p,PCH,1)+Rwire);
		TagdriveB = horowitz(*nextinputtime,tf,VSINV,VSINV,RISE);
		*nextinputtime = TagdriveB/(1.0-VSINV);
	      }
	    else
	      {
		Ceq = draincap(Wcamdrive2p,PCH,1)+draincap(Wcamdrive2n,NCH,1) +
		  gatecap(Wcamdrivep+Wcamdriven,10.0)*2+
		  + (Cmetal * (TCAMBitWidth + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/2
		  + (Cmetal * (TCAMBitHeight + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/2;
		Rwire = (Rmetal * 0.5 * ((TCAMBitWidth + 2* WIREPITCH ) / WIREWIDTH))*sqrt((rows+1)*Nrd)*.5/2+
		  (Rmetal * 0.5 * ((TCAMBitHeight + 2* WIREPITCH ) / WIREWIDTH))*sqrt((rows+1)*Nrd)*.5/2;
		tf = Ceq*(transreson(Wcamdrive2p,PCH,1)+Rwire);
		TagdriveB = horowitz(*nextinputtime,tf,VSINV,VSINV,RISE);
		*nextinputtime = TagdriveB/(1.0-VSINV);
		
		Ceq = draincap(Wcamdrivep,PCH,1)+draincap(Wcamdriven,NCH,1) +
		  gatecap(Wcamdrive2p+Wcamdrive2n,10.0);
		tf = Ceq*transreson(Wcamdriven,NCH,1);
		TagdriveA1 = horowitz(*nextinputtime,tf,VSINV,VSINV,FALL);
		*nextinputtime = TagdriveA1/(VSINV);
		
		if (Nrd<=16)
		  {
		    Ceq = draincap(Wcamdrive2p,PCH,1)+draincap(Wcamdrive2n,NCH,1) +
		      gatecap(Wcamdecdrive1p+Wcamdecdrive1n,10.0)*2+
		      + (Cmetal * (TCAMBitWidth + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/4
		      + (Cmetal * (TCAMBitHeight + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/4;
		    Rwire = (Rmetal * 0.5 * ((TCAMBitWidth + 2* WIREPITCH ) / WIREWIDTH))*sqrt((rows+1)*Nrd)*.5/4+
		      (Rmetal * 0.5 * ((TCAMBitHeight + 2* WIREPITCH ) / WIREWIDTH))*sqrt((rows+1)*Nrd)*.5/4;
		    tf = Ceq*(transreson(Wcamdrive2p,PCH,1)+Rwire);
		    TagdriveB1 = horowitz(*nextinputtime,tf,VSINV,VSINV,RISE);
		    *nextinputtime = TagdriveB1/(1.0-VSINV);
		  }
		else
		  {
		    Ceq = draincap(Wcamdrive2p,PCH,1)+draincap(Wcamdrive2n,NCH,1) +
		      gatecap(Wcamdrivep+Wcamdriven,10.0)*2+
		      + (Cmetal * (TCAMBitWidth + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/4
		      + (Cmetal * (TCAMBitHeight + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/4;
		    Rwire = (Rmetal * 0.5 * ((TCAMBitWidth + 2* WIREPITCH ) / WIREWIDTH))*sqrt((rows+1)*Nrd)*.5/4+
		      (Rmetal * 0.5 * ((TCAMBitHeight + 2* WIREPITCH ) / WIREWIDTH))*sqrt((rows+1)*Nrd)*.5/4;
		    tf = Ceq*(transreson(Wcamdrive2p,PCH,1)+Rwire);
		    TagdriveB1 = horowitz(*nextinputtime,tf,VSINV,VSINV,RISE);
		    *nextinputtime = TagdriveB1/(1.0-VSINV);

		    Ceq = draincap(Wcamdrivep,PCH,1)+draincap(Wcamdriven,NCH,1) +
		      gatecap(Wcamdrive2p+Wcamdrive2n,10.0);
		    tf = Ceq*transreson(Wcamdriven,NCH,1);
		    TagdriveA2 = horowitz(*nextinputtime,tf,VSINV,VSINV,FALL);
		    *nextinputtime = TagdriveA2/(VSINV);
		
		    Ceq = draincap(Wcamdrive2p,PCH,1)+draincap(Wcamdrive2n,NCH,1) +
		      gatecap(Wcamdecdrive1p+Wcamdecdrive1n,10.0)*2+
		      + (Cmetal * (TCAMBitWidth + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/8
		      + (Cmetal * (TCAMBitHeight + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/8;
		    Rwire = (Rmetal * 0.5 * ((TCAMBitWidth + 2* WIREPITCH ) / WIREWIDTH))*sqrt((rows+1)*Nrd)*.5/8+
		      (Rmetal * 0.5 * ((TCAMBitHeight + 2* WIREPITCH ) / WIREWIDTH))*sqrt((rows+1)*Nrd)*.5/8;
		    tf = Ceq*(transreson(Wcamdrive2p,PCH,1)+Rwire);
		    TagdriveB2 = horowitz(*nextinputtime,tf,VSINV,VSINV,RISE);
		    *nextinputtime = TagdriveB2/(1.0-VSINV);
		  }		
	      }
      	  }
	  total_delay =  tagdriveA + TagdriveB + TagdriveA1 + TagdriveB1 + TagdriveA2 + TagdriveB2;

          return total_delay;	
}


double
tcam_intra_subbank_power ( struct parameters tcam_parameters )
{
  	uint32_t Nrd = tcam_parameters.Nrd ;
	uint32_t rows = tcam_parameters.number_of_rows ;
	uint32_t bits = tcam_parameters.number_of_column_bits ;
	double Ceq;       
	double power = 0.0 ;

	if (Nrd>1)
	  {
	    Ceq = draincap(Wcamdrivep,PCH,1)+draincap(Wcamdriven,NCH,1) +
	      gatecap(Wcamdrive2p+Wcamdrive2n,0.0);
	    power+=.5*Ceq*VddPow*VddPow*bits;

	    if (Nrd<=4)
	      {
		Ceq = draincap(Wcamdrive2p,PCH,1)+draincap(Wcamdrive2n,NCH,1) +
		  gatecap(Wcamdecdrive1p+Wcamdecdrive1n,10.0)*2+
		  + (Cmetal * (TCAMBitWidth + 2 * WIRESPACING )) *sqrt((rows+1)*Nrd)/2
		  + (Cmetal * (TCAMBitHeight + 2 * WIRESPACING)) *sqrt((rows+1)*Nrd)/2;
		power+=Ceq*VddPow*VddPow*bits*.5*2;
	      }
	    else
	      {
		Ceq = draincap(Wcamdrive2p,PCH,1)+draincap(Wcamdrive2n,NCH,1) +
		  gatecap(Wcamdrivep+Wcamdriven,10.0)*2+
		  + (Cmetal * (TCAMBitWidth + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/2
		  + (Cmetal * (TCAMBitHeight + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/2;
		power+=Ceq*VddPow*VddPow*bits*.5*4;
		
		Ceq = draincap(Wcamdrivep,PCH,1)+draincap(Wcamdriven,NCH,1) +
		  gatecap(Wcamdrive2p+Wcamdrive2n,10.0);
		power+=Ceq*VddPow*VddPow*bits*.5;
		
		if (Nrd<=16)
		  {
		    Ceq = draincap(Wcamdrive2p,PCH,1)+draincap(Wcamdrive2n,NCH,1) +
		      gatecap(Wcamdecdrive1p+Wcamdecdrive1n,10.0)*2+
		      + (Cmetal * (TCAMBitWidth + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/4
		      + (Cmetal * (TCAMBitHeight + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/4;
		    power+=Ceq*VddPow*VddPow*bits*.5*8;
		  }
		else
		  {
		    Ceq = draincap(Wcamdrive2p,PCH,1)+draincap(Wcamdrive2n,NCH,1) +
		      gatecap(Wcamdrivep+Wcamdriven,10.0)*2+
		      + (Cmetal * (TCAMBitWidth + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/4
		      + (Cmetal * (TCAMBitHeight + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/4;
		    power+=Ceq*VddPow*VddPow*bits*.5*8;

		    Ceq = draincap(Wcamdrivep,PCH,1)+draincap(Wcamdriven,NCH,1) +
		      gatecap(Wcamdrive2p+Wcamdrive2n,10.0);
		    power+=Ceq*VddPow*VddPow*bits*.5;
		
		    Ceq = draincap(Wcamdrive2p,PCH,1)+draincap(Wcamdrive2n,NCH,1) +
		      gatecap(Wcamdecdrive1p+Wcamdecdrive1n,10.0)*2+
		      + (Cmetal * (TCAMBitWidth + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/8
		      + (Cmetal * (TCAMBitHeight + 2 * WIRESPACING ))*sqrt((rows+1)*Nrd)/8;
		    power+=Ceq*VddPow*VddPow*bits*.5*16;
		  }		
	      }
      	  }

          return power;	
}


/*----------------------------------------------------------------------*/

// calculate match-line power
float calculate_matchline_power(uint32_t rows, uint32_t columns, float *wire_cap)
{
  float CTotalML;
  float power;
  float slope, constant;
  float C_wire_per_cell,   C_drain_compareT, C_nand_inv_gate, C_precharge ;
 
  slope = 0.0;
  constant = 0.0;
  
  CTotalML = 0;

  /* precharge transistor - drain capacitance */
  CTotalML +=  draincap(Wtcamprechp,PCH,1);
  constant += draincap(Wtcamprechp,PCH,1) * 1e15 ;
  C_precharge = draincap(Wtcamprechp,PCH,1) * 1e15 ;

  /* Match line length  - metal wire capacitance - using metal2 */
  CTotalML += Cmetal * columns * (TCAMBitWidth + 2 * WIRESPACING);
  C_wire_per_cell = Cmetal * (TCAMBitWidth + 2 * WIRESPACING) * 1e15;
  // fprintf(stdout, "CTotalML capacitance (fF): %f\n", CTotalML*1e15);
  slope += (Cmetal * (TCAMBitWidth + 2 * WIRESPACING)) * 1e15; 

  /* drain capacitance of 2 comparator transistors in each bit cell */
  CTotalML +=  2 * columns * draincap(Wtcamcomparen1,NCH,2); 
  C_drain_compareT = draincap(Wtcamcomparen1,NCH,2) * 1e15;
  // fprintf(stdout, "CTotalML capacitance (fF): %f\n", CTotalML*1e15);
  slope += ( 2 * draincap(Wtcamcomparen1,NCH,2)) * 1e15;

  /* final match-line results combination either using NAND/INV or some other mechanism, we consider 
     NAND/INV for our case */
  CTotalML += gatecap(Wtcaminvn+Wtcaminvp,(10.0/SCALE_FACTOR)) + gatecap(Wtcamdriven+Wtcamnandp,(10.0/SCALE_FACTOR));
  C_nand_inv_gate = (gatecap(Wtcaminvn+Wtcaminvp,(10.0/SCALE_FACTOR)) + gatecap(Wtcamdriven+Wtcamnandp,(10.0/SCALE_FACTOR)))*1e15;
  // fprintf(stdout, "CTotalML capacitance (fF): %f\n", CTotalML*1e15);
  constant += (gatecap(Wtcaminvn+Wtcaminvp,(10.0/SCALE_FACTOR)) + gatecap(Wtcamdriven+Wtcamnandp,(10.0/SCALE_FACTOR))) * 1e15;  

  
  // fprintf ( stdout , "Match line slope : %f and constant : %f (fF) \n",
     //       slope, constant );

  fprintf ( stdout, "\nMatchline capacitance components : \n------------------------- \
		   \nC_wire_per_cell ( in fF ) : %f \
		   \nC_drain_compareT ( in fF ) : %f \
		   \nC_precharge ( in fF ) : %f \
		   \nC_nand_inv_gate ( in fF ) : %f\n", C_wire_per_cell, C_drain_compareT, C_precharge, C_nand_inv_gate );

  *wire_cap = CTotalML;
  power = rows * CTotalML * VddPow * VddPow;
  return power;
}



// calculate searchline power
float calculate_searchline_power(uint32_t rows, uint32_t columns, float *wire_cap)
{
   
  float CTotalSL;
  float power;
  float slope, constant;
  float C_search_wire, C_gate_compareT , C_driver ;
   
  CTotalSL = 0.0;
  slope = 0.0;
  constant = 0.0;
  
  /* searchline driver - the driver for the search line is taken as the driver for the tagline
     as incase of fully-associative cache in cacti */
  CTotalSL += draincap(Wtcamdriven,NCH,1)+draincap(Wtcamdrivep,PCH,1);
  C_driver = (draincap(Wtcamdriven,NCH,1)+draincap(Wtcamdrivep,PCH,1)) * 1e15; 
  // fprintf(stdout, "CTotalSL capacitance (fF): %f\n", CTotalSL*1e15);
  constant += (draincap(Wtcamdriven,NCH,1)+draincap(Wtcamdrivep,PCH,1))*1e15;
 
  /* searchline wire capacitance using metal wire capacitance */
  CTotalSL += Cmetal * rows * (TCAMBitHeight + 2 * WIRESPACING);
  C_search_wire = Cmetal * (TCAMBitHeight + 2 * WIRESPACING) * 1e15 ;
  // fprintf(stdout, "CTotalSL capacitance (fF): %f\n", CTotalSL*1e15);
  slope += (Cmetal * (TCAMBitHeight + 2 * WIRESPACING)) * 1e15;  

  /* gate capacitnce of 2nd nmos transistor in comparator circuit */
  CTotalSL += rows *  gatecappass(Wtcamcomparen2,2.0/SCALE_FACTOR);
  C_gate_compareT = gatecappass(Wtcamcomparen2,2.0/SCALE_FACTOR) * 1e15 ;
  // fprintf(stdout, "CTotalSL capacitance (fF): %f\n", CTotalSL*1e15);
  slope += (gatecappass(Wtcamcomparen2,2.0/SCALE_FACTOR))*1e15;


  //fprintf ( stdout , "Search line slope : %f and constant : %f (fF) \n",
  //          slope, constant );
  
  fprintf ( stdout, "\nSearchline capacitance components : \n------------------------- \
		   \nC_wire_per_cell ( in fF ) : %f \
		   \nC_gate_compareT ( in fF ) : %f \
		   \nC_driver ( in fF ) : %f\n", C_search_wire, C_gate_compareT, C_driver );
 
  *wire_cap = CTotalSL; 
  power = CTotalSL * VddPow * VddPow * columns;  
  return power;

}


// calculate priority encoder power
float calculate_priority_encoder_power(uint32_t rows)
{
    uint32_t number_of_256x1_pe_required = 0;
    float power;

    /* we are doing priority arbiter for now as we can drive the line to
       the wordline of SRAM */
    if(rows < 256 ) {
	number_of_256x1_pe_required = 1;
    }
    else {
	/* if we don't consider delay here, then we can just take ceil of 
           (rows / 256) to find the number of priorty encoders */
	// number_of_256x1_pe_required = ceil (rows *1.0 / 256);

	/* But if we consider delay into account, then we may have to see
	   multiple level lookahead, and we almost need double number of
           priority encoder than the previous one */
	number_of_256x1_pe_required = (ceil(rows *1.0 / 256) * 2 - 1);
    }

    power = PriorityEncoderEnergy256x1 * number_of_256x1_pe_required;
    return power;
}



// calculate searchline delay for discharging
double
calculate_searchline_delay ( uint32_t rows, uint32_t bits, double *nextinputtime, int is_rise)
{
   /* searchline variables */
   double CTotalSL ;
   double C_searchline ;
   double R_searchline ;
   double timeconstant_searchline ;
   double delay_searchline ;
  
   // calculate searchline capacitance
   CTotalSL = 0.0 ; 
   CTotalSL += draincap(Wtcamdriven,NCH,1)+draincap(Wtcamdrivep,PCH,1);
   CTotalSL += Cmetal * rows * (TCAMBitHeight + 2 * WIRESPACING);
   CTotalSL += rows *  gatecap(Wtcamcomparen2,10.0/SCALE_FACTOR);
   C_searchline = CTotalSL;
   
   // Resistance of searchline
   R_searchline = rows * 0.5 * ((TCAMBitHeight + 2 * WIREPITCH ) / WIREWIDTH)  * Rmetal ;
   R_searchline += transreson(Wcamdecdrivep,PCH,1) + transresswitch(Wcamdecdrivep,PCH,1) ;

   // fprintf ( stdout, "Rows: %d, Resistance: %e, capacitance: %e\n", rows, R_searchline, C_searchline );

   // Time constant of searchline
   timeconstant_searchline = R_searchline * C_searchline ;
   // fprintf (stdout, "time constant: %e \n", timeconstant_searchline );

   // Now, we use horowitz approximation equation to find the rise/fall time
   // for a given input signal slope and the time constant of a equivalent RC circuit
   // for a transistor cluster.
   delay_searchline = horowitz ( *nextinputtime, timeconstant_searchline, Vth, Vth, is_rise);
   if ( is_rise = 0 ) {
	*nextinputtime = delay_searchline / Vth;	
   }
   else {
	*nextinputtime = delay_searchline / ( 1.0 - Vth );	
   }

   // fprintf ( stdout, "TCAM searchline delay : %f (ns) \n", (float)delay_searchline * 1e9);

   return delay_searchline;
}

// calculate matchline delay for discharging
double
calculate_matchline_delay ( uint32_t rows, uint32_t bits, double *nextinputtime, int is_rise)
{
   /* matchline variables */
   double CTotalML ;
   double C_matchline ;
   double R_matchline ;
   double timeconstant_matchline ;
   double delay_matchline ;

   // calculate matchline capacitance
   CTotalML = 0.0 ; 
   CTotalML +=  draincap(Wtcamprechp,PCH,1);
   CTotalML += Cmetal * bits * (TCAMBitWidth + 2 * WIRESPACING);
   CTotalML +=  2 * bits * draincap(Wtcamcomparen1,NCH,2); 
   CTotalML += gatecap(Wtcaminvn+Wtcaminvp,(10.0/SCALE_FACTOR)) 
		+ gatecap(Wtcamdriven+Wtcamnandp,(10.0/SCALE_FACTOR));
   C_matchline = CTotalML;
   
   // Resistance of matchline
   R_matchline = bits * 0.5 * ((TCAMBitWidth + 2 * WIREPITCH ) / WIREWIDTH)  * Rmetal ; 
   R_matchline += transreson(Wdummyn,NCH,1)+transreson(Wdummyn,NCH,1) ;

   // Time constant of matchline
   timeconstant_matchline = R_matchline * C_matchline ;

   // Now, we use horowitz approximation equation to find the rise/fall time
   // for a given input signal slope and the time constant of a equivalent RC circuit
   // for a transistor cluster.
   delay_matchline = horowitz ( *nextinputtime, timeconstant_matchline, Vth, Vth, is_rise);
   if ( is_rise = 0 ) {
	*nextinputtime = delay_matchline / Vth;	
   }
   else {
	*nextinputtime = delay_matchline / ( 1.0 - Vth );	
   }

   // fprintf ( stdout, "TCAM matchline delay : %f (ns) \n", (float)delay_matchline * 1e9);

   return delay_matchline;
}



// precharge circuit delay - modeled using two inverters
double
calculate_delay_two_precharge_inverters ( double *nextinputtime )
{
   double C_inv1;
   double R_inv1;
   double timeconstant_inv1;
   double delay_inv1;
   double C_inv2;
   double R_inv2;
   double timeconstant_inv2;
   double delay_inv2;

   C_inv1 = draincap(Wcamdecdrive1p,PCH,1)+draincap(Wcamdecdrive1n,NCH,1)
	  + gatecap(Wcamdecdrive2p+Wcamdecdrive2n,0.0);
   R_inv1 = transreson(Wcamdecdrive1n,NCH,1);
   timeconstant_inv1 = C_inv1 *  R_inv1;
   delay_inv1 = horowitz(*nextinputtime, timeconstant_inv1, VSINV, VSINV, FALL);
   *nextinputtime = delay_inv1 / VSINV;

   C_inv2 = draincap(Wcamdecdrive2p,PCH,1) + draincap(Wcamdecdrive2n,NCH,1) +
	  + gatecap(Wcamdecdrivep+Wcamdecdriven,10.0) + gatecap(Wcamdecdrive2p+Wcamdecdrive2n,10.0);
   R_inv2 = transreson(Wcamdecdrive2p,PCH,1);
   timeconstant_inv2 = C_inv2 * R_inv2 ;
   delay_inv2 = horowitz(*nextinputtime, timeconstant_inv2, VSINV, VSINV, RISE);
   *nextinputtime = delay_inv2 / VSINV;

   return (delay_inv1 + delay_inv2);
}

// Calculate priority encoder delay
double 
calculate_priority_encoder_delay ( uint32_t rows )
{
   // From Micronet workshop 2004 paper, we find the delay of 256x1 bit priority encoder
   // and we assume multiple lookahead design to find the overall delay.
   int32_t  no_levels = 1;

   no_levels = ceil(logtwo((double)rows/256));
   if(no_levels <= 0) no_levels = 1;
   // fprintf ( stdout, "Number of levels: %d\n", no_levels );
   return (no_levels * PriorityEncoderDelay256x1);
}


// Calculate TCAM search time using equivalent resistance and capacitance
double
calculate_tcam_search_time(struct parameters tcam_parameters, struct search_delay *tcam_search_delay)
{
   uint32_t rows, bits;
   float tech, fudge_factor;
   uint32_t no_sub_banks, Nrd;

   /* timing variables */
   double nextinputtime = 0.0 ;
   double time_constant = 0.0 ;
   double Vthreshold ;  // Vth - Threshold voltage - proportions of Vdd for various subcomponents need to be determined.

   double routing_delay ;
   double intra_subbank_routing_delay ;
   double searchline_discharge_delay ;
   double searchline_precharge_delay ;
   double precharge_delay ;
   double matchline_precharge_delay ;
   double matchline_discharge_delay ;
   double priority_encoder_delay ;
   double total_delay ;
   double max_delay ;
   
   Nrd = tcam_parameters.Nrd ;
   no_sub_banks = tcam_parameters.number_of_subbanks;
   rows = tcam_parameters.number_of_rows / (no_sub_banks * Nrd) ;
   bits = tcam_parameters.number_of_column_bits;
   tech = tcam_parameters.CMOS_tech;
   fudge_factor = TECH_USED / tech;
   
   fprintf ( stdout, "\n\nTCAM delay in various components:\n");
   fprintf ( stdout, "---------------------------------------\n");

   

   // 1. Routing delay : Driving the data from input pins to search lines
   nextinputtime = 0.0;
   routing_delay = tcam_subbank_routing_delay ( tcam_parameters, &nextinputtime ) / fudge_factor ;
   tcam_search_delay->routing_delay = routing_delay ;
   fprintf ( stdout, "TCAM subbanks routing delay : %f (ns) \n", (float) routing_delay * 1e9);

   // 1a. Intra-subank routing delay 
   intra_subbank_routing_delay = tcam_intra_subbank_delay ( tcam_parameters, &nextinputtime ) / fudge_factor ;
   tcam_search_delay->intra_subbank_routing_delay = intra_subbank_routing_delay ;
   fprintf ( stdout, "TCAM intra-subbanks routing delay : %f (ns) \n", (float) intra_subbank_routing_delay * 1e9);

   // 2. In this phase, the matchline is precharged and precharge circuit's delay
   // is approximated with the delay of two_inverters.
   // nextinputtime = 0.0 ;
   precharge_delay = calculate_delay_two_precharge_inverters ( &nextinputtime ) / fudge_factor ;
   tcam_search_delay->precharge_delay = precharge_delay ;
   fprintf ( stdout, "Precharge circuit delay : %f (ns) \n", (float) precharge_delay * 1e9);

   // 2a. Matchline precharge
   matchline_precharge_delay = calculate_matchline_delay ( rows, bits, &nextinputtime, RISE ) / fudge_factor ;
   tcam_search_delay->matchline_precharge_delay = matchline_precharge_delay ;
   fprintf ( stdout, "TCAM matchline delay (precharge) : %f (ns) \n", (float) matchline_precharge_delay * 1e9);
   
   // 3. Now apply the search data to the searchlines, it will make atleast one of the
   // search line to go high while searching '0' or '1'.
   searchline_precharge_delay = calculate_searchline_delay ( rows, bits, &nextinputtime, RISE ) / fudge_factor ;
   tcam_search_delay->searchline_precharge_delay = searchline_precharge_delay ;
   fprintf ( stdout, "TCAM searchline delay (search data) : %f (ns) \n", (float) searchline_precharge_delay * 1e9);
  
 
   // 4. Matchlines are discharged as the search data is applied to the searchlines 
   // Once the searchlines reaches the voltage threshold specified by nextinputtime, matchlines
   // starts discharging. 
   matchline_discharge_delay = calculate_matchline_delay ( rows, bits, &nextinputtime, FALL ) / fudge_factor ;
   tcam_search_delay->matchline_discharge_delay = matchline_discharge_delay ;
   fprintf ( stdout, "TCAM matchline delay (discharge) : %f (ns) \n", (float) matchline_discharge_delay * 1e9);
   

   // 5. Measure the priority encoder delay
   priority_encoder_delay = calculate_priority_encoder_delay ( tcam_parameters.number_of_rows ) / fudge_factor ;
   tcam_search_delay->priority_encoder_delay  = priority_encoder_delay ;
   fprintf ( stdout, "Priority encoder delay  : %f (ns) \n", (float) priority_encoder_delay * 1e9);
   
   
   // 6. discharge both the searchlines so that matchline can be
   // precharged in the next cycle.
   // nextinputtime is kept zero assuming step input
   nextinputtime = 0.0 ;
   searchline_discharge_delay = calculate_searchline_delay ( rows, bits, &nextinputtime, FALL ) / fudge_factor ;
   tcam_search_delay->searchline_discharge_delay = searchline_discharge_delay ;
   fprintf ( stdout, "TCAM searchline delay (discharging) : %f (ns) \n", (float) searchline_discharge_delay * 1e9);
   
   // calculate total delay
   total_delay = routing_delay + intra_subbank_routing_delay + searchline_discharge_delay + precharge_delay + matchline_precharge_delay + 
		  searchline_precharge_delay + matchline_discharge_delay ;
   tcam_search_delay->total_delay = total_delay ;
   fprintf ( stdout, "TCAM total delay : %f (ns) \n", (float) total_delay * 1e9 );

   // Max delay between three stages : 
   //  (1) TCAM subbank routing delay
   //  (2) TCAM search delay
   //  (3) priority_encoder delay
   max_delay = total_delay ;
   if ( max_delay < priority_encoder_delay ) {
	max_delay = priority_encoder_delay ;
   }
   tcam_search_delay->max_delay = max_delay ;
   fprintf ( stdout, "TCAM access time : %f (ns) \n", (float) max_delay * 1e9 );

   return max_delay; 
}


//  calculate TCAM power
void calculate_tcam_search_energy(struct parameters tcam_parameters, struct search_power *tcam_search_power)
{
   uint32_t rows, bits;
   float tech, fudge_factor, Nrd;
   uint32_t no_sub_banks;
   float wire_cap;

   float routing_power;
   float intra_subbank_routing_power ;
   float matchline_power;
   float searchline_power;
   float priority_encoder_power;

   Nrd = tcam_parameters.Nrd ;
   rows = tcam_parameters.number_of_rows / (tcam_parameters.number_of_subbanks * Nrd);
   bits = tcam_parameters.number_of_column_bits;
   no_sub_banks = tcam_parameters.number_of_subbanks;
   tech = tcam_parameters.CMOS_tech;
   fudge_factor = TECH_USED / tech;

   /* TCAM  routing power */
   routing_power = tcam_subbanks_routing_power ( tcam_parameters ); 
   tcam_search_power-> routing_power = routing_power / fudge_factor ; 
   intra_subbank_routing_power = tcam_intra_subbank_power ( tcam_parameters );
   tcam_search_power->intra_subbank_routing_power = intra_subbank_routing_power / fudge_factor ;

   /* matchline power */
   matchline_power = calculate_matchline_power(rows, bits, &wire_cap) * no_sub_banks * Nrd ;
   tcam_search_power->matchline_power = (matchline_power / fudge_factor); 

   /* search line power */
   searchline_power = calculate_searchline_power(rows, bits, &wire_cap) * no_sub_banks * Nrd ;
   tcam_search_power->searchline_power = (searchline_power / fudge_factor);

   /* priority encoder power */
   priority_encoder_power = calculate_priority_encoder_power(rows * tcam_parameters.number_of_subbanks * Nrd);
   tcam_search_power->priority_encoder_power = ( priority_encoder_power / fudge_factor);
   
   /* total power */
   tcam_search_power->total_power = (routing_power + intra_subbank_routing_power +
      				matchline_power + searchline_power + priority_encoder_power) / fudge_factor ;

   return;
}
   

void   
calculate_tcam_read_energy(struct parameters tcam_parameters, struct read_power *tcam_read_power )
{
   double Wnmos, Wpmos;
   double capacitance;
   double wire_cap;
   uint32_t rows, bits;
   float tech, fudge_factor;
   float Nrd, no_sub_banks;
  

   Nrd = tcam_parameters.Nrd ;
   no_sub_banks = tcam_parameters.number_of_subbanks;
   rows = tcam_parameters.number_of_rows / (Nrd * no_sub_banks);
   bits = tcam_parameters.number_of_column_bits;
   tech = tcam_parameters.CMOS_tech;
   fudge_factor = TECH_USED / tech;
   
   // TCAM routing power 
   tcam_read_power->routing_power = tcam_subbanks_routing_power ( tcam_parameters ) / fudge_factor; 
   tcam_read_power->routing_power += tcam_intra_subbank_power ( tcam_parameters ) / fudge_factor;

   // calculate decoder power 
   capacitance = 3*draincap(Wdec3to8p,PCH,1) + draincap(Wdec3to8n,NCH,3) +
                 gatecap(WdecNORn+WdecNORp,((40.0 / fudge_factor)+20.0 / fudge_factor))/8 ;
   tcam_read_power->decoder_power = capacitance * VddPow * VddPow * rows *0.125 / fudge_factor ;
   capacitance = (gatecap(Wdecinvn+Wdecinvp,20.0/fudge_factor)+draincap(WdecNORn,NCH,1)+draincap(WdecNORp,PCH,1));
   tcam_read_power->decoder_power += capacitance * VddPow * VddPow  / fudge_factor ;
    

   // calculate wordline capacitance and wordline power
   wire_cap = (gatecappass(Wtcamcella,(TCAMBitWidth-2*Wtcamcella)/2.0) + gatecappass(Wtcamcella,(TCAMBitWidth-2*Wtcamcella)/2.0))* bits +
              Cmetal* bits * (TCAMBitWidth + 2 * WIRESPACING) + draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1);
   tcam_read_power->wordline_power = wire_cap * VddPow * VddPow / fudge_factor;

   // calculate wordline driver transistor size and equivalent capacitance
   Wpmos = (wire_cap / gatecap(1.0,0.0)) * 0.33 * 2/3 ;
   Wnmos = (wire_cap / gatecap(1.0,0.0)) * 0.33 * 1/3 ;
   capacitance = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) + gatecap(Wnmos+Wpmos, 20.0 / fudge_factor);
   tcam_read_power->wordline_power += capacitance * VddPow * VddPow  / fudge_factor; 

   // calculate bitline power
   wire_cap = rows*draincap (Wtcamcella,NCH,1)/2.0 + rows*Cmetal*(TCAMBitHeight+2*WIRESPACING) + 2*draincap(Wbitpreequ,PCH,1);
   wire_cap += 2.0*gatecap(WsenseN+WsenseP,10.0 / fudge_factor);
   tcam_read_power->bitline_power = wire_cap * VddPow * VddPow * bits / fudge_factor;

   // calculate sense amplifier power
   capacitance = gatecap(WsenseP+WsenseN,5.0 / fudge_factor) + draincap(WsPch,PCH,1) + 
           gatecap(Wdecnandn+Wdecnandp,1.0 / fudge_factor) + gatecap(WdecNORn+WdecNORp,1.0 / fudge_factor);
   tcam_read_power->senseamp_power = capacitance * VddPow * VddPow * bits / fudge_factor; 

   // total power in read operation
   tcam_read_power->total_power = tcam_read_power->routing_power + tcam_read_power->decoder_power + 
				 tcam_read_power->wordline_power + tcam_read_power->bitline_power + 
				 tcam_read_power->senseamp_power;

   return;
}

void   
calculate_tcam_write_energy(struct parameters tcam_parameters, struct write_power *tcam_write_power )
{
   double Wnmos, Wpmos;
   double capacitance;
   double wire_cap;
   uint32_t rows, bits;
   float tech, fudge_factor;
   float Nrd, no_sub_banks;
  

   Nrd = tcam_parameters.Nrd ;
   no_sub_banks = tcam_parameters.number_of_subbanks;
   rows = tcam_parameters.number_of_rows / (Nrd * no_sub_banks);
   bits = tcam_parameters.number_of_column_bits;
   tech = tcam_parameters.CMOS_tech;
   fudge_factor = TECH_USED / tech;
   
   // TCAM routing power 
   tcam_write_power->routing_power = tcam_subbanks_routing_power ( tcam_parameters ) / fudge_factor; 
   tcam_write_power->routing_power += tcam_intra_subbank_power ( tcam_parameters ) / fudge_factor;

   // calculate decoder power 
   capacitance = 3*draincap(Wdec3to8p,PCH,1) + draincap(Wdec3to8n,NCH,3) +
                 gatecap(WdecNORn+WdecNORp, 60.0/fudge_factor)/8 ;
   tcam_write_power->decoder_power = capacitance * VddPow * VddPow * rows * 0.125 / fudge_factor ;
   printf("decoder power: %f\n", tcam_write_power->decoder_power*1e9);
   capacitance = (gatecap(Wdecinvn+Wdecinvp,20.0/fudge_factor)+draincap(WdecNORn,NCH,1)+draincap(WdecNORp,PCH,1));
   tcam_write_power->decoder_power += capacitance * VddPow * VddPow  / fudge_factor ;
   printf("decoder power: %f\n", tcam_write_power->decoder_power*1e9);
    

   // calculate wordline capacitance and wordline power
   wire_cap = (gatecappass(Wtcamcella,(TCAMBitWidth-2*Wtcamcella)/2.0) + gatecappass(Wtcamcella,(TCAMBitWidth-2*Wtcamcella)/2.0))* bits +
              Cmetal* bits * (TCAMBitWidth + 2 * WIRESPACING) + draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1);
   tcam_write_power->wordline_power = wire_cap * VddPow * VddPow / fudge_factor;

   // calculate wordline driver transistor size and equivalent capacitance
   Wpmos = (wire_cap / gatecap(1.0,0.0)) * 0.33 * 2/3 ;
   Wnmos = (wire_cap / gatecap(1.0,0.0)) * 0.33 * 1/3 ;
   capacitance = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) + gatecap(Wnmos+Wpmos, 20.0 / fudge_factor);
   tcam_write_power->wordline_power += capacitance * VddPow * VddPow  / fudge_factor; 

   // calculate bitline power
   wire_cap = rows*draincap (Wtcamcella,NCH,1)/2.0 + rows*Cmetal*(TCAMBitHeight+2*WIRESPACING) + 2*draincap(Wbitpreequ,PCH,1);
   tcam_write_power->bitline_power =  wire_cap * VddPow * VddPow * bits / fudge_factor;

   // total power in read operation
   tcam_write_power->total_power = tcam_write_power->routing_power + tcam_write_power->decoder_power + 
				 tcam_write_power->wordline_power + tcam_write_power->bitline_power ;

   return;
}


void 
calculate_tcam_leakage_power(struct parameters tcam_parameters, struct static_power_worst_case *leakage_power)
{
   double Wnmos, Wpmos;
   double Ileakage;
   uint32_t rows, bits;
   float tech, fudge_factor;
   float wire_cap;
   float Nrd, no_sub_banks;
   int address_bits;

   Nrd = tcam_parameters.Nrd ;
   no_sub_banks = tcam_parameters.number_of_subbanks;
   rows = tcam_parameters.number_of_rows / (Nrd * no_sub_banks);
   bits = tcam_parameters.number_of_column_bits;
   tech = tcam_parameters.CMOS_tech;
   fudge_factor = TECH_USED / tech;
   address_bits = (int)(log(rows+1) / log(2)) ;

   // Sub-threshold leakage current for one tcam cell is equal to 2*NMOS + 2*PMOS cell of tcam memory cell
   // and two nmos subthreshold leakage current due to access transistors
   // scale the memory cell width to the current technology
   Wnmos = Wtcamcellnmos / fudge_factor ;
   Wpmos = Wtcamcellpmos / fudge_factor;
   // printf("1. wnmos: %f,  wpmos: %f\n", Wnmos, Wpmos);
   Ileakage = 2.0 * 0.5 * cmos_ileakage(Wnmos, Wpmos, Vthn, Vthp);
   // fprintf(stdout, "leakage current: %e,  Wnmos: %e, Wpmos, %e\n", Ileakage, Wnmos, Wpmos);
   leakage_power->memcell_leakage = Ileakage * VddPow * rows * bits * Nrd * no_sub_banks;     
   Wnmos = Wtcamcella / fudge_factor;
   Ileakage = 2.0 * 0.5 * cmos_ileakage(Wnmos, 0, Vthn, Vthp);
   leakage_power->memcell_access_leakage = Ileakage * VddPow * rows * bits * Nrd * no_sub_banks;     
	
   // decoder leakage power: Decoder3to8, NOR, and inverter stages
   Wnmos = Wdecdriven * 0.33 / fudge_factor; 
   Wpmos = Wdecdrivep * 0.33 / fudge_factor;
   Ileakage = 0.5*address_bits*cmos_ileakage(Wnmos, Wpmos, Vthn, Vthp);
   Wnmos = Wdec3to8n / fudge_factor; 
   Wpmos = Wdec3to8p / fudge_factor;
   Ileakage += 0.5*0.2*8.0*(rows/3.0)*0.25*cmos_ileakage(Wnmos, Wpmos, Vthn, Vthp);
   Wnmos = WdecNORn / fudge_factor; 
   Wpmos = WdecNORp / fudge_factor;
   Ileakage += 0.5*0.2*rows*cmos_ileakage(Wnmos, Wpmos, Vthn, Vthp);
   Wnmos = Wdecinvn / fudge_factor; 
   Wpmos = Wdecinvp / fudge_factor;
   Ileakage += 0.5*rows*cmos_ileakage(Wnmos, Wpmos, Vthn, Vthp);
   leakage_power->decoder_leakage = Ileakage * VddPow * Nrd * no_sub_banks;

   // Sub-threshold leakage current for matchline precharge circuit
   calculate_matchline_power(rows, bits, &wire_cap);
   Wpmos = (wire_cap / gatecap(1.0,0.0)) * 0.33 * 2/3 / fudge_factor;
   // printf("2. wnmos: %f,  wpmos: %f\n", Wnmos, Wpmos);
   Ileakage = 0.5 * cmos_ileakage(0.0, Wpmos, Vthn, Vthp);
   leakage_power->matchline_precharge_leakage = Ileakage * VddPow * rows * Nrd * no_sub_banks;     
   
   // Sub-threshold leakage current for search line driver
   calculate_searchline_power(rows, bits, &wire_cap); 
   Wpmos = (wire_cap / gatecap(1.0,0.0)) * 0.33 * 2/3 / fudge_factor ;
   Wnmos = (wire_cap / gatecap(1.0,0.0)) * 0.33 * 1/3 / fudge_factor ;
   // printf("3. wnmos: %f,  wpmos: %f\n", Wnmos, Wpmos);
   Ileakage = 0.5 * cmos_ileakage(Wnmos, Wpmos, Vthn, Vthp);
   leakage_power->searchline_driver_leakage = Ileakage * VddPow * bits;     

   // Sub-threshold leakage for wordline driver
   wire_cap = Cmetal * bits * (TCAMBitWidth + 2 * WIRESPACING) + gatecap(Wtcaminvn+Wtcaminvp,(10.0/SCALE_FACTOR)) 
		+ gatecap(Wtcamdriven+Wtcamnandp,(10.0/SCALE_FACTOR));
   Wpmos = (wire_cap / gatecap(1.0,0.0)) * 0.33 * 2/3 / fudge_factor;
   Wnmos = (wire_cap / gatecap(1.0,0.0)) * 0.33 * 1/3 / fudge_factor;
   // printf("4. wnmos: %f,  wpmos: %f\n", Wnmos, Wpmos);
   Ileakage = 0.5 * cmos_ileakage(Wnmos, Wpmos, Vthn, Vthp);
   leakage_power->wordline_driver_leakage = Ileakage * VddPow * rows * Nrd * no_sub_banks;     

   // total leakage power
   leakage_power->total_power = leakage_power->memcell_leakage + leakage_power->memcell_access_leakage + leakage_power->decoder_leakage +
				 leakage_power->matchline_precharge_leakage + leakage_power->searchline_driver_leakage +
				 leakage_power->wordline_driver_leakage;

   return;   
}

//  calculate TCAM power
void 
calculate_tcam_power(struct parameters tcam_parameters, struct search_power *tcam_search_power,
		struct static_power_worst_case *tcam_leakage_power,
		struct write_power *tcam_write_power, struct read_power *tcam_read_power, 
		struct search_delay *tcam_search_delay)
{
   /* calculate the power of each operation on TCAM */
   /*  1.  Search operation
       2.  Read operation
       3.  Write operation
    */
   float  FUDGEFACTOR;

   FUDGEFACTOR = TECH_USED / tcam_parameters.CMOS_tech;
   VddPow=4.5/(pow(FUDGEFACTOR,(2.0/3.0)));
   if (VddPow < 0.7)
     VddPow=0.7;
   if (VddPow > 5.0)
     VddPow=5.0;
   
   fprintf(stdout, "Scaled voltage: %f\n", VddPow);

   precalc_leakage_params(VddPow,Tkelvin,Tox, tech_length0);

   calculate_tcam_search_time(tcam_parameters , tcam_search_delay );
   calculate_tcam_search_energy(tcam_parameters, tcam_search_power );
   calculate_tcam_read_energy(tcam_parameters, tcam_read_power );
   calculate_tcam_write_energy(tcam_parameters, tcam_write_power );
   calculate_tcam_leakage_power(tcam_parameters, tcam_leakage_power );


   return;
}



