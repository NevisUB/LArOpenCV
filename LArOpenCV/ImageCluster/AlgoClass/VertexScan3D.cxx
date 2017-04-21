#ifndef __VERTEXSCAN3D_H__
#define __VERTEXSCAN3D_H__

#include <set>
#include "VertexScan3D.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include <array>

namespace larocv {

  void VertexScan3D::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    LAROCV_DEBUG() << "Set verbosity of VertexScan3D @ " << (ushort)this->logger().level() << std::endl;
    //this->set_verbosity((msg::Level_t)0);
    _dx = pset.get<float>("dX");
    _dy = pset.get<float>("dY");
    _dz = pset.get<float>("dZ");

    _step_size   = pset.get<float>("SizeStep3D");
    _step_radius = pset.get<float>("SizeStep2D");
    _min_radius  = pset.get<float>("MinRadius2D");
    _max_radius  = pset.get<float>("MaxRadius2D");
    
    _pi_threshold     = pset.get<float>("PIThreshold");
    _angle_supression = pset.get<float>("AngleSupression");
    _pca_box_size     = pset.get<float>("PCABoxSize");
    
  }

  bool VertexScan3D::SetPlanePoint(cv::Mat img,
				  const data::VertexSeed3D& vtx3d,
				  const size_t plane,
				  geo2d::Vector<float>& plane_pt) const {

    try {
      auto x = _geo.x2col(vtx3d.x, plane);
      auto y = _geo.yz2row(vtx3d.y, vtx3d.z, plane);


      if (x >= img.cols or x < 0) return false;
      if (y >= img.rows or y < 0) return false;
      
      if (!img.at<uchar>(y,x)) return false;
            
      plane_pt.x = x;
      plane_pt.y = y;
    }
    catch(const larbys& err) {
      return false;
    }

    return true;
  }

  
  bool VertexScan3D::CreateCircleVertex(cv::Mat img,
					const data::VertexSeed3D& vtx3d,
					const geo2d::Vector<float>& plane_pt,
					data::CircleVertex& cvtx) const
  {
    auto res = RadialScan2D(img, plane_pt);
    if(res.weight < 0) {
      res.center = plane_pt;
      res.radius = _min_radius;
    }
    std::swap(res,cvtx);
    return true;
  }

  double VertexScan3D::CircleWeight(const larocv::data::CircleVertex& cvtx) const
  {
    double dtheta_sum = cvtx.sum_dtheta();
    // check angular resolution
    double dtheta_sigma = 1. / cvtx.radius * 180 / M_PI ;
    dtheta_sigma = sqrt(pow(dtheta_sigma, 2) * cvtx.xs_v.size());

    if (dtheta_sum > dtheta_sigma) return dtheta_sum;

    if (cvtx.xs_v.size() < 2) return dtheta_sigma;
    
    // special handling for 2-crossing 180 degree guy
    if (cvtx.xs_v.size() == 2) {
      auto center_line0 = geo2d::Line<float>(cvtx.xs_v[0].pt, cvtx.xs_v[0].pt - cvtx.center);
      auto center_line1 = geo2d::Line<float>(cvtx.xs_v[1].pt, cvtx.xs_v[1].pt - cvtx.center);
      auto dtheta = fabs(geo2d::angle(center_line0) - geo2d::angle(center_line1));
      // LAROCV_DEBUG() << "... handling dtheta @ " << dtheta << std::endl;
      //if(dtheta < dtheta_sigma) { return -1; }
      if (dtheta < 10) {
	// LAROCV_DEBUG() << "& failed" << std::endl;
	return -1;
      }
    }

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
	
	if (rel_pt.x < 0 && rel_pt.y > 0) theta_loc = 180 - theta_loc;
	if (rel_pt.x < 0 && rel_pt.y <= 0) theta_loc += 180;
	if (rel_pt.x > 0 && rel_pt.y <= 0) theta_loc = 360 - theta_loc;
      }
      LAROCV_DEBUG()<<"("<<xs.pt.x<<","<<xs.pt.y<<") = ("<<rel_pt.x<<","<<rel_pt.y<<") ... @ " << theta_loc<<std::endl;
      theta_loc_s.insert(theta_loc);
    }

    std::vector<double> theta_loc_v;
    for (auto const& theta_loc : theta_loc_s)
      theta_loc_v.push_back(theta_loc);

    double weight = 0;
    for (size_t i = 0; (i + 1) < theta_loc_v.size(); ++i) {
      double dtheta = theta_loc_v[i + 1] - theta_loc_v[i];
      weight += std::fabs(cos(M_PI * dtheta / 180.));
    }
    weight /= (double)(theta_loc_v.size() - 1);
    weight *= dtheta_sigma;
    LAROCV_DEBUG()<<cvtx.radius<<" ... "<<dtheta_sigma << " ... " << cvtx.xs_v.size() << " ... " << theta_loc_s.size() << " ... " << theta_loc_v.size() << " ... " << weight << std::endl;

    return weight;
  }


  data::CircleVertex VertexScan3D::RadialScan2D(const cv::Mat& img, const geo2d::Vector<float>& pt) const
  {
    data::CircleVertex res;
    res.center = pt;

    std::vector<float> radius_v;
    radius_v.reserve( (size_t)( (_max_radius - _min_radius) / _step_radius ) );
    for (float radius = _min_radius; radius <= _max_radius; radius += _step_radius)
      radius_v.push_back(radius);

    // apply 2 degrees angular supression
    auto const temp_xs_vv = QPointArrayOnCircleArray(img, pt, radius_v, _pi_threshold, _angle_supression);
    
    double min_weight = kINVALID_DOUBLE;
    data::CircleVertex temp_res;
    for (size_t r_idx = 0; r_idx < radius_v.size(); ++r_idx) {

      auto const& radius    = radius_v[r_idx];
      auto temp_xs_v = temp_xs_vv[r_idx];

      if (temp_xs_v.empty()) continue;

      /*
      temp_xs_v = QPointOnCircleRefine(img,geo2d::Circle<float>(pt,radius),temp_xs_v,3);
      if (temp_xs_v.empty()) throw larbys();
      */
      
      std::vector<data::PointPCA> xs_v;
      std::vector<float> dtheta_v;
      xs_v.reserve(temp_xs_v.size());
      dtheta_v.reserve(temp_xs_v.size());

      // Estimate a local PCA and center-to-xs line's angle diff
      for (size_t xs_idx = 0; xs_idx < temp_xs_v.size(); ++xs_idx) {
	auto const& xs_pt = temp_xs_v[xs_idx];
	// Line that connects center to xs
	try {
	  auto local_pca   = SquarePCA(img, xs_pt, _pca_box_size, _pca_box_size);
	  auto center_line = geo2d::Line<float>(xs_pt, xs_pt - pt);
	  xs_v.push_back(data::PointPCA(xs_pt, local_pca));
	  dtheta_v.push_back(fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)));
	} catch (const larbys& err) {
	  continue;
	}
      }
      
      if (dtheta_v.empty()) continue;
      temp_res.center   = pt;
      temp_res.radius   = radius;
      temp_res.xs_v     = xs_v;
      temp_res.dtheta_v = dtheta_v;

      temp_res.weight = CircleWeight(temp_res);
	
      LAROCV_DEBUG() << "CircleWeight: " << temp_res.weight << " @ pt " << pt << " rad " << radius << " w " << xs_v.size() << " xs " << std::endl;
      if (temp_res.weight < 0)  continue;

      // if this is the 1st loop, set the result
      if (res.xs_v.empty()) {
	min_weight = temp_res.weight;
	res = temp_res;
	continue;
      }
      
      // if this (new) radius has more crossing point, that means we started to
      // cross something else other than particle trajectory from the circle's center
      // then we break
      if (temp_res.xs_v.size() != xs_v.size()) {
	LAROCV_DEBUG() << "Breaking @ radius = " << radius << " (not included) since # crossing point increased!" << std::endl;
	break;
      }

      // else check if this circle is more preferable than the last one
      if (temp_res.weight < min_weight) {
	min_weight = temp_res.weight;
	res = temp_res;
	LAROCV_DEBUG() << "... set min weight to " << min_weight << std::endl;
      }
    }

    return res;
  }

  larocv::data::Vertex3D
  VertexScan3D::RegionScan3D(const larocv::data::VertexSeed3D& vtx3d,
			     std::vector<cv::Mat> image_v,
			     size_t num_xspt) const
  {
    larocv::data::Vertex3D res;

    if (image_v.size() > _geo._num_planes) {
      LAROCV_CRITICAL() << "Provided image array length " << image_v.size()
			<< " exceeds # planes " << _geo._num_planes << std::endl;
      throw larbys();
    }

    size_t nstep_x = (_dx * 2 / _step_size) + 1;
    size_t nstep_y = (_dy * 2 / _step_size) + 1;
    size_t nstep_z = (_dz * 2 / _step_size) + 1;

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
    LAROCV_INFO() << "Start 3D scan around vertex (x,y,z)=(" 
		  << vtx3d.x << "," << vtx3d.y << "," << vtx3d.z << ")" << std::endl
		  << "    Scan X " << vtx3d.x - _dx << " => " << vtx3d.x + _dx << " in " << nstep_x << " steps" << std::endl
		  << "    Scan Y " << vtx3d.y - _dy << " => " << vtx3d.y + _dy << " in " << nstep_y << " steps" << std::endl
		  << "    Scan Z " << vtx3d.z - _dz << " => " << vtx3d.z + _dz << " in " << nstep_z << " steps" << std::endl;
    
    static data::Vertex3D trial_vtx3d;

    std::vector<size_t> num_xspt_count_v;
    std::array<geo2d::Vector<float>,3> plane_pt_v;
    std::array<bool,3> valid_v;
    geo2d::Vector<float> invalid_pt(kINVALID_FLOAT,kINVALID_FLOAT);
    
    double best_weight = kINVALID_DOUBLE;

    for (size_t step_x = 0; step_x < nstep_x; ++step_x) {
      trial_vtx3d.x = x_v[step_x];
      for (size_t step_y = 0; step_y < nstep_y; ++step_y) {
	trial_vtx3d.y = y_v[step_y];
	for (size_t step_z = 0; step_z < nstep_z; ++step_z) {
	  trial_vtx3d.z = z_v[step_z];



	  std::vector<larocv::data::CircleVertex> circle_v;
	  circle_v.resize(_geo._num_planes);
	  
	  for (auto &v : num_xspt_count_v) v = 0;
	  for (auto &v : plane_pt_v)       v = invalid_pt;
	  for (auto &v : valid_v)          v = false;
	  short valid_ctr = 0;
	  
	  for (size_t plane = 0; plane < _geo._num_planes; ++plane) {
	    if(!SetPlanePoint(image_v.at(plane), trial_vtx3d, plane, plane_pt_v[plane])) continue;
	       valid_v[plane]=true;
	       valid_ctr++;
	  }
	  
	  if (valid_ctr < 2) continue;

	  LAROCV_DEBUG() << "("<<trial_vtx3d.x<<","<<trial_vtx3d.y<<","<<trial_vtx3d.z<<")"<<std::endl;
	  
	  for (size_t plane = 0; plane < _geo._num_planes; ++plane) {
	    if (!valid_v[plane]) continue;
	    auto& cvtx = circle_v[plane];
	    const auto& plane_pt = plane_pt_v[plane];
	    auto res = CreateCircleVertex(image_v[plane], trial_vtx3d, plane_pt,cvtx);
	    auto const& xs_pts = cvtx.xs_v;
	    if (xs_pts.size() >= num_xspt_count_v.size())
	      num_xspt_count_v.resize(xs_pts.size() + 1);
	    num_xspt_count_v[xs_pts.size()] += 1;
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
	    LAROCV_DEBUG() << "skip" << std::endl;
	    continue;
	  }
	  
	  LAROCV_DEBUG() << "Enough valid planes, calculating weight (num_xspt="<<num_xspt<<")"<<std::endl;
	  // double weight1, weight2;
	  // weight1 = weight2 = kINVALID_DOUBLE;
	  std::array<double,3> weight_v;
	  for(auto& w : weight_v) w=kINVALID_DOUBLE;
	  
	  for (size_t plane = 0; plane < _geo._num_planes; ++plane) {
	    if(!valid_v[plane]) continue;
	    auto const& circle = circle_v[plane];
	    if (circle.xs_v.size() != num_xspt) continue;
	    auto weight = CircleWeight(circle);
	    weight_v[plane] = weight;
	    // if (weight < weight1) weight1 = weight;
	    // else if (weight < weight2) weight2 = weight;
	  }
	  std::sort(std::begin(weight_v), std::end(weight_v));
	  auto weight1 = weight_v[0];
	  auto weight2 = weight_v[1];
	  
	  LAROCV_DEBUG() << "Comparing (w1,w2)=("<<weight1<<","<<weight2<<") to best " << best_weight << std::endl;
	  if ((weight1 * weight2) < best_weight) {
	    LAROCV_DEBUG() << "... accepted" << std::endl;
	    best_weight  = weight1 * weight2;
	    res.x = vtx3d.x;
	    res.y = vtx3d.y;
	    res.z = vtx3d.z;
	    res.num_planes = valid_ctr;
	    res.cvtx2d_v = circle_v;
	    res.vtx2d_v.resize(_geo._num_planes);
	    for (size_t plane = 0; plane < _geo._num_planes; ++plane) {
	      if(!valid_v[plane]) continue;
	      auto col = _geo.x2col(x_v[step_x], plane);
	      auto row = _geo.yz2row(y_v[step_y], z_v[step_z], plane);
	      res.vtx2d_v[plane].pt.x = col;
	      res.vtx2d_v[plane].pt.y = row;
	      res.vtx2d_v[plane].score = res.cvtx2d_v[plane].sum_dtheta();
	    }
	  }
	  
	} // zstep
      } // ystep
    } // xstep

    return res;
  }

}

#endif
