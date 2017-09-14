#ifndef __SECONDSHOWERANALYSIS_CXX__
#define __SECONDSHOWERANALYSIS_CXX__

#include "SecondShowerAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/VectorAnalysis.h"

namespace larocv {

  static SecondShowerAnalysisFactory __global_SecondShowerAnalysisFactory__;

  void SecondShowerAnalysis::_Configure_(const Config_t &pset)
  {
    
    _shower_frac   = pset.get<float>("ShowerFrac",0.8);
    _shower_size   = pset.get<float>("ShowerSize",40);
    _shower_impact = pset.get<float>("ShowerImpact",4); // 4cm

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
      auto ctor_v  = FindContours(tadc_img_v[plane]); 
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
      
      // don't scan on track like vertex
      if (vtx3d.type == data::VertexType_t::kTrack) {
	_tree->Fill();
	continue;
      }

      //
      //
      //

      // Mask the vertex out of the shower image
      std::vector<cv::Mat> simg_v;
      simg_v.resize(3);
      
      std::array<size_t, 3> vtx_ctor_v;
      for(auto& v : vtx_ctor_v) v = kINVALID_SIZE;

      for(size_t plane=0; plane<3; ++plane) {
	LAROCV_DEBUG() << "@plane=" << plane << std::endl;
	simg_v[plane] = tshr_img_v[plane].clone();

	const auto& ctor_v = ctor_vv[plane];
	const auto& vtx2d = vtx3d.vtx2d_v[plane];

	LAROCV_DEBUG() << "2d pt=" << vtx2d.pt << std::endl;

	if (vtx2d.pt.x == kINVALID_FLOAT) continue;

	auto id = FindContainingContour(ctor_v, vtx2d.pt);

	LAROCV_DEBUG() << "masking vertex contour @id=" << id << std::endl;
	if (id == kINVALID_SIZE) continue;
	const auto& ctor = ctor_v[id];
	vtx_ctor_v[plane] = id;
	simg_v[plane] = MaskImage(simg_v[plane],ctor,0,true);
      }

      // Determine if there is a blob of shower charge large enough & mostly shower
      std::vector<GEO2D_ContourArray_t> sctor_vv;
      sctor_vv.reserve(3);
      
      for(size_t plane=0; plane<3; ++plane) {
	auto sctor_v  = FindContours(simg_v[plane]); 
	sctor_vv.emplace_back(std::move(sctor_v));
      }
      

      std::vector<GEO2D_ContourArray_t> actor_vv;
      actor_vv.resize(3);
      
      for(size_t plane = 0; plane<3; ++plane) {

	const auto& sctor_v = sctor_vv[plane];

	auto& ctor_v   = ctor_vv[plane];
	auto& actor_v = actor_vv[plane];
	actor_v.reserve(ctor_v.size());

	for(size_t aid = 0 ; aid < ctor_v.size(); ++aid) {
	  if (aid == vtx_ctor_v[plane]) continue; // its the vertex contour
	  auto& ctor = ctor_v[aid];

	  if(ContourArea(ctor) < _shower_size)  // it's to small
	  { simg_v[plane] = MaskImage(simg_v[plane],ctor,0,true); continue; }

	  auto sid = FindContainingContour(sctor_v,ctor);
	  if (sid == kINVALID_SIZE) continue;

	  double frac = AreaRatio(sctor_v.at(sid),ctor);
	  if (frac > _shower_frac) actor_v.emplace_back(std::move(ctor));
	  else simg_v[plane] = MaskImage(simg_v[plane],ctor,0,true); // it's not shower enough
	}
      }
      

      // not enough contours, continue
      size_t cnt=0;
      for(const auto& actor_v : actor_vv) 
	{ if (!actor_v.empty()) cnt +=1; }
      
      if (cnt<2)  {
	_tree->Fill();
	continue;
      }

      // Register regions
      auto reg_vv = _PixelScan3D.RegionScan3D(simg_v,vtx3d);
      
      // associate to contours
      auto ass_vv = _PixelScan3D.AssociateContours(reg_vv,actor_vv);
      
      _reg_vv = reg_vv;
      _actor_vv = actor_vv;

      // count the number of consistent 3D points per contour ID
      std::vector<std::array<size_t,3>> trip_v;
      std::vector<std::vector<const data::Vertex3D*> > trip_vtx_ptr_vv;
      std::vector<size_t> trip_cnt_v;

      bool found = false;
      for(size_t assvid =0; assvid < ass_vv.size(); ++assvid) {
	const auto& ass_v = ass_vv[assvid];
	for(size_t assid =0; assid < ass_v.size(); ++assid) {
	  const auto& ass = ass_v[assid];
	  const auto& reg = reg_vv.at(assvid).at(assid);

	  found = false;
	  for(size_t tid=0; tid<trip_v.size(); ++tid) {
	    if(!CompareAsses(ass,trip_v[tid])) continue;
	    found = true;
	    trip_cnt_v[tid] += 1;
	    trip_vtx_ptr_vv[tid].emplace_back(&reg);
	    break;
	  }
	  if (found) continue;
	  trip_v.emplace_back(ass);
	  trip_cnt_v.emplace_back(1);
	  trip_vtx_ptr_vv.emplace_back(std::vector<const data::Vertex3D*>(1,&reg));
	}
      }

      LAROCV_DEBUG() << "trip_v sz=    " << trip_v.size() << std::endl;
      LAROCV_DEBUG() << "trip_cnt_v sz=" << trip_cnt_v.size() << std::endl;
      for(size_t tid=0; tid< trip_v.size(); ++tid) {
	LAROCV_DEBUG() << "@tid=" << tid 
		       << " {" << trip_v[tid][0] << "," << trip_v[tid][1]  << "," << trip_v[tid][2] << "} = " 
		       << trip_cnt_v[tid] << std::endl;
      }
      
      // pick the most 3D consistent contours and do PCA
      size_t maxid = std::distance(trip_cnt_v.begin(), std::max_element(trip_cnt_v.begin(), trip_cnt_v.end()));
      LAROCV_DEBUG() << "Selected max element @pos=" << maxid;
      const auto& trip           = trip_v.at(maxid);
      const auto& trip_vtx_ptr_v = trip_vtx_ptr_vv.at(maxid);
      
      std::vector<data::SpacePt> sps_v;
      sps_v.resize(trip_vtx_ptr_v.size());
      for(size_t sid=0; sid<sps_v.size(); ++sid) {
	auto& sps = sps_v[sid];
	const auto vtx = trip_vtx_ptr_v[sid];
	sps.pt = *vtx;
      }

      data::Vertex3D mean_pt;
      auto angle = larocv::data::Angle3D(sps_v,vtx3d,mean_pt);
      auto avector = larocv::AsVector(angle.first,angle.second);
      auto mean_pos = larocv::AsVector(mean_pt.x,mean_pt.y,mean_pt.z);
      auto vtx3d_vec = AsVector(vtx3d.x,vtx3d.y,vtx3d.z);

      LAROCV_DEBUG() << "GOT angle = {" << angle.first << "," << angle.second << "}" << std::endl;
      LAROCV_DEBUG() << std::endl;

      // Determine the impact parameter
      auto dist = larocv::Distance(ClosestPoint(mean_pos,Sum(mean_pos,Scale(avector,2)),vtx3d_vec),
				   vtx3d_vec);
      LAROCV_DEBUG() << "Impact parameter=" << dist << std::endl;

      _angle = angle;
      _avector = avector;
      _mean_pos = mean_pos;
      _dist = dist;

      if (dist < _shower_impact) {
	_tree->Fill();
	continue;
      }

      LAROCV_DEBUG() << "Second shower candidate idenfitied" << std::endl;

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
