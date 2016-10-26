#ifndef __ATOMICTRACKANA_CXX__
#define __ATOMICTRACKANA_CXX__

#include "AtomicTrackAna.h"
#include "PCACandidates.h"

namespace larocv{

  /// Global larocv::AtomicTrackAnaFactory to register ClusterAlgoFactory
  static AtomicTrackAnaFactory __global_AtomicTrackAnaFactory__;

  void AtomicTrackAna::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const pca_algo_name = pset.get<std::string>("PCACandidatesName");
    _pca_algo_id = this->ID(pca_algo_name);
  }

  void AtomicTrackAna::_Process_(const larocv::Cluster2DArray_t& clusters,
				 const ::cv::Mat& img,
				 larocv::ImageMeta& meta,
				 larocv::ROI& roi)
  {
    auto const& pca_data = this->AlgoData<larocv::PCACandidatesData>(_pca_algo_id);
  }

  bool AtomicTrackAna::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  { return true; }

}

#endif
