/*
 *  File: ubersaw.hpp
 *
 * 2021 Graham Keane - Maynooth University
 *
 */

#pragma once // ensures the file is only included within a single compilation

#include "userosc.h" // Must include the userosc header to use its contents

# define NUM_OSC 7 // Number of main oscillators (primary + side)
# define MAIN_TUNING 440.f * k_samplerate_recipf // Convert standard tuning (A4) Hz frequency to radians
# define SUB_TUNING 220.f * k_samplerate_recipf // Convert A3 Hz frequency to radians
# define SUPER_TUNING 880.f * k_samplerate_recipf // Convert A5 Hz frequency to radians
# define SIDE_DRIFT 5.20833333333333e-006f // 0.25Hz@48KHz
# define SUB_DRIFT 3.125e-006f // 0.15Hz@48KHz
# define ZEROF 0.f

struct UberSaw {

	enum {
		flags_none    	= 0, 	// Reset flag 
		flag_detune  	= 1<<1 	// Flag for detune control value selected (2)
	};
  
	struct Params {
		float   submix;
		float   supermix;
		float   ringmix;
		float 	detune;
		float 	detune_value;
		float   shape;
		float   shiftshape;
    
		Params(void) :
			submix(ZEROF),
			supermix(ZEROF),
			ringmix(ZEROF),
			detune(ZEROF),
			detune_value(ZEROF),
			shape(ZEROF),
			shiftshape(ZEROF) 
		{ }
	};
  
	struct State {
		float    phi[NUM_OSC]; 	// Main oscillator phases
		float    phisuper;		// Super-Octave oscillator phase
		float    phisub;		// Sub-Octave oscillator phase
		float    w0[NUM_OSC];	// Main oscillator pitches
		float    w0super;		// Super-octave oscillator pitch
		float    w0sub;			// Sub-octave oscillator pitch
		float    lfo;			// LFO initial state (per cycle)
		float    lfoz;			// LFO final state (per cycle)
		uint32_t flags: 2; 		// Bit field for enum (max value will be 2^1 = 2, so 2 bit needed)
    
		State(void) :
			w0super(SUPER_TUNING),
			w0sub(SUB_TUNING),
			lfo(ZEROF),
			lfoz(ZEROF),
			flags(flags_none)
		{
			for(int i = 0; i < NUM_OSC; i++) {
				w0[i] = MAIN_TUNING; 
			}
		}
	};

	UberSaw(void) {
		init();
	}

	void init(void) {
		state = State();
		params = Params();
	}
  
	inline void updatePitch(float w0) {
		// Get phase drift from A knob
		const float drift = params.shiftshape;
		
		// Get detune curve value (provided by update detune)
		const float detune = params.detune;
		
		// Set pitch of central oscillator
		state.w0[0] = w0;
		
		// Set pitches of side oscillators
		float step = 1.f;
		for(int i = 1; i < NUM_OSC; i += 2) {
			// Calculate detune amounts (Alex Shore's method)
			float detune_amount = (step / 3.f) * detune;
			
			float detune_down = 1.f - detune_amount;
		
			float detune_up = 1.f + detune_amount;
			
			// Detune side oscs and add phase drift
			state.w0[i] = (w0 * detune_down) + (drift * SIDE_DRIFT);
			
			state.w0[i + 1] = (w0 * detune_up) + (drift * SIDE_DRIFT);
			step++;
		}
		
		// Make superwave an octave higher and add a phase drift
		state.w0super = (2.f * w0) + (drift * SUB_DRIFT);
	
		// Make subwave an octave lower and add a phase drift
		state.w0sub = (0.5f * w0) + (drift * SUB_DRIFT);
	}
	
	// Implements Adam Szabo's method
	inline void updateDetune() {
		// Get linear detune control value
		const float detune_value = params.detune_value;
		
		// Define variables for exponent calculations
		float x11, x10, x9, x8, x7, x6, x5, x4, x3, x2, x1;
		
		x11 = x10 = x9 = x8 = x7 = x6 = x5 = x4 = x3 = x2 = x1 = detune_value;  
		
		/*
		This loop calculates powers and provides better 
		performance for this function than the pow function or
		the functions provided in float_math.h
		*/
		
		for(int i = 0; i < 10; i++) {
			x11 *= detune_value;
			if(i > 0) {
				x10 *= detune_value;
			}
			if (i > 1) {
				x9 *= detune_value;
			}
			if (i > 2) {
				x8 *= detune_value;
			}
			if (i > 3) {
				x7 *= detune_value;
			}
			if (i > 4) {
				x6 *= detune_value;
			}
			if (i > 5) {
				x5 *= detune_value;
			}
			if (i > 6) {
				x4 *= detune_value;
			}
			if (i > 7) {
				x3 *= detune_value;
			}
			if (i > 8) {
				x2 *= detune_value;
			}
		}
		
		// Scale by values provided by Szabo
		x11 *= 10028.7312891634;
		x10 *= 50818.8652045924;
		x9 *= 111363.4808729368;
		x8 *= 138150.6761080548;
		x7 *= 106649.6679158292;
		x6 *= 53046.9642751875;
		x5 *= 17019.9518580080;
		x4 *= 3425.0836591318;
		x3 *= 404.2703938388;
		x2 *= 24.1878824391;
		x1 *= 0.6717417634;
		
		params.detune = x11 - x10 + x9 - x8 + x7 - x6 + x5 - x4 + x3 - x2 + x1 + 0.0030115596;
		
		// Clip in range [0-1]
		params.detune = clip01f(params.detune);
	}
	
	// Handles for inner classes
	State 	state;
	Params 	params;
};
