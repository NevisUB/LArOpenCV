#ifndef __IMAGECLUSTER_BASEUTIL_CXX__
#define __IMAGECLUSTER_BASEUTIL_CXX__

#include <limits.h>
#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/imgproc/imgproc.hpp>
#endif
#endif
#include "LArOpenCV/Core/larbys.h"
#include "BaseUtil.h"
#include <vector>
#include <algorithm>

namespace larocv {

  ::cv::Rect BoundingBox(const larocv::ContourArray_t& clusters, const double min_area)
  {
    int bb_xmin = INT_MAX;
    int bb_ymin = INT_MAX;
    int bb_xmax = 0;
    int bb_ymax = 0;
    for(auto const& c : clusters) {
      if(::cv::contourArea(c)<min_area) continue;
      auto rect = BoundingBox(c);
      if(rect.x < bb_xmin) bb_xmin = rect.x;
      if(rect.y < bb_ymin) bb_ymin = rect.y;
      if(rect.x+rect.width+1  > bb_xmax) bb_xmax = rect.x + rect.width + 1;
      if(rect.y+rect.height+1 > bb_ymax) bb_ymax = rect.y + rect.height + 1;
    }
    if(bb_xmin == INT_MAX) bb_xmin = 0;
    if(bb_ymin == INT_MAX) bb_ymin = 0;
    return ::cv::Rect(bb_xmin,bb_ymin,bb_xmax-bb_xmin,bb_ymax-bb_ymin);
  }
  
  ::cv::Rect BoundingBox(const larocv::Contour_t& cluster)
  {
    if(cluster.size()<2)
      throw larbys("Cannot create SubMatrix for a contour of size < 2!");
    
    int x_min = INT_MAX;
    int y_min = INT_MAX;
    int x_max = 0;
    int y_max = 0;

    for(auto const& pt : cluster) {

      if(pt.x < x_min) x_min = pt.x;
      if(pt.x > x_max) x_max = pt.x;
      if(pt.y < y_min) y_min = pt.y;
      if(pt.y > y_max) y_max = pt.y;

    }
    if(x_min) x_min -=1;
    if(y_min) y_min -=1;
    x_max +=1;
    y_max +=1;
    
    return ::cv::Rect( x_min, y_min,
		       x_max - x_min,
		       y_max - y_min);
  }

  ::cv::Mat CreateSubMatRef(const larocv::Contour_t& cluster, cv::Mat& img)
  {
    if(cluster.size()<2)
      throw larbys("Cannot create SubMatrix for a contour of size < 2!");

    auto bbox = BoundingBox(cluster);

    ::cv::Mat result;
    img(bbox).copyTo(result);

    return result;
  }

  ::cv::Mat CreateSubMatCopy(const larocv::Contour_t& cluster, const cv::Mat& img)
  {
    if(cluster.size()<2)
      throw larbys("Cannot create SubMatrix for a contour of size < 2!");

    auto bbox = BoundingBox(cluster);

    ::cv::Mat result = ::cv::Mat::zeros(img.size(),img.type());

    for(int x=0; x<img.rows; ++x) {

      for(int y=0; y<img.cols; ++y) {

	int submat_x = x - bbox.x;
	int submat_y = y - bbox.y;

	double inside = ::cv::pointPolygonTest(cluster,::cv::Point2f(x,y),false);

	if(inside<0) continue;

	result.at<float>(submat_x,submat_y,0) = img.at<float>(x,y,0);
      }

    }

    return result;
  }

    unsigned int Factorial(unsigned int n)
  {return (n == 1 || n == 0) ? 1 : Factorial(n - 1) * n;}
  
  std::vector<std::vector<size_t> > SimpleCombination(size_t n, size_t r) {
    
    if(!n || !r) throw std::exception();
    if(r > n) throw std::exception();
    
    std::vector<bool> v(n,false);
    std::fill(v.begin()+n-r,v.end(),true);
    std::vector<std::vector<size_t> > res;
    res.reserve(Factorial(n)/Factorial(n-r)/Factorial(r));
    
    do {
      std::vector<size_t> tmp;
      tmp.reserve(r);
      
      for(size_t i=0; i<n; ++i) { if(v[i]) tmp.push_back(i); }
      res.push_back(tmp);
    } while (std::next_permutation(v.begin(),v.end()));
    
    return res;
  }
  
  std::vector<std::vector<size_t> > ClusterCombinations(const std::vector<size_t>& seed)
  {
    
    std::vector<size_t> ctr(seed.size(),0);
    
    std::vector<std::vector<size_t> > res;
    
    while(1) {
      
      res.push_back(std::vector<size_t>(seed.size(),0));
      for(size_t index=0; index<ctr.size(); ++index)
	
	(*res.rbegin())[index] = ctr.at(index);
      
      for(size_t i=0; i<ctr.size(); ++i) {
	
	size_t index = (size_t)(ctr.size()-i-1);
	
	ctr.at(index) +=1;
	
	if(ctr.at(index) < seed.at(index))
	  
	  break;
	
	ctr.at(index) = 0;
	
      }
      
      bool abort = true;
      for(auto const& value : ctr)
	
	abort = abort && (!(value));
      
      if(abort) break;
    }
    return res;
  }

  std::vector<std::vector<std::pair<size_t,size_t> > > PlaneClusterCombinations(const std::vector<size_t>& seed)
  {
    // Result container
    std::vector<std::vector<std::pair<size_t,size_t> > > result;

    // Loop over N-planes: start from max number of planes => down to 2 planes
    for(size_t i=0; i<seed.size(); ++i) {

      // If finish making clusters down to 2 palnes, break
      if(seed.size() < 2+i) break;

      // Compute possible N-plane combinations
      auto const& plane_comb_v = SimpleCombination(seed.size(),seed.size()-i);

      // Loop over possible N-plane combinations
      for(auto const& plane_comb : plane_comb_v){

	// Make a seed for cluster combinations
	std::vector<size_t> cluster_seed_v;
	cluster_seed_v.reserve(plane_comb.size());
	for(auto const& index : plane_comb) cluster_seed_v.push_back(seed[index]);

	// Compute cluster combinations
	for(auto const& cluster_comb : ClusterCombinations(cluster_seed_v)) {

	  // Store result
	  result.push_back(std::vector<std::pair<size_t,size_t> >());
	  for(size_t i=0; i<cluster_comb.size(); ++i)

	    (*result.rbegin()).push_back(std::make_pair(plane_comb.at(i),cluster_comb.at(i)));
	}
      }
    }
    return result;
  }


}

#endif
