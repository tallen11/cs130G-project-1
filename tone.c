#include <stdlib.h>
#include "tone.h"

Tone make_tone(double freq, double len)
{
	Tone tone;
	tone.frequency = freq;
	tone.length = len;

	return tone;
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
