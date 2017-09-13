#ifndef __SECONDSHOWERANALYSIS_CXX__
#define __SECONDSHOWERANALYSIS_CXX__

#include "SecondShowerAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"

namespace larocv {

  static SecondShowerAnalysisFactory __global_SecondShowerAnalysisFactory__;

  void SecondShowerAnalysis::_Configure_(const Config_t &pset)
  {

    _PixelScan3D.set_verbosity(this->logger().level());
    _PixelScan3D.Configure(pset.get<Config_t>("PixelScan3D"));

    auto name_combined = pset.get<std::string>("VertexProducer");
    _combined_id=kINVALID_ALGO_ID;
    if (!name_combined.empty()) {
      _combined_id = this->ID(name_combined);
      if (_combined_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "VertexProducer ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    auto name_particle = pset.get<std::string>("ParticleProducer");
    _particle_id=kINVALID_ALGO_ID;
    if (!name_particle.empty()) {
      _particle_id = this->ID(name_particle);
      if (_particle_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "ParticleProducer ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    _tree = new TTree("SecondShowerAnalysis","");
    _tree->Branch("x" , &_x , "x/D");
    _tree->Branch("y" , &_y , "y/D");
    _tree->Branch("z" , &_z , "z/D");

    _roid = 0;
  }

  void SecondShowerAnalysis::_Process_() {
    
    LAROCV_INFO() << "start" << std::endl;

    if(NextEvent()) _roid=0;

    auto& ass_man = AssManager();
    
    // Get images
    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto shr_img_v = ImageArray(ImageSetID_t::kImageSetShower);

    auto tadc_img_v = adc_img_v;
    auto tshr_img_v = shr_img_v;

    for(auto& img : tadc_img_v)
      img = Threshold(img,10,255);

    for(auto& img : tshr_img_v)
      img = Threshold(img,10,255);

    std::vector<GEO2D_ContourArray_t> ctor_vv;
    ctor_vv.reserve(3);
    for(size_t plane=0; plane<3; ++plane) {
      auto ctor_v = FindContours(tadc_img_v[plane]); 
      ctor_vv.emplace_back(std::move(ctor_v));
    }

    const auto& meta_v = MetaArray();
    
    for(auto const& meta : meta_v)
      _PixelScan3D.SetPlaneInfo(meta);

    // Get the particle clusters from the previous module, go vertex-by-vertex
    const auto& vtx3d_arr = AlgoData<data::Vertex3DArray>(_combined_id,0);
    const auto& vtx3d_v = vtx3d_arr.as_vector();
    
    const auto& particle_arr = AlgoData<data::ParticleArray>(_particle_id,0);
    const auto& particle_v = particle_arr.as_vector();
    
    _vtxid = -1;
    
    LAROCV_DEBUG() << "Got " << vtx3d_v.size() << " vertices" << std::endl;
    for(size_t vtxid = 0; vtxid < vtx3d_v.size(); ++vtxid) {
      ClearVertex();

      const auto& vtx3d = vtx3d_v[vtxid];
      
      auto par_id_v = ass_man.GetManyAss(vtx3d,particle_arr.ID());
      if (par_id_v.empty()) continue;

      _vtxid += 1;
      
      _x = vtx3d.x;
      _y = vtx3d.y;
      _z = vtx3d.z;
      
      
      //
      //
      //

      // Mask the vertex out of the shower image
      std::vector<cv::Mat> simg_v;
      simg_v.resize(3);
      
      for(size_t plane=0; plane<3; ++plane) {
	simg_v[plane] = tshr_img_v[plane].clone();

	const auto& ctor_v = ctor_vv[plane];
	const auto& vtx2d = vtx3d.vtx2d_v[plane];
	

	auto id = FindContainingContour(ctor_v, vtx2d.pt);
	if (id == kINVALID_SIZE) continue;
	const auto& ctor = ctor_v[id];
	
	simg_v[plane] = MaskImage(simg_v[plane],ctor,0,true);
      }

      // Register regions
      auto reg_vv = _PixelScan3D.RegionScan3D(simg_v,vtx3d);

      // associate to contours
      auto ass_vv = _PixelScan3D.AssociateContours(reg_vv,ctor_vv);

      //
      //
      //
      
      
      LAROCV_DEBUG() << "Got " << par_id_v.size() << " particles" << std::endl;
      LAROCV_DEBUG() << " & algo data particle vector sz " << particle_v.size() << std::endl;

      ResetVectors(par_id_v.size());
      
      for(size_t par_idx=0; par_idx<par_id_v.size(); ++par_idx) {

	auto par_id = par_id_v[par_idx];
	const auto& par = particle_v[par_id];
	
	for(size_t plane=0; plane<3; ++plane) { }
      }
      _tree->Fill();
    } // end this vertex
    
    _roid += 1;
    LAROCV_INFO() << "end" << std::endl;
  }

  void SecondShowerAnalysis::ResetVectors(size_t sz) {
      
    return;
  }

  void SecondShowerAnalysis::ClearVertex() {
    _x = kINVALID_DOUBLE;
    _y = kINVALID_DOUBLE;
    _z = kINVALID_DOUBLE;

  }

}
#endif
