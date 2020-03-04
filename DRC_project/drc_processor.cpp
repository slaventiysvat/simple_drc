#include "drc_processor.h"
#include <math.h>
/*****************************************************************************
  functionname: drc_init

  description: 
  initial settings for DRC
  description of this detector please see in "Digital Audio Signal Processing"
  Autor: Udo Zölzer
  pgs: 226 - 239.
  input:
  cen_t* st, - adress first value of structure
  double NT, - Noise gate     (range 0.0 - 1.0)
  double ET, - Expander gate  (range 0.0 - 1.0)
  double CT, - Compresor gate (range 0.0 - 1.0)
  double LT, - limiter gate   (range 0.0 - 1.0)
  double agl_nt, - angle of slope for curve above noise      (range 0.0  - 180)
  double agl_et, - angle of slope for curve above expander   (range 1  -   180)
  double agl_ct, - angle of slope for curve above compressor (range 1  -   180)
  double agl_lt, - angle of slope for curve above limiter    (range 1  -   180)
  double gain - gain for signal in area expander - limiter 
  output: void
*****************************************************************************/
void drc_init(cen_t* st, double NT, double ET, double CT, double LT, double agl_nt,
	double agl_et, double agl_ct, double agl_lt, double gain) {

	st->NT = NT;
    st->ET = ET;
	st->CT = CT;
	st->LT = LT;
	st->gain = gain;

	if (agl_lt == 0) {

		st->k_lt = 0;
		st->b_lt = 0;
	}
	else {

		st->k_lt = 1 / tan((agl_lt * 3.1415926535897932) / 180.0);

		st->b_lt = (st->k_lt * LT) - LT;

	}

	if (agl_nt == 0) {

		st->k_nt = 0;
		st->b_nt = 0;
	}
	else {

		st->k_nt = 1 / tan((agl_nt * 3.1415926535897932) / 180.0);
		st->b_nt = (st->k_nt * NT) - NT;
	}

	if (agl_et == 0) {

		st->k_et = 0;
		st->b_et = 0;
	}
	else {

		st->k_et = 1 / tan((agl_et * 3.1415926535897932) / 180.0);
		st->b_et = (st->k_et * NT) - NT;
	}

	if (agl_ct == 0) {

		st->k_ct = 0;
		st->b_ct = 0;
	}
	else {

		st->k_ct = 1 / tan((agl_ct * 3.1415926535897932) / 180.0);
		st->b_ct = (st->k_ct * LT) - LT;
	}


}
/*****************************************************************************
  functionname: peak_init

  description: initialisation of structure for input parameters of peak detector
  information about this detector please see in "Digital Audio Signal Processing"
  Autor: Udo Zölzer
  pgs: 228 - 229.

  input:

  peakDet_t* st       - structure is which will be initialize

  double attack_time - The attack is the period when
  the compressor is decreasing gain in response to increased
  level at the input to reach the gain determined by the ratio.

  double release_time - The release is the period when the compressor
  is increasing gain in response to reduced level at the input
  to reach the output gain determined by the ratio.

  double Fs           - sampling frequency or sampling rate

  output: void
*****************************************************************************/
void peak_init(peakDet_t *st, double attack_time, double release_time, unsigned int Fs) {

	//attack_time = 0.2;
	//release_time = 200;
	double AT = (-2.2 *(1.0 / Fs)) / (attack_time / 1000.0);
	double RT = (-2.2 *(1.0 / Fs)) / (release_time / 1000.0);

	st->AT = 1.0 - exp(AT);
	st->RT = 1.0 - exp(RT);
	st->Fs = Fs;
	st->previous_y = 0;

}

/*****************************************************************************
  functionname: peak_process

  description: find the signal envelope using the input vector (in)
  and write the result to (out)
  description of this detector please see in "Digital Audio Signal Processing"
  Autor: Udo Zölzer
  pgs: 228 - 229.
  input:

  short* in   - input signal

  short* peak  - output (peak) envelope values

  int len     - size of input and output buffers. The buffers must
				be same or size must be less than real size
				of arrays.

  peakDet_t* st  - structure which has initialize parameters

  output: void
*****************************************************************************/
void peak_process(short* in, short* peak, int len, peakDet_t *st) {


	if (abs(in[0]) > st->previous_y) {

		peak[0] = (1 - st->AT) * st->previous_y + abs(in[0])*st->AT;

	}
	if (abs(in[0]) <= st->previous_y) {

		peak[0] = (1 - st->RT) * st->previous_y;

	}

	for (int i = 1; i < len; i++) {


		if (abs(in[i]) > peak[i - 1]) {

			peak[i] = (1 - st->AT) * peak[i - 1] + abs(in[i])*st->AT;

		}
		if (abs(in[i]) <= peak[i - 1]) {

			peak[i] = (1 - st->RT) * peak[i - 1];

		}

	}

	//return last estimated value of envelope;
    st->previous_y = peak[len - 1];

}

/*****************************************************************************
  functionname: rms_process

  description: find the signal rms using the input vector (in)
  and write the result to (out)
  description of this detector please see in "Digital Audio Signal Processing"
  Autor: Udo Zölzer
  pgs: 229.
  input:

  short* in  - input signal

  short* rms - output rms values

  int len - size of input and output buffers. The buffers must
				be same or size must be less than real size
				of arrays.

  output: previous_y - last value after etimating
*****************************************************************************/
void rms_process(short* in, short* rms, int len, rmsDet_t *st) {

	double N = (double)len;

	for (int i = 0; i < len; i++) {

		st->in_dble[i] = (double)in[i];

	}

	st->out_dble[0] = (st->previous_y * ((N - 1.0) / N)) + ((st->in_dble[0] * st->in_dble[0]) * (1.0 / N));

	for (int i = 1; i < len; i++) {

		st->out_dble[i] = (st->out_dble[i - 1] * ((N - 1.0) / N)) + ((st->in_dble[i] * st->in_dble[i]) * (1.0 / N));

	}

	st->previous_y = st->out_dble[len - 1];

	for (int i = 0; i < len; i++) {


		st->out_dble[i] = sqrt(st->out_dble[i]);

	}


	for (int i = 0; i < len; i++) {


		rms[i] = (short)st->out_dble[i];


	}

}

/*****************************************************************************
  functionname: drc_process

  description: 
  this function realize a dynamic range control processor without translating to dB.
  description of limiter/compressor/expander/noise gate please see
  in "Digital Audio Signal Processing"
  Autor: Udo Zölzer
  pgs:231 -  239.

  input:

  short* input - 16 bit values of signal

  short* envelope - from rms or peak detector

  short* output - 16 bit values estimated signal

  int len - length of one packet for processing

  cen_t* st - structure for setting the processor

  output: void
*****************************************************************************/
void drc_process(short* input, short* envelope, short* output, int len, cen_t* st) {

	//translate input envelope from 16 bit data to double

	for (int i = 0; i < len; i++) {

		st->in_dble[i] = (double)envelope[i] / (1 << 15);

	}

	for (int i = 0; i < len; i++) {

		//noise gate control
		if (st->in_dble[i] < st->NT) {

			st->out_dble[i] = (st->in_dble[i] * st->k_nt) - st->b_nt;

			if (st->out_dble[i] < 0.0) {

				st->out_dble[i] = 0;

			}

		}
	

		//expander control
		else if ((st->in_dble[i] > st->NT) && (st->in_dble[i] < st->ET)) {

			st->out_dble[i] = (st->in_dble[i] * st->k_et) - st->b_et;


			if (st->out_dble[i] < 0.0) {

				st->out_dble[i] = 0;

			}

		}

	    // straight curve + gain

		else if ((st->in_dble[i] > st->ET) && (st->in_dble[i] < st->CT)) {

			st->out_dble[i] = st->in_dble[i] + st->gain;


		}

		//compressor gate control
		else if ((st->in_dble[i] > st->CT) && (st->in_dble[i] < st->LT)) {

			st->out_dble[i] = (st->in_dble[i] * st->k_ct) - st->b_ct;

			if (st->out_dble[i] < 0.0) {

				st->out_dble[i] = 0;

			}

		}

		else if ((st->in_dble[i] > st->LT)) {
		
			//limiter gain control
			if (st->in_dble[i] > st->LT) {

				st->out_dble[i] = (st->in_dble[i] * st->k_lt) - st->b_lt;

				if (st->out_dble[i] < 0.0) {

					st->out_dble[i] = 0;

				}

			}

		}
		
		
	}

	output[0] = 0;

	for (int i = 1; i < len; i++) {

		//convert estimated 16 bit data to fixed point
		st->out_dble[i] = st->out_dble[i] * ((double)input[i - 1] / (1 << 15));

		st->out_dble[i] = st->out_dble[i] * (1 << 15);

		output[i] = (short)(st->out_dble[i]);

	}

}
