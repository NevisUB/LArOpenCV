#ifndef __SPECTRUMANALYSIS_CXX_
#define __SPECTRUMANALYSIS_CXX_

#include "SpectrumAnalysis.h"
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <iostream>

namespace larocv {

  ///////////////////////////////////Truncated
  std::vector<float> 
  Calc_smooth_mean(const std::vector<float>& dq,
		   const double _n_window_size,
		   const int window_cutoff,
		   const double frac)
  {
    std::vector<float> truncatedQ;
    
    for(auto window : Get_windows(dq,_n_window_size) ) {
      if(window.size() > (size_t)window_cutoff) 
	Cut(window,frac);
      truncatedQ.push_back(Calc_mean(window));
    }
    return truncatedQ;
  }

  template<typename T>
  std::vector<std::vector<T>> 
  Get_windows(const std::vector<T>& the_thing,
	      const size_t window_size)
  {

    // given a vector of values return a vector of the same length
    // with each element being a vector of the values of the local neighbors
    // of the element at position i in the original vector
    // input  : [0,1,2,3,4,5,6,...,...,N-3,N-2,N-1] (input vector of size N)
    // output  (assuming a value of 'w' below == 3):
    // 0th element: [0]
    // 1st element: [0,1,2]
    // 2nd element: [0,1,2,3,4]
    // jth element: [j-w,j-w+1,..,j+w-2,j+w-1]
    
    std::vector<std::vector<T> > data;
    
    auto w = window_size + 2;
    w = (unsigned int)((w - 1)/2);
    auto num = the_thing.size();
    
    data.reserve(num);
    
    for(size_t i = 1; i <= num; ++i) {
      std::vector<T> inner;
      inner.reserve(20);
      // if we are at the beginning of the vector (and risk accessing -1 elements)
      if(i < w)
	{
	  if (i == 1)
	    {
	      for(size_t j = 0; j < 2; ++j)
		inner.push_back(the_thing[j]);
	    }
	  else
	    {
	      for(size_t j = 0; j < 2 * (i%w) - 1; ++j)
		inner.push_back(the_thing[j]);
	    }
	}
      // if we are at the end of the vector (and risk going past it)
      else if (i > num - w + 1)
	{
	  for(size_t j = num - 2*((num - i)%w)-1 ; j < num; ++j)
	    inner.push_back(the_thing[j]);
	}
      // if we are in the middle of the waveform
      else
	{
	  for(size_t j = i - w; j < i + w - 1; ++j)
	    inner.push_back(the_thing[j]);
	}
      data.emplace_back(inner);
    }

    return data;
  
  }

  template<typename W>
  void Cut(std::vector<W>& data, double frac)
  {
    auto size   = data.size();
    // calcualte number of elements to be kept
    int to_stay = floor(frac*size);
  
    // sort the array based on charge
    // so that high-charge hits are removed
    std::sort(data.begin(),data.end(),
	      [](const W& a, const W& b) -> bool
	      {
		return a < b;	      
	      });

    // erase all elements after the last one to be kept
    data.erase(data.begin(), data.begin() + to_stay);
    data.erase(data.end() - to_stay, data.end());
    //data.erase(data.begin() + to_stay, data.end());
  }

  template<typename S>
  S Calc_mean(const std::vector<S>& data)
  {
    if(!data.size())
      std::invalid_argument("You have me nill to Calc_mean");

    auto sum = S{0.0};
    size_t denominator = 0;
    std::cout<<"==============="<<std::endl;
    
    for(const auto& d : data) 
      {
	if (d!=0) denominator+=1;
	sum += d;
	std::cout<<"d is "<<d<<std::endl;
      }
    return sum / ( (S) data.size() ); 
    //return sum / ((S)denominator); 
  }

  float
  VectorMean(const std::vector<float>& v)
  {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / (float) v.size();
    
    return mean;
  }
  


  double
  Mean(const std::vector<float>& array,
       size_t start,
       size_t nsample)
  {
    if(!nsample) nsample = array.size();
    if(start > array.size() || (start+nsample) > array.size())
      throw std::invalid_argument("start larger than array size or too many samples from start point requested"); 
    
    double sum = std::accumulate(array.begin()+start,array.begin()+start+nsample,(double)0.0) / ((double)nsample);
    
    return sum;
  }
  
  double 
  Sigma(const std::vector<float>& array,
	size_t start,
	size_t nsample)
  {
    if(!nsample) nsample = array.size();
    if(start > array.size() || (start+nsample) > array.size())
      throw std::invalid_argument("start larger than array size or too many samples from start point requested"); 
    
    double sum  = std::accumulate   (array.begin()+start,array.begin()+start+nsample,(double)0.0);
    double sum2 = std::inner_product(array.begin()+start,array.begin()+start+nsample,array.begin()+start,(double)0.0);
    
    sum = std::sqrt( sum2 / (double)nsample - std::pow(sum / (double)nsample,2) );
    
    return sum;
  }

  double
  Covariance(std::vector<float> array1, //makes a copy
  	      std::vector<float> array2, //same..
  	      size_t start,
  	      size_t nsample)
  {
    if (array1.size() != array2.size()) throw std::invalid_argument("array 1 and array 2 do not have equal size");
    if(!nsample) nsample = array1.size();
    if(start > array1.size() || (start+nsample) > array1.size())
      throw std::invalid_argument("start larger than array size or too many samples from start point requested"); 

    double mean_array1 = Mean(array1);
    double mean_array2 = Mean(array2);

    for(auto& v:array1)v-=mean_array1;
    for(auto& v:array2)v-=mean_array2;

    double cov = std::inner_product(array1.begin()+start,array1.begin()+start+nsample,array2.begin()+start,(double)0.0);
    cov /= (double) nsample;
    
    return cov;
  }

  
  std::vector<float>
  RollingMean(const std::vector<float>& array,
	      size_t pre, size_t post,
	      float ignore_value)
  {
    std::vector<float> res(array.size(),ignore_value);
    float  local_sum = 0;
    size_t valid_ctr = 0;
    for(size_t idx=0; idx<array.size(); ++idx) {
      if(array[idx] == ignore_value) continue;
      if(idx < pre) continue;
      if((idx+post) >= array.size()) continue;
      local_sum = 0;
      valid_ctr = 0;
      for(size_t subidx=(idx-pre); subidx <= idx+post; ++subidx) {
	if(array[subidx] == ignore_value) continue;
	local_sum += array[subidx];
	++valid_ctr;
      }
      if(valid_ctr < 2) continue;
      res[idx] = local_sum / (float)(valid_ctr);
    }
    return res;
  }
  std::vector<float>
  RollingSigma(const std::vector<float>& array,
	       size_t pre, size_t post,
	       float ignore_value)
  {
    std::vector<float> res(array.size(),ignore_value);
    float  local_sum = 0;
    float  local_sum2= 0;
    size_t valid_ctr = 0;
    for(size_t idx=0; idx<array.size(); ++idx) {
      if(array[idx] == ignore_value) continue;
      if(idx < pre) continue;
      if((idx+post) >= array.size()) continue;
      local_sum = 0;
      local_sum2= 0;
      valid_ctr = 0;
      for(size_t subidx=(idx-pre); subidx <= idx+post; ++subidx) {
	if(array[subidx] == ignore_value) continue;
	local_sum   += array[subidx];
	local_sum2  += array[subidx] * array[subidx];
	++valid_ctr;
      }
      if(valid_ctr < 2) continue;
      res[idx] = std::sqrt( local_sum2 / (float)(valid_ctr) - std::pow(local_sum / (float)(valid_ctr),2) );
    }
    return res;
  }
  

  std::vector<float>
  RollingGradient(const std::vector<float>& array,
		  size_t pre, size_t post,
		  float ignore_value)
  {
    std::vector<float> slope(array.size(),ignore_value);
    float  local_sum = 0;
    size_t valid_ctr = 0;
    float  post_mean = 0;
    float  pre_mean  = 0;
    for(size_t idx=0; idx<array.size(); ++idx) {
      if(array[idx] == ignore_value) continue;
      //if(idx < pre) continue;
      //if(idx+post >= array.size()) continue;
      
      // Compute average in pre sample
      local_sum = 0;
      valid_ctr = 0;
      if(idx >= pre) {
	for(size_t subidx=(idx-pre); subidx < idx; ++subidx){
	  if(array[subidx] == ignore_value) continue;
	  local_sum += array[subidx];
	  ++valid_ctr;
	}
	if(valid_ctr < 2) continue;
	pre_mean = local_sum / (float)(valid_ctr);
      }else if(array[idx] != ignore_value) {
	pre_mean = array[idx];
      }else
	continue;
      
      // Compute average in post sample
      local_sum = 0;
      valid_ctr = 0;
      if(idx+post < array.size()) {
	for(size_t subidx=idx+1; subidx <= idx+post; ++subidx){
	  if(array[subidx] == ignore_value) continue;
	  local_sum += array[subidx];
	  ++valid_ctr;
	}
	if(valid_ctr < 2) continue;
	post_mean = local_sum / (float)(valid_ctr);
      }else if(array.back() != ignore_value) {
	post_mean = array.back();
      }else
	continue;
      
      slope[idx] = (post_mean - pre_mean) / (((float)pre)/2. + ((float)post)/2. + 1.);
    }
    return slope;
  }

  float
  BinnedMaxOccurrence(const std::vector<float>& array,const size_t nbins)
  {
    if(nbins<1) throw std::invalid_argument("nbins is less than 1");
    
    auto res = std::minmax_element(std::begin(array),std::end(array));
    
    float bin_width = ((*res.second) - (*res.first)) / ((float)nbins);
    
    if(nbins==1) return ((*res.first) + bin_width /2.);
    
    //std::cout<<"Min: "<<(*res.first)<<" Max: "<<(*res.second)<<" Width: "<<bin_width<<std::endl;
    
    // Construct array of nbins
    static std::vector<size_t> ctr_v(nbins,0);
    for(auto& v : ctr_v) v=0;
    for(auto const& v : array) {
      
      size_t index = int((v - (*res.first))/bin_width);
      //std::cout<<"adc = "<<v<<" width = "<<bin_width<< " ... "
      //<<index<<" / "<<ctr_v.size()<<std::endl;
      
      ctr_v[index]++;
      
    }
    
    // Find max occurrence
    auto max_it = std::max_element(std::begin(ctr_v),std::end(ctr_v));
    
    // Get the mean of max-occurrence bins
    float mean_max_occurrence = 0;
    float num_occurrence = 0;
    for(size_t bin=0; bin<ctr_v.size(); ++bin) {
      
      if(ctr_v[bin] != (*max_it)) continue;
      
      mean_max_occurrence += ((*res.first) + bin_width / 2. + bin_width * bin);
      
      num_occurrence += 1.0;
    }
    
    return (mean_max_occurrence / num_occurrence);
  }
  
}

#endif
