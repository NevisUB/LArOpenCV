#ifndef __VERTEXSCAN3D_CXX__
#define __VERTEXSCAN3D_CXX__

#include "VertexScan3D.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include <set>
#include <array>


namespace larocv {

  void VertexScan3D::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    _dx = pset.get<float>("dX",2);
    _dy = pset.get<float>("dY",2);
    _dz = pset.get<float>("dZ",2);

    _step_size   = pset.get<float>("SizeStep3D",0.5);
    _step_radius = pset.get<float>("SizeStep2D",2);
    _min_radius  = pset.get<float>("MinRadius2D",4);
    _max_radius  = pset.get<float>("MaxRadius2D",10);

    _radius_v.reserve( (size_t)( (_max_radius - _min_radius) / _step_radius ) );
    for (float radius = _min_radius; radius <= _max_radius; radius += _step_radius)
      _radius_v.push_back(radius);
    
    _pi_threshold      = pset.get<float>("PIThreshold",10);
    _angle_supression  = pset.get<float>("AngleSupression",0);
    _width_supression  = pset.get<float>("WidthSupression",0);
    _pca_box_size      = pset.get<float>("PCABoxSize",2);
    _use_circle_weight = pset.get<bool> ("CircleWeight"); // true
    _prohibit_one_xs   = pset.get<bool> ("ProhibitOneXs",false); 
    _dtheta_cut        = pset.get<float>("dThetaCut"); // 10
    _merge_voxels      = pset.get<bool> ("MergeVoxels",false);
    _polar_qpoint      = pset.get<bool> ("UsePolarQPoint",true);
    _ignore_four       = pset.get<bool> ("IgnoreFour",false);
    _req_n_planes      = pset.get<int>  ("ReqNPlanes",2);
    _connect           = pset.get<bool> ("Connect",false);
    _connect_xs        = pset.get<bool> ("ConnectXs",false);

  }

  bool VertexScan3D::SetPlanePoint(cv::Mat img,
				   const data::VertexSeed3D& vtx3d,
				   const size_t plane,
				   geo2d::Vector<float>& plane_pt) {
    
    try {
      auto x = _geo.x2col(vtx3d.x, plane);
      if (x >= img.cols or x < 0) return false;

      auto y = _geo.yz2row(vtx3d.y, vtx3d.z, plane);
      if (y >= img.rows or y < 0) return false;

      uint x_0 = x + 0.5;
      uint y_0 = y + 0.5;

      uint p = (uint)img.at<uchar>(y_0,x_0);
      if(!p) return false;
      
      plane_pt.x = (float)x_0;
      plane_pt.y = (float)y_0;
    }
    catch(const larbys& err) {
      return false;
    }
    return true;
  }

  
  bool VertexScan3D::CreateCircleVertex(cv::Mat img,
					const geo2d::Vector<float>& plane_pt,
					data::CircleVertex& cvtx)
  {
    cvtx = RadialScan2D(img, plane_pt);
    if(cvtx.weight < 0) return false;
    return true;
  }

  double VertexScan3D::CircleWeight(data::CircleVertex& cvtx) {
    const cv::Mat empty_img;
    return CircleWeight(cvtx,empty_img);
  }

  double VertexScan3D::CircleWeight(data::CircleVertex& cvtx,const cv::Mat& img)
  {
    double dtheta_sum = cvtx.sum_dtheta();
    // check angular resolution
    double dtheta_sigma = 1. / cvtx.radius * 180 / M_PI ;
    dtheta_sigma = sqrt(pow(dtheta_sigma, 2) * (double) cvtx.xs_v.size());

    if (cvtx.xs_v.size() < 2) return dtheta_sigma;
    
    // special handling for 2-crossing 180 degree guy
    cvtx.dtheta_xs = kINVALID_FLOAT;
    cvtx.mid_pt2   = EstimateMidPoint(img,cvtx.center,-1);

    if (cvtx.xs_v.size() == 2) { 
      auto center_line0 = draw_line(cvtx.xs_v[0].pt,cvtx.center);
      auto center_line1 = draw_line(cvtx.xs_v[1].pt,cvtx.center);
      cvtx.dtheta_xs = make_dtheta(center_line0,center_line1);
      if (cvtx.dtheta_xs < _dtheta_cut) return -1;
      assert (img.rows>0 and img.cols>0);

      //
      // both axis mid point
      //
      center_line0 = draw_line(cvtx.xs_v[0].pt,cvtx.mid_pt2);
      center_line1 = draw_line(cvtx.xs_v[1].pt,cvtx.mid_pt2);
      cvtx.dtheta_mxs2 = make_dtheta(center_line0,center_line1);
      if (cvtx.dtheta_mxs2 < _dtheta_cut) return -1;

      //
      // point pca
      //
      cvtx.dtheta_mxs3 = make_dtheta(cvtx.xs_v.front().line,
				     cvtx.xs_v.back().line);
      if (cvtx.dtheta_mxs3 < _dtheta_cut) return -1;
	
      //
      // stability 
      //
      GEO2D_Contour_t pts_c_v;
      auto ctor_c_v = OnCircle(img,cvtx.as_circle(),pts_c_v);
      if (ctor_c_v.size() == 1) return -1;
      
      auto ass_v = Associate(pts_c_v,cvtx.xs_as_array());

      for(size_t aid0=0; aid0<ass_v.size(); ++aid0) { // group 1
	for(size_t aid1=aid0+1; aid1<ass_v.size(); ++aid1) { // group2
	  for(const auto& pt0 : ass_v[aid0]) {
	    for(const auto& pt1 : ass_v[aid1]) {
	      center_line0 = draw_line(pt0,cvtx.mid_pt2);
	      center_line1 = draw_line(pt1,cvtx.mid_pt2);
	      auto dtheta = make_dtheta(center_line0,center_line1);
	      if (dtheta < _dtheta_cut) return -1;
	    }
	  }
	}
      }

    } // end cvtx.size==2
    
    else if (cvtx.xs_v.size() == 3) { 
      auto center_line0 = draw_line(cvtx.xs_v[0].pt,cvtx.center);
      auto center_line1 = draw_line(cvtx.xs_v[1].pt,cvtx.center);
      auto center_line2 = draw_line(cvtx.xs_v[2].pt,cvtx.center);

      auto dtheta0      = make_dtheta(center_line0,center_line1);
      if (dtheta0<_dtheta_cut) return -1;
      auto dtheta1      = make_dtheta(center_line0,center_line2);
      if (dtheta1<_dtheta_cut) return -1;
      auto dtheta2      = make_dtheta(center_line1,center_line2);
      if (dtheta2<_dtheta_cut) return -1;

      //
      // both axis mid point
      //
      center_line0 = draw_line(cvtx.xs_v[0].pt,cvtx.mid_pt2);
      center_line1 = draw_line(cvtx.xs_v[1].pt,cvtx.mid_pt2);
      center_line2 = draw_line(cvtx.xs_v[2].pt,cvtx.mid_pt2);
      dtheta0      = make_dtheta(center_line0,center_line1);
      if (dtheta0<_dtheta_cut) return -1;
      dtheta1      = make_dtheta(center_line0,center_line2);
      if (dtheta1<_dtheta_cut) return -1;
      dtheta2      = make_dtheta(center_line1,center_line2);
      if (dtheta2<_dtheta_cut) return -1;


      //
      // point pca
      //
      center_line0 = cvtx.xs_v[0].line;
      center_line1 = cvtx.xs_v[1].line;
      center_line2 = cvtx.xs_v[2].line;

      dtheta0      = make_dtheta(center_line0,center_line1);
      if (dtheta0<_dtheta_cut) return -1;
      dtheta1      = make_dtheta(center_line0,center_line2);
      if (dtheta1<_dtheta_cut) return -1;
      dtheta2      = make_dtheta(center_line1,center_line2);
      if (dtheta2<_dtheta_cut) return -1;
	
      //
      // stability 
      //
      GEO2D_Contour_t pts_c_v;
      auto ctor_c_v = OnCircle(img,cvtx.as_circle(),pts_c_v);
      if (ctor_c_v.size() == 1) return -1;
      auto ass_v = Associate(pts_c_v,cvtx.xs_as_array());
      for(size_t aid0=0; aid0<ass_v.size(); ++aid0) { // group 1
	for(size_t aid1=aid0+1; aid1<ass_v.size(); ++aid1) { // group2
	  for(const auto& pt0 : ass_v[aid0]) {
	    for(const auto& pt1 : ass_v[aid1]) {
	      center_line0 = draw_line(pt0,cvtx.mid_pt2);
	      center_line1 = draw_line(pt1,cvtx.mid_pt2);
	      auto dtheta = make_dtheta(center_line0,center_line1);
	      if (dtheta < _dtheta_cut) return -1;
	    }
	  }
	}
      }

    } // end cvtx.size==3
    else {
      if (_ignore_four) return -1;
    }



    LAROCV_DEBUG() << "ret=" << dtheta_sum << std::endl;
    if (dtheta_sum > dtheta_sigma) return dtheta_sum;
    
    // if dtheta better than resolution, then compute weight differently
    std::set<double> theta_loc_s;
    geo2d::Vector<float> rel_pt;
    for (auto const& xs : cvtx.xs_v) {
      rel_pt.x = xs.pt.x - cvtx.center.x;
      rel_pt.y = xs.pt.y - cvtx.center.y;
      double theta_loc = 0;
      if (xs.pt.x == 0.) {
	if (xs.pt.y > 0) theta_loc = 90.;
	else theta_loc = 270.;
      } else {
	float numerator = std::fabs(rel_pt.x);
	float denominator = cvtx.radius;
	float arg = numerator / denominator;
	assert(denominator>0);

	if (arg > 1) {
	  //make sure we aren't making a huge error...
	  if (std::fabs(arg - 1) > 0.001) throw larbys("logic error");
	  int arg_i = std::floor(arg);
	  arg = (float) arg_i;
	}

	theta_loc = acos(arg) * 180 / M_PI;
	//Dumb isnan check;
	assert(theta_loc == theta_loc);
	
	if (rel_pt.x < 0 && rel_pt.y > 0)  theta_loc  = 180 - theta_loc;
	if (rel_pt.x < 0 && rel_pt.y <= 0) theta_loc += 180;
	if (rel_pt.x > 0 && rel_pt.y <= 0) theta_loc  = 360 - theta_loc;
      }

      theta_loc_s.insert(theta_loc);
    }

    std::vector<double> theta_loc_v;
    theta_loc_v.reserve(theta_loc_s.size());
    for (auto const& theta_loc : theta_loc_s)
      theta_loc_v.push_back(theta_loc);

    double weight = 0;
    for (size_t i = 0; (i + 1) < theta_loc_v.size(); ++i) {
      double dtheta = theta_loc_v[i + 1] - theta_loc_v[i];
      weight += std::fabs(cos(M_PI * dtheta / 180.));
    }
    weight /= (double)(theta_loc_v.size() - 1);
    weight *= dtheta_sigma;

    LAROCV_DEBUG() << "weight="<<weight<<std::endl;
    return weight;
  }


  data::CircleVertex VertexScan3D::RadialScan2D(const cv::Mat& img, const geo2d::Vector<float>& pt)
  {
    data::CircleVertex res;
    res.center = pt;

    // apply 2 degrees angular supression
    std::vector<geo2d::VectorArray<float> > temp_xs_vv;
    if (_polar_qpoint)  {
      temp_xs_vv = QPointArrayOnCircleArray(img, pt,
					    _radius_v, _pi_threshold, 
					    _angle_supression, _width_supression);
    } else {
      temp_xs_vv = OnCircleGroupsOnCircleArray(img,pt,_radius_v);
    }
    
    std::vector<std::vector<data::PointPCA> > xs_vv(temp_xs_vv.size());
    double min_weight = kINVALID_DOUBLE;
    static data::CircleVertex temp_res;
    std::vector<float> dtheta_v;

    bool first = true;
    for (size_t r_idx = 0; r_idx < _radius_v.size(); ++r_idx) {
      auto const& radius = _radius_v[r_idx];
      auto temp_xs_v = temp_xs_vv[r_idx];
      auto& xs_v = xs_vv[r_idx];
      xs_v.reserve(temp_xs_v.size());

      LAROCV_DEBUG() << "@r=" << radius << " & nxs=" << temp_xs_v.size() << std::endl;

      if (temp_xs_v.size() < 2) { 
	LAROCV_DEBUG() << "...empty xs" << std::endl;
	continue;
      }

      static bool connected = true;

      if(_connect) {
	connected = true;
	for(const auto& xs : temp_xs_v) {
	  connected &= Connected(img,pt,xs,2);
	}
	if(!connected) continue;
      }

      if(_connect_xs) {
	connected = true;
	for(size_t xsid1=0;xsid1<temp_xs_v.size();++xsid1) {
	  for(size_t xsid2=xsid1+1;xsid2<temp_xs_v.size();++xsid2) {
	    connected &= Connected(img,temp_xs_v[xsid1],temp_xs_v[xsid2],1);
	  }
	}
	if(connected) continue;
      }
      
      dtheta_v.clear();
      dtheta_v.reserve(temp_xs_v.size());

      // Estimate a local PCA and center-to-xs line's angle diff
      for (size_t xs_idx = 0; xs_idx < temp_xs_v.size(); ++xs_idx) {
	auto const& xs_pt = temp_xs_v[xs_idx];
	// Line that connects center to xs
	try {
	  auto local_pca   = SquarePCA(img, xs_pt, _pca_box_size, _pca_box_size);
	  auto center_line = draw_line(xs_pt,pt);
	  xs_v.push_back(data::PointPCA(xs_pt, local_pca));
	  dtheta_v.push_back(make_dtheta(center_line,local_pca));
	} catch (const larbys& err) {
	  continue;
	}
      }

      if (xs_v.size() < 2) { 
	LAROCV_DEBUG() << "...empty xs" << std::endl;
	continue;
      }
      
      if(!first and (temp_res.xs_v.size() < xs_v.size()) ) {
	LAROCV_DEBUG() << "...more xs pt found break" << std::endl;
	break;
      }
      
      if(r_idx) {
	if(xs_vv[r_idx-1].size() < xs_v.size()) {
	  LAROCV_DEBUG() << "...more xs pt found break" << std::endl;
	  break;
	}
      }
      
      if (dtheta_v.size() != xs_v.size()) {
	LAROCV_DEBUG() << "...no dtheta" << std::endl;
	continue;
      }

      temp_res.center   = pt;
      temp_res.radius   = radius;
      temp_res.xs_v     = xs_v;
      temp_res.dtheta_v = dtheta_v;
      temp_res.weight   = -1;

      if (_use_circle_weight) {
	temp_res.weight = CircleWeight(temp_res,img);
      } else {
	temp_res.weight = temp_res.mean_dtheta();
      }
      
      if (temp_res.weight < 0) {
	LAROCV_DEBUG() << "...bad weight" << std::endl;
	continue;
      }

      assert(temp_res.weight != kINVALID_FLOAT);

      // if this is the 1st loop, set the result
      if (first) {
	LAROCV_DEBUG() << "...set result" << std::endl;
	min_weight = temp_res.weight;
	res        = temp_res;
	first = false;
	continue;
      }
      
      // else check if this circle is more preferable than the last one
      if (temp_res.weight < min_weight) {
	min_weight = temp_res.weight;
	res        = temp_res;
      }

    } // end radius
    if (res.weight>0)
      LAROCV_DEBUG() << "return @pt="<<pt<<" res.rad=" << res.radius << " res.wei=" << res.weight<<std::endl;

    return res;
  }

  void
  VertexScan3D::RegisterRegions(const std::vector<data::Vertex3D>& vtx3d_v) {

    size_t nstep_x = (_dx * 2 / _step_size) + 1;
    size_t nstep_y = (_dy * 2 / _step_size) + 1;
    size_t nstep_z = (_dz * 2 / _step_size) + 1;

    auto& voxel_vv = _voxel_vv;
    voxel_vv.clear();
    
    for(const auto& vtx3d : vtx3d_v) {
    
      // construct possible coordinate array
      std::vector<float> x_v(nstep_x, 0.);
      std::vector<float> y_v(nstep_y, 0.);
      std::vector<float> z_v(nstep_z, 0.);
      for (size_t i = 0; i < nstep_x; ++i)
	{ x_v[i] = vtx3d.x - _dx + _step_size * i; }
      for (size_t i = 0; i < nstep_y; ++i)
	{ y_v[i] = vtx3d.y - _dy + _step_size * i; }
      for (size_t i = 0; i < nstep_z; ++i)
	{ z_v[i] = vtx3d.z - _dz + _step_size * i; }

      Voxel voxel;
      voxel.x_v = std::move(x_v);
      voxel.y_v = std::move(y_v);
      voxel.z_v = std::move(z_v);

      voxel_vv.emplace_back(std::move(voxel));
    }

    size_t nbefore = voxel_vv.size();
    LAROCV_DEBUG() << "Merging " << voxel_vv.size() << " voxels" << std::endl;
    if (_merge_voxels) MergeVoxels(voxel_vv);
    LAROCV_DEBUG() << "... into " << voxel_vv.size() << std::endl;

    size_t nafter = 0;
    for(const auto& v : voxel_vv) nafter += v.size(); 
    
    assert(nafter == nbefore);
    
  }

  void
  VertexScan3D::MergeVoxels(std::vector<VoxelArray>& voxel_vv) {
    
    bool merged = true;
    while(merged) {
      
      std::vector<VoxelArray> voxel_tmp_vv;
      voxel_tmp_vv.reserve(voxel_vv.size());

      std::vector<bool> used_v(voxel_vv.size(),false);

      merged = false;
      
      for(size_t vid1=0; vid1<voxel_vv.size(); ++vid1) {
	if (used_v[vid1]) continue;
	for(size_t vid2=vid1+1; vid2<voxel_vv.size(); ++vid2) {
	  if (used_v[vid2]) continue;

	  auto& voxel1_v = voxel_vv[vid1];
	  auto& voxel2_v = voxel_vv[vid2];

	  if(!voxel1_v.Overlap(voxel2_v)) continue;

	  auto voxel_merged_v = voxel1_v.Merge(voxel2_v);
	  voxel_tmp_vv.emplace_back(std::move(voxel_merged_v));
	    
	  used_v[vid1] = true;
	  used_v[vid2] = true;
	  merged = true;
	  break;
	}
      }	    

      for(size_t vid=0; vid<voxel_vv.size(); ++vid) {
	if (used_v[vid]) continue;
	voxel_tmp_vv.emplace_back(std::move(voxel_vv[vid]));
      }

      std::swap(voxel_vv,voxel_tmp_vv);
    }
    
  }
  
  std::vector<data::Vertex3D> 
  VertexScan3D::RegionScan3D(const std::vector<cv::Mat>& image_v)
  {
    
    // _history_vv.clear();
    LAROCV_DEBUG() << "See " << _voxel_vv.size() << " registered regions" << std::endl;
    std::vector<data::Vertex3D> vtx3d_v;
    vtx3d_v.reserve(_voxel_vv.size());
    
    for(size_t region_id=0; region_id<_voxel_vv.size();++region_id) {
      const auto& voxel_v = _voxel_vv[region_id];

      LAROCV_DEBUG() << "Region ID " << region_id << " sz " << voxel_v.size() << std::endl;

      auto vtx3d = ScanRegion(voxel_v,image_v);
      if (vtx3d.x == kINVALID_DOUBLE) continue;
      if (vtx3d.y == kINVALID_DOUBLE) continue;
      if (vtx3d.z == kINVALID_DOUBLE) continue;

      size_t num_good_plane = 0;
      int plane = -1;
      
      // Require atleast 2 crossing point on 2 planes (using ADC image)
      for(auto const& cvtx2d : vtx3d.cvtx2d_v) {
	plane += 1;
	LAROCV_DEBUG() << "Found " << cvtx2d.xs_v.size() << " xs on plane " << plane << std::endl;
        if(cvtx2d.xs_v.size()<2) continue;
	LAROCV_DEBUG() << "... accepted" << std::endl;
	num_good_plane++;
      }

      if(num_good_plane < 2) {
	LAROCV_DEBUG() << "Num good plane < 2, SKIP!!" << std::endl;
	continue;
      }
      
      // Move seed into the output
      vtx3d_v.emplace_back(std::move(vtx3d));
    } // end candidate vertex seed


    return vtx3d_v;
  }

  
  data::Vertex3D
  VertexScan3D::ScanRegion(const VoxelArray& voxel_v,
			   const std::vector<cv::Mat>& image_v,
			   size_t num_xspt) {
    data::Vertex3D res;
    res.x = kINVALID_DOUBLE;
    res.y = kINVALID_DOUBLE;
    res.z = kINVALID_DOUBLE;
    
    if (image_v.size() > _geo._num_planes) {
      LAROCV_CRITICAL() << "Provided image array length " << image_v.size()
			<< " exceeds # planes " << _geo._num_planes << std::endl;
      throw larbys();
    }
    static data::Vertex3D trial_vtx3d;
    static geo2d::Vector<float> invalid_pt(kINVALID_FLOAT,kINVALID_FLOAT);
    
    std::vector<size_t> num_xspt_count_v;
    std::array<geo2d::Vector<float>,3> plane_pt_v;
    std::array<bool,3> valid_v;
    std::array<double,3> weight_v;
    std::vector<data::CircleVertex> circle_v;    

    double best_weight = kINVALID_DOUBLE;

    LAROCV_DEBUG() << "Scanning (" << voxel_v.nx() << "," << voxel_v.ny() << "," << voxel_v.nz() << ") region sz " << voxel_v.size() << std::endl;
    for(const auto& voxel : voxel_v.as_vector()) {
      LAROCV_DEBUG() << "... @ (" << voxel.nx() << "," << voxel.ny() << "," << voxel.nz() << ")" << std::endl;
      LAROCV_DEBUG() << "("<<voxel.min_x()<<","<<voxel.min_y()<<","<<voxel.min_z()<<") => ("<<voxel.max_x()<<","<<voxel.max_y()<<","<<voxel.max_z()<<")"<<std::endl;
      for (size_t step_x = 0; step_x < voxel.nx(); ++step_x) {
	for (size_t step_y = 0; step_y < voxel.ny(); ++step_y) {
	  for (size_t step_z = 0; step_z < voxel.nz(); ++step_z) {
	    
	    trial_vtx3d.x = voxel.x_v[step_x];
	    trial_vtx3d.y = voxel.y_v[step_y];
	    trial_vtx3d.z = voxel.z_v[step_z];
	  
	    for (auto& v : plane_pt_v) v = invalid_pt;
	    for (auto& v : valid_v)    v = false;
	    short valid_ctr = 0;
	  
	    for (size_t plane = 0; plane < _geo._num_planes; ++plane) {
	      if(!SetPlanePoint(image_v[plane], trial_vtx3d, plane, plane_pt_v[plane])) continue;
	      valid_v[plane] = true;
	      valid_ctr++;
	    }
	  
	    // no valid plane point, continue
	    if (valid_ctr < _req_n_planes) continue;

	    valid_ctr = 0;
	    circle_v.clear();
	    circle_v.resize(_geo._num_planes);

	    for (auto& v : num_xspt_count_v) v = 0;
	    for (auto& w : weight_v) w=kINVALID_DOUBLE;
	    
	    bool dtheta_valid = false;
	    for (size_t plane = 0; plane < _geo._num_planes; ++plane) {
	      if (!valid_v[plane]) continue;
	      auto& cvtx = circle_v[plane];
	      const auto& plane_pt = plane_pt_v[plane];
	      LAROCV_DEBUG() << "@plane="<<plane<<" @pt="<<plane_pt<<std::endl;
	      auto valid = CreateCircleVertex(image_v[plane], plane_pt, cvtx);
	      valid_v[plane] = valid;
	      if (!valid) continue;
	      valid_ctr++;
	      auto const& xs_pts = cvtx.xs_v;
	      if (xs_pts.size() >= num_xspt_count_v.size())
		num_xspt_count_v.resize(xs_pts.size() + 1);

	      num_xspt_count_v[xs_pts.size()] += 1;
	      weight_v[plane] = cvtx.weight;
	    }
	    
	    // no valid circle vertex, continue
	    if (valid_ctr < 2) {
	      LAROCV_DEBUG() << "valid_ctr < 2" << std::endl;
	      continue;
	    }

	    // Decide which plane to use
	    if (!num_xspt) {
	      size_t num_valid_plane = 0;
	      for (size_t num_xspt_idx = 0; num_xspt_idx < num_xspt_count_v.size(); ++num_xspt_idx) {
		if (num_xspt_count_v[num_xspt_idx] < 2) continue;
		if (num_xspt_count_v[num_xspt_idx] < num_valid_plane) continue;
		num_valid_plane = num_xspt_count_v[num_xspt_idx];
		num_xspt = num_xspt_idx;
	      }
	    }

	    // If num_xspt == 0, or it's not valid, skip this point
	    if (!num_xspt || num_xspt_count_v.size() <= num_xspt || num_xspt_count_v[num_xspt] < 2) {
	      LAROCV_DEBUG() << "!num_xspt:"<<!num_xspt
			     << " num_xspt_count_v.size() <=num_xspt:" << (num_xspt_count_v.size() <= num_xspt)
			     << " num_xspt_count_v[num_xspt] < 2:" << (num_xspt_count_v[num_xspt] < 2) << std::endl;
	      continue;
	    }

	    std::sort(std::begin(weight_v), std::end(weight_v));
	    auto weight1 = weight_v[0];
	    auto weight2 = weight_v[1];
	    
	    assert (weight1 != kINVALID_DOUBLE);
	    assert (weight2 != kINVALID_DOUBLE);

	    assert (weight1 > 0);
	    assert (weight2 > 0);

	    if ((weight1 * weight2) < best_weight) {
	      best_weight  = weight1 * weight2;
	      res.x = trial_vtx3d.x;
	      res.y = trial_vtx3d.y;
	      res.z = trial_vtx3d.z;
	      res.num_planes = valid_ctr;
	      res.cvtx2d_v = circle_v;
	      res.vtx2d_v.resize(_geo._num_planes);
	      for (size_t plane = 0; plane < _geo._num_planes; ++plane) {
		if(!valid_v[plane]) continue;
		auto col = _geo.x2col(res.x, plane);
		auto row = _geo.yz2row(res.y, res.z, plane);
		res.vtx2d_v[plane].pt.x  = col;
		res.vtx2d_v[plane].pt.y  = row;
		res.vtx2d_v[plane].score = res.cvtx2d_v[plane].sum_dtheta();
	      }
	    }
	  
	  } // zstep
	} // ystep
      } // xstep
    } // end this voxel
    return res;
  }

}

#endif
