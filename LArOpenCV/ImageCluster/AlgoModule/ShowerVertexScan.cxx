#ifndef __SHOWERVERTEXSCAN_CXX__
#define __SHOWERVERTEXSCAN_CXX__

#include "ShowerVertexScan.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"

namespace larocv {

  /// Global larocv::ShowerVertexScanFactory to register AlgoFactory
  static ShowerVertexScanFactory __global_ShowerVertexScanFactory__;
  
  void ShowerVertexScan::_Configure_(const Config_t &pset)
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

    // 2D edge producer module
    auto name_seed = pset.get<std::string>("EdgeSeedProducer");
    _seed_id=kINVALID_ALGO_ID;
    if (!name_seed.empty()) {
      _seed_id = this->ID(name_seed);
      if (_seed_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    // ADC super cluster modules
    auto name_adc_super_cluster = pset.get<std::string>("ADCSuperCluster");
    _adc_super_cluster_algo = kINVALID_ALGO_ID;
    if(!name_adc_super_cluster.empty()) {
      _adc_super_cluster_algo = this->ID(name_adc_super_cluster);
      if(_adc_super_cluster_algo == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "ADC super cluster algo does not exists!" << std::endl;
	throw larbys();
      }
    }
    
    // Merge very nearby candidate vertices
    _merge_nearby = pset.get<bool>("MergeNearby",true);
    if(_merge_nearby) 
      _merge_distance = pset.get<float>("MergeDistance",3);

    // Optionally require 3 planes charge in vicinity of projected vertex location
    _require_3planes_charge=pset.get<bool>("Require3PlanesCharge");
    
    if(_require_3planes_charge)
      _allowed_radius=pset.get<float>("SearchRadiusSize");//5      

    Register(new data::Vertex3DArray);
  }

  bool ShowerVertexScan::_PostProcess_() const
  { return true; }

  void ShowerVertexScan::_Process_() {

    //
    // 0) Prep
    //
    auto img_v  = ImageArray();
    auto meta_v = MetaArray();


    std::vector<std::vector<GEO2D_Contour_t> > super_ctor_vv;
    super_ctor_vv.resize(3);
    for(size_t plane=0; plane < 3; ++plane) {
      const auto& adc_super_par_v = AlgoData<data::ParticleClusterArray>(_adc_super_cluster_algo,plane).as_vector();
      auto& super_ctor_v = super_ctor_vv[plane];
      super_ctor_v.reserve(adc_super_par_v.size());
      for(const auto& adc_super_par : adc_super_par_v)
	super_ctor_v.push_back(adc_super_par._ctor);
    }
    
    // Inform algorithms of image information (will change per ROI processed)
    for(auto const& meta : meta_v) {
      _geo.ResetPlaneInfo(meta);
      _vtxana.ResetPlaneInfo(meta);
      _VertexScan3D.SetPlaneInfo(meta);
    }
    
    auto nplanes = img_v.size();

    std::vector<std::vector<const data::VertexSeed2D*> > seed_vv;
    seed_vv.resize(nplanes);
		   
    for(size_t plane=0;plane<nplanes;++plane) {

      // Access AlgoData vertex seeds from previous algorithm
      const auto& vertexseed2darray = AlgoData<data::VertexSeed2DArray>(_seed_id,plane);
      const auto& vertexseed2d_v = vertexseed2darray.as_vector();
      
      auto& seed_v = seed_vv[plane];
      seed_v.reserve(vertexseed2d_v.size());

      // Insert a pointer to this seed into vector
      for(const auto& vertexseed2d : vertexseed2d_v) {
	seed_v.push_back(&vertexseed2d);
      }
      LAROCV_DEBUG() << "Got " << seed_v.size() << " seed @ plane " << plane << std::endl;
    }
    
    //
    // 1) Compute pairwire 3D verticies with a generous time allowance
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

    //
    // 2) Scan for a 3D vertex @ candidate seeds
    //
    std::vector<data::Vertex3D> vertex3d_v;
    std::vector<cv::Mat> img_thresh_v;
    img_thresh_v.reserve(3);
    for(auto& im : img_v)
      img_thresh_v.emplace_back(larocv::Threshold(im,10,255));

    size_t acc_vtx = 0;
    for(auto& cand_vtx3d : cand_vtx_v) {

      // For this 3D candidate seed, check how many planes have vertices still in the image after projection
      std::array<bool,3> in_image_v;
      _vtxana.UpdatePlanePosition(cand_vtx3d,_geo,in_image_v);

      ushort in_image_ctr=0;
      for (auto each : in_image_v) {
	if (each) {
	  in_image_ctr++;
	}
      }

      if (in_image_ctr < 2) continue;

      // Scan 3D region centered @ this vertex seed
      auto vtx3d  = _VertexScan3D.RegionScan3D(data::VertexSeed3D(cand_vtx3d), img_thresh_v);

      //
      // Require atleast 2 crossing point on 2 planes (using ADC image)
      //
      size_t num_good_plane = 0;
      int plane = -1;
      for(auto const& cvtx2d : vtx3d.cvtx2d_v) {
	plane += 1;
	LAROCV_DEBUG() << "Found " << cvtx2d.xs_v.size() << " xs on plane " << plane << std::endl;
        if(cvtx2d.xs_v.size() < 2) continue;
	LAROCV_DEBUG() << "... accepted" << std::endl;
        num_good_plane++;
      }
      
      if(num_good_plane < 2) {
	LAROCV_DEBUG() << "Num good plane < 2, SKIP!!" << std::endl;
	continue;
      }

      LAROCV_DEBUG() << "Registering vertex seed type="<<(uint)cand_vtx3d.type
		     << " @ ("<<vtx3d.x<<","<<vtx3d.y<<","<<vtx3d.z<<")"<<std::endl;

      //
      // Optional: require there to be some charge in vincinity of projected vertex on 3 planes
      //
      uint nvalid=0;
      if(_require_3planes_charge) {
	LAROCV_DEBUG() << "Requiring 3 planes charge in circle... " << std::endl;
	for(size_t plane=0;plane<3;++plane)  {
	  auto vtx2d= vtx3d.vtx2d_v[plane];
	  auto npx = CountNonZero(img_thresh_v[plane],geo2d::Circle<float>(vtx2d.pt,_allowed_radius));
	  LAROCV_DEBUG() << "@ (" << vtx2d.pt.x << "," << vtx2d.pt.y
			 << ") w/ rad " << _allowed_radius
			 << " see " << npx << " nonzero pixels" << std::endl;
	  if(npx) nvalid++;
	}
	if(nvalid != 3) {
	  LAROCV_DEBUG() << "... invalid, SKIP!" << std::endl;
	  continue;
	}
      }

      // Move seed into the output
      vertex3d_v.emplace_back(std::move(vtx3d));
      LAROCV_DEBUG() << "AlgoData size @ " << vertex3d_v.size() << std::endl;
    } // end candidate vertex seed
    
    auto& vertex3darr = AlgoData<data::Vertex3DArray>(0);
    for(auto& vertex3d : vertex3d_v) {
      vertex3darr.emplace_back(std::move(vertex3d));
      const auto& vtx3d = vertex3darr.as_vector().back();
	
      for(size_t plane=0; plane<3; ++plane) {
	const auto& super_ctor_v = super_ctor_vv[plane];
	const auto& cvtx = vtx3d.cvtx2d_v[plane];

	if (cvtx.weight < 0) continue;

	auto ctor_id = FindContainingContour(super_ctor_v,cvtx.center);
	if (ctor_id == kINVALID_SIZE) throw larbys("Unknown super contour but @ nonzero pixel");

	const auto& adc_par_v = AlgoData<data::ParticleClusterArray>(_adc_super_cluster_algo,plane).as_vector();
	const auto& adc_par = adc_par_v.at(ctor_id);
	AssociateOne(vtx3d,adc_par);
      }
    }
  }
}
#endif
