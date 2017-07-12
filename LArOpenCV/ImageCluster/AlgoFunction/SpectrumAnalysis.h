#ifndef __SPECTRUMANALYSIS_H_
#define __SPECTRUMANALYSIS_H_

#include <vector>
#include <stdlib.h>
#include <algorithm>
namespace larocv {

  float
  Calc_truncated_mean(std::vector<float> input, float thre);
  
  std::vector<float> 
  Calc_smooth_mean(const std::vector<float>& dq,
		   const double _n_window_size,
		   const int window_cutoff,
		   const double p_above);

  template<typename T> 
  std::vector<std::vector<T>> 
  Get_windows(const std::vector<T>& the_thing,
	      const size_t window_size);
  
  template<typename W>
  void 
  Cut(std::vector<W>& data, double frac);

  template<typename W>
  std::vector<W> 
  Remove(std::vector<W> data, double frac);

  template<typename S>
  S Calc_mean(const std::vector<S>& data);
  //Above are truncated mean Methods

  std::vector<float> CutHeads(std::vector<float>, double frac1, double frac2);
  
  float
  VectorMean(const std::vector<float>& v);
  
  double
  Mean(const std::vector<float>& array,
       size_t start=0,
       size_t nsample=0);
  
  double
  Sigma(const std::vector<float>& array,
	size_t start=0,
	size_t nsample=0);


  double
  Covariance(std::vector<float> array1, //makes a copy
	     std::vector<float> array2, //same..
	     size_t start=0,
	     size_t nsample=0);
  
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
