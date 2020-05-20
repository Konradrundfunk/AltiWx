/**
 * This is the definition of the main struct used to pass samples around. It
 * stores the data read as well as some metadata that might be useful when
 * parsing the data stream.
 */
#ifndef METEOR_SOURCE_H
#define METEOR_SOURCE_H

#include <stdlib.h>
#include <stdint.h>
#include <liquid/liquid.h>

typedef struct sample {
	size_t count;
	unsigned bps;       /* Bytes per sample */
	unsigned samplerate;
	liquid_float_complex *data;
	int (*read)(struct sample *, size_t);
	int (*close)(struct sample *);
	uint64_t (*size)(const struct sample *);
	uint64_t (*done)(const struct sample *);
	void *_backend;     /* Opaque pointer to stuff used by read() and close() */
} Source;

#endif
