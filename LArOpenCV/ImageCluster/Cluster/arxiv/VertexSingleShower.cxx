#ifndef __VERTEXSINGLESHOWER_CXX__
#define __VERTEXSINGLESHOWER_CXX__

#include "Core/VectorArray.h"
#include "Core/Geo2D.h"
#include "VertexSigleCluster.h"

#include "AlgoData/dQdXProfilerData.h"
#include "AlgoData/VertexClusterData.h"

//#include <typeinfo>

namespace larocv {

  /// Global larocv::VertexSigleClusterFactory to register AlgoFactory
  static VertexSigleClusterFactory __global_VertexSigleClusterFactory__;

  void VertexSigleCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    auto algo_name_dqdx  = pset.get<std::string>("dQdXProfilerName");
    auto algo_name_vertex_track = pset.get<std::string>("VertexTrackClusterName");
    auto algo_name_linear_track = pset.get<std::string>("LinearTrackClusterName");

    _algo_id_dqdx = this->ID( algo_name_dqdx );
    _algo_id_vertex_track = this->ID( algo_name_vertex_track );
    _algo_id_linear_track = this->ID( algo_name_linear_track );

    _part_pxfrac_threshold = pset.get<float>("PixelFracThreshold");

    _circle_min_separation = 3;
    _vertex_min_separation = 3;
    _circle_default_radius = 10;

    _min_compat_dist = 5;
    _xplane_tick_resolution = 3;
    _trigger_tick = 3200;
    _num_planes = 3;

    _tick_offset_v.clear();
    _tick_offset_v.resize(_num_planes);
    _tick_offset_v[0] = 0.;
    _tick_offset_v[1] = 4.54;
    _tick_offset_v[2] = 7.78;
    //_tick_offset_v[1] = 0.;
    //_tick_offset_v[2] = 0.;
    _wire_comp_factor_v.clear();
    _time_comp_factor_v.clear();
    _origin_v.clear();
    _wire_comp_factor_v.resize(_num_planes,1);
    _time_comp_factor_v.resize(_num_planes,1);
    _origin_v.resize(_num_planes);

  }
  
  void VertexSigleCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
				   const ::cv::Mat& img,
				   larocv::ImageMeta& meta,
				   larocv::ROI& roi)
  {
    _time_comp_factor_v.at(meta.plane()) = meta.pixel_height();
    _wire_comp_factor_v.at(meta.plane()) = meta.pixel_width();
    LAROCV_DEBUG() << "Plane " << meta.plane()
		   << " wire comp factor: " << _wire_comp_factor_v[meta.plane()]
		   << " time comp factor: " << _time_comp_factor_v[meta.plane()]
		   << std::endl;

    _origin_v.at(meta.plane()).x = meta.origin().x;
    _origin_v.at(meta.plane()).y = meta.origin().y;
    
    _ltrack_vertex_v.clear();
    _vtrack_vertex_v.clear();
    _vedge_vertex_v.clear();

    return;
  }

  float VertexSingleShower::y2wire(float y, const size_t plane) const
  { return (y * _wire_comp_factor_v.at(plane) + _origin_v.at(plane).y); }
  
  float VertexSingleShower::x2tick(float x, const size_t plane) const 
  { return (x * _time_comp_factor_v.at(plane) + _origin_v.at(plane).x); }

  float VertexSingleShower::wire2y(float wire, const size_t plane) const
  { return (wire - _origin_v.at(plane).y) / _wire_comp_factor_v.at(plane); }

  float VertexSingleShower::tick2x(float tick, const size_t plane) const
  { return (tick - _origin_v.at(plane).x) / _time_comp_factor_v.at(plane); }

  bool VertexSingleShower::YZPoint(const geo2d::Vector<float>& pt0, const size_t plane0,
				   const geo2d::Vector<float>& pt1, const size_t plane1,
				   larocv::data::Vertex3D& result) const
  {
    result.clear();
    
    if(std::fabs(pt0.x - pt1.x) > _xplane_tick_resolution) return false;

    auto wire0 = y2wire(pt0.y, plane0);
    auto wire1 = y2wire(pt1.y, plane1);

    auto wire1_range = larocv::OverlapWireRange(wire0, plane0, plane1);
    if(wire1 < wire1_range.first || wire1_range.second < wire1) return false;

    larocv::IntersectionPoint((size_t)(wire0+0.5), plane0, (size_t)(wire1+0.5), plane1, result.y, result.z);

    result.x = larocv::TriggerTick2Cm( (pt0.x + pt1.x) / 2. * _time_comp_factor_v[plane0] +
				       _origin_v[plane1].x - _trigger_tick);

    result.num_planes = _num_planes;

    result.vtx2d_v.resize(_num_planes);
    for(size_t plane=0; plane<_num_planes; ++plane) {
      auto& vtx2d = result.vtx2d_v[plane].pt;
      auto wire = larocv::WireCoordinate(result.y, result.z, plane);
      vtx2d.y = wire2y(wire,plane);
      vtx2d.x = (pt0.x + pt1.x)/2.;
    }
    return true;
  }

  void VertexSigleCluster::RetrieveVertex(const larocv::data::LinearTrackArray& data)
  {
    for(auto const& ltrack : data.get_clusters()) {

      if(ltrack.edge1.vtx2d_v.size())
	_ltrack_vertex_v.push_back(ltrack.edge1);
      if(ltrack.edge2.vtx2d_v.size())
	_ltrack_vertex_v.push_back(ltrack.edge2);
    }
  }

  void VertexSigleCluster::RetrieveVertex(const std::vector<cv::Mat>& img_v,
					  const larocv::data::VertexClusterArray& part,
					  const larocv::data::dQdXProfilerData& dqdx)
  {
    std::vector<bool> good_plane_v(_num_planes,false);
    std::multimap<float,larocv::data::Vertex3D> vtrack_vtx_cand_m;
    std::multimap<float,larocv::data::Vertex3D> vedge_vtx_cand_m;
    for(auto const& vtx_data : part._vtx_cluster_v) {

      for(auto& good_plane : good_plane_v) good_plane = false;
      
      size_t num_good_planes = 0;
      float  pxfrac_sum = 0;
      for(size_t plane=0; plane<_num_planes; ++plane) {
	float pxfrac = vtx_data.num_pixel_frac(plane);
	if(pxfrac < _part_pxfrac_threshold) continue;
	pxfrac_sum += pxfrac;
	good_plane_v[plane] = true;
	++num_good_planes;
      }
      if(num_good_planes < 2) continue;

      // Fill a candidate interaction vertex
      vtrack_vtx_cand_m.emplace(pxfrac_sum,vtx_data.get_vertex());
      
      auto const& partarray = dqdx.get_vertex_cluster(vtx_data.id());

      // Next fill edge-vertex
      for(size_t plane1=0; plane1<_num_planes; ++plane1) {
	if(!good_plane_v[plane1]) continue;
	for(size_t plane2=plane1+1; plane2<_num_planes; ++plane2) {
	  if(!good_plane_v[plane2]) continue;

	  std::vector<geo2d::Vector<float> > edge1_v;
	  for(auto const& part : partarray.get_cluster(plane1))
	    edge1_v.push_back(part.end_pt());
	  
	  std::vector<geo2d::Vector<float> > edge2_v;
	  for(auto const& part : partarray.get_cluster(plane2))
	    edge2_v.push_back(part.end_pt());

	  // Loop over possible combinations
	  larocv::data::Vertex3D pt;
	  for(auto const& edge1 : edge1_v) {
	    for(auto const& edge2 : edge2_v) {

	      if(!YZPoint(edge1,plane1,edge2,plane2,pt))
		continue;

	      float check_dist_min = _circle_default_size;
	      for(size_t check_plane=0; check_plane<_num_planes; ++check_plane) {
		if(check_plane == plane1) continue;
		if(check_plane == plane2) continue;
		auto const& check_pt = pt.vtx2d_v[check_plane].pt;
		auto const& mat = img_v[check_plane];
		size min_x = (size_t)(check_pt.x - _circle_default_size/2. + 0.5);
		size min_y = (size_t)(check_pt.y - _circle_default_size/2. + 0.5);
		for(size_t dx=0; dx<(size_t)(_circle_default_size+0.5); ++dx) {
		  for(size_t dy=0; dy<(size_t)(_circle_default_size+0.5); ++dy) {
		    if(mat.at<unsigned char>(min_y+dy,min_x+dx) < _pi_threshold) continue;
		    float dist = sqrt(pow((min_x+dx)-check_pt.x,2) + pow((min_y+dy)-check_pt.y,2));
		    if(dist < check_dist_min) check_dist_min = dist;
		  }
		}
	      }
	      vedge_cand_vtx_m.emplace(check-dist_min, pt);
	    }
	  }
	}
      }
    }

    _vedge_vertex_v.clear();
    for(auto const& score_vtx : vedge_cand_vtx_m) {
      // check distance to the previous ... needs to be at least required 3D separation
      auto const& cand_vtx = score_vtx.second;
      float min_dist=1e9;
      for(auto const& vedge_vtx : _vedge_vertex_v) {
	float dist = sqrt(pow(vedge_vtx.x - cand_vtx.x,2) +
			  pow(vedge_vtx.y - cand_vtx.y,2) +
			  pow(vedge_vtx.z - cand_vtx.z,2));
	if(dist < min_dist) min_dist = dist;
	if(dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) continue;
      _vedge_vertex_v.push_back(cand_vtx);
    }

    _vtrack_vertex_v.clear();
    for(auto const& score_vtx : vtrack_cand_vtx_m) {
      // check distance to the previous ... needs to be at least required 3D separation
      auto const& cand_vtx = score_vtx.second;
      float min_dist=1e9;
      for(auto const& vedge_vtx : _vtrack_vertex_v) {
	float dist = sqrt(pow(vedge_vtx.x - cand_vtx.x,2) +
			  pow(vedge_vtx.y - cand_vtx.y,2) +
			  pow(vedge_vtx.z - cand_vtx.z,2));
	if(dist < min_dist) min_dist = dist;
	if(dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) continue;
      _vtrack_vertex_v.push_back(cand_vtx);
    }

  }

  std::vector<larocv::data::Vertex3D>
  VertexSingleCluster::ListCandidateVertex(const std::vector<larocv::data::Vertex3D>& ltrack_vertex_v,
					   const std::vector<larocv::data::Vertex3D>& vtrack_vertex_v,
					   const std::vector<larocv::data::Vertex3D>& vedge_vertex_v) const
  {
    std::vector<larocv::data::Vertex3D> res_v;
    for(auto const& pt : ltrack_vertex_v) {
      float min_dist = 1e9;
      for(auto const& vtx : res_v) {
	float dist = sqrt(pow(vtx.x-pt.x,2)+pow(vtx.y-pt.y,2)+pow(vt.z-pt.z,2));
	if(dist < min_dist) min_dist = dist;
	if(dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) continue;
      res_v.push_back(pt);
    }
    for(auto const& pt : vtrack_vertex_v) {
      float min_dist = 1e9;
      for(auto const& vtx : res_v) {
	float dist = sqrt(pow(vtx.x-pt.x,2)+pow(vtx.y-pt.y,2)+pow(vt.z-pt.z,2));
	if(dist < min_dist) min_dist = dist;
	if(dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) continue;
      res_v.push_back(pt);
    }
    for(auto const& pt : vedge_vertex_v) {
      float min_dist = 1e9;
      for(auto const& vtx : res_v) {
	float dist = sqrt(pow(vtx.x-pt.x,2)+pow(vtx.y-pt.y,2)+pow(vt.z-pt.z,2));
	if(dist < min_dist) min_dist = dist;
	if(dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) continue;
      res_v.push_back(pt);
    }
    return res_v;
  }

  geo2d::VectorArray<float>
  VertexSingleShower::QPointOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle)
  {
    geo2d::VectorArray<float> res;
    // Find crossing point
    ::cv::Mat polarimg;
    ::cv::linearPolar(img, polarimg, circle.center, circle.radius*2, ::cv::WARP_FILL_OUTLIERS);
    
    size_t col = (size_t)(polarimg.cols / 2);

    std::vector<std::pair<int,int> > range_v;
    std::pair<int,int> range(-1,-1);
    for(size_t row=0; row<polarimg.rows; ++row) {

      float q = (float)(polarimg.at<unsigned char>(row, col));
      if(q < _pi_threshold) {
	if(range.first >= 0) {
	  range_v.push_back(range);
	  range.first = range.second = -1;
	}
	continue;
      }
      //std::cout << row << " / " << polarimg.rows << " ... " << q << std::endl;
      if(range.first < 0) range.first = range.second = row;

      else range.second = row;

    }
    // Check if end should be combined w/ start
    if(range_v.size() >= 2) {
      if(range_v[0].first == 0 && (range_v.back().second+1) == polarimg.rows) {
	range_v[0].first = range_v.back().first - (int)(polarimg.rows);
	range_v.pop_back();
      }
    }
    // Compute xs points
    for(auto const& r : range_v) {

      //std::cout << "XS @ " << r.first << " => " << r.second << " ... " << polarimg.rows << std::endl;
      float angle = ((float)(r.first + r.second))/2.;
      if(angle < 0) angle += (float)(polarimg.rows);
      angle = angle * M_PI * 2. / ((float)(polarimg.rows));

      geo2d::Vector<float> pt;
      pt.x = circle.center.x + circle.radius * cos(angle);
      pt.y = circle.center.y + circle.radius * sin(angle);

      res.push_back(pt);
    }
    return res;
  }

  // here
  VertexSingleShower::ShowerHypothesis(const ::cv::Mat& img,
				       const ImageMeta& meta,
				       const data::CircleVertex& vtx)
  {

    auto const& ref_vtx  = vtx.center;
    auto const& ref_xs_v = vtx.xs_v;

    std::vector< larocv::data::ParticleCluster >result_v;

    bool use_half_angle = (ref_xs_v.size() > 2) && _use_theta_half_angle;
    
    for(int xs_pt_idx=0; xs_pt_idx<ref_xs_v.size(); ++xs_pt_idx) {

      auto ref_vtx_copy = ref_vtx;

      float angle,theta_lo,theta_hi;

      cv::Mat rot_img;
      cv::Mat img_copy = img;

      cv::Mat img_padded;
      int padding = sqrt(img.rows*img.rows+img.cols*img.cols)/2;
      LAROCV_DEBUG() << "Diagonal is: " << padding << "... from row: " << img.rows << "... and cols: " << img.cols << std::endl;

      img_padded.create(img_copy.rows + 2*padding, img_copy.cols + 2*padding, img_copy.type());
      img_padded.setTo(cv::Scalar::all(0));
      img_copy.copyTo(img_padded(cv::Rect(padding, padding, img.cols, img.rows)));

      ref_vtx_copy.x+=padding;
      ref_vtx_copy.y+=padding;
      
      LAROCV_DEBUG() << "Reference vertex @ " << ref_vtx_copy << std::endl;
      
      if (use_half_angle) {
	int idx0  = xs_pt_idx-1 >= 0 ? (xs_pt_idx-1)%ref_xs_v.size() : xs_pt_idx-1+ref_xs_v.size();
	int idx1  = xs_pt_idx;
	int idx2  = (xs_pt_idx+1)%ref_xs_v.size();
	
	LAROCV_DEBUG() << "idx0 : " << idx0 << "... idx1: " << idx1 << "... idx2: " << idx2 << std::endl;

	auto xs0 = ref_xs_v[idx0].pt; xs0.x+=padding; xs0.y+=padding;
	auto xs1 = ref_xs_v[idx1].pt; xs1.x+=padding; xs1.y+=padding;
	auto xs2 = ref_xs_v[idx2].pt; xs2.x+=padding; xs2.y+=padding;
	
	LAROCV_DEBUG() << "Inspecting XS0 @ " << xs0 << " XS1 @ " << xs1 << " XS2 @ " << xs2 << std::endl;

	float angle0  = geo2d::angle(ref_vtx_copy,xs0);
	float angle1  = geo2d::angle(ref_vtx_copy,xs1);
	float angle2  = geo2d::angle(ref_vtx_copy,xs2);

	LAROCV_DEBUG() << "Angles are 0: " << angle0 << "... 1: " << angle1 << "... 2: " << angle2 << std::endl;

	if ( angle0 < 0 ) angle0+=360;
	if ( angle1 < 0 ) angle1+=360;
	if ( angle2 < 0 ) angle2+=360;
      
	float dangle10 = std::abs(angle1-angle0)/2.0;
	float dangle21 = std::abs(angle2-angle1)/2.0;
      
	angle = angle1 < 0 ? angle1+180-360. : angle1+180;

	LAROCV_DEBUG() << "Computed angle: " << angle << "... dangle10: " << dangle10 << "... dangle21: " << dangle21 << std::endl;

	theta_lo = dangle10;
	theta_hi = dangle21;
	
      } else { 
      
	auto xs = ref_xs_v[xs_pt_idx].pt; xs.x+=padding; xs.y+=padding;
	
	LAROCV_DEBUG() << "Inspecting XS @ " << xs << std::endl;
	
	angle = geo2d::angle(ref_vtx_copy,xs);
	angle += 180.;

	theta_lo = _theta_lo;
	theta_hi = _theta_hi;
      
      }
      
      auto rot = ::cv::getRotationMatrix2D(ref_vtx_copy,angle,1.);
      cv::Rect bbox = cv::RotatedRect(ref_vtx_copy,img_padded.size(),angle).boundingRect(); 
      LAROCV_DEBUG() << "bbox : " << bbox << "... size " << bbox.size() << std::endl;
      
      cv::warpAffine(img_padded, rot_img, rot, bbox.size());

      LAROCV_DEBUG() << "rot_img rows: " << rot_img.rows << "... cols: " << rot_img.cols << std::endl;

      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss1,ss2;
	ss1 << "norm_plane_"<<meta.plane()<< "_xs" << xs_pt_idx << ".png";
	ss2 << "rot_plane_"<<meta.plane() << "_xs" << xs_pt_idx << ".png";
	cv::imwrite(std::string(ss1.str()).c_str(), img_padded);
	cv::imwrite(std::string(ss2.str()).c_str(), rot_img);
      }

      cv::Mat rot_polarimg, sb_img;
      
      // Cluster per xs-point found in Refine2DVertex

      int max_radius=1000;

      ::cv::threshold(rot_img,rot_img,_pi_threshold,255,CV_THRESH_BINARY);
      
      cv::linearPolar(rot_img,        //input
		      rot_polarimg,   //output
		      ref_vtx_copy,
		      max_radius,
		      cv::WARP_FILL_OUTLIERS); //seems like it has to set

      auto kernel = ::cv::getStructuringElement(cv::MORPH_RECT,
      						cv::Size(20,2));
      ::cv::dilate(rot_polarimg,rot_polarimg,kernel,::cv::Point(-1,-1),1);     

      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss3;
	ss3 << "polar_plane_"<<meta.plane()<<"_xs" << xs_pt_idx << ".png";
	cv::imwrite(std::string(ss3.str()).c_str(), rot_polarimg);
      }

      /*
      // mask-out very-near vtx pixels
      size_t mask_col_max = _mask_radius/1000. * rot_polarimg.cols + 1;
      for(size_t row=0; row<rot_polarimg.rows; row++) {
	for(size_t col=0; col<mask_col_max; ++col) {
	  rot_polarimg.at<unsigned char>(row, col) = (unsigned char)0;
	}
      }
      */
      
      // mask-out a bit further pixels for angles outside the range
      size_t row_min, row_max;

      LAROCV_DEBUG() << "rot_polarimg... rows: " << rot_polarimg.rows << "... cols: " << rot_polarimg.cols << std::endl;
      LAROCV_DEBUG() << "theta_lo: " << theta_lo << "... theta_hi: " << theta_hi << std::endl;

      row_min = (size_t)((float)(rot_polarimg.rows) * (0.5 - theta_lo/360.)); // was 10/360.
      row_max = (size_t)((float)(rot_polarimg.rows) * (0.5 + theta_hi/360.));

      LAROCV_DEBUG() << "row min: " << row_min << "... row max: " << row_max << std::endl;      

      for(size_t row=0; row<=row_min; ++row) {
	for(size_t col=0; col<rot_polarimg.cols; col++) {
	  rot_polarimg.at<unsigned char>(row,col) = (unsigned char)0;
	}
      }
      
      for(size_t row=row_max; row<rot_polarimg.rows; ++row) {
	for(size_t col=0; col<rot_polarimg.cols; col++) {
	  rot_polarimg.at<unsigned char>(row,col) = (unsigned char)0;
	}
      }
      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss4;
	ss4 << "mask_plane_"<<meta.plane() << "_xs" << xs_pt_idx << ".png";
	cv::imwrite(std::string(ss4.str()).c_str(), rot_polarimg);     
      }
      //Contour finding
      ContourArray_t polar_ctor_v;    
      std::vector<::cv::Vec4i> cv_hierarchy_v;
      polar_ctor_v.clear(); cv_hierarchy_v.clear();


      ::cv::findContours(rot_polarimg,
			 polar_ctor_v,
			 cv_hierarchy_v,
			 CV_RETR_EXTERNAL,
			 CV_CHAIN_APPROX_SIMPLE);

      LAROCV_DEBUG() << "Found " << polar_ctor_v.size() << " contours\n";
      // Find one contour that is closest to the vtx
      float min_radius=1e4;
      int   target_idx=-1;
      for(size_t polar_ctor_idx = 0; polar_ctor_idx < polar_ctor_v.size(); ++polar_ctor_idx) {
	auto const& polar_ctor = polar_ctor_v[polar_ctor_idx];
	LAROCV_DEBUG() << "Polar contour idx : " << polar_ctor_idx << " of size " << polar_ctor.size() << std::endl;
	for(auto const& pt : polar_ctor) {
	  float angle = pt.y / (float)(rot_polarimg.rows) * 360. - 180;
	  if(angle < -5 || angle > 5) continue;
	  if(pt.x > min_radius) continue;
	  min_radius = pt.x;
	  target_idx = polar_ctor_idx;
	}
	LAROCV_DEBUG() << "min_radius : " << min_radius << std::endl;
      }
      if(target_idx < 0) {
	LAROCV_DEBUG() << "No relevant contour find for this xs point" << std::endl;
	continue;
      }
      
      auto const& polar_contour = polar_ctor_v[target_idx];
      LAROCV_DEBUG() << "Chose polar contour at index : " << target_idx << " of size " << polar_contour.size() << std::endl;
      
      float rows = rot_polarimg.rows;
      float cols = rot_polarimg.cols;
      
      //cv::Mat polar_ctor_mat = cv::Mat(img_padded.rows,img_padded.cols,CV_8UC1,cvScalar(0.));
            
      //Cluster2D res_contour;
      //res_contour._contour.resize(polar_contour.size());
      larocv::data::ParticleCluster part;
      auto& contour = part._ctor;
      contour.reserve(polar_contour.size());

      //std::stringstream ss5;
      //ss3 << "Inserting into contour : ";
      ::geo2d::Vector<int> this_pt,last_pt;
      for (size_t pt_idx=0; pt_idx<polar_contour.size(); ++pt_idx) {
      	auto const& polar_pt = polar_contour[pt_idx];
	
	//polar_ctor_mat.at<unsigned char>(polar_pt.y,polar_pt.x) = (unsigned char) 255;
	
      	//up case to floating point
      	float r = polar_pt.x;
      	float t = polar_pt.y;
	
      	r = (r / cols) * max_radius;
      	t = ((t / rows) * 360.0 + angle) * M_PI / 180.0;
	
      	this_pt.x = (size_t)((float) r * std::cos(t) + ref_vtx_copy.x + 0.5);
      	this_pt.y = (size_t)((float) r * std::sin(t) + ref_vtx_copy.y + 0.5);
	
      	//res_contour._contour[pt_idx].x = (int)(pt.x + 0.5) - padding;
      	//res_contour._contour[pt_idx].y = (int)(pt.y + 0.5) - padding;

	this_pt.x -= padding;
	this_pt.y -= padding;
	if(pt_idx == 0 || this_pt.x != last_pt.x || this_pt.y != last_pt.y)
	  contour.push_back(this_pt);
	last_pt = this_pt;
	
	//ss5<<"[" << res_contour._contour[pt_idx].x << "," << res_contour._contour[pt_idx].y << "],";
      }
      //ss5<<std::endl;
      //LAROCV_DEBUG() << ss5.str();
      // cv::Mat cart_ctor_mat  = cv::Mat(img_padded.rows,img_padded.cols,CV_8UC1,cvScalar(0.));
      // for(auto& cpt : res_contour._contour)
      // 	cart_ctor_mat.at<unsigned char>(cpt.y,cpt.x) = (unsigned char) 255;

      // std::stringstream pp0;
      // pp0 << "cart_ctor_mat_"<<meta.plane()<<".png";
      // //cv::imwrite(pp0.str().c_str(),cart_ctor_mat);

      // std::stringstream pp1;
      // pp1 << "polar_ctor_mat_"<<meta.plane()<<".png";
      // //cv::imwrite(pp1.str().c_str(),polar_ctor_mat);

      //geo2d::UntanglePolygon(contour);

      // Make sure this contour is valid (= particle is valid cluster)
      if(part._ctor.size()<=2) continue;
      
      result_v.emplace_back(std::move(part));
    }

    return result_v;
  }
  

  std::vector<larocv::data::SingleShower>
  VertexSingleCluster::ListShowerVertex(const std::vector<const cv::Mat>& img_v,
					const std::vector<larocv::data::Vertex3D>& cand_v) const
  {
    std::vector<larocv::data::Vertex3D> res_v;
    geo2d::Circle<float> circle;
    circle.radius = _circle_default_radius;
    
    for(auto const& cand_vtx : cand_v) {
      size_t num_good_plane=0;
      std::vector<std::vector<geo2d::Vector<float> > > xs_pts_v;
      xs_pts_v.resize(img_v.size());
      for(size_t plane=0; plane<img_v.size(); ++plane) {
	circle.x = cand_vtx.vtx2d_v.at(plane).pt.x;
	circle.y = cand_vtx.vtx2d_v.at(plane).pt.y;
	auto const& img = img_v[plane];
	xs_pts_v[plane] = QPointOnCircle(img,circle);
	if(xs_pts_v[plane].size()>1) {
	  num_good_plane = 0;
	  break;
	}
	if(xs_pts_v[plane].empty()) continue;
	++num_good_plane;
      }
      if(num_good_plane<2) continue;

      // Construct a single shower
      
    }
  }


  
  bool VertexSigleCluster::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {

    auto const& dqdx_data = AlgoData<data::dQdXProfilerData>     ( _algo_id_dqdx         );
    auto const& vtrack_data = AlgoData<data::VertexClusterArray> ( _algo_id_vertex_track );
    auto const& ltrack_data = AlgoData<data::LinearTrackArray>   ( _algo_id_linear_track );

    RetrieveVertex(ltrack_data);
    RetrieveVertex(vtrack_data, dqdx_data);

    // Construct a single list of vertex candidates (ltrack=>vtrack=>vedge)
    _cand_vertex_v = ListCandidateVertex(_ltrack_vertex_v, _cand_vertex_v, _vedge_vertex_v);

    // Now search for a single shower
    auto single_shower_v = ListShowerVertex(_cand_vertex_v);
    
    auto& data = AlgoData<data::SingleShowerArray>();
    
    
    // done
    return true;
  }

}
#endif
