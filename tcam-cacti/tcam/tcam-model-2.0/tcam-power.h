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

#ifndef TCAM_POWER_H
#define TCAM_POWER_H

/*  feature size in um */
#define TECH_USED 	0.80
#define FEATURESIZE 	TECH_USED
#define Leff           (TECH_USED)
#define SCALE_FACTOR	(0.8/TECH_USED)

//#define WIRESPACING (2*FEATURESIZE)
//#define WIREWIDTH   (3*FEATURESIZE)
//#define WIREPITCH   (WIRESPACING+WIREWIDTH)




/* bit width of TCAM cell in um */
#ifndef TCAMBitWidth
#define TCAMBitWidth	(4.33 * TECH_USED / 0.18)
#endif

/* bit height of TCAM cell in um */
#ifndef TCAMBitHeight
#define TCAMBitHeight	(4.05 * TECH_USED / 0.18)
#endif


#define VTHNAND       0.561
#define VSINV         0.452   
#define VTHINV        0.42   /* inverter with p=360, n=240 */
#define Vth           0.50


/* 0.8um CMOS tech parameters */

//#define Cmetal 275e-18
//#define Rmetal 48e-3

extern float WIRESPACING;
extern float WIREWIDTH;
extern float WIREHEIGTHRATIO;
extern float WIREPITCH;
extern float Cmetal;
extern float Rmetal;



/* fF/um2 at 1.5V */
#define Cndiffarea    0.137e-15

/* fF/um2 at 1.5V */
#define Cpdiffarea    0.343e-15

/* fF/um at 1.5V */
#define Cndiffside    0.275e-15

/* fF/um at 1.5V */
#define Cpdiffside    0.275e-15

/* fF/um at 1.5V */
#define Cndiffovlp    0.138e-15

/* fF/um at 1.5V */
#define Cpdiffovlp    0.138e-15

/* fF/um assuming 25% Miller effect */
#define Cnoxideovlp   0.263e-15

/* fF/um assuming 25% Miller effect */
#define Cpoxideovlp   0.338e-15

/* fF/um2 */
#define Cgate         1.95e-15	

/* fF/um2 */
#define Cgatepass     1.45e-15	

/* note that the value of Cgatepass will be different depending on 
   whether or not the source and drain are at different potentials or
   the same potential.  The two values were averaged */

/* fF/um */
#define Cpolywire	(0.25e-15)			 

/* ohms*um of channel width */
#define Rnchannelstatic	(25800)

/* ohms*um of channel width */
#define Rpchannelstatic	(61200)

#define Rnchannelon	(8751)

#define Rpchannelon	(20160)



#define Wtcamcellpmos    (4.0/SCALE_FACTOR)
#define Wtcamcellnmos    (2.0/SCALE_FACTOR)
#define Wtcamcella	(2.4/SCALE_FACTOR)
#define Wtcamcellbscale	2		/* means 2x bigger than Wmemcella */


#define Wtcamdriven    (50.0/SCALE_FACTOR)
#define Wtcamdrivep    (100.0/SCALE_FACTOR)
#define Wtcamdrive2n    (200.0/SCALE_FACTOR)
#define Wtcamdrive2p    (400.0/SCALE_FACTOR)

#define Wtcamdecdrive1n    (5.0/SCALE_FACTOR)
#define Wtcamdecdrive1p    (10.0/SCALE_FACTOR)
#define Wtcamdecdrive2n    (20.0/SCALE_FACTOR)
#define Wtcamdecdrive2p    (40.0/SCALE_FACTOR)
#define Wtcamdecdriven    (50.0/SCALE_FACTOR)
#define Wtcamdecdrivep    (100.0/SCALE_FACTOR)

#define Wtcamprechn       (6.0/SCALE_FACTOR)
#define Wtcamprechp       (10.0/SCALE_FACTOR)

#define Wdummyn         (10.0/SCALE_FACTOR)
#define Wdummyinvn      (60.0/SCALE_FACTOR)
#define Wdummyinvp      (80.0/SCALE_FACTOR)


#define Wtcaminvn         (10.0/SCALE_FACTOR)
#define Wtcaminvp         (20.0/SCALE_FACTOR)

#define Waddrnandn      (50.0/SCALE_FACTOR)
#define Waddrnandp      (50.0/SCALE_FACTOR)

#define Wtcamnandn        (20.0/SCALE_FACTOR)
#define Wtcamnandp        (30.0/SCALE_FACTOR)

#define Wtcamnorn         (5.0/SCALE_FACTOR)
#define Wtcamnorp         (10.0/SCALE_FACTOR)

#define Wdecnandn       (10.0/SCALE_FACTOR)
#define Wdecnandp       (30.0/SCALE_FACTOR)
#define Wdecdrivep	(360.0/SCALE_FACTOR)
#define Wdecdriven	(240.0/SCALE_FACTOR)

#define Wtcamcomparen1		(0.32/SCALE_FACTOR)
#define Wtcamcomparen2		(0.32/SCALE_FACTOR)

#define Wcamdriven    (50.0/SCALE_FACTOR)
#define Wcamdrivep    (100.0/SCALE_FACTOR)
#define Wcamdrive2n    (200.0/SCALE_FACTOR)
#define Wcamdrive2p    (400.0/SCALE_FACTOR)
#define Wcamdecdrive1n    (5.0/SCALE_FACTOR)
#define Wcamdecdrive1p    (10.0/SCALE_FACTOR)
#define Wcamdecdrive2n    (20.0/SCALE_FACTOR)
#define Wcamdecdrive2p    (40.0/SCALE_FACTOR)
#define Wcamdecdriven    (50.0/SCALE_FACTOR)
#define Wcamdecdrivep    (100.0/SCALE_FACTOR)
#define Wcamprechn       (6.0/SCALE_FACTOR)
#define Wcamprechp       (10.0/SCALE_FACTOR)
#define Wcaminvn         (10.0/SCALE_FACTOR)
#define Wcaminvp         (20.0/SCALE_FACTOR)

#define Wsenseextdrv1p (80.0/SCALE_FACTOR)
#define Wsenseextdrv1n (40.0/SCALE_FACTOR)
#define Wsenseextdrv2p (240.0/SCALE_FACTOR)
#define Wsenseextdrv2n (160.0/SCALE_FACTOR)
#define WsenseN        (3.75*TECH_USED)
#define WsenseP        (7.5*TECH_USED)

#define WsPch		(6.25*TECH_USED)

#define Wdec3to8n     120.0
#define Wdec3to8p     60.0
#define WdecNORn       2.4
#define WdecNORp      12.0
#define Wdecinvn      20.0
#define Wdecinvp      40.0 
#define Wworddrivemax 100.0
#define Wmemcellpmos    (4.0)
#define Wmemcellnmos    (2.0)
#define Wmemcella	(2.4)
#define Wmemcellbscale	2		/* means 2x bigger than Wmemcella */
#define Wbitpreequ	(80.0)
#define Wbitmuxn	(10.0)
#define WsenseQ1to4	(4.0)
#define Wcompinvp1	(10.0)
#define Wcompinvn1	(6.0)
#define Wcompinvp2	(20.0)
#define Wcompinvn2	(12.0)
#define Wcompinvp3	(40.0)
#define Wcompinvn3	(24.0)
#define Wevalinvp	(80.0)
#define Wevalinvn	(40.0)



#define krise		(0.4e-9)
#define tsensedata	(5.8e-10)
#define psensedata      (0.02e-9)
#define tsensescale     0.02e-10
#define tsensetag	(2.6e-10)
#define psensetag	(0.016e-9)
#define tfalldata	(7e-10)
#define tfalltag	(7e-10)
#define Vbitpre		(3.3)
#define Vt		(1.09)
#define Vbitsense	(0.10)



#define NCH 	1
#define PCH     0

#define RISE		1
#define FALL		0


/* high-speed and low-power CMOS priority encoder in jssc */
#define PriorityEncoderEnergy256x1  (1.5e-12 * TECH_USED / 0.18 )
#define PriorityEncoderDelay256x1  (3.67e-10 * TECH_USED / 0.18 )


typedef unsigned int uint32_t; 

struct parameters {
  	uint32_t number_of_rows; /* total number of rows */
	uint32_t number_of_column_bits;
	float  CMOS_tech;
	uint32_t number_of_subbanks;
	uint32_t Nrd ;		// Number of rows divisions to minimize the bitline access time
};


struct search_delay {
        double routing_delay ;
   	double intra_subbank_routing_delay ;

 	double matchline_precharge_delay;
 	double matchline_discharge_delay;
	double searchline_precharge_delay;
	double searchline_discharge_delay;
	double precharge_delay;

	double priority_encoder_delay;

	double total_delay;
	double max_delay;
};

struct static_power_worst_case {
	double memcell_leakage;
	double memcell_access_leakage;
	double decoder_leakage;
	double matchline_precharge_leakage;
	double bitline_precharge_leakage;
	double searchline_driver_leakage;
	double wordline_driver_leakage;
	double total_power ;
};

struct search_power {
	float routing_power ;
	float intra_subbank_routing_power ;

 	float matchline_power;
	float searchline_power;
	float priority_encoder_power;

	float total_power ;
};

struct read_power {
	float routing_power;
	float decoder_power;
 	float wordline_power;
	float bitline_power;
	float senseamp_power;
	float total_power;
};

struct write_power {
	float routing_power;
	float decoder_power;
 	float wordline_power;
	float bitline_power;
	float total_power;
};

// calculate TCAM power
void calculate_tcam_power(struct parameters tcam_parameters, struct search_power *tcam_search_power, 
			struct static_power_worst_case *tcam_leakage_power,
			struct write_power *tcam_write_power, struct read_power *tcam_read_power,
			struct search_delay *tcam_search_delay);

#endif /* TCAM_POWER_H */

