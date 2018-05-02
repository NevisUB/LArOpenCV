#ifndef __PIXELSCAN3D_CXX__
#define __PIXELSCAN3D_CXX__

#include "PixelScan3D.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

#define PI 3.1415926
#include <unordered_map>

namespace larocv {

  void PixelScan3D::Configure(const Config_t &pset) { Configure(); }

  void PixelScan3D::Configure()
  {
    //this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));

    float _radius_min  = 5;
    float _radius_max  = 30;
    float _radius_step = 1;

    _theta_base = 10;
    _phi_base = 20;

    _radius_v.resize((size_t) _radius_max - (size_t) _radius_min);
    
    for(float r=0; r<_radius_v.size(); r+=_radius_step)  {
      
      _radius_v[r] = r + _radius_min;

      _theta_v.resize(r*_theta_base,kINVALID_FLOAT);
      _phi_v.resize(_phi_base  ,kINVALID_FLOAT);
      
      for(size_t tid=0; tid<_theta_v.size(); ++tid) 
	_theta_v[tid] = (float) tid * PI / ( (float) _theta_v.size() );

      _theta_v.push_back(PI);
      
      for(size_t pid=0; pid<_phi_v.size(); ++pid) 
	_phi_v[pid] = (float) pid * 2 * PI / ( (float) _phi_v.size() );

    }
    return;
  }

  void PixelScan3D::Reconfigure(float radius_min,
				float radius_max,
				float radius_step,
				float theta_min,
				float theta_max,
				float phi_min,
				float phi_max) {
    
    _radius_v.clear();
    _theta_vv.clear();
    _phi_vv.clear();

    size_t sz_est = (size_t) ((radius_max - radius_min)/ radius_step);
    _radius_v.reserve(sz_est);
    
    for(float rad = radius_min; rad < radius_max; rad+=radius_step) {
      
      _radius_v.push_back(rad);

      _theta_vv.resize(_radius_v.size());
      _phi_vv.resize(_radius_v.size());

      auto& theta_v = _theta_vv.back();
      auto& phi_v   = _phi_vv.back();
      
      float theta_step = 1.0 / (4*rad);
      float phi_step   = 1.0 / (2*rad);
      
      for(float theta = theta_min; theta < theta_max; theta += theta_step)
	theta_v.push_back(theta);
      
      for(float phi = phi_min; phi < phi_max; phi += phi_step)
	phi_v.push_back(phi);

    }
    
  }
  
  std::vector<std::vector<data::Vertex3D> > 
  PixelScan3D::RegisterRegions(const data::Vertex3D& vtx) const{
    LAROCV_DEBUG() << "start" << std::endl;
    std::vector<std::vector<data::Vertex3D> > pts_vv;
    if (_radius_v.empty()) throw larbys("No radii specified");

    pts_vv.resize(_radius_v.size());

    std::vector<data::Vertex3D> pts_v;
    pts_v.reserve(_theta_v.size() * _phi_v.size());

    for(size_t rid = 0; rid < _radius_v.size(); ++rid) {
      
      auto radius = _radius_v[rid];
          
      for(auto theta : _theta_v) {
	for(auto phi :  _phi_v) {
	  data::Vertex3D res;
	  res.x = vtx.x + X(radius,theta,phi);
	  res.y = vtx.y + Y(radius,theta,phi);
	  res.z = vtx.z + Z(radius,theta);
	  res.vtx2d_v.resize(3);
	  pts_v.emplace_back(std::move(res));
	}
      }
      pts_vv[rid] = std::move(pts_v);
    }
    LAROCV_DEBUG() << "end" << std::endl;
    return pts_vv;
  }

  std::vector<std::vector<data::Vertex3D> > 
  PixelScan3D::RegionScan3D(const std::array<cv::Mat,3>& image_v, 
			    const data::Vertex3D& vtx3d) const {

    auto res_vv = RegisterRegions(vtx3d);

    std::vector<std::vector<data::Vertex3D> > ret_vv;
    ret_vv.resize(res_vv.size());
    
    geo2d::Vector<float> plane_pt(kINVALID_FLOAT,kINVALID_FLOAT);
    const auto inv_plane_pt = plane_pt;

    size_t nvalid = kINVALID_SIZE;

    if (_radius_v.empty()) throw larbys("No radii specified");

    for(size_t rid = 0; rid < _radius_v.size(); ++rid) {

      auto& shell_v = res_vv[rid];
      auto& ret_v   = ret_vv[rid];
      ret_v.reserve(shell_v.size());      

      for(size_t sid = 0; sid < shell_v.size(); ++ sid) {
	
	auto& shell_pt = shell_v[sid];
	
	nvalid = 0;
	for(size_t plane=0; plane<3; ++plane) {
	  auto valid = SetPlanePoint(image_v[plane],shell_pt,plane,plane_pt);
	  if (!valid)
	    { shell_pt.vtx2d_v[plane].pt = inv_plane_pt; continue;}

	  nvalid++;
	  shell_pt.vtx2d_v[plane].pt = plane_pt;
	}
	if (nvalid<2) continue;

	ret_v.emplace_back(std::move(shell_pt));
      } // end this shell pt
    } // end this radius

    return ret_vv;
  }


  std::vector<std::vector<std::array<size_t,3> > > 
  PixelScan3D::AssociateContours(const std::vector<std::vector<data::Vertex3D> >& reg_vv,
				 const std::array<GEO2D_ContourArray_t,3>& ctor_vv) {

    LAROCV_DEBUG() << "start" << std::endl;
    std::vector<std::vector<std::array<size_t,3> > > ass_vv;
    ass_vv.resize(reg_vv.size());
    
    // For each vertex point associate a set of up to 3 contour IDs

    for(size_t rid = 0 ; rid<reg_vv.size(); ++rid) {

      const auto& reg_v = reg_vv[rid];
      auto& ass_v = ass_vv[rid];
      ass_v.resize(reg_v.size());

      for(size_t regid = 0; regid < reg_v.size(); ++ regid) {
	const auto& reg = reg_v[regid];
	auto& ass = ass_v[regid];

	for(size_t plane=0; plane<3; ++plane) {
	  const auto& ctor_v = ctor_vv[plane];
	  const auto& pt = reg.vtx2d_v[plane].pt;
	  if (pt.x==kINVALID_FLOAT) {
	    ass[plane] = kINVALID_SIZE;
	  }
	  ass[plane] = FindContainingContour(ctor_v,pt);
	} // end plane
      } // end this scan
    } // end this radii
    LAROCV_DEBUG() << "start" << std::endl;
    return ass_vv;
  }


  ///////////////////////////////////////////////////////////////
  //
  //

  std::vector<data::Vertex3D>
  PixelScan3D::RegisterSpheres(const data::Vertex3D& vtx) const {
    LAROCV_DEBUG() << "start" << std::endl;

    std::vector<data::Vertex3D> pts_v;
    if (_radius_v.empty()) throw larbys("No radii specified");

    size_t sz_estimate = _theta_vv.back().size() * _phi_vv.back().size();
    sz_estimate *= _radius_v.back();
    pts_v.reserve(sz_estimate);

    for(size_t rid = 0; rid < _radius_v.size(); ++rid) {

      auto radius = _radius_v[rid];
      for(auto theta : _theta_vv[rid]) {
	for(auto phi :  _phi_vv[rid]) {
	  data::Vertex3D res;
	  res.x = vtx.x + X(radius,theta,phi);
	  res.y = vtx.y + Y(radius,theta,phi);
	  res.z = vtx.z + Z(radius,theta);
	  res.vtx2d_v.resize(3);
	  pts_v.emplace_back(std::move(res));
	}
      }
      
    }

    LAROCV_DEBUG() << "end" << std::endl;
    return pts_v;
  }

  std::vector<data::Vertex3D>
  PixelScan3D::SphereScan3D(const std::array<cv::Mat,3>& image_v, 
			    const std::vector<cv::Mat*>& dead_v,
			    const data::Vertex3D& vtx3d) const {


    auto res_v = RegisterSpheres(vtx3d);

    std::vector<data::Vertex3D> ret_v;
    ret_v.reserve(res_v.size());
    
    geo2d::Vector<float> plane_pt(kINVALID_FLOAT,kINVALID_FLOAT);
    const auto inv_plane_pt = plane_pt;
    
    if (_radius_v.empty()) 
      throw larbys("No radii specified");
    
    int ndead  = 0;
    int nvalid = 0;
    int nempty = 0;
    
    for(auto& shell_pt : res_v) { 
      
      ndead  = 0;
      nvalid = 0;
      nempty = 0;
      
      for(size_t plane=0; plane<3; ++plane) {
	auto valid = SetPlanePoint(image_v[plane],shell_pt,plane,plane_pt,*(dead_v[plane]));
	
	if (valid == 0) {
	  shell_pt.vtx2d_v[plane].pt = inv_plane_pt; 
	  nempty++;
	  continue;
	}
	
	shell_pt.vtx2d_v[plane].pt = plane_pt;
	
	if (valid==-1) ndead++;
	if (valid== 1) nvalid++;
	
      }
      
      if ((nvalid==2 and ndead==1) or nvalid==3)
	ret_v.emplace_back(std::move(shell_pt));
      
    } // end this shell point
    
    return ret_v;
  }

  std::vector<data::Vertex3D>
  PixelScan3D::SphereScan3D(const std::array<cv::Mat,3>& image_v, 
			    const data::Vertex3D& vtx3d) const {
    
    
    auto res_v = RegisterSpheres(vtx3d);
    
    std::vector<data::Vertex3D> ret_v;
    ret_v.reserve(res_v.size());
    
    geo2d::Vector<float> plane_pt(kINVALID_FLOAT,kINVALID_FLOAT);
    const auto inv_plane_pt = plane_pt;
    
    if (_radius_v.empty()) 
      throw larbys("No radii specified");
    
    int nvalid = 0;
    
    for(auto& shell_pt : res_v) { 
      
      nvalid = 0;
      
      for(size_t plane=0; plane<3; ++plane) {
	auto valid = SetPlanePoint(image_v[plane],shell_pt,plane,plane_pt);
	
	if (!valid) {
	  shell_pt.vtx2d_v[plane].pt = inv_plane_pt; 
	  continue;
	}
	
	shell_pt.vtx2d_v[plane].pt = plane_pt;
	nvalid++;
      }
      
      if (nvalid==3) 
	ret_v.emplace_back(std::move(shell_pt));
      
    } // end this shell point
    
    return ret_v;
  }

  std::vector<const data::Vertex3D*>
  PixelScan3D::ProjectAndDistance(cv::Mat& image,
				  const size_t plane,
				  const std::vector<data::Vertex3D>& pts_v,
				  const std::vector<float>& distance_v) const {
    
    std::vector<const data::Vertex3D*> ret_v;
    std::unordered_map<int, size_t> pt_m;

    for(size_t pid=0; pid < pts_v.size(); ++pid) {
      const auto& pt = pts_v[pid];

      int px_x = pt.vtx2d_v[plane].pt.x;
      int px_y = pt.vtx2d_v[plane].pt.y;
      
      if (px_x < 0) continue;
      if (px_y < 0) continue;
      
      if (px_x >= image.rows) continue;
      if (px_y >= image.cols) continue;
      
      int px = (int) image.at<uchar>(px_y,px_x);
      if (px==0) continue;
      
      int idx = px_y*image.cols + px_x;
      
      auto iter = pt_m.find(idx);
      if (iter == pt_m.end()) {
	pt_m[idx] = pid;
	continue;
      }
      
      if (distance_v[(*iter).second] < distance_v[pid])
	continue;

      (*iter).second = pid;
    }
    
    ret_v.reserve(pt_m.size());
    for(auto pt : pt_m)
      ret_v.emplace_back(&(pts_v[pt.second]));
    
    return ret_v;
  }
  

  bool PixelScan3D::SetPlanePoint(const cv::Mat& img,
				  const data::Vertex3D& vtx3d,
				  const size_t plane,
				  geo2d::Vector<float>& plane_pt) const
  {
    
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

  int PixelScan3D::SetPlanePoint(const cv::Mat& img,
				 const data::Vertex3D& vtx3d,
				 const size_t plane,
				 geo2d::Vector<float>& plane_pt,
				 const cv::Mat& dead) const
  {
    
    try {
      auto x = _geo.x2col(vtx3d.x, plane);

      if (x >= img.cols or x < 0) return false;

      auto y = _geo.yz2row(vtx3d.y, vtx3d.z, plane);

      if (y >= img.rows or y < 0) return false;

      uint x_0 = x + 0.5;
      uint y_0 = y + 0.5;

      plane_pt.x = (float)x_0;
      plane_pt.y = (float)y_0;

      uint p = (uint)img.at<uchar>(y_0,x_0);

      if(!p) { 
	uint d = (uint)dead.at<uchar>(y_0,x_0);
	
	// invalid
	if (d) 
	  return 0;
	
	// in dead
	else
	  return -1;
      }

    }
    // invalid
    catch(const larbys& err) { return 0; }
    
    // valid & not in dead
    return 1; 
  }

}

#endif
