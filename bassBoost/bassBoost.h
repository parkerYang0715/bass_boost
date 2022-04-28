// Q23
// b: b0,b1,b2;
// a: a0,a1,a2;   a0=1, a1 = a[0],a2=a[1];

typedef struct {
	int w[2]; //[w_old, wold_old]Q23
	// w[n] = x[n]-a1*w[n-1]-a2*w[n-2]
	// y[n] = b0*w[n]+b1*w[n-1]+b2*w[n-2]
	int b[3];
	int a[2];
}sBiquad;
void Biquad_init(sBiquad* biquad_p, unsigned int fs);
void bassBoost_fx(sBiquad* biquad_p, short* input, short* output);
