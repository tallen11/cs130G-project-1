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

void writeToneData(FILE *file, double *toneData, uint32_t length);
void writeToneSineWave(FILE *file, Tone tone);
void writeToneSquareWave(FILE *file, Tone tone);
void writeExperimentalWave(FILE *file, Tone tone);
void writeChord(FILE *file, double first, double sec, double third, uint32_t octave, double length);
void writeLoop(FILE *file, uint32_t loopCount, ToneSequence *sequence);
double envelope(uint32_t x, uint32_t len);
void normalizeData(double *data, uint32_t length);
int16_t* packageData(double *data, uint32_t length);

void addSineWave(double *data, uint32_t length, Tone tone);
void addSquareWave(double *data, uint32_t length, Tone tone);
void addSawtoothWave(double *data, uint32_t length, Tone tone);
void addSuperSawWave(double *data, uint32_t length, Tone tone, uint32_t layers);

void buildChord(double *data, uint32_t length, Tone tone1, Tone tone2, Tone tone3);

void buildChordArp(double *data, uint32_t length, Tone tone1, Tone tone2, Tone tone3);

void filterTremolo(double *data, uint32_t length, double frequency);
void addSinWaveLFOPhaser(double *data, uint32_t length, Tone tone, double frequency);

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int main(/*int argc, char const *argv[]*/)
{
	srand(time(NULL));

	FILE *file = wavfile_open("sound.wav");

	writeExperimentalWave(file, make_tone(40.0, 5.0, 0));

	// ToneSequence *s = make_sequence();
	// add_tone(s, make_tone(200.0, 1.0, 0));
	// writeLoop(file, 10, s);
	// writeToneSineWave(file, make_tone(200.0, 15.0, 0));
	// writeToneSquareWave(file, make_tone(200.0, 5.0, 0));

	// writeChord(file, C1, E1, G1, 7, 1.0);
	// writeChord(file, G1, B1, D1, 7, 1.0);
	// writeChord(file, A1, C1, E1, 7, 1.0);
	// writeChord(file, D1, 46.249, A1, 7, 1.0);


	// ToneSequence *seq = make_sequence();
	// for (int i = 0; i < 15; ++i) {
	// 	// uint32_t randIndex = rand() % 7;
	// 	add_tone(seq, make_tone(c_scale[i > 7 ? 14 - i : i] * 7, 0.1, 6));
	// }

	// // add_tone(seq, make_tone(659.26, 0.5));
	// // add_tone(seq, make_tone(587.33, 0.5));
	// // add_tone(seq, make_tone(523.25, 0.5));
	// // add_tone(seq, make_tone(0.0, 0.5));

	// writeLoop(file, 5, seq);
	wavfile_close(file);

	return 0;
}

void writeToneData(FILE *file, double *toneData, uint32_t length)
{
	normalizeData(toneData, length);
	int16_t *waveformData = packageData(toneData, length);
	wavfile_write(file, waveformData, length);
	free(waveformData);
}

void writeToneSineWave(FILE *file, Tone tone)
{
	uint32_t sampleCount = ceil(WAV_SAMPLES_PER_SEC * tone.length);
	double *data = malloc(sizeof(double) * sampleCount);
	addSineWave(data, sampleCount, tone);
	// filterTremolo(data, sampleCount, 0.5);
	// addSinWaveLFOPhaser(data, sampleCount, tone, 0.25);
	writeToneData(file, data, sampleCount);
	free(data);
}

void writeToneSquareWave(FILE *file, Tone tone)
{
	uint32_t sampleCount = ceil(WAV_SAMPLES_PER_SEC * tone.length);
	double *data = malloc(sizeof(double) * sampleCount);
	addSquareWave(data, sampleCount, tone);
	writeToneData(file, data, sampleCount);
	free(data);
}

void writeExperimentalWave(FILE *file, Tone tone)
{
	uint32_t sampleCount = ceil(WAV_SAMPLES_PER_SEC * tone.length);
	double *data = malloc(sizeof(double) * sampleCount);
	addSuperSawWave(data, sampleCount, tone, 8);
	writeToneData(file, data, sampleCount);
	free(data);
}

void writeLoop(FILE *file, uint32_t loopCount, ToneSequence *toneSequence)
{
	for (uint32_t loop = 0; loop < loopCount; ++loop) {
		for (uint32_t i = 0; i < toneSequence->toneCount; ++i) {
			writeToneSineWave(file, toneSequence->tones[i]);
			// writeToneSquareWave(file, toneSequence->tones[i]);
		}
	}
}

void writeChord(FILE *file, double first, double sec, double third, uint32_t octave, double length)
{
	uint32_t sampleCount = ceil(WAV_SAMPLES_PER_SEC * length);
	double *data = malloc(sizeof(double) * sampleCount);
	buildChord(data, sampleCount, 
					 make_tone(first * octave, length, 1),
					 make_tone(sec * octave, length, 3),
					 make_tone(third * octave, length, 5));

	writeToneData(file, data, sampleCount);
	free(data);

	// uint32_t sampleCount = ceil(WAV_SAMPLES_PER_SEC * length);
	// double *data = malloc(sizeof(double) * sampleCount);
	// for (uint32_t i = 0; i < sampleCount; ++i) {
	// 	double step = (double)i / WAV_SAMPLES_PER_SEC;
	// 	data[i] = sin(C1 * 7 * step * 2 * M_PI);
	// 	data[i] += sin(E1 * 7 * step * 2 * M_PI);
	// 	data[i] += sin(G1 * 7 * step * 2 * M_PI);
	// 	// for (uint32_t h = 2; h < tone.harmonics + 2; ++h) {
	// 	// 	data[i] += sin(tone.frequency * h * step * 2 * M_PI);
	// 	// }
	// }

	// writeToneData(file, data, sampleCount);
	// free(data);
}

void addSineWave(double *data, uint32_t length, Tone tone)
{
	for (uint32_t i = 0; i < length; ++i) {
		double step = (double)i / WAV_SAMPLES_PER_SEC;
		double inside = (tone.frequency * step * 2 * M_PI);
		data[i] += sin(inside);
		for (uint32_t h = 2; h < tone.harmonics + 2; ++h) {
			data[i] += sin(tone.frequency * h * step * 2 * M_PI);
		}
	}
}

void addSquareWave(double *data, uint32_t length, Tone tone)
{
	for (uint32_t i = 0; i < length; ++i) {
		double step = (double)i / WAV_SAMPLES_PER_SEC;
		data[i] += (sin(tone.frequency * step * 2 * M_PI) > 0.0 ? 1.0 : -1.0);
		for (uint32_t h = 2; h < tone.harmonics + 2; ++h) {
			data[i] += (sin(tone.frequency * h * step * 2 * M_PI) > 0.0 ? 1.0 : -1.0);
		}
	}
}

void addSawtoothWave(double *data, uint32_t length, Tone tone)
{
	tone.harmonics = 50;
	addSineWave(data, length, tone);
}

void addSuperSawWave(double *data, uint32_t length, Tone tone, uint32_t layers)
{
	tone.harmonics = 50;
	for (uint32_t l = 0; l < layers; ++l) {
		for (uint32_t i = 0; i < length; ++i) {
			double step = (double)i / WAV_SAMPLES_PER_SEC;
			double inside = (tone.frequency * step * 2 * M_PI);
			data[i] += sin(inside + 0.01 * M_PI * l);
			for (uint32_t h = 2; h < tone.harmonics + 2; ++h) {
				data[i] += sin(tone.frequency * h * step * 2 * M_PI + 0.01 * M_PI * l);
			}
		}
	}
}

void buildChord(double *data, uint32_t length, Tone tone1, Tone tone2, Tone tone3)
{
	addSineWave(data, length, tone1);
	addSineWave(data, length, tone2);
	addSineWave(data, length, tone3);

	// addSquareWave(data, length, tone1);
	// addSquareWave(data, length, tone2);
	// addSquareWave(data, length, tone3);
}

void buildChordArp(double *data, uint32_t length, Tone tone1, Tone tone2, Tone tone3)
{
	const int arpeggioFreq = ceil(WAV_SAMPLES_PER_SEC * 0.1);

}

void filterTremolo(double *data, uint32_t length, double frequency)
{
	for (uint32_t i = 0; i < length; ++i) {
		double step = (double)i / WAV_SAMPLES_PER_SEC;
		data[i] *= fabs(sin(frequency * 2 * step * 2 * M_PI));
	}	
}

void addSinWaveLFOPhaser(double *data, uint32_t length, Tone tone, double frequency)
{
	for (uint32_t i = 0; i < length; ++i) {
		double step = (double)i / WAV_SAMPLES_PER_SEC;
		double lfoPhase = sin(frequency * step * 2 * M_PI);
		data[i] += sin(tone.frequency * step * 2 * M_PI + 400 * lfoPhase);
		for (uint32_t h = 2; h < tone.harmonics + 2; ++h) {
			data[i] += sin(tone.frequency * h * step * 2 * M_PI + 400 * lfoPhase);
		}
	}
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
	int16_t *waveformData = malloc(sizeof(int16_t) * length);
	for (uint32_t i = 0; i < length; ++i) {
		waveformData[i] = data[i] * envelope(i, length) * MAX_VOLUME;
	}

	return waveformData;
}

