#include"bassBoost.h"
#include<stdio.h>
#include<stdlib.h>
#include "tinywavein_c.h"
#include "tinywaveout_c.h"
#define MAX_CHANNELS (2)
#define MAX_SAMPLES (480)
static void exit_if(int condition, const char* message);
static void deinterleave(int32_t* in, int32_t** out, int n, int channels);
static void interleave_short(int16_t** in, int16_t* out, int32_t n, int32_t channels);
static void interleave_int(int32_t** in, int32_t* out, int32_t n, int32_t channels);
static void scale_24_to_16(const int32_t* in, int16_t* out, int n);

static void printUsage(void)
{
    printf("  A software to implement bass boost on input wav file.\n");
    printf("  Usage: bassBoost ref.wav output.wav \n");
    printf("  Biquad filter calculation: ");
    printf("  y[n] = b0*x[n]+b1*x[n-1]+b2*x[n-2]-a1*y[n-1]-a2*y[n-2]");
    exit(0);
}

int main(int ac, char* av[]) {

    char* inputFilename = NULL, * outputFilename = NULL;
    WAVEFILEIN* in_file1;
    WAVEFILEOUT* output_wav;
    unsigned int sampleRate = 0, nLength = 0, nSamplesRead1 = 0;
    short nChannels = 0,  bipsIn1 = 0;
    unsigned int  nSamples = 480, nSamplesRead = 0;
    int32_t   sample_buf[MAX_CHANNELS * MAX_SAMPLES];
    int32_t   buf_24[MAX_CHANNELS * MAX_SAMPLES];
    int16_t   buf_16[MAX_CHANNELS * MAX_SAMPLES];
    int16_t   buf_16_out[MAX_CHANNELS * MAX_SAMPLES];
    int32_t* input24[] = { buf_24, buf_24 + nSamples };
    int16_t* output16[] = { buf_16_out, buf_16_out + nSamples };
    int16_t* sample_buf_short[MAX_CHANNELS * MAX_SAMPLES] = { 0 };

    if (ac < 3)
    {
        printf("  Not enough input arguments!\n");
        printUsage();
    }

    inputFilename = av[1]; /* input wav */
    outputFilename = av[2]; /* output wav after bass boost 6 dB */
    
    in_file1 = OpenWav(inputFilename, &sampleRate, &nChannels, &nLength, &bipsIn1);
    /* Open Output Wav File */
    printf("outputFilename = %s\n", outputFilename);
    output_wav = CreateWav(outputFilename, sampleRate, nChannels, bipsIn1);
    exit_if(!output_wav, "Error creating wav file!");

    /* Initialize bassBoost struct */
    sBiquad* biquad_p[MAX_CHANNELS];
    for (int i = 0; i < nChannels; i++) {
        biquad_p[i] = malloc(sizeof(sBiquad));
        Biquad_init(biquad_p[i], sampleRate);
    }

    while (1) {
        ReadWavInt(in_file1, sample_buf, nSamples * nChannels, &nSamplesRead);
        if (nSamplesRead == 0)
            break;
        /* deinterleave channels */
        deinterleave(sample_buf, input24, nSamples, nChannels);

        /* only deal with 16-bit PCM data */
        int16_t* input16[] = { buf_16, buf_16 + nSamples };
        scale_24_to_16(buf_24, buf_16, nSamples * nChannels);

        /* Bass Boost */
        for (int ch = 0; ch < nChannels;ch++) {
            bassBoost_fx(biquad_p[ch], input16[ch], output16[ch], nSamples);
        }

        /* Write wav output */

        for (int i = 0; i < nChannels; i++)
        {
            output16[i] = buf_16 + i * nSamples;
        }
        interleave_short(output16, sample_buf_short, nSamples, nChannels);
        WriteWavShort(output_wav, sample_buf_short, nSamples * nChannels);

    }
    for (int i = 0; i < nChannels; i++) {
        free(biquad_p[i]);
    }
	return 0;
}

/*
*  output[0][0] = input[0]
*  output[0][1] = input[2]
*  output[0][2] = input[4]
*  ...
*  output[1][0] = input[1]
*  output[1][1] = input[3]
*  output[1][2] = input[5]
*/

static void deinterleave(int32_t* in, int32_t** out, int n, int channels)
{
    int ch, i;
    for (ch = 0; ch < channels; ch++)
    {
        for (i = 0; i < n; i++)
        {
            out[ch][i] = in[i * channels + ch];
        }
    }
}

static void interleave_short(int16_t** in, int16_t* out, int32_t n, int32_t channels)
{
    int32_t ch, i;
    for (ch = 0; ch < channels; ch++) {
        for (i = 0; i < n; i++) {
            out[i * channels + ch] = in[ch][i];
        }
    }
}

static void interleave_int(int32_t** in, int32_t* out, int32_t n, int32_t channels)
{
    int32_t ch, i;
    for (ch = 0; ch < channels; ch++) {
        for (i = 0; i < n; i++) {
            out[i * channels + ch] = in[ch][i];
        }
    }
}

static void scale_24_to_16(const int32_t* in, int16_t* out, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        out[i] = in[i];
    }
}

static void exit_if(int condition, const char* message)
{
    if (condition)
    {
        puts(message);
        exit(0);
    }
}