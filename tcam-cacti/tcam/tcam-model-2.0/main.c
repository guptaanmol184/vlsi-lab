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

#include <stdio.h>
#include <stdlib.h>

#include "tcam-power.h"

/* global variables */
int use_itrs=0;
enum tcam_operation { tcam_search, tcam_read, tcam_write};
struct parameters tcam_parameters;
struct search_power tcam_search_power;
struct static_power_worst_case tcam_leakage_power;
struct read_power tcam_read_power;
struct write_power tcam_write_power;
struct search_delay tcam_search_delay;

/* global function declarations */
void process_option_string(int argc, char *argv[]);
void print_tcam_output();

int 
main(int argc, char *argv[])
{
   // tcam-power <rows> <no-bits> <tech> <sub-banks>
   process_option_string(argc, argv);

   // Intialize the technology parameters for leakage power in tcam
   init_leakage_params(tcam_parameters.CMOS_tech);

   // If using ITRS data, then initialize the wire features based on ITRS roadmap
   if(use_itrs) {
	init_wire_parameters_from_itrs(tcam_parameters.CMOS_tech);
   }

   // calculate TCAM power
   calculate_tcam_power(tcam_parameters, &tcam_search_power, &tcam_leakage_power, &tcam_write_power, &tcam_read_power, &tcam_search_delay);

   // print the output results
   print_tcam_output(); 
}


void 
process_option_string(int argc, char *argv[])
{
  if(argc < 5 ) {
	fprintf(stderr, "Usage: tcam-power <rows> <bits> <TECH> <sub-banks> [Number-row-divisions] \n");
	exit(1);
  }

  tcam_parameters.number_of_rows = atoi(argv[1]);
  tcam_parameters.number_of_column_bits = atoi(argv[2]);
  tcam_parameters.CMOS_tech = atof(argv[3]);
  tcam_parameters.number_of_subbanks = atoi(argv[4]);
  if ( argc > 5 ) {
  	tcam_parameters.Nrd = atoi(argv[5]);
  }
  else {
  	tcam_parameters.Nrd = 1;
  }
  return;
}



void 
print_output_to_file()
{
  FILE *fptr;

  fptr = fopen("results.txt", "w");
  if(fptr == NULL) {
	fprintf(stderr, "could not open file result.txt for writing\n");
	return;
  }

  fprintf(fptr, "%d\t %d\t %1.2f\t %d\t %d\t %3.5f\t %3.5f\t %3.5f\t\n",
		tcam_parameters.number_of_rows, 
		tcam_parameters.number_of_column_bits, 
		tcam_parameters.CMOS_tech, 
		tcam_parameters.number_of_subbanks,
		tcam_parameters.Nrd,
		tcam_search_power.matchline_power*1e9,
		tcam_search_power.searchline_power*1e9,
		tcam_search_power.priority_encoder_power*1e9); 

  fclose(fptr);
  fptr = fopen("leakage.txt", "w");
  if(fptr == NULL) {
	fprintf(stderr, "could not open file leakage.txt for writing\n");
	return;
  }

  fprintf(fptr, "%d\t %d\t %1.2f\t %d\t %d\t %.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t\n",
		tcam_parameters.number_of_rows, 
		tcam_parameters.number_of_column_bits, 
		tcam_parameters.CMOS_tech, 
		tcam_parameters.number_of_subbanks,
		tcam_parameters.Nrd,
		(float)tcam_leakage_power.memcell_leakage*1e3,
		(float)tcam_leakage_power.memcell_access_leakage*1e3,
		(float)tcam_leakage_power.decoder_leakage*1e3,
		(float)tcam_leakage_power.matchline_precharge_leakage*1e3,
		(float)tcam_leakage_power.searchline_driver_leakage*1e3,
		(float)tcam_leakage_power.wordline_driver_leakage*1e3,
		(float)tcam_leakage_power.total_power*1e3);

  fclose(fptr);
  return;
}


void 
print_tcam_output()
{

  print_output_to_file();

  fprintf(stdout, "\n\n");
  fprintf(stdout, "      TCAM Configuration and Power Statistics \n");
  fprintf(stdout, "------------------------------------------------ \n");
  fprintf(stdout, "TCAM number of rows: %d\n", tcam_parameters.number_of_rows);
  fprintf(stdout, "TCAM number of bits: %d\n", tcam_parameters.number_of_column_bits);
  fprintf(stdout, "CMOS technology used: %f\n", tcam_parameters.CMOS_tech);
  fprintf(stdout, "Number of subbanks: %d\n", tcam_parameters.number_of_subbanks);
  fprintf(stdout, "Number of row divisions (Nrd): %d\n", tcam_parameters.Nrd);
  fprintf(stdout, "\n\n");
   
  fprintf(stdout, "   TCAM Seach time \n");
  fprintf(stdout, "-------------------------------------------------- \n");
  fprintf ( stdout, "TCAM routing delay : %f (ns) \n", (float) tcam_search_delay.routing_delay * 1e9 );
  fprintf ( stdout, "TCAM total delay : %f (ns) \n", (float) tcam_search_delay.total_delay * 1e9 );
  fprintf ( stdout, "Priority encoder delay : %f (ns) \n", (float) tcam_search_delay.priority_encoder_delay * 1e9 );
  fprintf ( stdout, "TCAM access time : %f (ns) \n", (float) tcam_search_delay.max_delay * 1e9 );
  fprintf(stdout, "\n\n");
  
  fprintf(stdout, "   TCAM Leakage power (Sub-threshold leakage power)  \n");
  fprintf(stdout, "-------------------------------------------------- \n");
  fprintf(stdout, "TCAM Cell leakage power (mW): %f \n", tcam_leakage_power.memcell_leakage*1e3);	
  fprintf(stdout, "TCAM Cell access trasistor leakage power (mW): %f \n", tcam_leakage_power.memcell_access_leakage*1e3);
  fprintf(stdout, "Deocder leakage power (mW): %f \n", tcam_leakage_power.decoder_leakage*1e3);
  fprintf(stdout, "Matchline precharge circuit leakage power (mW): %f \n", tcam_leakage_power.matchline_precharge_leakage*1e3);	
  fprintf(stdout, "Searchline driver leakage power (mW): %f \n", tcam_leakage_power.searchline_driver_leakage*1e3);	
  fprintf(stdout, "Wordline driver leakage power (mW): %f \n", tcam_leakage_power.wordline_driver_leakage*1e3);	
  fprintf(stdout, "Total leakage power (mW) : %f\n", tcam_leakage_power.total_power*1e3);
  fprintf(stdout, "\n\n");
  
  fprintf(stdout, "    1. TCAM write operation (Dynamic power) \n");
  fprintf(stdout, "------------------------------------------------ \n");
  fprintf(stdout, "routing power (nJ): %f\n", tcam_write_power.routing_power*1e9);	
  fprintf(stdout, "decoder power (nJ): %f\n", tcam_write_power.decoder_power*1e9);	
  fprintf(stdout, "wordline power (nJ): %f\n", tcam_write_power.wordline_power*1e9);	
  fprintf(stdout, "bitline power (nJ): %f\n", tcam_write_power.bitline_power*1e9);
  fprintf(stdout, "Total read power (nJ): %f\n", tcam_write_power.total_power*1e9);	
  fprintf(stdout, "\n\n");
  
  fprintf(stdout, "    2. TCAM read operation (Dynamic Power) \n");
  fprintf(stdout, "------------------------------------------------ \n");
  fprintf(stdout, "routing power (nJ): %f\n", tcam_read_power.routing_power*1e9);	
  fprintf(stdout, "decoder power (nJ): %f\n", tcam_read_power.decoder_power*1e9);	
  fprintf(stdout, "wordline power (nJ): %f\n", tcam_read_power.wordline_power*1e9);	
  fprintf(stdout, "bitline power (nJ): %f\n", tcam_read_power.bitline_power*1e9);
  fprintf(stdout, "sense amplifier paper (nJ): %f\n", tcam_read_power.senseamp_power*1e9);
  fprintf(stdout, "Total read power (nJ): %f\n", tcam_read_power.total_power*1e9);	
  fprintf(stdout, "\n\n");

  fprintf(stdout, "    3. TCAM search operation (Dynamic power) \n");
  fprintf(stdout, "------------------------------------------------ \n");
  fprintf(stdout, "Routing power (nJ): %f\n", tcam_search_power.routing_power*1e9);	
  fprintf(stdout, "Intra-subbank Routing power (nJ): %f\n", tcam_search_power.intra_subbank_routing_power*1e9);	
  fprintf(stdout, "Match line power (nJ): %f\n", tcam_search_power.matchline_power*1e9);	
  fprintf(stdout, "Search line power (nJ): %f\n", tcam_search_power.searchline_power*1e9);	
  fprintf(stdout, "Priority encoder power (nJ) : %f\n", tcam_search_power.priority_encoder_power*1e9);
  fprintf(stdout, "Total TCAM power (nJ) : %f\n", tcam_search_power.total_power*1e9);
  fprintf(stdout, "\n\n");

  return;
}
