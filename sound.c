#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "tone.h"
#include "wavefile.h"
#include "scales.h"

const int WAV_SAMPLES_PER_SEC = 44100;
const int MAX_VOLUME = 32000;

void writeTonewriteToneSineWave(FILE *file, Tone tone);
void writeTonewriteToneSquareWave(FILE *file, Tone tone);
void writeChord();
void writeLoop(FILE *file, uint32_t loopCount, ToneSequence *sequence);
double envelope(uint32_t x, uint32_t len);
void normalizeData(double *data, uint32_t length);
int16_t* packageData(double *data, uint32_t length);

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int main(/*int argc, char const *argv[]*/)
{
	FILE *file = wavfile_open("sound.wav");

	// srand(time(NULL));

	ToneSequence *seq = make_sequence();
	for (int i = 0; i < 15; ++i) {
		// uint32_t randIndex = rand() % 7;
		add_tone(seq, make_tone(c_scale[i > 7 ? 14 - i : i] * 7, 0.5, 0));
	}

	// add_tone(seq, make_tone(659.26, 0.5));
	// add_tone(seq, make_tone(587.33, 0.5));
	// add_tone(seq, make_tone(523.25, 0.5));
	// add_tone(seq, make_tone(0.0, 0.5));

	writeLoop(file, 5, seq);
	wavfile_close(file);

	return 0;
}

void writeToneSineWave(FILE *file, Tone tone)
{
	uint32_t sampleCount = ceil(WAV_SAMPLES_PER_SEC * tone.length);
	double data[sampleCount];
	for (uint32_t i = 0; i < sampleCount; ++i) {
		double step = (double)i / WAV_SAMPLES_PER_SEC;
		data[i] = sin(tone.frequency * step * 2 * M_PI);
		for (uint32_t h = 2; h < tone.harmonics + 2; ++h) {
			data[i] += sin(tone.frequency * h * step * 2 * M_PI);
		}
	}

	normalizeData(data, sampleCount);
	int16_t *waveformData = packageData(data, sampleCount);

	wavfile_write(file, waveformData, sampleCount);
}

void writeToneSquareWave(FILE *file, Tone tone)
{
	uint32_t sampleCount = ceil(WAV_SAMPLES_PER_SEC * tone.length);
	int16_t waveformData[sampleCount];
	for (uint32_t i = 0; i < sampleCount; ++i) {
		double step = (double)i / WAV_SAMPLES_PER_SEC;
		waveformData[i] = envelope(i, sampleCount) * MAX_VOLUME * (sin(tone.frequency * step * 2 * M_PI) > 0.0 ? 1 : -1);
	}

	wavfile_write(file, waveformData, sampleCount);
}

void writeLoop(FILE *file, uint32_t loopCount, ToneSequence *toneSequence)
{
	for (uint32_t loop = 0; loop < loopCount; ++loop) {
		for (uint32_t i = 0; i < toneSequence->toneCount; ++i) {
			// writeToneSineWave(file, toneSequence->tones[i]);
			writeToneSquareWave(file, toneSequence->tones[i]);
		}
	}
}

void writeChord()
{
	/*
b(t) = sin(Api(t)) is the base note of the chord at frequency A.
T(t) = sin(5/4piA(t)) is the major third of the base b(t).
D(t) = sin(3/2piA(t)) is the dominant (fifth) of the base b(t).
A(t) = sin(2Api(t)) is the octave.
	*/


}

double envelope(uint32_t x, uint32_t len)
{
	double section = (double)x / len;
	if (section <= 0.25) {
		return (4.0/len) * x;
	} else if (section <= 0.50) {
		return -(1.2/len) * (x - (len/4.0)) + 1.0;
	} else if (section <= 0.75) {
		return 0.70;
	} else {
		return -(2.8/len) * (x - (3*len/4.0)) + 0.70;
	}
}

void normalizeData(double *data, uint32_t length)
{
	double max = 0.0;
	for (uint32_t i = 0; i < length; ++i) {
		if (fabs(data[i]) > max) {
			max = fabs(data[i]);
		}
	}

	for (uint32_t i = 0; i < length; ++i) {
		data[i] /= max;
	}
}

int16_t* packageData(double *data, uint32_t length)
{
	int16_t waveformData[length];
	for (uint32_t i = 0; i < length; ++i) {
		waveformData[i] = envelope(i, length) * MAX_VOLUME * data[i];
	}

	return waveformData;
}

