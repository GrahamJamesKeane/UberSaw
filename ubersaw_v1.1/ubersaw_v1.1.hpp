/*
 * File: ubersaw_v1.1.hpp
 *
 * 2021 Graham Keane - Maynooth University
 *
 */

#pragma once

#include "userosc.h"
#include "biquad.hpp"

// =========================================================
// Number of main oscillators (primary + side)
// =========================================================

#define NUM_OSC 	7 

// =========================================================
// Phase drift constants
// =========================================================

#define SIDE_DRIFT 	5.20833333333333e-006f 	// 0.25Hz@48KHz
#define SUB_DRIFT 	3.125e-006f 			// 0.15Hz@48KHz

// =========================================================
// Default values
// =========================================================

#define ZEROF 		0.f

// =========================================================
// Ratios for chord options:
// =========================================================

#define OCTAVE 		2.f
#define FIFTH 		1.5f
#define MAJOR_3RD 	0.75f
#define MINOR_3RD	1.2f

// =========================================================
// Amplitude correction for side oscillators
// =========================================================

#define AMP_CORRECTION 1.f / (NUM_OSC - 1)

// =========================================================
// Detune curve lookup table
// =========================================================

static float detune_lut[101]; 

// =========================================================
// Ubersaw structure
// =========================================================

struct UberSaw {
	
	struct Params {
		float   	mix_A;
		float   	mix_B;
		float   	ringmix;
		float 		detune;
		float   	shape;
		float   	shiftshape;
		float 		chord;
    
		Params(void) :
			mix_A(ZEROF),
			mix_B(ZEROF),
			ringmix(ZEROF),
			detune(ZEROF),
			shape(ZEROF),
			shiftshape(ZEROF),
			chord(OCTAVE)
		{ }
	};
  
	struct State {
		float    phi[NUM_OSC]; 	// Main oscillator phases
		float    phiA;			// Secondary oscillator A phase
		float    phiB;			// Secondary oscillator B phase
		float    w0[NUM_OSC];	// Main oscillator pitches
		float    w0A;			// Secondary oscillator A pitch
		float    w0B;			// Secondary oscillator B pitch
		float    lfo;			// LFO initial state (per cycle)
		float    lfoz;			// LFO final state (per cycle)
    
		State(void) :
			phiA(ZEROF),
			phiB(ZEROF),
			w0A(ZEROF),
			w0B(ZEROF),
			lfo(ZEROF),
			lfoz(ZEROF)
		{
			for(int i = 0; i < NUM_OSC; i++) {
				w0[i] 	= ZEROF; 
				phi[i] 	= ZEROF;
			}
		}
	};

	UberSaw(void) {
		state = State();
		params = Params();
		buildDetuneTable();
	}
  
	inline void updatePitch(float w0) {
		
		// =========================================================
		// Get phase drift from A knob
		// =========================================================
		
		const float drift = params.shiftshape;
		
		// =========================================================
		// Get detune curve value (provided by lookup table)
		// =========================================================
		
		const float detune = params.detune;
		
		// =========================================================
		// Set pitch of central oscillator
		// =========================================================
		
		state.w0[0] = w0;
		
		// =========================================================
		// Set pitches of side oscillators
		// =========================================================
		
		float step = 1.f;
		for(int i = 1; i < NUM_OSC; i += 2) {
			
			// =========================================================
			// Calculate detune amounts (Alex Shore's method)
			// =========================================================
			
			float detune_amount = (step / 3.f) * detune;
			
			float detune_down = 1.f - detune_amount;
		
			float detune_up = 1.f + detune_amount;
			
			// =========================================================
			// Detune side oscs and add phase drift
			// =========================================================

			state.w0[i] = (w0 * detune_down) + (drift * SIDE_DRIFT);
			
			state.w0[i + 1] = (w0 * detune_up) + (drift * SIDE_DRIFT);
			step++;
		}
		
		// =========================================================
		// Set pitch and phase drift of secondary oscillators
		// =========================================================
		
		float chord = params.chord;
		state.w0A = (chord * w0) + (drift * SUB_DRIFT);
		state.w0B = ((1.f / chord) * w0) + (drift * SUB_DRIFT);

		// Set pole for HPF
        HPF.mCoeffs.setPoleHP((1.f / chord) * w0);
	}
	
	/* // =========================================================
	* Implements Adam Szabo's method: First build a detune curve  
	* lookup table to store detune values and speed up processing time.
	*/ // =========================================================

	inline void buildDetuneTable() {
		
		for(int i = 0; i <= 100; i++) {
			
			// =========================================================
			// Get linear detune control value
			// =========================================================
			
			const float detune_value = i * 0.01f;
			
			// =========================================================
			// Define variables for exponent calculations
			// =========================================================

			float x11, x10, x9, x8, x7, x6, x5, x4, x3, x2, x1;
			
			x11 = x10 = x9 = x8 = x7 = x6 = x5 = x4 = x3 = x2 = x1 = detune_value;  
			
			/* // =========================================================
			* This loop calculates powers and provides better 
			* performance for this function than the pow function or
			* the functions provided in float_math.h
			*/ // =========================================================
			
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
			
			// =========================================================
			// Scale by values provided by Szabo
			// =========================================================
			
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
			
			float detune = x11 - x10 + x9 - x8 + x7 - x6 + x5 - x4 + x3 - x2 + x1 + 0.0030115596;
			
			// =========================================================
			// Clip in range [0-1]
			// =========================================================
			
			detune = clip01f(detune);
			
			// =========================================================
			// Add to lookup table at index i
			// =========================================================
			
			detune_lut[i] = detune;
		}
		
	}

	State 	state;
	Params 	params;
    dsp::BiQuad HPF;
};
