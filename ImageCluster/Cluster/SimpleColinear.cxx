#ifndef __SIMPLECOLINEAR_CXX__
#define __SIMPLECOLINEAR_CXX__

#include "SimpleColinear.h"
#include "AlgoFunction/ImagePatchAnalysis.h"
#include "AlgoFunction/Contour2DAnalysis.h"
#include "AlgoFunction/SpectrumAnalysis.h"

namespace larocv {

  /// Global larocv::SimpleColinearFactory to register AlgoFactory
  static SimpleColinearFactory __global_SimpleColinearFactory__;

  void SimpleColinear::_Configure_(const ::fcllite::PSet &pset)
  {
    LAROCV_DEBUG() << "processing" << std::endl;
  }


  void SimpleColinear::_Process_(const larocv::Cluster2DArray_t& clusters,
				const ::cv::Mat& img,
				larocv::ImageMeta& meta,
				larocv::ROI& roi)
  {
    LAROCV_DEBUG() << "processing" << std::endl;
  }
  
  bool SimpleColinear::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    LAROCV_DEBUG() << "processing" << std::endl;
    return true;
  }
}
  

#endif

// std::cout << "[";
// for(const auto& s : rmean_v) std::cout << s << ",";
// std::cout << "]" << std::endl;
// std::cout << std::endl;

// trk_id is always 0, right??
// LAROCV_DEBUG() << "This atom id:  " << atom_id << std::endl;
// LAROCV_DEBUG() << "This plane id: " << plane_id << std::endl;
// size_t particle_id = 0; // only one particle associated
// LAROCV_DEBUG() << "This particle id: " << particle_id << std::endl;
// LAROCV_DEBUG() << "This track id: " << trk_id << std::endl;
	
// auto const  pca_id = pca_data.index_atom(atom_id, particle_id, plane_id, trk_id);
// auto const& pca    = pca_data.line(pca_id);   

// LAROCV_DEBUG() << "Got line: pt: " << pca.pt << " dir: " << pca.dir << std::endl;
