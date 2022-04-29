# Bass Boost
Use a biquad filter to enhance bass part of music.

# Usage:  
./bassBoost ref.wav output.wav in command line like Cygwin

# Design bass boost filter (biquad filter) by Matlab
% ------- example of bass boost filter design -------     
gain = 6;  % peak gain (dB)  -12~12     
slope = 0.8;    
FcL = 800/(Fs/2);   
% Design the filter coefficients using the specified parameters.   
[B1,A1] = designShelvingEQ(gain,slope,FcL);   
[h1,w1]=freqz(B1,[1;A1],1024,Fs);   
