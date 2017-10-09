/*------------------------------------------------------------
 *                              CACTI 4.0
 *         Copyright 2005 Hewlett-Packard Development Corporation
 *                         All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein, and
 * hereby grant back to Hewlett-Packard Company and its affiliated companies ("HP")
 * a non-exclusive, unrestricted, royalty-free right and license under any changes, 
 * enhancements or extensions  made to the core functions of the software, including 
 * but not limited to those affording compatibility with other hardware or software
 * environments, but excluding applications which incorporate this software.
 * Users further agree to use their best efforts to return to HP any such changes,
 * enhancements or extensions that they make and inform HP of noteworthy uses of
 * this software.  Correspondence should be provided to HP at:
 *
 *                       Director of Intellectual Property Licensing
 *                       Office of Strategy and Technology
 *                       Hewlett-Packard Company
 *                       1501 Page Mill Road
 *                       Palo Alto, California  94304
 *
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HP DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL HP 
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *------------------------------------------------------------*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "leakage.h"

/*dt: added for windows compatibility */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double inv_Leff;

/* Box-Mueller Method */

double box_mueller(double std_var, double value)
{
	double temp;
	double temp1;
	double random;
	/* dt: drand48 not supported in windows 
			random = drand48();
	*/
	random = rand();
	temp = sqrt((double)(-2.00 * (double)log(random)));
	random = rand();
	
	temp1 = cos(2.00 * M_PI * random);

	return(temp * temp1 * std_var * value);
}


/* ************************************************************************ */

/* Calculating the NMOS I Normalized Leakage From the BSIM Equation.*/
/* Also Using Box-Mueller to Find the Random Samples Due to Any Variation */
/* In any of the parameters like length, Vdd etc. */
/* ************************************************************************ */
double nmos_ileakage(double aspect_ratio, double Volt, double Vth0, double Tkelvin, double tox0)
{
	double Tox_Std_Array[No_of_Samples];
	double Vdd_Std_Array[No_of_Samples];
	double Tech_Std_Array[No_of_Samples];
	double Vthn_Std_Array[No_of_Samples];
	double Ileak_Std_Array[No_of_Samples];
	int i;
	double mean =0.0;

	if(Tox_Std || Tech_Std || Vdd_Std || Vthn_Std)
	{
		for(i =0; i<No_of_Samples;i++)
		{
		    Tox_Std_Array[i] = tox0;
		    Vdd_Std_Array[i] = Volt;
		    Tech_Std_Array[i] = tech_length0;
		    Vthn_Std_Array[i] = Vth0;
		}
	}

	if(Tox_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tox_Std_Array[i] = tox0 + box_mueller(Tox_Std,tox0);
	  }
	if(Tech_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tech_Std_Array[i] = tech_length0 + box_mueller(Tech_Std,tech_length0);
	  }
	if(Vdd_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vdd_Std_Array[i] = Volt + box_mueller(Vdd_Std,Volt);
	  }
	if(Vthn_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vthn_Std_Array[i] = Vth0 + box_mueller(Vthn_Std,Vth0);
	  }

	if(Tox_Std || Tech_Std || Vdd_Std || Vthn_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      {
	     Ileak_Std_Array[i] = nmos_ileakage_var(aspect_ratio, Vdd_Std_Array[i], Vthn_Std_Array[i],  Tkelvin,  Tox_Std_Array[i], Tech_Std_Array[i]);

	      }
	  }
	else
	  {
		return(nmos_ileakage_var(aspect_ratio,Volt,Vth0,Tkelvin, tox0, tech_length0));
	  }

	for(i =0; i<No_of_Samples;i++)
		mean += Ileak_Std_Array[i];
	mean = mean/(double)No_of_Samples;

	return mean;


}

double nmos_ileakage_var(double aspect_ratio, double Volt, double Vth0, double Tkelvin, double tox0, double tech_length)
{
	double Ileak;
	double Vthermal;
	double Vth,temp , Vnoff;
	double param1,param2,param3, param4,param5,param6;
	double temp1;
	param1 = (aspect_ratio * tech_length0 *M0n*Cox)/tech_length;
	/* Thermal Voltage */
	Vthermal =((Bk*Tkelvin)/Qparam);
	/* Vdd Fitting */
	temp =  Nb*(Volt- Volt0);
	param2 = exp(temp);
	param3 = 1-exp((-Volt/Vthermal));
	Vth =Vth0 + Vnthx * (Tkelvin-300);
	Vnoff = Vnoff0 + Nfix*(Vth0-Vthn);
	param4 = exp(((-fabs(Vth)-Vnoff)/(NEta*Vthermal)));
	temp = (tech_length0 - tech_length) * L_nmos_d ;
	param5 = exp(temp);
	temp1 = (tox0 - Tox) * Tox_nmos_e;
	param6 = exp(temp1);
	Ileak = param1*pow(Vthermal,2.0)*param2*param3*param4*param5*param6;

	return Ileak;
}

/* ************************************************************************ */

/* Calculating the PMOS I Normalized Leakage From the BSIM Equation.*/
/* Also Using Box-Mueller to Find the Random Samples Due to Any Variation */
/* In any of the parameters like length, Vdd etc. */
/* ************************************************************************ */
double pmos_ileakage(double aspect_ratio,double Volt, double Vth0,double Tkelvin,double tox0)
{

	double Tox_Std_Array[No_of_Samples];
	double Vdd_Std_Array[No_of_Samples];
	double Tech_Std_Array[No_of_Samples];
	double Vthp_Std_Array[No_of_Samples];
	double Ileak_Std_Array[No_of_Samples];
	int i;
	double mean =0.0;

	if(Tox_Std || Tech_Std || Vdd_Std || Vthp_Std) {
		for(i =0; i<No_of_Samples;i++)
	  	{
	  		Tox_Std_Array[i] = tox0;
	  		Vdd_Std_Array[i] = Volt;
	    	Tech_Std_Array[i] = tech_length0;
	    	Vthp_Std_Array[i] = Vth0;
	  	}
	}

	if(Tox_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tox_Std_Array[i] = tox0 + box_mueller(Tox_Std,tox0);
	  }
	if(Tech_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tech_Std_Array[i] = tech_length0 + box_mueller(Tech_Std,tech_length0);
	  }
	if(Vdd_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vdd_Std_Array[i] = Volt + box_mueller(Vdd_Std,Volt);
	  }
	if(Vthp_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vthp_Std_Array[i] = Vth0 + box_mueller(Vthp_Std,Vth0);
	  }

	if(Tox_Std || Tech_Std || Vdd_Std || Vthp_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
		    Ileak_Std_Array[i] =  pmos_ileakage_var(aspect_ratio, Vdd_Std_Array[i], Vthp_Std_Array[i],  Tkelvin,  Tox_Std_Array[i], Tech_Std_Array[i]);
	  }

	else
	  {
		return (pmos_ileakage_var(aspect_ratio,Volt,  Vth0, Tkelvin, tox0, tech_length0));
	  }

	for(i =0; i<No_of_Samples;i++)
		mean += Ileak_Std_Array[i];
	mean = mean/(double)No_of_Samples;

	return mean;

}

double pmos_ileakage_var(double aspect_ratio,double Volt, double Vth0,double Tkelvin,double tox0, double tech_length) {
	double Ileak;
	double Vthermal;
	double Vth, temp ,temp1,Vpoff;
	double param1,param2,param3,param4,param5,param6;
	param1 = (aspect_ratio * tech_length0 *M0p*Cox )/tech_length;
	/* Thermal Voltage */
	Vthermal =((Bk*Tkelvin)/Qparam);
	/* Vdd Fitting */
	temp =  Pb*(Volt- Volt0);
	param2 = exp(temp);
	param3 = 1-exp((-Volt/Vthermal));
	Vth =Vth0 + Vpthx * (Tkelvin-300);
	Vpoff = Vpoff0 + Pfix*(Vth0-Vthp);
	param4 = exp(((-fabs(Vth)-Vpoff)/(PEta*Vthermal)));
	temp = (tech_length0 - tech_length) * L_nmos_d ;
	param5 = exp(temp);
	temp1 = (tox0 - Tox) * Tox_nmos_e;
	param6 = exp(temp1);
	Ileak = param1*pow(Vthermal,2.0)*param2*param3*param4*param5*param6;

	return Ileak;
}

double simplified_cmos_leakage(double naspect_ratio,double paspect_ratio, double nVth0, double pVth0,
							   double *norm_nleak, double *norm_pleak)
{
	double sum;
	double pIleak,nIleak;
	double nVth,pVth,Vpoff,Vnoff;
	double nparam1,pparam1,nparam4,pparam4;

	nparam1 = naspect_ratio * precalc_nparamf;
	pparam1 = paspect_ratio * precalc_pparamf;
	
	nVth =nVth0 + precalc_Vnthx;
	pVth =pVth0 + precalc_Vpthx;
	Vnoff = Vnoff0 + Nfix*(nVth0-Vthn);
	Vpoff = Vpoff0 + Pfix*(pVth0-Vthp);
	nparam4 = exp(((-fabs(nVth)-Vnoff)*precalc_inv_nVthermal));
	pparam4 = exp(((-fabs(pVth)-Vpoff)*precalc_inv_pVthermal));

	*norm_nleak = precalc_nparamf*nparam4*precalc_nparaml;
	*norm_pleak = precalc_pparamf*pparam4*precalc_pparaml;
	
	nIleak = nparam1*nparam4*precalc_nparaml;
	pIleak = pparam1*pparam4*precalc_pparaml;

	sum = nIleak + pIleak;
	return sum;

}
double optimized_simplified_cmos_leakage(double naspect_ratio,double paspect_ratio, double nVth0, double pVth0,
							   double * nleak, double * pleak)
{
	double sum;
	double pIleak,nIleak;
	double nVth,pVth,Vpoff,Vnoff;
	double nparam1,pparam1,nparam4,pparam4;

	nparam1 = naspect_ratio * precalc_nparamf;
	pparam1 = paspect_ratio * precalc_pparamf;
	
	nVth =nVth0 + precalc_Vnthx;
	pVth =pVth0 + precalc_Vpthx;
	Vnoff = Vnoff0 + Nfix*(nVth0-Vthn);
	Vpoff = Vpoff0 + Pfix*(pVth0-Vthp);
	nparam4 = exp(((-fabs(nVth)-Vnoff)/(NEta*precalc_Vthermal)));
	pparam4 = exp(((-fabs(pVth)-Vpoff)/(PEta*precalc_Vthermal)));
	
	nIleak = nparam1*nparam4*precalc_nparaml;
	pIleak = pparam1*pparam4*precalc_pparaml;
	(*nleak) = precalc_nparamf*nparam4*precalc_nparaml;
	(*pleak) = precalc_pparamf*pparam4*precalc_pparaml;

	sum = nIleak + pIleak;
	return sum;

}
double simplified_nmos_leakage(double naspect_ratio, double nVth0)
{
	//double sum;
	double nIleak;
	double nVth,Vnoff;
	double nparam1,nparam4;

	if(have_leakage_params) {
		nparam1 = naspect_ratio * precalc_nparamf;
		nVth =nVth0 + precalc_Vnthx;
		Vnoff = Vnoff0 + Nfix*(nVth0-Vthn);
		nparam4 = exp(((-fabs(nVth)-Vnoff)/(NEta*precalc_Vthermal)));
		nIleak = nparam1*nparam4*precalc_nparaml;
	}
	else {
		nIleak = 0;
	}

	return nIleak;
}

double simplified_pmos_leakage(double paspect_ratio, double pVth0)
{
	//double sum;
	double pIleak;
	double pVth,Vpoff;
	double pparam1,pparam4;

	if(have_leakage_params) {
		pparam1 = paspect_ratio * precalc_pparamf;
		pVth =pVth0 + precalc_Vpthx;
		Vpoff = Vpoff0 + Pfix*(pVth0-Vthp);
		pparam4 = exp(((-fabs(pVth)-Vpoff)/(PEta*precalc_Vthermal)));
		pIleak = pparam1*pparam4*precalc_pparaml;
	}
	else {
		pIleak = 0;
	}
		
	return pIleak;
}

double 
cmos_ileakage(double nWidth, double pWidth, double nVthreshold, double pVthreshold) 
{
        double leakage = 0.0;
        static int valid_cache = 0;
        static double cached_nmos_thresh = 0;
        static double cached_pmos_thresh = 0;

        static double norm_nmos_leakage = 0;
        static double norm_pmos_leakage = 0;

        if(have_leakage_params) {
		if((cached_nmos_thresh == nVthreshold) && (cached_pmos_thresh == pVthreshold) && valid_cache) {
			leakage = nWidth*norm_nmos_leakage + pWidth*norm_pmos_leakage;
		}
		else {
			leakage = simplified_cmos_leakage(nWidth*inv_Leff,pWidth*inv_Leff,nVthreshold,pVthreshold,&norm_nmos_leakage,&norm_pmos_leakage);
			cached_nmos_thresh = nVthreshold;
			cached_pmos_thresh = pVthreshold;
			norm_nmos_leakage = inv_Leff*norm_nmos_leakage;
			norm_pmos_leakage = inv_Leff*norm_pmos_leakage;
			valid_cache = 1;
		}
        }
        else {
                leakage = 0;
        }
        return leakage;
}



void 
precalc_leakage_params(double Volt,double Tkelvin,double tox0, double tech_length) {
	double temp1, temp2, temp3;
	double precalc_param3, precalc_param5, precalc_param6;

	precalc_Vnthx = Vnthx * (Tkelvin-300);
	precalc_Vpthx = Vpthx * (Tkelvin-300);
	/* Thermal Voltage */
	precalc_Vthermal =((Bk*Tkelvin)/Qparam);
	precalc_inv_nVthermal = 1.0/(NEta *precalc_Vthermal);
	precalc_inv_pVthermal = 1.0/(PEta *precalc_Vthermal);
	precalc_nparamf = tech_length0 *M0n*Cox /tech_length;
	precalc_pparamf = tech_length0 *M0p*Cox /tech_length;
	/* Vdd Fitting */
	temp1 =  Nb*(Volt- Volt0);
	precalc_nparam2 = exp(temp1);
	temp1 =  Pb*(Volt- Volt0);
	precalc_pparam2 = exp(temp1);
	precalc_param3 = 1-exp((-Volt/precalc_Vthermal));

	temp2 = (tech_length0 - tech_length) * L_nmos_d ;
	precalc_param5 = exp(temp2);
	temp3 = (tox0 - Tox) * Tox_nmos_e;
	precalc_param6 = exp(temp3);
	precalc_nparaml = pow(precalc_Vthermal,2.0)*precalc_nparam2*precalc_param3*precalc_param5*precalc_param6;
	precalc_pparaml = pow(precalc_Vthermal,2.0)*precalc_pparam2*precalc_param3*precalc_param5*precalc_param6;
}

void 
init_leakage_params(double technology) {

	double tech = technology * 1000.0;
	Tkelvin = 373;

	have_leakage_params = 1;

	//tech = (int) ceil(technology * 1000.0);

	if (tech < 181 && tech > 179) {
		process_tech = 0.18;
		tech_length0 = 180E-9;
		M0n =   3.5E-2;      /* Zero Bias Mobility for N-Type */
		M0p =  8.0E-3 ;      /* Zero Bias Mobility for P-Type */
		Tox = 3.5E-9;

		Cox = (Eox/Tox);     /* Gate Oxide Capacitance per unit area */
		Vnoff0 = 7.5E-2 ;    /* Empirically Determined Model Parameter for N-Type */
	                         /* FIX ME */
		Vpoff0 = -2.8e-2 ;   /* Empirically Determined Model Parameter for P-Type */
  	  	Nfix =  0.22;        /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Pfix =  0.17 ;       /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */

  		Vthn =  0.3979 ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) (original) */
  		Vthp =  0.4659 ;     /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) (original) */

  		Vnthx = -1.0E-3 ;    /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
  		Vpthx =  -1.0E-3 ;   /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
  		Vdd_init=  2.0 ;     /* Default Vdd. Can be Changed in leakage.c */
  		Volt0 =  1.7 ;

  		Na    =  -1.5 ;      /* Empirical param for the Vdd fit */
  		Nb    =   1.2 ;      /* Empirical param for the Vdd fit */
  		Pa    =   5.0 ;      /* Empirical param for the Vdd fit */
  		Pb    =   0.75 ;     /* Empirical param for the Vdd fit */
  		NEta =  1.5 ;        /* Sub-threshold Swing Co-efficient N-Type */
  		PEta =  1.6  ;       /* Sub-threshold Swing Co-efficient P-Type */

		/* gate Vss */
  		Vth0_gate_vss= 0.65;
  		aspect_gate_vss= 5;

		/*drowsy cache*/
  		Vdd_low= 0.6;

		/*RBB*/
  		k1_body_n= 0.5613;
  		k1_body_p= 0.5560;
  		vfi = 0.6;

  		VSB_NMOS= 0.5;
  		VSB_PMOS= 0.5;

		/* dual VT*/
  		Vt_cell_nmos_high= 0.45  ;
  		Vt_cell_pmos_high= 0.5;
  		Vt_bit_nmos_low = 0.35;
  		Vt_bit_pmos_low = 0.4;

  		L_nmos_d  = 0.1E+9;    /* Adjusting Factor for Length */
  		Tox_nmos_e  = 2.6E+9;  /* Adjusting Factor for Tox */
  		L_pmos_d  = 0.1E+9;    /* Adjusting Factor for Length */
  		Tox_pmos_e  = 2.6E+9;  /* Adjusting Factor for Tox */
	}

	/* TECH_POINT130nm */

	else if (tech < 131 && tech > 129)  {

		process_tech = 0.13;
  		tech_length0 = 130E-9;
  		M0n =  1.34E-2;      /* Zero Bias Mobility for N-Type */
  		M0p =  5.2E-3 ;      /* Zero Bias Mobility for P-Type */
  		Tox =  3.3E-9;
  		//Tox = 2.52731e-09;
  		Cox =  (Eox/Tox);    /* Gate Oxide Capacitance per unit area */
  		Vnoff0 = -6.2E-2 ;   /* Empirically Determined Model Parameter for N-Type */
  		//Vnoff0 = -1.68E-1 ;   /* Empirically Determined Model Parameter for N-Type */
  		Vpoff0 = -0.1;       /* Empirically Determined Model Parameter for P-Type */
  		//Vpoff0 = -0.28;       /* Empirically Determined Model Parameter for P-Type */
  		Nfix =   0.16 ;      /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Pfix =  0.13 ;       /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vthn =  0.3353;      /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Vthp =  0.3499 ;     /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vnthx = -0.85E-3;    /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
  		Vpthx = -0.1E-3;     /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
  		Vdd_init =  1.5 ;    /* Default Vdd. */
  		//Vdd_init =  1.08 ;    /* Default Vdd. */
		//Vdd_init = 1.3;
  		Volt0 =  1.5;

  		Na  =    3.275 ;     /* Empirical param for the Vdd fit */
  		Nb  =     1.1 ;      /* Empirical param for the Vdd fit */
  		Pa  =     4.65 ;     /* Empirical param for the Vdd fit */
  		Pb  =     2.2 ;      /* Empirical param for the Vdd fit */
  		NEta =  1.6 ;        /* Sub-threshold Swing Co-efficient N-Type */
  		PEta = 1.8   ;       /* Sub-threshold Swing Co-efficient P-Type */


		/* gate Vss */
  		Vth0_gate_vss = 0.55;
  		aspect_gate_vss = 5;

		/*drowsy cache*/
  		Vdd_low =  0.45;

		/*RBB*/
  		k1_body_n = 0.3662;
  		k1_body_p = 0.4087;
  		vfi =  0.55;
  		VSB_NMOS = 0.45;
  		VSB_PMOS = 0.45;

		/* dual VT*/
  		Vt_cell_nmos_high = 0.38  ;
  		Vt_cell_pmos_high=  0.4;
  		Vt_bit_nmos_low = 0.28;
  		Vt_bit_pmos_low = 0.29;


  		L_nmos_d  = 0.285E+9;  /* Adjusting Factor for Length */
  		Tox_nmos_e  = 4.3E+9;  /* Adjusting Factor for Tox */
  		L_pmos_d  = 0.44E+9;   /* Adjusting Factor for Length */
  		Tox_pmos_e  = 5.0E+9;  /* Adjusting Factor for Tox */
	}

	/* TECH_POINT100nm */

     else if (tech < 101 && tech > 99)  {

  		process_tech = 0.10;
  		tech_length0 = 100E-9;
  		M0n = 1.8E-2 ;      /* Zero Bias Mobility for N-Type */
  		M0p = 5.5E-3  ;     /* Zero Bias Mobility for P-Type */
  		Tox = 2.5E-9 ;
  		Cox = (Eox/Tox);    /* Gate Oxide Capacitance per unit area */
  		Vnoff0 = -2.7E-2;   /* Empirically Determined Model Parameter for N-Type */
  		                    /* FIX ME */
  		Vpoff0 = -1.04E-1;  /* Empirically Determined Model Parameter for P-Type */
  		Nfix  = 0.18  ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Pfix =  0.14   ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vthn =  0.2607  ;   /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Vthp =  0.3030 ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vnthx = -0.77E-3 ;  /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
  		Vpthx =  -0.72E-3;  /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
  		Vdd_init = 1.2 ;    /* Default Vdd. Can be Changed for different parts in leakage.c */
  		Volt0 =  1.0;
  		Na   =   3.6 ;      /* Empirical param for the Vdd fit */
  		Nb   =   2.5  ;     /* Empirical param for the Vdd fit */
  		Pa   =    4.49 ;    /* Empirical param for the Vdd fit */
  		Pb   =    2.4 ;     /* Empirical param for the Vdd fit */

  		NEta =  1.7 ;       /* Sub-threshold Swing Co-efficient N-Type */
  		PEta =  1.7  ;      /* Sub-threshold Swing Co-efficient P-Type */


		/* gate Vss */
  		Vth0_gate_vss = 0.50;
  		aspect_gate_vss = 5;

		/*drowsy cache*/
  		Vdd_low =  0.36;

		/*RBB*/
  		k1_body_n =  0.395;
  		k1_body_p =  0.391;
  		vfi = 0.5;
  		VSB_NMOS =  0.4;
  		VSB_PMOS =  0.4 ;

		/* dual VT*/
  		Vt_cell_nmos_high =  0.31  ;
  		Vt_cell_pmos_high =  0.35;
  		Vt_bit_nmos_low  = 0.21;
  		Vt_bit_pmos_low = 0.25;


  		L_nmos_d  = 0.102E+9;   /* Adjusting Factor for Length */
  		Tox_nmos_e  = 2.5E+9;   /* Adjusting Factor for Tox */
  		L_pmos_d  = 0.21E+9;    /* Adjusting Factor for Length */
  		Tox_pmos_e  = 3.1E+9;   /* Adjusting Factor for Tox */

 	} else if (tech > 69 && tech < 71)  {

		process_tech = 0.07;
 		tech_length0 = 70E-9;
 		M0n =  3.5E-2 ;     /* Zero Bias Mobility for N-Type */
 		M0p = 8.0E-3 ;      /* Zero Bias Mobility for P-Type */
 		Tox = 1.0E-9;
 		Cox = (Eox/Tox) ;   /* Gate Oxide Capacitance per unit area */
 		Vnoff0 = -3.9E-2 ;  /* Empirically Determined Model Parameter for N-Type */
 		                    /* FIX ME */
 		Vpoff0 = -4.35E-2;  /* Empirically Determined Model Parameter for P-Type */
 		Nfix =  -0.05 ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
 		Pfix =  0.07 ;      /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
 		Vthn =  0.1902  ;   /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
 		Vthp =  0.2130 ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
 		Vnthx = -0.49E-3 ;  /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
 		Vpthx =  -0.54E-3;  /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
 		Vdd_init =  1.0;    /* Default Vdd. Can be Changed in leakage.c */
 		Volt0 =  0.8;
 		Na   =   -2.94 ;    /* Empirical param for the Vdd fit */
 		Nb   =    2.0  ;    /* Empirical param for the Vdd fit */
 		Pa   =    -2.245;   /* Empirical param for the Vdd fit */
 		Pb   =    2.89;     /* Empirical param for the Vdd fit */
 		NEta =  1.3;        /* Sub-threshold Swing Co-efficient N-Type */
 		PEta  = 1.4 ;       /* Sub-threshold Swing Co-efficient P-Type */

		/*gate leakage factor for 70nm*/
 		nmos_unit_leakage = 53e-12;
 		a_nmos_vdd = 7.75;
 		b_nmos_t =  0.15e-12;
 		c_nmos_tox =  11.75e9;

		pmos_unit_leakage = 22.9e-12;
 		a_pmos_vdd = 12;
 		b_pmos_t =  0.086e-12;
 		c_pmos_tox = 11.25e9;

		/* gate Vss */
 		Vth0_gate_vss =  0.45;
 		aspect_gate_vss =  5;

 	 	/*drowsy cache*/
 		Vdd_low =  0.3;

		/*RBB*/
 		k1_body_n = 0.37;
 		k1_body_p = 0.38;
 		vfi = 0.45;
 		VSB_NMOS = 0.35 ;
 		VSB_PMOS = 0.35;

		/* dual VT*/
 		Vt_cell_nmos_high =  0.29 ;
 		Vt_cell_pmos_high =  0.31;
 		Vt_bit_nmos_low = 0.19;
 		Vt_bit_pmos_low = 0.21;

 		L_nmos_d  = 0.1E+9;    /* Adjusting Factor for Length */
 		Tox_nmos_e  = 3.0E+9;  /* Adjusting Factor for Tox */
 		L_pmos_d  = 0.225E+9;  /* Adjusting Factor for Length */
 		Tox_pmos_e  = 4.0E+9;  /* Adjusting Factor for Tox */

	}

	else {
		have_leakage_params = 0;
		//printf("\n ERROR: Technology parameters not known for %3.4f feature size \n    (Parameters available for 0.18u, 0.13, 0.10, and 0.07u technologies)\nExiting...\n\n");
		//exit(0);
	}

	inv_Leff = 1.0 / technology ;

        return;
}
