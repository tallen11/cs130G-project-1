#include <stdlib.h>
#include "tone.h"

Tone make_tone(double freq, double len, uint32_t harmonics)
{
	Tone tone;
	tone.frequency = freq;
	tone.length = len;
	tone.harmonics = harmonics;

	return tone;
}

Triad make_triad(uint32_t octave, Tone first, Tone second, Tone third)
{
	Triad triad;
	triad.octave = octave;
	triad.first = first;
	triad.second = second;
	triad.third = third;

	return triad;
}

ToneSequence* make_sequence()
{
	ToneSequence *seq = malloc(sizeof(ToneSequence));
	seq->tones = malloc(4 * sizeof(Tone));
	seq->toneCount = 0;
	seq->toneSlots = 4;

	return seq;
}

void add_tone(ToneSequence *sequence, Tone tone)
{
	sequence->tones[sequence->toneCount] = tone;
	sequence->toneCount++;
	if (sequence->toneCount == sequence->toneSlots) {
		uint32_t newToneSlots = 2 * sequence->toneSlots;
		Tone *newTones = malloc(newToneSlots * sizeof(tone));
		for (uint32_t i = 0; i < sequence->toneCount; ++i) {
			newTones[i] = sequence->tones[i];
		}

		Tone *temp = sequence->tones;
		sequence->tones = newTones;
		sequence->toneSlots = newToneSlots;

		free(temp);
	}
}
