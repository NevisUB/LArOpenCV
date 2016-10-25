#ifndef __ATOMICTRACKANA_CXX__
#define __ATOMICTRACKANA_CXX__

#include "ProjectionCluster.h"
#include "PCACandidatesData.h"
#include "PCACandidatesData.h"
#include "Refine2DVertex.h"
#include "AtomicTrackAna.h"
#include "VertexTrackCluster.h"

namespace larocv{

  /// Global larocv::ProjectionClusterFactory to register ClusterAlgoFactory
  static ProjectionClusterFactory __global_ProjectionClusterFactory__;

  void ProjectionCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const vtx_algo_name = pset.get<std::string>("Refine2DVertexName");
    _vtx_algo_id = this->ID(vtx_algo_name);

    auto const cluster_algo_name = pset.get<std::string>("VertexTrackClusterName");
    _cluster_algo_id = this->ID(cluster_algo_name);

    auto const atom_ana_name = pset.get<std::string>("AtomicTrackAnaName");
    _atom_ana_id = this->ID(atom_ana_name);
    
    auto const pca_algo_name = pset.get<std::string>("PCACandidatesName");
    _pca_algo_id = this->ID(pca_algo_name);
  }

  void ProjectionCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
				 const ::cv::Mat& img,
				 larocv::ImageMeta& meta,
				 larocv::ROI& roi)
  {
    auto const& vtx_data     = this->AlgoData<larocv::Refine2DVertexData>     ( _vtx_algo_id     );
    auto const& cluster_data = this->AlgoData<larocv::VertexTrackClusterData> ( _cluster_algo_id );
    auto const& atom_data    = this->AlgoData<larocv::AtomicTrackAnaData>     ( _atom_ana_id     );
    auto const& pca_data     = this->AlgoData<larocv::PCACandidatesData>      ( _pca_algo_id     );
  }

  void ProjectionCluster::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {}

}

#endif
