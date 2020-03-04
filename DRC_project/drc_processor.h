#ifndef  DRC_PROCESSOR
#define  DRC_PROCESSOR

#define MAX_SIZE_ARRAY 100
#define PEAK_ON 1 
//#define RMS_ON 1

typedef struct {

	short previous_y = 0;
	double AT = 0;
	double RT = 0;
	unsigned int Fs = 0;

}peakDet_t;

typedef struct {

	double previous_y = 0;
	double in_dble[MAX_SIZE_ARRAY] = {0};
	double out_dble[MAX_SIZE_ARRAY] = {0};

}rmsDet_t;

typedef struct {

	//threshould  
	double NT = 0;
	double ET = 0;
	double CT = 0;
	double LT = 0;
	//static curve setting

	double k_nt = 0;
	double b_nt = 0;

	double k_et = 0;
	double b_et = 0;

	double k_ct = 0;
	double b_ct = 0;

	double k_lt = 0;
	double b_lt = 0;
    double gain = 0;
	//temporal arrays
	double in_dble[MAX_SIZE_ARRAY] = { 0 };
	double out_dble[MAX_SIZE_ARRAY] = { 0 };

}cen_t;


void drc_init(cen_t* st, double NT, double ET, double CT, double LT,double agl_nt, double agl_et, double agl_ct, double agl_lt, double gain);

void peak_init(peakDet_t *st, double attack_time, double release_time, unsigned int Fs);

void rms_process(short* in, short* rms, int len, rmsDet_t *st);

void peak_process(short* in, short* peak, int len, peakDet_t* st);

void drc_process(short* input, short* envelope, short* output, int len, cen_t* st);

#endif // DRC_PROCESSOR
