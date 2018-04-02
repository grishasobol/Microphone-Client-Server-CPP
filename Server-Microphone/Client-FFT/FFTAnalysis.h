#pragma once

// AVal - array of analyzed data, Nvl - size of array, should be degree of 2.
// FTvl - array of fft result data, Nft - size of array should be equal to Nvl.

const double TwoPi = 6.283185307179586;

void FFTAnalysis(const double *AVal, double *FTvl, int Nvl, int Nft);