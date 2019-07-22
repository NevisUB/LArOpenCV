#ifndef __TRACKVERTEXSCAN_CXX__
#define __TRACKVERTEXSCAN_CXX__

#include "TrackVertexScan.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"

namespace larocv {

  /// Global larocv::TrackVertexScanFactory to register AlgoFactory
  static TrackVertexScanFactory __global_TrackVertexScanFactory__;
  
  void TrackVertexScan::_Configure_(const Config_t &pset)
  {
    //
    // Prepare algorithms via config
    //

    // LArPlaneGeo, and image space to real space helping class
    _geo.set_verbosity(this->logger().level());
    _geo.Configure(pset.get<Config_t>("LArPlaneGeo"));

    // VertexScan3D -- 3D search for candidate vertices
    _VertexScan3D.set_verbosity(this->logger().level());
    _VertexScan3D.Configure(pset.get<Config_t>("VertexScan3D"));


    bool has_input = false;
    
    // 3D Vertex Points
    _vertex_estimate_algo_id = kINVALID_ALGO_ID;
    const auto vtx3d_algo_name = pset.get<std::string>("VertexEstimateAlgo");
    if (!vtx3d_algo_name.empty())  {

      LAROCV_INFO() << "Specified 3D vertex estimate algo name: " << vtx3d_algo_name << std::endl;
      _vertex_estimate_algo_id = this->ID(vtx3d_algo_name);

      if(_vertex_estimate_algo_id == kINVALID_ALGO_ID) 
	throw larbys("Could not find specified VertexEstimateAlgo");

      has_input = true;
    } 

    // 2D Vertex Seed Points 
    _vertex_seed_algo_id = kINVALID_ALGO_ID;
    const auto vtx_seed_algo_name = pset.get<std::string>("VertexSeedAlgo","");
    if (!vtx_seed_algo_name.empty()) {

      LAROCV_INFO() << "Specified 2D vertex seed algo name: " << vtx_seed_algo_name << std::endl;
	
      if(has_input) throw larbys("Already have input from VertexEstimateAlgo...");
      _vertex_seed_algo_id = this->ID(vtx_seed_algo_name);

      if(_vertex_seed_algo_id == kINVALID_ALGO_ID)
				throw larbys("Count not find specified VertexSeedAlgo");
      
      has_input = true;
    }
    
    if(!has_input) throw larbys("No vertex 3D or seed input given!");
    
    Register(new data::Vertex3DArray);
  }

  bool TrackVertexScan::_PostProcess_() const
  { return true; }

  void TrackVertexScan::_Process_() {

    //
    // 0) Prep vertex to be scanned from vertex estimate
    //
    auto img_v  = ImageArray();
    auto meta_v = MetaArray();

    // Inform algorithms of image information (will change per ROI processed)
    for(auto const& meta : meta_v) {
      _geo.ResetPlaneInfo(meta);
      _vtxana.ResetPlaneInfo(meta);
      _VertexScan3D.SetPlaneInfo(meta);
    }
    
    auto nplanes = img_v.size();
    
    std::vector<cv::Mat> img_thresh_v;
    img_thresh_v.reserve(3);
    for(auto& im : img_v)
      img_thresh_v.emplace_back(larocv::Threshold(im,10,255));


    
    if (_vertex_estimate_algo_id != kINVALID_ALGO_ID) {

      //
      // Use the 3D vertices from shower scan
      //
      const auto& cand_vtx_v = AlgoData<data::Vertex3DArray>(_vertex_estimate_algo_id,0).as_vector();
			std::cout << "Candidate Verts: " << cand_vtx_v.size() << std::endl;
      _VertexScan3D.RegisterRegions(cand_vtx_v);
      
    }
    else if (_vertex_seed_algo_id != kINVALID_ALGO_ID) {

      //
      // Rearrange the seeds
      //
      std::vector<std::vector<const data::VertexSeed2D*> > seed_vv;
      seed_vv.resize(nplanes);
		   
      for(size_t plane=0; plane<nplanes; ++plane) {

				// Access AlgoData vertex seeds from previous algorithm
				const auto& vertexseed2darray = AlgoData<data::VertexSeed2DArray>(_vertex_seed_algo_id,plane);
				const auto& vertexseed2d_v = vertexseed2darray.as_vector();
      
				auto& seed_v = seed_vv[plane];
				seed_v.reserve(vertexseed2d_v.size());
	
				// Insert a pointer to this seed into vector
				for(const auto& vertexseed2d : vertexseed2d_v) 
				  seed_v.push_back(&vertexseed2d);

				LAROCV_DEBUG() << "Got " << seed_v.size() << " seed @ plane " << plane << std::endl;
			}

      //
      // Compute pairwire 3D verticies with a generous time allowance
      //
    
      std::vector<data::Vertex3D> cand_vtx_v;
    
      for(size_t plane0=0;plane0<nplanes;++plane0) {
				const auto& seed0_v = seed_vv[plane0];
				for(size_t plane1=plane0+1;plane1<nplanes;++plane1) {
				  const auto& seed1_v = seed_vv[plane1];

				  for(size_t seed0_id=0;seed0_id<seed0_v.size();++seed0_id) {
				    const auto seed0 = seed0_v[seed0_id];
				    for(size_t seed1_id=0;seed1_id<seed1_v.size();++seed1_id) {
				      const auto seed1 = seed1_v[seed1_id];
	    
				      data::Vertex3D vtx;
				      auto res = _geo.YZPoint(*seed0,plane0,*seed1,plane1,vtx);

				      // could vertex be made?
				      if(!res) continue;
	
	  			    cand_vtx_v.emplace_back(std::move(vtx));
				    } //seed1
				  } //seed0
				} //plane1
      } //plane0
      
			std::cout << "We've got " << cand_vtx_v.size() << " vertices which pass the 2-planes comparison." << std::endl;
			
      _VertexScan3D.RegisterRegions(cand_vtx_v);
    }
    else
      throw larbys("Neither VertexEstimate or VertexSeed used as input to 3D scan");

    //
    // 1) Scan for a 3D vertex @ registered candidate seeds
    //
    auto vertex3d_v = _VertexScan3D.RegionScan3D(img_thresh_v);
    
    auto& vertex3darr = AlgoData<data::Vertex3DArray>(0);
    for(auto& vertex3d : vertex3d_v)
      vertex3darr.emplace_back(std::move(vertex3d));
   
		std::cout << "Verts after 3d scan: " << vertex3d_v.size() << std::endl; 
  }
}
#endif
