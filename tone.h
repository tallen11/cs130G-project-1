#ifndef TONE_H
#define TONE_H

#include <stdint.h>

typedef struct {
	double frequency;
	double length;
} Tone;

typedef struct {
	Tone *tones;
	uint32_t toneCount;
	uint32_t toneSlots;
} ToneSequence;

Tone make_tone(double freq, double len);
ToneSequence* make_sequence();
void add_tone(ToneSequence *sequence, Tone tone);

#endif // TONE_H
