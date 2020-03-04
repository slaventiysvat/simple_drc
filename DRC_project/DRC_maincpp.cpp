#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <conio.h>
#include "drc_processor.h"

//simple 16 bit arrays
short in[MAX_SIZE_ARRAY] = {0};//16bit values

short out[MAX_SIZE_ARRAY] = {0};//16bit values

short envelope[MAX_SIZE_ARRAY] = { 0 };//16bit values

int main(int argc, char *argv[]) {

	FILE* fp_in = fopen("check_16_11025.raw", "rb");

	FILE* fp_out = fopen("out.raw", "wb");

	if (!fp_in) {

		perror("File opening failed");
		return EXIT_FAILURE;
	}

	if (!fp_out) {

		perror("File opening failed");
		return EXIT_FAILURE;

	}




	size_t len_out = 0;
	size_t len_in = 0;

	unsigned int Fs = 11025;

	//in mimlisecounds
	double AT = 0.2;
	double RT = 200.0;


#ifdef PEAK_ON

	peakDet_t* st = (peakDet_t*)malloc(sizeof(peakDet_t));
    peak_init(st, AT, RT, Fs);

#endif

#ifdef RMS_ON

	rmsDet_t* rms_det = (rmsDet_t*)malloc(sizeof(rmsDet_t));
	rms_det->previous_y = 0;

#endif
	
	cen_t* lmt = (cen_t*)malloc(sizeof(cen_t));


	double LT = 0.9;

	double NT = 0.1;

	double ET = 0.4;

	double CT = 0.6;

    double agl_nt = 0;

	double agl_et = 5;

	double agl_ct = 150;
	
	double agl_lt = 90;

	double gain = 0.01*200;

	drc_init(lmt, NT, ET, CT, LT, agl_nt, agl_et,agl_ct, agl_lt,gain);

	while (1) {


		len_in = fread(in, 2, MAX_SIZE_ARRAY, fp_in);

	
		if (len_in < MAX_SIZE_ARRAY) {

			break;

		}

#ifdef PEAK_ON

		peak_process(in, envelope, MAX_SIZE_ARRAY, st);

#endif
#ifdef RMS_ON

		rms_process(in, envelope, MAX_SIZE_ARRAY, rms_det);

#endif
		drc_process(in, envelope, out, MAX_SIZE_ARRAY, lmt);
		
		len_out = fwrite(out, 2, MAX_SIZE_ARRAY, fp_out);

	}

	 
	//after break
#ifdef PEAK_ON

	peak_process(in, envelope, len_in, st);
	
#endif
	
#ifdef RMS_ON

	rms_process(in, envelope, len_in, rms_det);

#endif

	drc_process(in, envelope, out, len_in, lmt);

	len_out = fwrite(out, 2, len_in, fp_out);

	printf("%d\n", len_in);
	printf("%d\n", len_out);
	fclose(fp_in);
	fclose(fp_out);
	_getch();
	return 0;

}

