/**
 * Phase-locked loop (actually a Costas loop) defined here. Feed samples to
 * costas_resync, and it'll return the samples resync'd to the reconstructed
 * carrier.
 */
#ifndef METEOR_PLL_H
#define METEOR_PLL_H

#include <liquid/liquid.h>
#include "utils.h"

/* Costas loop default parameters */
#define COSTAS_BW 100
#define COSTAS_DAMP 1/M_SQRT2
#define COSTAS_INIT_FREQ 0.001
#define FREQ_MAX 0.8
#define AVG_WINSIZE 2500

typedef struct {
	float nco_phase, nco_freq;
	float alpha, beta;
	float damping, bw;
	int locked;
	float moving_avg;
	ModScheme mode;
} Costas;

Costas*       costas_init(float bw, ModScheme mode);
liquid_float_complex costas_resync(Costas *self, liquid_float_complex samp);
liquid_float_complex costas_mix(Costas *self, liquid_float_complex samp);
void          costas_free(Costas *self);

float         costas_delta(liquid_float_complex samp, liquid_float_complex cosamp);

void          costas_recompute_coeffs(Costas *self, float damping, float bw);
void          costas_correct_phase(Costas *self, float err);

#endif
