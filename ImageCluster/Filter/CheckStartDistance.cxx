#ifndef __CHECKSTARTDISTANCE_CXX__
#define __CHECKSTARTDISTANCE_CXX__

#include "CheckStartDistance.h"
#include "PCAUtilities.h"

namespace larocv {

  void CheckStartDistance::_Configure_(const ::fcllite::PSet& pset) {
    _max_start_d = pset.get<double>("MaxStartEndDistance");
    
    _w = 0.0557;
    _h = 0.3;
  }

  Cluster2DArray_t CheckStartDistance::_Process_(const larocv::Cluster2DArray_t& clusters, const ::cv::Mat& img,
						 larocv::ImageMeta& meta, larocv::ROI& roi) {

    Cluster2DArray_t oclusters;
    oclusters.reserve(clusters.size());

    std::map<int, bool> keep;

    for (int i = 0; i < clusters.size(); ++i) keep[i] = false;

    for (unsigned i = 0; i < clusters.size(); ++i) {
      for (unsigned j = i; j < clusters.size(); ++j) {

	auto& s1 = clusters[i]._startPt;
	auto& s2 = clusters[j]._startPt;

	auto d = dist(s1.x * _w, s2.x * _w, s1.y * _h, s2.y * _h);

	if (d <= _max_start_d) {
	  keep[i] = true;
	  keep[j] = true;
	}
      }
    }

    for (auto& k : keep) {
      if (k.second) oclusters.emplace_back(clusters[k.first]);
    }

    return oclusters;
  }
}
#endif
