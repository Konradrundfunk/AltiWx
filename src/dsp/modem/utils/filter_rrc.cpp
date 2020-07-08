/* 
RRC Filter from meteor_demod (https://github.com/dbdexter-dev/meteor_demod) 
Ported to C++ & Made standalone
*/

#include "filter_rrc.h"
#include <memory.h>

float compute_rrc_coeff(int stage_no, unsigned n_taps, float osf, float alpha);

/* Create a new filter, a FIR if back_count is 0, an IIR filter otherwise.
 * Variable length arguments are two ptrs to doubles, holding the coefficients
 * to use in the filter */
Filter *filter_new(unsigned fwd_count, double *fwd_coeff)
{
    Filter *flt;
    unsigned i;

    flt = (Filter *)malloc(sizeof(*flt));

    flt->fwd_count = fwd_count;

    if (fwd_count)
    {
        /* Initialize the filter memory nodes and forward coefficients */
        flt->fwd_coeff = (float *)malloc(sizeof(*flt->fwd_coeff) * fwd_count);
        flt->mem = (std::complex<float> *)malloc(sizeof(*flt->mem) * fwd_count);
        for (i = 0; i < fwd_count; i++)
        {
            flt->fwd_coeff[i] = (float)fwd_coeff[i];
        }
    }

    return flt;
}

/* Basically a deep clone of the filter */
Filter *filter_copy(const Filter *orig)
{
    Filter *ret;
    unsigned i;

    ret = (Filter *)malloc(sizeof(*ret));

    ret->fwd_count = orig->fwd_count;

    if (ret->fwd_count)
    {
        /* Copy feed-forward parameters and initialize the memory */
        ret->fwd_coeff = (float *)malloc(sizeof(*ret->fwd_coeff) * ret->fwd_count);
        ret->mem = (std::complex<float> *)malloc(sizeof(*ret->mem) * ret->fwd_count);
        for (i = 0; i < ret->fwd_count; i++)
        {
            ret->mem[i] = 0;
            ret->fwd_coeff[i] = orig->fwd_coeff[i];
        }
    }
    return ret;
}

/* Create a RRC (root raised cosine) filter */
Filter *filter_rrc(unsigned order, unsigned factor, float osf, float alpha)
{
    unsigned i;
    unsigned taps;
    double *coeffs;
    Filter *rrc;

    taps = order * 2 + 1;

    coeffs = (double *)malloc(sizeof(*coeffs) * taps);
    /* Compute the filter coefficients */
    for (i = 0; i < taps; i++)
    {
        coeffs[i] = compute_rrc_coeff(i, taps, osf * factor, alpha);
    }

    rrc = filter_new(taps, coeffs);
    free(coeffs);

    return rrc;
}

/* Feed a signal through a filter, and output the result */
std::complex<float> filter_fwd(Filter *const self, std::complex<float> in)
{
    size_t i;
    std::complex<float> out;

    /* Update the memory nodes */
    memmove(self->mem + 1, self->mem, sizeof(*self->mem) * (self->fwd_count - 1));
    self->mem[0] = in;

    /* Calculate the feed-forward output */
    out = in * self->fwd_coeff[0];
    for (i = self->fwd_count - 1; i > 0; i--)
    {
        out += self->mem[i] * self->fwd_coeff[i];
    }

    return out;
}

/* Free a filter object */
void filter_free(Filter *self)
{
    if (self->mem)
    {
        free(self->mem);
    }
    if (self->fwd_count)
    {
        free(self->fwd_coeff);
    }
    free(self);
}

/*Static functions {{{*/
/* Variable alpha RRC filter coefficients */
/* Taken from https://www.michael-joost.de/rrcfilter.pdf */
float compute_rrc_coeff(int stage_no, unsigned taps, float osf, float alpha)
{
    float coeff;
    float t;
    float interm;
    int order;

    order = (taps - 1) / 2;

    /* Handle the 0/0 case */
    if (order == stage_no)
    {
        return 1 - alpha + 4 * alpha / M_PI;
    }

    t = abs(order - stage_no) / osf;
    coeff = sin(M_PI * t * (1 - alpha)) + 4 * alpha * t * cos(M_PI * t * (1 + alpha));
    interm = M_PI * t * (1 - (4 * alpha * t) * (4 * alpha * t));

    return coeff / interm;
}