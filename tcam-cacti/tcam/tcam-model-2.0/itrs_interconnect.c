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

#include<stdio.h>
#include<stdlib.h>

#include "tcam-power.h"
#include "itrs_interconnect.h"

int num_tech_node=6;
ITRS_roadmap_data itrs_predicted_data[20] =  { 
	{90, 214,1.7,29.9,   275,1.7,27,  360,2.2,24.5,   3.4,  2.7}, 
	{65, 136,1.7,34.7,   140,1.8,34.3,  210,2.3,27.3,   2.8,  2.4}, 
	{45, 90,1.8,40.8,   90,1.8,40.8,  135,2.4,31,   2.7,  2.2}, 
	{32, 64,1.9,48.3,   64,1.9,48.3,  96,2.5,35.2,   2.3,  2.0}, 
	{22, 44,2.0,60.1,   44,2.0,60.1,  66,2.6,42,   2.1,  1.8}, 
	{18, 36,2.0,63.3,   36,2.0,63.3,  54,2.8,45.8,   1.8,  1.6}
};

// Constants
float permittivity0=8.854e-21;	/* permittivity in F/nm */
float permittivity;	/* permittivity of inter-metallic layers for different technology node */

// globals
float tech;

// cacti parameters to be set (declared in tcam-power.h file)
float WIRESPACING=2*FEATURESIZE;
float WIREWIDTH=3*FEATURESIZE;
float WIREHEIGTHRATIO=1.8;
float WIREPITCH=5*FEATURESIZE;
float Cmetal=275e-18;
float Rmetal=48e-3;

/* 
   Link: http://www.eas.asu.edu/~ptm/cgi-bin/interconnect/local.cgi

   R = resisitivity *  l  /  ( t * w) ;
   
   Ctotal  = 2 * Cg + 2 * Cc;

   Cg : area and fringe flux to the underlying plane 
     Cg = permittivity * [ w/h + 2.04*(s/(s+0.54h ))^1.77 * (t/(t+4.53h))^0.07 ]

   Cc: coupling capacitance
     Cc =  permittivity * [ 1.14*t/s*e^(-4s/(s+8.01h) + 2.37(w/(w+0.31s))^0.28*(h/(h+8.96s))^0.76*e^(-2s/(s+6h)) ]

*/

double
model_cu_wire_cmetal()
{
   int i;
   float Cg, Cc;
   float L=1000;
   float w,t,s,h;
   double itrs_cu_wire_capacitance;

    /* calculate the capacitance using ITRS data */
    for(i=0; i<num_tech_node; i++) {
	if((int)itrs_predicted_data[i].tech_node==(int)tech) {
  		permittivity = itrs_predicted_data[i].effective_permittivity * permittivity0 * 1e3;  /* F/nm to F/um */
	        w = itrs_predicted_data[i].inter_wire_pitch / 2 * 1e-3;
		t = w * itrs_predicted_data[i].inter_wire_aspect_ratio;
    		s = w;
    		h = 2 * w;
    		Cg  =  permittivity * ( w/h  +  2.04 * powf((s/(s+0.54*h)), 1.77) * powf((t/(t+4.53*h)), 0.07) ) * L * 1e-3;
    		Cc = permittivity * ( 1.412 * (t/s) * powf(2.7182818, (-4.0*s/(s+8.01*h))) +  
           		2.37 * powf( (w/(w+0.31*s)), 0.28) * powf( (h/(h+8.96*s)), 0.76) * powf(2.7182818, (-2.0*s/(s+6.0*h))) )  * L * 1e-3;
		break;
        }
    }
    if(i==num_tech_node) {
	itrs_cu_wire_capacitance = 0;
	fprintf(stderr, "This technology node is not stored in our current ITRS prediction data for calculating Cmetal\n");
    }
    else {
    	itrs_cu_wire_capacitance = (2 * Cg + 2 * Cc) ;
    }
    // fprintf(stderr, "ITRS ->   Cg: %e,  Cc: %e\n", Cg, Cc);

    return itrs_cu_wire_capacitance;
}

double
model_cu_wire_rmetal()
{
    int i;
    float w,t;
    float L=1000;	// for 1um, 
    double itrs_cu_wire_resistance;

    /* calculate the resistance using ITRS data */
    for(i=0; i<num_tech_node; i++) {
	if((int)itrs_predicted_data[i].tech_node==(int)tech) {
	        w = itrs_predicted_data[i].inter_wire_pitch / 2;
		t = w * itrs_predicted_data[i].inter_wire_aspect_ratio;
		itrs_cu_wire_resistance = itrs_predicted_data[i].inter_wire_resistivity * L / ( w * t ) ;
		break;
        }
    }
    if(i==num_tech_node) {
	itrs_cu_wire_resistance = 0;
	fprintf(stderr, "This technology node is not stored in our current ITRS prediction data for calculating Rmetal\n");
    }

    return itrs_cu_wire_resistance;
}

int 
init_wire_parameters_from_itrs(double technology)
{
   int i, found=0;
   float L, w;
   int tech_in_nm = round(technology * 1000);

   // if technology is older than 130nm then just assign the cacti values, else
   // set the itrs paramaters for wires, rmetal and cmetal
   if(tech_in_nm >= 129) {
        WIRESPACING=2*FEATURESIZE;
        WIREWIDTH=3*FEATURESIZE;
        WIREHEIGTHRATIO=1.8;
        WIREPITCH=WIRESPACING+WIREWIDTH;
	tech = (int) technology * 1000;
	// set it for 0.80 technology
        Cmetal=275e-18;
  	Rmetal = 48e-3;
        found=1;
	fprintf(stderr, "ITRS: Wire Cmetal: %e,   Rmetal: %e for technology node %fum\n",  Cmetal, Rmetal, technology);
	return found;
   } 
 
   // closest tech node for 100nm and 70nm nodes 
   if((tech_in_nm < 101) && (tech_in_nm > 99)) {
	tech_in_nm = 90;
   }
   if((tech_in_nm < 71) && (tech_in_nm > 69)) {
	tech_in_nm = 65;
   }
   for(i=0; i<num_tech_node; i++) {
	if((int)itrs_predicted_data[i].tech_node==(int)tech_in_nm) {
		break;
	}
   }
   if(i==num_tech_node) {
	fprintf(stderr, "Technology node (%fnm) not stored in the ITRS data\n", technology*1000);
	exit(1);
   }
   else {
        WIRESPACING=itrs_predicted_data[i].inter_wire_pitch/2.0;
        WIREWIDTH=WIRESPACING;
        WIREHEIGTHRATIO=itrs_predicted_data[i].inter_wire_aspect_ratio;
        WIREPITCH=WIRESPACING+WIREWIDTH;
	tech = tech_in_nm;
	// set it for 0.80um technology since we scale it accordingly at the end
        Cmetal=model_cu_wire_cmetal() ;
  	Rmetal = model_cu_wire_rmetal() ;
        found=1;
	fprintf(stderr, "ITRS: Wire Cmetal: %e,   Rmetal: %e for technology node %fum\n",  Cmetal, Rmetal, technology);
   }

   return found;
}

