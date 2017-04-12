#ifndef __SHOWERVERTEXSCAN_CXX__
#define __SHOWERVERTEXSCAN_CXX__

#include "ShowerVertexScan.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"

namespace larocv {

  /// Global larocv::ShowerVertexScanFactory to register AlgoFactory
  static ShowerVertexScanFactory __global_ShowerVertexScanFactory__;
  
  void ShowerVertexScan::_Configure_(const Config_t &pset)
  {

    _geo.set_verbosity(this->logger().level());
    _geo.Configure(pset.get<Config_t>("LArPlaneGeo"));
    
    _VertexScan3D.set_verbosity(this->logger().level());
    _VertexScan3D.Configure(pset.get<Config_t>("VertexScan3D"));
    
    auto name_seed = pset.get<std::string>("EdgeSeedProducer");
    _seed_id=kINVALID_ALGO_ID;
    if (!name_seed.empty()) {
      _seed_id = this->ID(name_seed);
      if (_seed_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    _require_3planes_charge=pset.get<bool>("Require3PlanesCharge");

    if(_require_3planes_charge)
      _allowed_radius=pset.get<float>("SearchRadiusSize");//5      

    Register(new data::VertexSeed3DArray);
  }

  bool ShowerVertexScan::_PostProcess_() const
  { return true; }

  void ShowerVertexScan::_Process_() {

    auto img_v  = ImageArray();
    auto meta_v = MetaArray();
    for(auto const& meta : meta_v) {
      _geo.ResetPlaneInfo(meta);
      _vtxana.ResetPlaneInfo(meta);
      _VertexScan3D.SetPlaneInfo(meta);
    }
    
    auto nplanes = img_v.size();

    std::vector<std::vector<const data::VertexSeed2D*> > seed_vv;
    seed_vv.resize(nplanes);
		   
    for(size_t plane=0;plane<nplanes;++plane) {
      const auto& vertexseed2darray = AlgoData<data::VertexSeed2DArray>(_seed_id,plane);
      const auto& vertexseed2d_v = vertexseed2darray.as_vector();
      
      auto& seed_v = seed_vv[plane];
      seed_v.reserve(vertexseed2d_v.size());
      
      for(const auto& vertexseed2d : vertexseed2d_v) {
	seed_v.push_back(&vertexseed2d);
      }
      LAROCV_DEBUG() << "Got " << seed_v.size() << " seed @ plane " << plane << std::endl;
    }

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
	    
	    if(!res) continue;

	    cand_vtx_v.emplace_back(std::move(vtx));
	    
	    LAROCV_DEBUG()<<"Plane ("<<plane0<<","<<plane1<<") seeds ("<<seed0_id<<","<<seed1_id<<") @ ["<<*seed0<<","<<*seed1<<"] " << res << std::endl;
	    for(size_t plane=0;plane<nplanes;++plane) {
	      LAROCV_DEBUG()<<"... "<< plane<<") @ "<<vtx.vtx2d_v[plane].pt<<std::endl;
	    }
	    LAROCV_DEBUG()<<"... dseed0 "<<geo2d::dist(*seed0,vtx.vtx2d_v[plane0].pt)<<" dseed1 "<<geo2d::dist(*seed1,vtx.vtx2d_v[plane1].pt)<<std::endl;

	  } //seed1
	} //seed0
	
      } //plane1
    } //plane0
    
    LAROCV_DEBUG() << "Merging nearby start @ " << cand_vtx_v.size() << std::endl;
    bool _merge_nearby = true;
    if(_merge_nearby)
      _vtxana.MergeNearby(cand_vtx_v,3);
    
    LAROCV_DEBUG() << "& end with " << cand_vtx_v.size() << std::endl;

    auto& vertex3dseedarr = AlgoData<data::VertexSeed3DArray>(0);
    
    for(auto& cand_vtx3d : cand_vtx_v) {

      try {
	_vtxana.UpdatePlanePosition(cand_vtx3d,_geo);
      }
      catch(const larbys& err) {
	LAROCV_WARNING() << "Predicted vertex has 2D point outside image" << std::endl;
	continue;
      }
      
      auto cand_vtx = data::Seed2Vertex(cand_vtx3d);
      auto vtx3d    = _VertexScan3D.RegionScan3D(cand_vtx, img_v);
      
      size_t num_good_plane = 0;
      double dtheta_sum = 0;

      for(auto const& cvtx2d : vtx3d.cvtx2d_v) {
        if(cvtx2d.xs_v.size()<2) continue;
        num_good_plane++;
        dtheta_sum += cvtx2d.sum_dtheta();
      }
      
      if(num_good_plane<2) continue;
      dtheta_sum /= (double)num_good_plane;
      LAROCV_DEBUG() << "Registering vertex seed type="<<(uint)cand_vtx3d.type
		     << " @ ("<<vtx3d.x<<","<<vtx3d.y<<","<<vtx3d.z<<")"<<std::endl;
      
      data::VertexSeed3D seed(vtx3d);
      seed.type = data::SeedType_t::kEdge;

      uint  nvalid=0;
      for(size_t plane=0;plane<3;++plane)  {
	auto pt= seed.vtx2d_v[plane];
	LAROCV_DEBUG() << plane << ") @ " << seed.vtx2d_v[plane] << std::endl;
	if(_require_3planes_charge) {
	  auto npx = CountNonZero(img_v[plane],geo2d::Circle<float>(pt,_allowed_radius));
	  if(npx) nvalid++;
	}
      }
      
      if(_require_3planes_charge && nvalid!=3) continue;
      
      vertex3dseedarr.emplace_back(std::move(seed));
      LAROCV_DEBUG() << "AlgoData size @ " << vertex3dseedarr.as_vector().size() << std::endl;
    } // end candidate vertex seed
    
  }
}
#endif
