#ifndef __LINEARTRACKCLUSTER_CXX__
#define __LINEARTRACKCLUSTER_CXX__

#include "Geo2D/Core/VectorArray.h"
#include "Geo2D/Core/Geo2D.h"
#include "LinearTrackCluster.h"
#include "Geo2D/Core/spoon.h"

#include "LArOpenCV/ImageCluster/AlgoData/VertexClusterData.h"
#include <map>
#include <array>
//#include <typeinfo>


namespace larocv {

  /// Global larocv::LinearTrackClusterFactory to register AlgoFactory
  static LinearTrackClusterFactory __global_LinearTrackClusterFactory__;

  void LinearTrackCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _algo.set_verbosity(this->logger().level());
    _algo.Configure(pset.get<Config_t>("SingleLinearTrack"));
    
    auto algo_name_part = pset.get<std::string>("VertexTrackClusterName");
    _algo_id_part = this->ID( algo_name_part );

    Register(new data::LinearTrackArray);
  }
  
  void LinearTrackCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
				     const ::cv::Mat& img,
				     larocv::ImageMeta& meta,
				     larocv::ROI& roi)
  { _algo.SetPlaneInfo(meta); }

  bool LinearTrackCluster::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    // Get a list of vertx already found in the image
    auto const& part_data = AlgoData<data::VertexClusterArray> ( _algo_id_part, 0 );
    std::vector<std::vector<geo2d::Vector<float> > > vtx2d_vv;
    for(auto const& interaction : part_data._vtx_cluster_v) {
      for(size_t plane=0; plane<interaction.num_planes(); ++plane) {
	vtx2d_vv.resize(plane+1);
	vtx2d_vv[plane].push_back(interaction.get_circle_vertex(plane).center);
      }
    }

    // Register to SingleLinearTrack algorithm to search for an independent (separated) single track
    for(size_t plane=0; plane<vtx2d_vv.size(); ++plane)

      _algo.RegisterVertex2D(plane,vtx2d_vv[plane]);

    // Find linear track
    auto ltrack_v = _algo.FindLinearTrack(img_v);    

    // Record
    auto& data = AlgoData<larocv::data::LinearTrackArray>(0);
    for(size_t idx=0; idx<ltrack_v.size(); ++idx)
      data.emplace_back(std::move(ltrack_v[idx]));

    return true;
  }

}
#endif
