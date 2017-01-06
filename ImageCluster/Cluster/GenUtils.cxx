#ifndef __GENUTILS_CXX_
#define __GENUTILS_CXX_

#include "GenUtils.h"

double
Mean(const std::vector<float>& array, size_t start, size_t nsample)
{
  if(!nsample) nsample = array.size();
  if(start > array.size() || (start+nsample) > array.size())
    throw std::invalid_argument("start larger than array size or too many samples from start point requested"); 
  
  double sum = std::accumulate(array.begin()+start,array.begin()+start+nsample,0.0) / ((double)nsample);
  
  return sum;
}

double
Sigma(const std::vector<float>& array, size_t start, size_t nsample)
{
  if(!nsample) nsample = array.size();
  if(start > array.size() || (start+nsample) > array.size())
    throw std::invalid_argument("start larger than array size or too many samples from start point requested"); 

  double sum  = std::accumulate   (array.begin()+start,array.begin()+start+nsample,0.0);
  double sum2 = std::inner_product(array.begin()+start,array.begin()+start+nsample,array.begin()+start,0.0);

  sum = std::sqrt( sum2 / (double)nsample - std::pow(sum / (double)nsample,2) );
  
  return sum;
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

geo2d::VectorArray<float>
QPointOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle, const float pi_threshold)
{
  geo2d::VectorArray<float> res;

  // Find crossing point
  ::cv::Mat polarimg;
  ::cv::linearPolar(img, polarimg, circle.center, circle.radius*2, ::cv::WARP_FILL_OUTLIERS);
  
  size_t col = (size_t)(polarimg.cols / 2);

  std::vector<std::pair<int,int> > range_v;
  std::pair<int,int> range(-1,-1);

  for(size_t row=0; row<polarimg.rows; ++row) {

    float q = (float)(polarimg.at<unsigned char>(row, col));
    if(q < pi_threshold) {
      if(range.first >= 0) {
	range_v.push_back(range);
	range.first = range.second = -1;
      }
      continue;
    }
    //std::cout << row << " / " << polarimg.rows << " ... " << q << std::endl;
    if(range.first < 0) range.first = range.second = row;

    else range.second = row;

  }
  if(range.first>=0 && range.second>=0) range_v.push_back(range);
  // Check if end should be combined w/ start
  if(range_v.size() >= 2) {
    if(range_v[0].first == 0 && (range_v.back().second+1) == polarimg.rows) {
      range_v[0].first = range_v.back().first - (int)(polarimg.rows);
      range_v.pop_back();
    }
  }
  // Compute xs points
  for(auto const& r : range_v) {

    //std::cout << "XS @ " << r.first << " => " << r.second << " ... " << polarimg.rows << std::endl;
    float angle = ((float)(r.first + r.second))/2.;
    if(angle < 0) angle += (float)(polarimg.rows);
    angle = angle * M_PI * 2. / ((float)(polarimg.rows));

    geo2d::Vector<float> pt;
    pt.x = circle.center.x + circle.radius * cos(angle);
    pt.y = circle.center.y + circle.radius * sin(angle);

    res.push_back(pt);
  }
  return res;
}


#endif
