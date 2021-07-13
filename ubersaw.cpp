/*
 * File: ubersaw.cpp
 *
 * 2021 Graham Keane - Maynooth University
 *
 */
 
// Must include the userosc and UberSaw headers to use their contents
#include "userosc.h"
#include "ubersaw.hpp"

// Create UberSaw object
static UberSaw ubersaw;

void OSC_INIT(uint32_t platform, uint32_t api) {
	(void)platform;
	(void)api;
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames){
	
	// Create local copies of the state and params objects.
	UberSaw::State &s = ubersaw.state;
	const UberSaw::Params &p = ubersaw.params;
	
	// Get the current note being played.
	uint8_t note = params->pitch>>8;
	
	// Get its wave index.
	const float index = osc_bl_saw_idx(note);
	
	{ 
		// Get current flag.
		const uint32_t flags = s.flags; 
		// Reset s.flags.
		s.flags = UberSaw::flags_none;
		// Check if action required.
		if(flags & UberSaw::flag_detune) {
			ubersaw.updateDetune();
		}
		
		// Update the pitches.
		ubersaw.updatePitch(osc_w0f_for_note(note, params->pitch & 0xFF));
		
		// Get the current LFO value.
		s.lfo = q31_to_f32(params->shape_lfo);
		
	}
	
	// Create local copies of the state object fields.
	float phi[NUM_OSC];
	for(int i = 0; i < NUM_OSC; i++) {
		phi[i] = s.phi[i];
	}
	float phisub = s.phisub;
	float phisuper = s.phisuper;

	// Update Final LFO state
	float lfoz = s.lfoz;
	
	// Get LFO increment per frame
	const float lfo_inc = (s.lfo - lfoz) / frames;
	
	// Create local copies of parameter object fields.
	const float submix = p.submix;
	const float supermix = p.supermix;
	const float ringmix = p.ringmix;
	
	// Prepare to load buffer.
	q31_t *__restrict y = (q31_t*)yn; // y is buffer start position.
	const q31_t *y_e = y + frames; // y_e is buffer end position.
	
	// Load the buffer.
	for (; y != y_e; ) {
		// Get mix control value, range [0-1].
		const float wavemix = clipminmaxf(0.00f, p.shape+lfoz, 1.f);
		
		// Calculate primary and secondary mix values (Adam Szabo's method).
		float primary_mix = (-0.55366 * wavemix) + 0.99785;
		float secondary_mix = (-0.73764 * wavemix * wavemix) + (1.2841 * wavemix) + 0.44372;
		
		/*
		* Get band-limited primary sawtooth wave sample for given
		* phase and wave index, then apply primary mix.
		*/ 
		float main_sig = primary_mix * osc_bl2_sawf(phi[0], index);
		
		/*
		* Get band-limited secondary sawtooth wave samples for given
		* phases and wave indices, then apply secondary mix.
		* Need to correct amplitude to prevent clipping, 0.2f is close 
		* enough for each osc.
		*/ 
		for(int i = 1; i < NUM_OSC; i++) {
			float sig = secondary_mix * osc_bl2_sawf(phi[i], index);
			main_sig += sig * 0.2f;
		}
		
		/*
		* Get band-limited sub-osc sawtooth wave sample for given
		* phase and wave index.
		*/ 
		const float subsig = 0.5f * osc_bl2_sawf(phisub, index);
		
		// Apply submix, can set as dominant or inaudible.
		main_sig = (1.f - submix) * main_sig + (submix * subsig);
		
		/*
		* Get band-limited super-osc sawtooth wave sample for given
		* phase and wave index.
		*/ 
		const float supersig = 0.5f * osc_bl2_sawf(phisuper, index);
		
		// Apply supermix, can set from dominant to inaudible.
		main_sig = (1.f - supermix) * main_sig + (supermix * supersig);
		
		// Apply ringmix, can set as dominant or inaudible.
		main_sig = (1.f - ringmix) * main_sig + ringmix * (subsig * main_sig) + ringmix * (supersig * main_sig);
		
		// Softclip signal before sending to buffer
		main_sig = osc_softclipf(0.125f, main_sig);
		
		// Add frame to buffer
		// Must be formatted in Q31 binary fixed point representation
		*(y++) = f32_to_q31(main_sig);
		
		// Update local Central and Side osc phases
		for(int i = 0; i < NUM_OSC; i++) {
			phi[i] += s.w0[i];
			phi[i] -= (uint32_t)phi[i];
		}
		
		// Update local sub-osc phase
		phisub += s.w0sub;
		phisub -= (uint32_t)phisub;
		
		// Update local super-osc phase
		phisuper += s.w0super;
		phisuper -= (uint32_t)phisuper;
		
		// Update local LFO
		lfoz += lfo_inc;
	}
	
	// Update global Central and Side osc phases
	for(int i = 0; i < NUM_OSC; i++) {
		s.phi[i] = phi[i];
	}
	
	// Update global sub and super wave phases
	s.phisub = phisub;
	s.phisuper = phisuper;
	
	// Update global final LFO state
	s.lfoz = lfoz;
}

void OSC_NOTEON(const user_osc_param_t *const params) {
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
	(void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value) { 

	// Create local copies of the state and params objects.
	UberSaw::Params &p = ubersaw.params;
	UberSaw::State &s = ubersaw.state;

	// Update parameter values from user control input
	switch (index) {
		case k_user_osc_param_id1:
			// Sub mix
			// Percent parameter
			// Scale in 0.0 - 1.00
			p.submix = clip01f(value * 0.01f); break; 
			
		case k_user_osc_param_id2:
			// Super mix
			// Percent parameter
			// Scale in 0.0 - 1.00
			p.supermix = clip01f(value * 0.01f); break; 
			
		case k_user_osc_param_id3:
			// Ring mix
			// Percent parameter
			// Scale in 0.0 - 1.00
			p.ringmix = clip01f(value * 0.01f); break;
			
		case k_user_osc_param_id4:
			// Detune
			// Percent parameter
			// Scale in 0.0 - 1.00
			p.detune_value = value * 0.01f;
			// Raise flag
			s.flags |= UberSaw::flag_detune; break;
		case k_user_osc_param_id5: break;
			
		case k_user_osc_param_id6: break;
			
		case k_user_osc_param_shape:
			// 10bit parameter
			// Value from A knob
			p.shape = param_val_to_f32(value); break;
			
		case k_user_osc_param_shiftshape:
			// 10bit parameter
			// Value from B knob
			p.shiftshape = param_val_to_f32(value); break;
			
		default: break;
	}
}

