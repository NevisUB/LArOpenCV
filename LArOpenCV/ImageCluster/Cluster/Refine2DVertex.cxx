#ifndef __REFINE2DVERTEX_CXX__
#define __REFINE2DVERTEX_CXX__

#include "Refine2DVertex.h"
#include "DefectCluster.h"
#include "PCACandidates.h"
#include "Geo2D/Core/HalfLine.h"
#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/BoundingBox.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/SpectrumAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataParticle.h"
#include "LArOpenCV/ImageCluster/AlgoData/Refine2DVertexData.h"
#include <array>
#include <map>

namespace larocv{

  /// Global larocv::Refine2DVertexFactory to register ClusterAlgoFactory
  static Refine2DVertexFactory __global_Refine2DVertexFactory__;

  void Refine2DVertex::_Configure_(const Config_t &pset)
  {

    _algo.set_verbosity(this->logger().level());
    _algo.Configure(pset.get<Config_t>("TrackVertexScan2D"));
    
    //auto const circle_vtx_algo_name = pset.get<std::string>("CircleVertexAlgo");
    //_circle_vertex_algo_id = this->ID(circle_vtx_algo_name);
    _defect_algo_id = _pca_algo_id = kINVALID_ID;

    auto const defect_algo_name = pset.get<std::string>("DefectClusterAlgo","");
    if(!defect_algo_name.empty()) _defect_algo_id = this->ID(defect_algo_name);

    auto const pca_algo_name = pset.get<std::string>("PCACandidatesAlgo","");
    if(!pca_algo_name.empty()) _pca_algo_id = this->ID(pca_algo_name);
    
    if(_defect_algo_id == kINVALID_ID && _pca_algo_id == kINVALID_ID) {
      LAROCV_CRITICAL() << "Must provide either PCACandidates or DefectCluster algorithm ID!" << std::endl;
      throw larbys();
    }

    Register(new data::Refine2DVertexData);
  }

  void Refine2DVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
				 const ::cv::Mat& img,
				 larocv::ImageMeta& meta,
				 larocv::ROI& roi)
  {
    std::vector<geo2d::Vector<float> > points;
    if(_defect_algo_id != kINVALID_ID) {
      auto const& defect_pts = AlgoData<data::DefectClusterData>(_defect_algo_id,0);
      for(auto const& compound : defect_pts._raw_cluster_vv[meta.plane()].get_cluster()) {
	for(auto const& defect_pt : compound.get_defects())
	  points.push_back(defect_pt._pt_defect);
      }
    }

    if(_pca_algo_id != kINVALID_ID) {
      auto const& pca_pts    = AlgoData<data::PCACandidatesData>(_pca_algo_id,0);    
      for(auto const& pt : pca_pts.points(meta.plane()))
	points.push_back(pt);
    }

    _algo.AnalyzePlane(img,meta,points);
  }

  bool Refine2DVertex::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    std::vector<data::Vertex3D> vtx3d_v;
    std::vector<std::vector<data::CircleVertex> > vtx2d_vv;
    _algo.CreateTimeVertex3D(img_v,vtx3d_v,vtx2d_vv);

    auto& data = AlgoData<data::Refine2DVertexData>(0);

    for(size_t idx=0; idx<vtx3d_v.size(); ++idx)
      data.emplace_back(0,std::move(vtx3d_v[idx]),std::move(vtx2d_vv[idx]));

    vtx3d_v.clear();
    vtx2d_vv.clear();
    _algo.CreateWireVertex3D(img_v,vtx3d_v,vtx2d_vv);

    for(size_t idx=0; idx<vtx3d_v.size(); ++idx)
      data.emplace_back(0,std::move(vtx3d_v[idx]),std::move(vtx2d_vv[idx]));
    
    return true;
  }

}
#endif
