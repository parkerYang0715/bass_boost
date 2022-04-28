#include"bassBoost.h"
#include"consts.h"
#include<stdlib.h>
#include<stdint.h>
void Biquad_init(sBiquad* biquad_p, unsigned int fs) {
	//biquad_p->w[0] = 0;
	memset(biquad_p->w, 0, sizeof(biquad_p->w));
	if (fs == 8000) {
		memcpy(biquad_p->b, biquad_b[0], 3*sizeof(int));
		memcpy(biquad_p->a, biquad_a[0], 2 * sizeof(int));
	}
	else if (fs == 16000) {
		memcpy(biquad_p->b, biquad_b[1], 3 * sizeof(int));
		memcpy(biquad_p->a, biquad_a[1], 2 * sizeof(int));
	}
	else if (fs == 32000) {
		memcpy(biquad_p->b, biquad_b[2], 3 * sizeof(int));
		memcpy(biquad_p->a, biquad_a[2], 2 * sizeof(int));
	}
	else if (fs == 44100) {
		memcpy(biquad_p->b, biquad_b[3], 3 * sizeof(int));
		memcpy(biquad_p->a, biquad_a[3], 2 * sizeof(int));
	}
	else {
		memcpy(biquad_p->b, biquad_b[4], 3 * sizeof(int));
		memcpy(biquad_p->a, biquad_a[4], 2 * sizeof(int));
	}
}
void bassBoost_fx(sBiquad* biquad_p, short* input, short* output, short frameSize) {
	// w[n] = x[n]-a1*w[n-1]-a2*w[n-2]
	// y[n] = b0*w[n]+b1*w[n-1]+b2*w[n-2]
	int64_t lltemp;
	int newW;
	for (int i = 0; i < frameSize; i++) {
		/* step1: calculate new w */
		lltemp = (int64_t) input[i]<<31;  //Q46
		lltemp -= (int64_t)biquad_p->a[0] * biquad_p->w[0];
		lltemp -= (int64_t)biquad_p->a[1] * biquad_p->w[1];
		newW = lltemp >> 23;  // Q23
		
		/* step2: update output */
		lltemp = (int64_t)biquad_p->b[0] * newW;
		lltemp += (int64_t)biquad_p->b[1] * biquad_p->w[0];
		lltemp += (int64_t)biquad_p->b[2] * biquad_p->w[1];
		output[i]= lltemp >> 31;  // Q23

		/* step3: update old w */
		biquad_p->w[1] = biquad_p->w[0];
		biquad_p->w[0] = newW;
	}
}