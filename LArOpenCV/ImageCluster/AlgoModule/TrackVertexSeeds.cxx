#ifndef __TRACKVERTEXSEEDS_CXX__
#define __TRACKVERTEXSEEDS_CXX__

#include "TrackVertexSeeds.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {

  /// Global TrackVertexSeedsFactory to register AlgoFactory
  static TrackVertexSeedsFactory __global_TrackVertexSeedsFactory__;

  void TrackVertexSeeds::_Configure_(const Config_t &pset)
  {
    
    _ClusterHIPMIP.Configure(pset.get<Config_t>("ClusterHIPMIP"));
    _DefectBreaker.Configure(pset.get<Config_t>("DefectBreaker"));
    _PCACrossing.Configure(pset.get<Config_t>("PCACrossing"));

    for(size_t plane=0; plane<3; ++plane) {
      LAROCV_DEBUG() << "Registering VertexSeed2DArray on plane " << plane << std::endl;
      Register(new data::VertexSeed2DArray);
    }
    for(size_t plane=0; plane<3; ++plane) {
      LAROCV_DEBUG() << "Registering TrackClusterCompoundArray on plane " << plane << std::endl;
      Register(new data::TrackClusterCompoundArray);
    }

  }

  void TrackVertexSeeds::_Process_()
  {
    auto img_v = ImageArray();
    auto const& meta_v = MetaArray();

    for(size_t img_idx=0; img_idx < img_v.size(); ++img_idx) {

      auto& img = img_v[img_idx];
      auto const& meta = meta_v.at(img_idx);
      
      auto& vertex_seeds_v = AlgoData<data::VertexSeed2DArray>(meta.plane());
      auto& track_cluster_v = AlgoData<data::TrackClusterCompoundArray>(meta.plane()+3);
      
      // Cluster the HIPS and MIPS
      auto hip_mip_p = _ClusterHIPMIP.IsolateHIPMIP(img,meta.plane());
      
      LAROCV_DEBUG() << "Isolated " << hip_mip_p.first.size()
		     << " HIPs and " << hip_mip_p.second.size()
		     << " MIPS" << std::endl;
      
      // Merge the hips and the mips into a single array of contours
      auto ctor_arr_v = AddContourArrays(hip_mip_p.first,hip_mip_p.second);
      
      LAROCV_DEBUG() << "Combined: " << ctor_arr_v.size() << std::endl;
      
      std::vector<geo2d::Line<float> > line_v;
      line_v.reserve(ctor_arr_v.size()*3);
      
      // loop over ctors in this plane    
      for (const auto& ctor : ctor_arr_v) {
	LAROCV_DEBUG() << "Analyzing contour of size " << ctor.size() << std::endl;
	
	// split ctor and create a cluster compound (has defects, atomics, start/end pt)
	auto cluscomp = _DefectBreaker.BreakContour(ctor);
	
	LAROCV_DEBUG() << "I split this contour into " << cluscomp.size() << " atomics" << std::endl;
	LAROCV_DEBUG() << "Found " << cluscomp.get_defects().size() << " defects for seeds" << std::endl;
	
	// generate seeds from PCA
	for(auto & pca : _PCACrossing.ComputePCALines(cluscomp))
	  line_v.emplace_back(std::move(pca));
	
	LAROCV_DEBUG() << "Inserting " << cluscomp.get_defects().size() << " defects" << std::endl;
	for(const auto& ctor_defect : cluscomp.get_defects()) {
	  LAROCV_DEBUG() << "..." << ctor_defect._pt_defect << std::endl;
	  data::VertexSeed2D seed(ctor_defect._pt_defect);
	  seed.type=data::SeedType_t::kDefect;
	  vertex_seeds_v.emplace_back(std::move(seed));
	}
	
	track_cluster_v.emplace_back(std::move(cluscomp));
      }
      
      LAROCV_DEBUG() << "Generated " << line_v.size() << " pca lines" << std::endl;
      for ( const auto& pca_seed : _PCACrossing.ComputeIntersections(line_v,img) ) {
	data::VertexSeed2D seed(pca_seed);
	seed.type=data::SeedType_t::kPCA;
	vertex_seeds_v.emplace_back(std::move(seed));
      }
    }
    return;
  }
  

}
#endif
