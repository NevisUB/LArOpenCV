#ifndef __VERTEXSINGLESHOWER_CXX__
#define __VERTEXSINGLESHOWER_CXX__

#include "Core/VectorArray.h"
#include "Core/Geo2D.h"
#include "Core/Line.h"
#include "VertexSingleShower.h"
#include "Base/ImageClusterFMWKInterface.h"

#include "AlgoData/dQdXProfilerData.h"
#include "AlgoData/VertexClusterData.h"
#include "AlgoData/LinearTrackClusterData.h"

//#include <typeinfo>

namespace larocv {

  /// Global larocv::VertexSingleShowerFactory to register AlgoFactory
  static VertexSingleShowerFactory __global_VertexSingleShowerFactory__;

  void VertexSingleShower::_Configure_(const ::fcllite::PSet &pset)
  {
    auto algo_name_dqdx  = pset.get<std::string>("dQdXProfilerName");
    auto algo_name_vertex_track = pset.get<std::string>("VertexTrackClusterName");
    auto algo_name_linear_track = pset.get<std::string>("LinearTrackClusterName");

    _algo_id_dqdx = this->ID( algo_name_dqdx );
    _algo_id_vertex_track = this->ID( algo_name_vertex_track );
    _algo_id_linear_track = this->ID( algo_name_linear_track );

    _theta_hi = pset.get<int>("ThetaHi",20);
    _theta_lo = pset.get<int>("ThetaLo",20);

    _part_pxfrac_threshold = pset.get<float>("PixelFracThreshold");

    _pi_threshold = 5;
    _circle_min_separation = 3;
    _vertex_min_separation = 3;
    _circle_default_radius = 6;

    _min_compat_dist = 5;
    _xplane_tick_resolution = 3;
    _trigger_tick = 3200;
    _num_planes = 3;

    _valid_xs_npx = pset.get<size_t>("ValidXsNpx",1);

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
  
  void VertexSingleShower::_Process_(const larocv::Cluster2DArray_t& clusters,
				     const ::cv::Mat& img,
				     larocv::ImageMeta& meta,
				     larocv::ROI& roi)
  {
    _time_comp_factor_v.at(meta.plane()) = meta.pixel_height();
    _wire_comp_factor_v.at(meta.plane()) = meta.pixel_width();
    LAROCV_DEBUG() << " Plane " << meta.plane()
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

  void VertexSingleShower::RetrieveVertex(const larocv::data::LinearTrackArray& data)
  {
    for(auto const& ltrack : data.get_clusters()) {

      if(ltrack.edge1.vtx2d_v.size())
	_ltrack_vertex_v.push_back(ltrack.edge1);
      if(ltrack.edge2.vtx2d_v.size())
	_ltrack_vertex_v.push_back(ltrack.edge2);
    }
  }

  void VertexSingleShower::RetrieveVertex(const std::vector<const cv::Mat>& img_v,
					  const larocv::data::VertexClusterArray& part,
					  const larocv::data::dQdXProfilerData& dqdx)
  {
    std::vector<bool> good_plane_v(_num_planes,false);
    std::multimap<float,larocv::data::Vertex3D> vtrack_vtx_cand_m;
    std::multimap<float,larocv::data::Vertex3D> vedge_vtx_cand_m;
    for(auto const& vtx_data : part._vtx_cluster_v) {

      for(size_t plane=0; plane<good_plane_v.size(); ++plane)
	good_plane_v[plane] = false;
      
      size_t num_good_planes = 0;
      float  pxfrac_sum = 0;
      for(size_t plane=0; plane<_num_planes; ++plane) {
	float pxfrac = vtx_data.num_pixel_fraction(plane);
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

	      float check_dist_min = _circle_default_radius;
	      for(size_t check_plane=0; check_plane<_num_planes; ++check_plane) {
		if(check_plane == plane1) continue;
		if(check_plane == plane2) continue;
		auto const& check_pt = pt.vtx2d_v[check_plane].pt;
		auto const& mat = img_v[check_plane];
		size_t min_x = (size_t)(check_pt.x - _circle_default_radius/2. + 0.5);
		size_t min_y = (size_t)(check_pt.y - _circle_default_radius/2. + 0.5);
		for(size_t dx=0; dx<(size_t)(_circle_default_radius+0.5); ++dx) {
		  for(size_t dy=0; dy<(size_t)(_circle_default_radius+0.5); ++dy) {
		    if(mat.at<unsigned char>(min_y+dy,min_x+dx) < _pi_threshold) continue;
		    float dist = sqrt(pow((min_x+dx)-check_pt.x,2) + pow((min_y+dy)-check_pt.y,2));
		    if(dist < check_dist_min) check_dist_min = dist;
		  }
		}
	      }
	      vedge_vtx_cand_m.emplace(check_dist_min, pt);
	    }
	  }
	}
      }
    }

    _vedge_vertex_v.clear();
    for(auto const& score_vtx : vedge_vtx_cand_m) {
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
    for(auto const& score_vtx : vtrack_vtx_cand_m) {
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
  VertexSingleShower::ListCandidateVertex(const std::vector<larocv::data::Vertex3D>& ltrack_vertex_v,
					   const std::vector<larocv::data::Vertex3D>& vtrack_vertex_v,
					   const std::vector<larocv::data::Vertex3D>& vedge_vertex_v) const
  {
    std::vector<larocv::data::Vertex3D> res_v;
    for(auto const& pt : ltrack_vertex_v) {
      float min_dist = 1e9;
      for(auto const& vtx : res_v) {
	float dist = sqrt(pow(vtx.x-pt.x,2)+pow(vtx.y-pt.y,2)+pow(vtx.z-pt.z,2));
	if(dist < min_dist) min_dist = dist;
	if(min_dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) {
	LAROCV_INFO() << "Skipping LinearTrack vertex @ ("<<pt.x<<","<<pt.y<<","<<pt.z<<")"<<std::endl;
	continue;
      }
      res_v.push_back(pt);
    }
    for(auto const& pt : vtrack_vertex_v) {
      float min_dist = 1e9;
      for(auto const& vtx : res_v) {
	float dist = sqrt(pow(vtx.x-pt.x,2)+pow(vtx.y-pt.y,2)+pow(vtx.z-pt.z,2));
	if(dist < min_dist) min_dist = dist;
	if(min_dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) {
	LAROCV_INFO() << "Skipping VertexTrackCluster vertex @ ("<<pt.x<<","<<pt.y<<","<<pt.z<<")"<<std::endl;
	continue;
      }
      res_v.push_back(pt);
    }
    for(auto const& pt : vedge_vertex_v) {
      float min_dist = 1e9;
      for(auto const& vtx : res_v) {
	float dist = sqrt(pow(vtx.x-pt.x,2)+pow(vtx.y-pt.y,2)+pow(vtx.z-pt.z,2));
	if(dist < min_dist) min_dist = dist;
	if(min_dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) {
	LAROCV_INFO() << "Skipping ParticleEnd vertex @ ("<<pt.x<<","<<pt.y<<","<<pt.z<<")"<<std::endl;
	continue;
      }
      res_v.push_back(pt);
    }
    return res_v;
  }

  void VertexSingleShower::edge_rect(const ::cv::Mat& img, cv::Rect& rect,int w, int h) const
  {

    //make it edge aware
    if ( rect.x < 0 ) rect.x = 0;
    if ( rect.y < 0 ) rect.y = 0;
    
    if ( rect.x > img.cols - w ) rect.x = img.cols - w;
    if ( rect.y > img.rows - h ) rect.y = img.rows - h;
    
  }

  
  geo2d::Line<float> VertexSingleShower::SquarePCA(const ::cv::Mat& img,
						   geo2d::Vector<float> pt,
						   float width, float height) const
  {

    cv::Rect rect(pt.x-width, pt.y-height, 2*width+1, 2*height+1);
    edge_rect(img,rect,2*width+1,2*height+1);
    
    auto small_img = ::cv::Mat(img,rect);
    ::cv::Mat thresh_small_img;
    //::cv::threshold(small_img,thresh_small_img,_pi_threshold,1,CV_THRESH_BINARY);
    ::cv::threshold(small_img,thresh_small_img,1,1000,3);
    geo2d::VectorArray<int> points;
    findNonZero(thresh_small_img, points);

    if(points.size() < 2) {
      LAROCV_DEBUG() << "SquarePCA approx cannot be made (# points " << points.size() << " < 2)" << std::endl;
      throw larbys("SquarePCA found no point...");
    }
    
    cv::Mat mat_points(points.size(), 2, CV_32FC1);
    for (unsigned i = 0; i < mat_points.rows; ++i) {
      mat_points.at<float>(i, 0) = points[i].x;
      mat_points.at<float>(i, 1) = points[i].y;
    }

    ::cv::PCA pcaobj(mat_points,::cv::Mat(),::cv::PCA::DATA_AS_ROW,0);
    
    pt.x += pcaobj.mean.at<float>(0,0) - width;
    pt.y += pcaobj.mean.at<float>(0,1) - height;
    
    auto dir = geo2d::Vector<float>(pcaobj.eigenvectors.at<float>(0,0),
				    pcaobj.eigenvectors.at<float>(0,1));

    return geo2d::Line<float>(pt,dir);
  }

  geo2d::VectorArray<float>
  VertexSingleShower::ValidShowerPointOnCircle(const ::cv::Mat& img,
					       const geo2d::Circle<float>& circle,
					       const std::vector<geo2d::Vector<float> >& xs_pts) const
  {
    // require significant # pixels to have a charge when drawing a line from the center to the xs point
    geo2d::VectorArray<float> res_v;
    std::vector<geo2d::Vector<int> > check_pts;
    geo2d::Vector<int> pt;
    for(auto const& xs_pt : xs_pts) {
      auto const dir = geo2d::dir(circle.center,xs_pt);
      if(dir.x == 0 && dir.y ==0) throw larbys("No direction found!");
      check_pts.clear();
      // fill point coordinates to be checked
      if(dir.x==0) {
	for(size_t i=0; i<check_pts.size(); ++i) {
	  if(circle.center.y>0)
	    { pt.x = circle.center.x; pt.y = circle.center.y+i; }
	  else
	    { pt.x = circle.center.x; pt.y = circle.center.y-i; }
	  LAROCV_DEBUG() << " ... check point " << check_pts.size() << " @ " << pt << std::endl;
	  check_pts.push_back(pt);
	}
      }else if(dir.y==0) {
	for(size_t i=0; i<check_pts.size(); ++i) {
	  if(circle.center.x>0)
	    { pt.x = circle.center.x+i; pt.y = circle.center.y; }
	  else
	    { pt.x = circle.center.x-i; pt.y = circle.center.y; }
	  LAROCV_DEBUG() << " ... check point " << check_pts.size() << " @ " << pt << std::endl;
	  check_pts.push_back(pt);
	}
      }else{
	size_t dx = (size_t)(std::fabs(xs_pt.x - circle.center.x))+1;
	size_t dy = (size_t)(std::fabs(xs_pt.y - circle.center.y))+1;

	float dxd = xs_pt.x - circle.center.x < 0 ? -1 : 1;
	float dyd = xs_pt.y - circle.center.y < 0 ? -1 : 1;

	geo2d::Line<float> l(circle.center,dir);
	for(size_t i=0; i<dy; ++i) {
	  float step = i*dyd;
	  pt.y = (size_t)(circle.center.y + step + 0.5);
	  pt.x = (size_t)(l.x(circle.center.y + step)+0.5);
	  bool included=false;
	  for(auto const check_pt : check_pts) {
	    if(check_pt.x != pt.x || check_pt.y != pt.y) continue;
	    included = true;
	    break;
	  }
	  if(!included) {
	    check_pts.push_back(pt);
	    LAROCV_DEBUG() << " ... check point " << check_pts.size() << " @ " << pt << std::endl;
	  }
	}
	for(size_t i=0; i<dx; ++i) {
	  float step = i*dxd;
	  pt.y = (size_t)(l.y(circle.center.x + step)+0.5);
	  pt.x = (size_t)(circle.center.x + step + 0.5);
	  bool included=false;
	  for(auto const check_pt : check_pts) {
	    if(check_pt.x != pt.x || check_pt.y != pt.y) continue;
	    included = true;
	    break;
	  }
	  if(!included) {
	    check_pts.push_back(pt);
	    LAROCV_DEBUG() << " ... check point " << check_pts.size() << " @ " << pt << std::endl;
	  }
	}
      }

      // now check coordinates registered
      size_t valid_px_count=0;
      for(auto const& pt : check_pts) {
	if( ((float)(img.at<unsigned char>(pt.y,pt.x))) > _pi_threshold )
	  ++valid_px_count;
      }

      bool skip_xs_pt = (valid_px_count < _valid_xs_npx);

      LAROCV_INFO() << "For xs @ " << xs_pt
		    << " found " << valid_px_count << "/" << check_pts.size()
		    << " pixels above threshold ("
		    << (skip_xs_pt ? "skipping)" : "keeping)")
		    << std::endl;

      if(!skip_xs_pt)
	res_v.push_back(xs_pt);
    }
    return res_v;
  }
  

  geo2d::VectorArray<float>
  VertexSingleShower::QPointOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle) const
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
      //std::cout<<row << "," << col << " @ " << q << std::endl;

      if(q < _pi_threshold) {
	if(range.first >= 0) {
	  range_v.push_back(range);
	  range.first = range.second = -1;
	}
	continue;
      }

      if(range.first < 0) range.first = range.second = row;

      else range.second = row;

    }

    if(range.first>=0 && range.second>=0) range_v.push_back(range);

    // Check if end should be combined w/ start
    if(range_v.size()>=2) {
      if(range_v[0].first == 0 && (range_v.back().second+1) == polarimg.rows) {
	range_v[0].first = range_v.back().first - (int)(polarimg.rows);
	range_v.pop_back();
      }
    }
    // Compute xs points
    for(auto const& r : range_v) {

      //LAROCV_DEBUG() << "XS @ " << r.first << " => " << r.second << " ... " << polarimg.rows << std::endl;
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

  larocv::data::ShowerCluster
  VertexSingleShower::SingleShowerHypothesis(const ::cv::Mat& img,
					     const size_t plane,
					     const data::CircleVertex& vtx)
  {
    if(vtx.xs_v.size()!=1) {
      LAROCV_CRITICAL() << "Only 1 crossing-point CircleVertex can be provided..." << std::endl;
      throw larbys();
    }
    auto const& ref_vtx  = vtx.center;
    auto const& ref_xs_v = vtx.xs_v;

    std::vector< larocv::data::ParticleCluster >result_v;

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
    
    auto xs = ref_xs_v[0].pt; xs.x+=padding; xs.y+=padding;
    
    LAROCV_DEBUG() << "Inspecting XS @ " << xs << std::endl;
    
    angle = geo2d::angle(ref_vtx_copy,xs);
    angle += 180.;
    
    theta_lo = _theta_lo;
    theta_hi = _theta_hi;
    
    auto rot = ::cv::getRotationMatrix2D(ref_vtx_copy,angle,1.);
    cv::Rect bbox = cv::RotatedRect(ref_vtx_copy,img_padded.size(),angle).boundingRect(); 
    LAROCV_DEBUG() << "bbox : " << bbox << "... size " << bbox.size() << std::endl;
    
    cv::warpAffine(img_padded, rot_img, rot, bbox.size());
    
    LAROCV_DEBUG() << "rot_img rows: " << rot_img.rows << "... cols: " << rot_img.cols << std::endl;
    
    if(this->logger().level() == ::larocv::msg::kDEBUG) {
      std::stringstream ss1,ss2;
      ss1 << "norm_plane_"<<plane<< "_xs" << ".png";
      ss2 << "rot_plane_"<<plane << "_xs" << ".png";
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
      ss3 << "polar_plane_"<<plane<<"_xs" << ".png";
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
      ss4 << "mask_plane_"<<plane << "_xs" << ".png";
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
	//std::cout<<"pt " << pt << " ... angle " << angle << std::endl;
	if(angle < -5 || angle > 5) continue;
	if(pt.x > min_radius) continue;
	min_radius = pt.x;
	target_idx = polar_ctor_idx;
      }
      //LAROCV_DEBUG() << "min_radius : " << min_radius << std::endl;
    }

    larocv::data::ShowerCluster shower;
    if(target_idx < 0) {
      LAROCV_DEBUG() << "No relevant contour find for this xs point" << std::endl;
      return shower;
    }
    
    auto const& polar_contour = polar_ctor_v[target_idx];
    LAROCV_DEBUG() << "Chose polar contour at index : " << target_idx
		   << " of size " << polar_contour.size() << std::endl;
    
    float rows = rot_polarimg.rows;
    float cols = rot_polarimg.cols;
    
    //cv::Mat polar_ctor_mat = cv::Mat(img_padded.rows,img_padded.cols,CV_8UC1,cvScalar(0.));
    
    //Cluster2D res_contour;
    //res_contour._contour.resize(polar_contour.size());
    auto& contour = shower.ctor;
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
    // pp0 << "cart_ctor_mat_"<<plane<<".png";
    // //cv::imwrite(pp0.str().c_str(),cart_ctor_mat);
    
    // std::stringstream pp1;
    // pp1 << "polar_ctor_mat_"<<plane<<".png";
    // //cv::imwrite(pp1.str().c_str(),polar_ctor_mat);
    
    //geo2d::UntanglePolygon(contour);
    
    // Make sure this contour is valid (= particle is valid cluster)
    if(shower.ctor.size()<=2)
      shower.ctor.clear();
    else
      shower.start = vtx.center;
    return shower;
  }

  void VertexSingleShower::ListShowerVertex(const std::vector<const cv::Mat>& img_v,
					    const std::vector<larocv::data::Vertex3D>& cand_v,
					    std::vector<larocv::data::Vertex3D>& res_vtx_v,
					    std::vector<std::vector<larocv::data::CircleVertex> >& res_cvtx_v) const
  {
    res_vtx_v.clear();
    res_cvtx_v.clear();
    
    geo2d::Circle<float> circle;
    circle.radius = _circle_default_radius;

    for(size_t vtx_idx=0; vtx_idx<cand_v.size(); ++vtx_idx) {
      auto const& cand_vtx = cand_v[vtx_idx];
      LAROCV_INFO() << "Inspecting vertex " << vtx_idx
		    << " @ (" << cand_vtx.x << "," << cand_vtx.y << "," << cand_vtx.z << ")" << std::endl;
      std::vector<larocv::data::CircleVertex> cvtx_v;
      cvtx_v.resize(_num_planes);
      for(size_t plane=0; plane<img_v.size(); ++plane) {
	larocv::data::ShowerCluster scluster;
	auto& cvtx = cvtx_v[plane];
	cvtx.radius = _circle_default_radius;
	circle.center.x = cand_vtx.vtx2d_v.at(plane).pt.x;
	circle.center.y = cand_vtx.vtx2d_v.at(plane).pt.y;
	cvtx.center.x = circle.center.x;
	cvtx.center.y = circle.center.y;
	auto const& img = img_v[plane];
	auto xs_pt_v = this->QPointOnCircle(img,circle);
	LAROCV_INFO() << "Inspecting plane " << plane
		      << " ... " << xs_pt_v.size()
		      << " crossing points on circle @ " << circle.center << std::endl;
	xs_pt_v = this->ValidShowerPointOnCircle(img, circle, xs_pt_v);
	/*
	if(xs_pt_v.size()>1) {
	  LAROCV_DEBUG() << "     # crossing points = " << xs_pt_v.size() << std::endl;
	  num_good_plane = 0;
	  break;
	}
	if(xs_pt_v.empty()) {
	  LAROCV_DEBUG() << "     # crossing points = " << xs_pt_v.size() << std::endl;
	  continue;
	}
	LAROCV_DEBUG() << "     # crossing points = " << xs_pt_v.size() << std::endl;
	*/
	for(auto const& xs_pt : xs_pt_v) {
	  larocv::data::PointPCA pca_pt;
	  pca_pt.pt = xs_pt;
	  pca_pt.line = this->SquarePCA(img, pca_pt.pt, 5, 5);
	  cvtx.xs_v.push_back(pca_pt);
	}
      }

      res_vtx_v.push_back(cand_vtx);
      res_cvtx_v.push_back(cvtx_v);
    }
  }
  
  bool VertexSingleShower::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {

    auto const& dqdx_data = AlgoData<data::dQdXProfilerData>     ( _algo_id_dqdx         );
    auto const& vtrack_data = AlgoData<data::VertexClusterArray> ( _algo_id_vertex_track );
    auto const& ltrack_data = AlgoData<data::LinearTrackArray>   ( _algo_id_linear_track );

    RetrieveVertex(ltrack_data);
    LAROCV_INFO() << "# Vertex3D from LinearCluster: " << _ltrack_vertex_v.size() << std::endl;

    RetrieveVertex(img_v, vtrack_data, dqdx_data);
    LAROCV_INFO() << "# Vertex3D from VertexTrackCluster: " << _vtrack_vertex_v.size() << std::endl;
    LAROCV_INFO() << "# Vertex3D from dQdXProfiler: " << _vedge_vertex_v.size() << std::endl;

    // Construct a single list of vertex candidates (ltrack=>vtrack=>vedge)
    _cand_vertex_v = ListCandidateVertex(_ltrack_vertex_v, _vtrack_vertex_v, _vedge_vertex_v);
    LAROCV_INFO() << "# Vertex3D after combining all: " << _cand_vertex_v.size() << std::endl;
    
    // Now search for a single shower
    _shower_vtx3d_v.clear();
    _shower_vtx2d_vv.clear();
    
    ListShowerVertex(img_v,_cand_vertex_v, _shower_vtx3d_v, _shower_vtx2d_vv);
    LAROCV_INFO() << "# Vertex3D candidate for single shower: " << _shower_vtx3d_v.size() << std::endl;
    
    auto& data = AlgoData<data::SingleShowerArray>();

    for(size_t i=0; i<_shower_vtx3d_v.size(); ++i) {

      auto const& vtx3d = _shower_vtx3d_v[i];
      auto const& vtx2d_v = _shower_vtx2d_vv[i];
      
      LAROCV_INFO() << "Inspecting 3D vertex ("
		    << vtx3d.x << ","
		    << vtx3d.y << ","
		    << vtx3d.z << ")" << std::endl;
      // accept if 2 planes have 2D circle w/ only 1 crossing
      std::vector<size_t> num_xs_v(_num_planes+1,0);
      size_t num_plane_unique_xs = 0;
      for(size_t plane=0; plane<vtx2d_v.size(); ++plane) {
	auto const& vtx2d = vtx2d_v[plane];
	LAROCV_DEBUG() << "  2D vertex @ " << vtx2d.center
		       << " ... # crossing points = " << vtx2d.xs_v.size() << std::endl;
	num_xs_v[plane] = vtx2d.xs_v.size();
	if(vtx2d.xs_v.size() == 1) ++num_plane_unique_xs;
      }
      if(num_plane_unique_xs<2) {
	LAROCV_DEBUG() << "Skipping this candidate... (# of planes w/ unique-circle-xs-pt is <2)" << std::endl;
	continue;
      }

      larocv::data::SingleShower shower;
      for(size_t plane=0; plane<img_v.size(); ++plane) {
	auto const& img  = img_v[plane];
	auto const& cvtx = vtx2d_v[plane];
	if(cvtx.xs_v.size()!=1) continue;
	LAROCV_INFO() << "Clustering attempt on plane " << plane << " with circle @ " << cvtx.center << std::endl;
	shower.insert(plane,SingleShowerHypothesis(img, plane, cvtx));
      }
      size_t num_good_plane=0;
      for(auto const& scluster: shower.get_clusters()) {
	if(scluster.ctor.size()<3) continue;
	++num_good_plane;
      }
      
      if(num_good_plane<2) continue;

      shower.set_vertex(vtx3d);

      data.insert(data.num_showers(), shower);
    }

    // done
    return true;
  }

}
#endif
