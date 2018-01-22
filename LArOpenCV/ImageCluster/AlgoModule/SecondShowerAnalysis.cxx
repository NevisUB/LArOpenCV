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

    _skip_track_alg = pset.get<bool>("SkipTrackAlgo",false);
    
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


    auto name_match = pset.get<std::string>("MatchAnalysisProducer");
    _match_id=kINVALID_ALGO_ID;
    if (!name_match.empty()) {
      _match_id = this->ID(name_match);
      if (_match_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "MatchAnalysisProducer ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }


    auto name_shape = pset.get<std::string>("ShapeAnalysisProducer");
    _shape_id=kINVALID_ALGO_ID;
    if (!name_shape.empty()) {
      _shape_id = this->ID(name_shape);
      if (_shape_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "ShapeAnalysisProducer ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    _tree = new TTree("SecondShowerAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid" , &_roid  , "roid/I");
    _tree->Branch("vtxid", &_vtxid , "vtxid/I");
    _tree->Branch("x"    , &_x     , "x/D");
    _tree->Branch("y"    , &_y     , "y/D");
    _tree->Branch("z"    , &_z     , "z/D");
    
    _tree->Branch("secondshower"  , &_secondshower  , "secondshower/I");
    _tree->Branch("shr_rad_pts"   , &_shr_rad_pts   , "shr_rad_pts/I");

    _tree->Branch("shr_theta", &_shr_theta , "shr_theta/F");
    _tree->Branch("shr_phi"  , &_shr_phi   , "shr_phi/F");

    _tree->Branch("shr_mean_x"  , &_shr_mean_x , "shr_mean_x/F");
    _tree->Branch("shr_mean_y"  , &_shr_mean_y , "shr_mean_y/F");
    _tree->Branch("shr_mean_z"  , &_shr_mean_z , "shr_mean_z/F");

    _tree->Branch("shr_min_dist"  , &_shr_min_dist  , "shr_min_dist/F");
    _tree->Branch("shr_imp_dist"  , &_shr_imp_dist  , "shr_imp_dist/F");

    _tree->Branch("shr_shr_angle"   , &_shr_shr_angle   , "shr_shr_angle/F");
    _tree->Branch("shr_start_angle" , &_shr_start_angle , "shr_start_angle/F");


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
    ctor_vv.resize(3);

    for(size_t plane=0; plane<3; ++plane) {
      auto ctor_v  = FindContours(tadc_img_v.at(plane)); 
      // GEO2D_ContourArray_t tmp_ctor_v;
      // tmp_ctor_v.reserve(ctor_v.size());
      // for(auto& ctor : ctor_v)  {
      // 	if (ctor.size()<3) continue;
      // 	tmp_ctor_v.emplace_back(std::move(ctor));
      // }
      //ctor_vv[plane] = std::move(tmp_ctor_v);
      ctor_vv[plane] = std::move(ctor_v);
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
    

    std::array<size_t, 3> vtx_ctor_v;
    std::array<bool,3> valid_plane_v;
    std::array<cv::Mat,3> simg_v;
    std::array<GEO2D_ContourArray_t,3> sctor_vv;
    std::array<GEO2D_ContourArray_t,3> actor_vv;

    LAROCV_DEBUG() << "Got " << vtx3d_v.size() << " vertices" << std::endl;
    for(size_t vtxid = 0; vtxid < vtx3d_v.size(); ++vtxid) {
      LAROCV_DEBUG() << "@vtxid=" << vtxid << std::endl;
      ClearVertex();

      const auto& vtx3d = vtx3d_v[vtxid];
      
      auto par_id_v = ass_man.GetManyAss(vtx3d,particle_arr.ID());
      if (par_id_v.empty()) continue;

      _vtxid += 1;
      
      _x = vtx3d.x;
      _y = vtx3d.y;
      _z = vtx3d.z;
      
      LAROCV_DEBUG() << "@vtx3d=(" << _x << "," << _y << "," << _z << ")" << std::endl;

      // Scan on track like vertices?
      if (vtx3d.type == data::VertexType_t::kTrack && _skip_track_alg) {
	_tree->Fill();
	continue;
      }

      //
      // Mask the vertex and associated super contour
      // out of the shower image
      //
      for(auto& v : simg_v)        v.release();
      for(auto& v : vtx_ctor_v)    v = kINVALID_SIZE;
      for(auto& v : valid_plane_v) v = false;

      for(size_t plane=0; plane<3; ++plane) {
	LAROCV_DEBUG() << "@plane=" << plane << std::endl;
	simg_v[plane] = tshr_img_v.at(plane).clone();

	const auto& ctor_v = ctor_vv.at(plane);
	const auto& vtx2d = vtx3d.vtx2d_v.at(plane);

	LAROCV_DEBUG() << "2d pt=" << vtx2d.pt << std::endl;

	if (vtx2d.pt.x < 0) continue;
	if (vtx2d.pt.y < 0) continue;

	auto id = FindContainingContour(ctor_v, vtx2d.pt);

	valid_plane_v[plane] = true;

	LAROCV_DEBUG() << "masking vertex contour @id=" << id << std::endl;
	if (id == kINVALID_SIZE) continue;

	const auto& ctor = ctor_v.at(id);
	vtx_ctor_v[plane] = id;
	simg_v[plane] = MaskImage(simg_v[plane],ctor,0,true);
      }
      
      //
      // Mask the particle contours
      //
      for(size_t par_idx=0; par_idx<par_id_v.size(); ++par_idx) {
	auto par_id = par_id_v[par_idx];
	const auto& par = particle_v[par_id];
	for(size_t plane=0; plane<3; ++plane) {
	  const auto& pcluster = par._par_v.at(plane);
	  const auto& pctor = pcluster._ctor;
	  simg_v[plane] = MaskImage(simg_v[plane],pctor,0,true);
	}
      }

      for(auto& v : sctor_vv) v.clear();

      for(size_t plane=0; plane<3; ++plane) {
	auto sctor_v  = FindContours(simg_v[plane]); 
	// GEO2D_ContourArray_t tmp_sctor_v;
	// tmp_sctor_v.reserve(sctor_v.size());
	// for(auto& ctor : sctor_v) {
	//   if (ctor.size()<3) continue;
	//   tmp_sctor_v.emplace_back(std::move(ctor));
	// }
	//sctor_vv[plane] = std::move(tmp_sctor_v);
	sctor_vv[plane] = std::move(sctor_v);
      }
      
      for(auto& v : actor_vv) v.clear();
      
      for(size_t plane = 0; plane<3; ++plane) {

	if(!valid_plane_v[plane]) continue;

	const auto& sctor_v = sctor_vv[plane];

	const auto& ctor_v   = ctor_vv[plane];
	auto& actor_v = actor_vv[plane];
	actor_v.reserve(ctor_v.size());

	for(size_t aid = 0 ; aid < ctor_v.size(); ++aid) {
	  if (aid == vtx_ctor_v[plane]) continue; // its the vertex contour
	  const auto& ctor = ctor_v[aid];
	  
	  if(ContourArea(ctor) < _shower_size)
	  { simg_v[plane] = MaskImage(simg_v[plane],ctor,0,true); continue; }

	  auto sid = FindContainingContour(sctor_v,ctor);
	  if (sid == kINVALID_SIZE) continue;

	  double frac = AreaRatio(sctor_v.at(sid),ctor);
	  if (frac > _shower_frac) actor_v.emplace_back(ctor);
	  else simg_v[plane] = MaskImage(simg_v[plane],ctor,0,true); // it's not shower enough
	}
      }
      
      //
      // not enough contours, continue
      //
      size_t cnt=0;
      for(const auto& actor_v : actor_vv) 
	{ if (!actor_v.empty()) cnt +=1; }
      
      if (cnt<2)  {
	_tree->Fill();
	continue;
      }

      //
      // Register regions for the scan (spheres)
      //
      auto reg_vv = _PixelScan3D.RegionScan3D(simg_v,vtx3d);
      
      //
      // associate to contours to valid 3D points on sphere
      //
      auto ass_vv = _PixelScan3D.AssociateContours(reg_vv,actor_vv);
      
      //
      // count the number of consistent 3D points per contour ID
       //
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
      
      if (trip_v.empty()) {
	_tree->Fill();
	continue;
      }
      
      //
      // pick the most 3D consistent contours and do PCA
      //
      size_t maxid = std::distance(trip_cnt_v.begin(), std::max_element(trip_cnt_v.begin(), trip_cnt_v.end()));
      LAROCV_DEBUG() << "Selected max element @pos=" << maxid << std::endl;
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
      _shr_mean_x = mean_pos[0];
      _shr_mean_y = mean_pos[1];
      _shr_mean_z = mean_pos[2];
      auto vtx3d_vec = AsVector(vtx3d.x,vtx3d.y,vtx3d.z);

      LAROCV_DEBUG() << "GOT angle = {" << angle.first << "," << angle.second << "}" << std::endl;
      LAROCV_DEBUG() << std::endl;

      //
      // Determine the impact parameter
      //
      auto imp_dist = larocv::Distance(ClosestPoint(mean_pos,Sum(mean_pos,Scale(avector,2)),vtx3d_vec),
				       vtx3d_vec);
      LAROCV_DEBUG() << "Impact parameter=" << imp_dist << std::endl;
      
      // bool _debug = false;
      // if(_debug) {
      // 	_reg_vv = reg_vv;
      // 	_actor_vv = actor_vv;
      // 	_angle = angle;
      // 	_avector = avector;
      // 	_mean_pos = mean_pos;
      // 	_dist = imp_dist;
      // }

      LAROCV_DEBUG() << "Second shower candidate idenfitied" << std::endl;

      //
      // Get the radial point closest to the vertex in 3D
      //
      double min_dist = kINVALID_DOUBLE;
      const data::SpacePt* min_sp = nullptr;
      for(const auto& sp : sps_v) {
	auto d = Distance(sp,vtx3d);
	if (d<min_dist) 
	  {min_dist = d; min_sp = &sp;}
      }
      
      _secondshower = 1;
      _shr_rad_pts = (int)sps_v.size();

      _shr_theta = angle.first;
      _shr_phi = angle.second;

      _shr_half_dist = (float)larocv::data::Distance(vtx3d,mean_pt);
      _shr_min_dist = min_dist;
      _shr_imp_dist = imp_dist;


      LAROCV_DEBUG() << "Got " << par_id_v.size() << " particles" << std::endl;
      LAROCV_DEBUG() << " & algo data particle vector sz " << particle_v.size() << std::endl;

      //ResetVectors(par_id_v.size());
      
      const auto& info2d_data = AlgoData<data::Info2DArray>(_shape_id,0);
      const auto& info2d_v = info2d_data.as_vector();

      const auto& info3d_data = AlgoData<data::Info3DArray>(_match_id,0);
      const auto& info3d_v = info3d_data.as_vector();

      for(size_t par_idx=0; par_idx<par_id_v.size(); ++par_idx) {

	auto par_id = par_id_v[par_idx];
	const auto& par = particle_v[par_id];
	
	const auto info2d_id = ass_man.GetOneAss(par,info2d_data.ID());
	const auto& info2d = info2d_data.as_vector().at(info2d_id);

	if (info2d.ptype != larocv::data::ParticleType_t::kShower) continue;

	LAROCV_DEBUG() << "shower par @id=" << par_idx << std::endl;
	const auto info3d_id = ass_man.GetOneAss(par,info3d_data.ID());
	const auto& info3d = info3d_data.as_vector().at(info2d_id);
	
	const auto& trunk_pca_dir = info3d.trunk_pca_dir;

	auto dot = larocv::Dot(trunk_pca_dir,avector) / Norm(trunk_pca_dir) / Norm(avector);
	_shr_shr_angle = std::acos(dot);

	auto min_vec = Diff(AsVector(min_sp->pt.x,min_sp->pt.y,min_sp->pt.z),vtx3d_vec);

	dot = larocv::Dot(min_vec,avector) / Norm(min_vec) / Norm(avector);
	_shr_start_angle = std::acos(dot);

	break;
      }
      
      LAROCV_DEBUG() << "...fill" << std::endl;
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
    
    _secondshower = 0;
    _shr_rad_pts = kINVALID_INT;

    _shr_theta = kINVALID_FLOAT;
    _shr_phi = kINVALID_FLOAT;

    _shr_half_dist = kINVALID_FLOAT;
    _shr_min_dist = kINVALID_FLOAT;
    _shr_imp_dist = kINVALID_FLOAT;
    _shr_shr_angle = kINVALID_FLOAT;
    _shr_start_angle = kINVALID_FLOAT;

    _shr_mean_x = kINVALID_FLOAT;
    _shr_mean_y = kINVALID_FLOAT;
    _shr_mean_z = kINVALID_FLOAT;
  }

}
#endif
