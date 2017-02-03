#ifndef __SPECTRUMANALYSIS_H_
#define __SPECTRUMANALYSIS_H_

#include <vector>

namespace larocv {
  
  double
  Mean(const std::vector<float>& array, size_t start=0, size_t nsample=0);
  
  double
  Sigma(const std::vector<float>& array, size_t start=0, size_t nsample=0);
  
  std::vector<float>
  RollingMean(const std::vector<float>& array,
	      size_t pre, size_t post,
	      float ignore_value=-1);
  
  std::vector<float>
  RollingSigma(const std::vector<float>& array,
	       size_t pre, size_t post,
	       float ignore_value=-1);
  
  
  
  std::vector<float>
  RollingGradient(const std::vector<float>& array,
		  size_t pre, size_t post,
		  float ignore_value);
  
  
  float
  BinnedMaxOccurrence(const std::vector<float>& array,
		      const size_t nbins);
}

#endif