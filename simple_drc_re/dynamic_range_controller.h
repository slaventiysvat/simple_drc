#pragma once
#include <vector>
#include <stdint.h>

template <typename T>
struct drc_settings {

	//threshould  
	T NT = 0;
	T ET = 0;
	T CT = 0;
	T LT = 0;

	//static curve setting
	T k_nt = 0;
	T b_nt = 0;

	T k_et = 0;
	T b_et = 0;

	T k_ct = 0;
	T b_ct = 0;

	T k_lt = 0;
	T b_lt = 0;
	T gain = 0;

	//temporal arrays
	std::vector<T> out_signal;
	std::vector<T> input_signal;

	drc_settings() = default;

	drc_settings(const drc_settings& other)
	{
		*this = other;
	}

	drc_settings& operator=(const drc_settings& other) {

		//drc_settings<T> copied_settings;

		//copied_settings.NT = drc_in_to_copy.NT;
		//copied_settings.ET = drc_in_to_copy.ET;
		//copied_settings.CT = drc_in_to_copy.CT;
		//copied_settings.LT = drc_in_to_copy.LT;
		//copied_settings.k_nt = drc_in_to_copy.k_nt;
		//copied_settings.b_nt = drc_in_to_copy.b_nt;
		//copied_settings.k_et = drc_in_to_copy.k_et;
		//copied_settings.b_et = drc_in_to_copy.b_et;
		//copied_settings.k_ct = drc_in_to_copy.k_ct;
		//copied_settings.b_ct = drc_in_to_copy.b_ct;
		//copied_settings.k_lt = drc_in_to_copy.k_lt;
		//copied_settings.b_lt = drc_in_to_copy.b_lt;
		//copied_settings.gain = drc_in_to_copy.gain;
		//copied_settings.out_signal<T>(drc_in_to_copy.out_signal);
		//copied_settings.input_signal<T>(drc_in_to_copy.input_signal);

		//return copied_settings;

		NT = other.NT;
		ET = other.ET;
		CT = other.CT;
		LT = other.LT;
		k_nt = other.k_nt;
		b_nt = other.b_nt;
		k_et = other.k_et;
		b_et = other.b_et;
		k_ct = other.k_ct;
		b_ct = other.b_ct;
		k_lt = other.k_lt;
		b_lt = other.b_lt;
		gain = other.gain;
		out_signal = other.out_signal;
		input_signal = other.input_signal;

		return *this;

	}
};

typedef drc_settings<float> drc_settingsF;
typedef drc_settings<double> drc_settingsDB;
typedef drc_settings<int32_t> drc_settingsInt32;
typedef drc_settings<int64_t> drc_settingsInt64;
typedef drc_settings<int16_t> drc_settingsInt16;


template <typename type_in, typename type_out>
class dynamic_range_controller
{

private:
	drc_settings<type_in> drc_init;

public:

	explicit dynamic_range_controller() = default {
	
		//default init

	};

	dynamic_range_controller(const &dynamic_range_controller_in) {

		this->drc_init = dynamic_range_controller_in.drc_init;

	}

	dynamic_range_controller& operator=(const dynamic_range_controller& a) { return *this; }


	~dynamic_range_controller() = default;

	~dynamic_range_controller() {
		drc_init.input_signal.clear();
		drc_init.out_signal.clear();
	}


};

