// simple_drc_re.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <conio.h>
#include "drc_processor.h"
#include "dynamic_range_controller.h"
//simple 16 bit arrays
int16_t in[MAX_SIZE_ARRAY] = { 0 };//16bit values

int16_t out[MAX_SIZE_ARRAY] = { 0 };//16bit values

int16_t envelope[MAX_SIZE_ARRAY] = { 0 };//16bit values

int main(int argc, char* argv[]) {


	drc_settings<float> settings1;
	drc_settings<float> settings2;
	
	settings1.NT = 50.0f;
	settings1.ET = 50.0f;
	settings1.CT = 50.0f;
	settings1.LT = 50.0f;
	settings1.k_nt = 150.0f;
	settings1.b_nt = 150.0f;
	settings1.k_et = 150.0f;
	settings1.b_et = 150.0f;
	settings1.k_ct = 150.0f;
	settings1.b_ct = 150.0f;
	settings1.k_lt = 150.0f;
	settings1.b_lt = 150.0f;
	settings1.gain = 150.0f;
	settings1.input_signal = std::vector<float>(30,50);
	settings1.out_signal = std::vector<float>(30, 50);

	drc_settings<float> settings3 = settings1;
	settings2 = settings1;


	FILE* fp_in = fopen("../wav_in/check_16_11025.raw", "rb");

	FILE* fp_out = fopen("../wav_out/out.raw", "wb");

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

	double gain = 0.01 * 50;

	drc_init(lmt, NT, ET, CT, LT, agl_nt, agl_et, agl_ct, agl_lt, gain);

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
	//_getch();
	return 0;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
