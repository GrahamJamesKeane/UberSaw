/*
 * File: ubersaw_v1.1.cpp
 *
 * 2021 Graham Keane - Maynooth University
 *
 */
 
#include "userosc.h"
#include "ubersaw_v1.1.hpp"

static UberSaw ubersaw;

void OSC_INIT(uint32_t platform, uint32_t api) {
	(void)platform;
	(void)api;
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames){
	
	// =========================================================
	
	// Create local copies of the state and params objects.
	
	// =========================================================
	
	UberSaw::State 			&s = ubersaw.state;
	const UberSaw::Params 	&p = ubersaw.params;
	
	// =========================================================
	
	// Get the current note being played.
	
	// =========================================================
	
	uint8_t note = params->pitch>>8;
	
	// =========================================================
	
	// Update pitches.
	
	// =========================================================
	
	ubersaw.updatePitch(osc_w0f_for_note(note, params->pitch & 0xFF));
	
	// =========================================================

    // Get the HPF object.

    dsp::BiQuad &HPF = ubersaw.HPF;

    // =========================================================
	
	// Get the current LFO value.
	
	// =========================================================
	
	s.lfo = q31_to_f32(params->shape_lfo);
	
	// =========================================================
	
	
	/* =========================================================
	*
	* Create local copies of the state object fields.
	*
	* ==========================================================
	*/ 
	
	float phi[NUM_OSC];
	for(int i = 0; i < NUM_OSC; i++) {
		phi[i] = s.phi[i];
	}
	
	float phiA = s.phiA;
	float phiB = s.phiB;
	
	// =========================================================

	// Update Final LFO state
	
	// =========================================================
	
	float lfoz = s.lfoz;
	
	// =========================================================
	
	// Get LFO increment per frame
	
	// =========================================================
	
	const float lfo_inc = (s.lfo - lfoz) / frames;
	
	// =========================================================
	
	// Create local copies of parameter object fields.
	
	// =========================================================
	
	const float mix_A = p.mix_A;
	const float mix_B = p.mix_B;
	const float ringmix = p.ringmix;
	
	// =========================================================
	
	// Prepare to load buffer.
	
	// =========================================================
	
	q31_t *__restrict y = (q31_t*)yn; // y is buffer start position.
	const q31_t *y_e = y + frames; // y_e is buffer end position.
	
	// =========================================================
	
	// Load the buffer.
	
	// =========================================================
	
	for (; y != y_e; ) {
		
		// =========================================================
		
		// Get LFO modulated mix control value, range [0-1].
		
		// =========================================================
		
		const float wavemix = clip01f(p.shape + lfoz);
		
		// =========================================================
		
		// Calculate primary and secondary mix values (Adam Szabo's method).
		
		// =========================================================
		
		float primary_mix = (-0.55366 * wavemix) + 0.99785;
		float secondary_mix = (-0.73764 * wavemix * wavemix) + (1.2841 * wavemix) + 0.44372;
		
		// =========================================================
		
		/*
		* Get primary sawtooth wave sample for given
		* phase, then apply primary mix.
		*
		*/ 
		
		// =========================================================
		
		float main_sig = primary_mix * osc_sawf(phi[0]);
		
		// =========================================================
		
		/*
		* Get secondary sawtooth wave samples for given
		* phases, then apply secondary mix.
		* Need to correct amplitude to prevent clipping.
		*/ 
		
		// =========================================================
		
		float sig = 0.f;
		for(int i = 1; i < NUM_OSC; i++) {
			sig += secondary_mix * osc_sawf(phi[i]);
		}
		
		sig *= AMP_CORRECTION;
		main_sig += sig;
		
		// =========================================================
		
		/*
		* Get sawtooth wave sample for given
		* phase for Secondary oscillator A.
		*/ 
		
		// =========================================================
		
		const float sig_A = 0.5f * osc_sawf(phiA);
		
		// =========================================================
		
		// Apply mix_A, can set as dominant or inaudible.
		
		// =========================================================
		
		main_sig = (1.f - mix_A) * main_sig + (mix_A * sig_A);
		
		// =========================================================
		
		/*
		* Get sawtooth wave sample for given
		* phase for Secondary oscillator B.
		*/ 
		
		// =========================================================
		
		const float sig_B = 0.5f * osc_sawf(phiB);
		
		// =========================================================
		
		// Apply supermix, can set from dominant to inaudible.
		
		// =========================================================
		
		main_sig = (1.f - mix_B) * main_sig + (mix_B * sig_B);
		
		// =========================================================
		
		// Apply ringmix, can set as dominant or inaudible.
		
		// =========================================================
		
		main_sig = (1.f - ringmix) * main_sig + ringmix * (sig_A * main_sig) + ringmix * (sig_B * main_sig);
		
		// =========================================================

        // Apply HPF

        // =========================================================

        main_sig = HPF.process_fo(main_sig);

        // =========================================================

		// Softclip signal before sending to buffer
		
		// =========================================================
		
		main_sig = osc_softclipf(0.125f, main_sig);
		
		// =========================================================
		
		/*
		* Add frame to buffer
		* Must be formatted in Q31 binary fixed point representation
		*/ 
		
		// =========================================================
		
		*(y++) = f32_to_q31(main_sig);
		
		// =========================================================
		
		// Update local Central and Side osc phases
		
		// =========================================================
		
		for(int i = 0; i < NUM_OSC; i++) {
			phi[i] += s.w0[i];
			phi[i] -= (uint32_t)phi[i];
		}
		
		// =========================================================
		
		// Update local secondary oscillator A phase
		
		// =========================================================
		
		phiA += s.w0A;
		phiA -= (uint32_t)phiA;
		
		// =========================================================
		
		// Update local secondary oscillator B phase
		
		// =========================================================
		
		phiB += s.w0B;
		phiB -= (uint32_t)phiB;
		
		// =========================================================
		
		// Update local LFO
		
		// =========================================================
		
		lfoz += lfo_inc;
		
		// =========================================================
	}
	
	// =========================================================
	
	// Update global Central and Side osc phases
	
	// =========================================================
	
	for(int i = 0; i < NUM_OSC; i++) {
		s.phi[i] = phi[i];
	}
	
	// =========================================================
	
	// Update global secondary oscillator phases
	
	// =========================================================
	
	s.phiA = phiA;
	s.phiB = phiB;
	
	// =========================================================
	
	// Update global final LFO state
	
	// =========================================================
	
	s.lfoz = lfoz;
	
	// =========================================================
}

void OSC_NOTEON(const user_osc_param_t *const params) {
	(void)params;
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
	(void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value) { 
	
	// =========================================================
	
	// Create local copies of the state and params objects.
	
	// =========================================================
	
	UberSaw::Params &p 	= ubersaw.params;
	UberSaw::State 	&s 	= ubersaw.state;
	
	// =========================================================
	
	// Update parameter values from user control input
	
	// =========================================================
	
	switch (index) {
		case k_user_osc_param_id1:
			/*
			* User Parameter 1:
			* Secondary oscillator A mix control value
			* Percent parameter: Scale in 0.0 - 1.00
			*/ 
			p.mix_A = clip01f(value * 0.01f); 
			break; 
			
		case k_user_osc_param_id2:
			/*
			* User Parameter 2:
			* Secondary oscillator B mix control value
			* Percent parameter: Scale in 0.0 - 1.00
			*/ 
			p.mix_B = clip01f(value * 0.01f); 
			break; 
			
		case k_user_osc_param_id3:
			/*
			* User Parameter 3:
			* Ring mix control value
			* Percent parameter: Scale in 0.0 - 1.00
			*/ 
			p.ringmix = clip01f(value * 0.01f); 
			break;
			
		case k_user_osc_param_id4:
			/*
			* User Parameter 4:
			* Detune linear value (Get curve value from lookup table)
			* Percent parameter: Scale in 0.0 - 1.00
			*/ 
			p.detune = detune_lut[value];
			break;
			
		case k_user_osc_param_id5: 
			/*
			* User Parameter 5:
			* Chord selection value
			* Percent parameter: range [1-4]
			*/ 
			switch(value) {
				case 1: p.chord = OCTAVE; break;
				case 2: p.chord = FIFTH; break;
				case 3: p.chord = MAJOR_3RD; break;
				case 4: p.chord = MINOR_3RD; break;
			} break;
			
		case k_user_osc_param_id6: break;
			// User Parameter 6:
			
		case k_user_osc_param_shape:
			/*
			* A knob value:
			* Main Oscillator mix control value
			* 10bit parameter
			*/ 
			p.shape = param_val_to_f32(value); break;
			
		case k_user_osc_param_shiftshape:
			/*
			* B knob value:
			* Drift control value
			* 10bit parameter
			*/ 
			p.shiftshape = param_val_to_f32(value); break;
			
		default: break;
	}
	
	// =========================================================
}

