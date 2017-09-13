#ifndef __PIXELSCAN3D_CXX__
#define __PIXELSCAN3D_CXX__

#include "PixelScan3D.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#define PI 3.1415926

namespace larocv {

  void PixelScan3D::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));

    _radius_v.resize(50);
    for(float r=0; r<_radius_v.size(); ++r) _radius_v[r] = r;

    _theta_base = 20;
    _phi_base = 40;

  }

  std::vector<std::vector<data::Vertex3D> > 
  PixelScan3D::RegisterRegions(const data::Vertex3D& vtx) const{
    
    std::vector<std::vector<data::Vertex3D> > pts_vv;
    pts_vv.resize(_radius_v.size());


    for(size_t rid = 0; rid < _radius_v.size(); ++rid) {
      
      float radius = _radius_v[rid];
      std::vector<float> theta_v(_theta_base,kINVALID_FLOAT);
      std::vector<float> phi_v  (_phi_base  ,kINVALID_FLOAT);

      for(size_t tid=0; tid<theta_v.size(); ++tid) 
	theta_v[tid] = (float) tid * PI / ( (float) theta_v.size() );

      theta_v.push_back(PI);

      for(size_t pid=0; pid<phi_v.size(); ++pid) 
	phi_v[pid] = (float) pid * 2 * PI / ( (float) phi_v.size() );
    
      std::vector<data::Vertex3D> pts_v;
      pts_v.reserve(theta_v.size() * phi_v.size());
      
      for(auto theta : theta_v) {
	for(auto phi :  phi_v) {
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

    return pts_vv;
  }
  
  std::vector<std::vector<data::Vertex3D> > 
  PixelScan3D::RegionScan3D(const std::vector<cv::Mat>& image_v, 
			    const data::Vertex3D& vtx3d) const {

    auto res_vv = RegisterRegions(vtx3d);

    std::vector<std::vector<data::Vertex3D> > ret_vv;
    ret_vv.resize(res_vv.size());
    
    geo2d::Vector<float> plane_pt(kINVALID_FLOAT,kINVALID_FLOAT);
    const auto inv_plane_pt = plane_pt;

    size_t nvalid = kINVALID_SIZE;

    for(size_t rid = 0; rid < _radius_v.size(); ++rid) {
      auto radius = _radius_v[rid];
      LAROCV_DEBUG() << "@rid=" << rid << " radius=" << radius << std::endl;

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
				 const std::vector<GEO2D_ContourArray_t>& ctor_vv) {
    
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
    return ass_vv;
  }

  bool PixelScan3D::SetPlanePoint(cv::Mat img,
				  const data::Vertex3D& vtx3d,
				  const size_t plane,
				  geo2d::Vector<float>& plane_pt,
				  bool check_img) const {

    try {
      auto x = _geo.x2col(vtx3d.x, plane);
      auto y = _geo.yz2row(vtx3d.y, vtx3d.z, plane);

      if (x >= img.cols or x < 0) return false;
      if (y >= img.rows or y < 0) return false;
    
      if (check_img) {
	uint x_0 = std::floor(x);
	uint y_0 = std::floor(y);
      
	uint x_1 = x_0+1 < img.cols ? x_0+1 : x_0;
	uint y_1 = y_0+1 < img.rows ? y_0+1 : y_0;
      
	uint p00 = (uint)img.at<uchar>(y_0,x_0);
	uint p10 = (uint)img.at<uchar>(y_1,x_0);
	uint p01 = (uint)img.at<uchar>(y_0,x_1);
	uint p11 = (uint)img.at<uchar>(y_1,x_1);

	if(0){}
	else if (p00) { x = x_0; y = y_0; }
	else if (p11) { x = x_1; y = y_1; }
	else if (p10) { x = x_0; y = y_1; }
	else if (p01) { x = x_1; y = y_0; }

	else { return false; }
      }

      plane_pt.x = x;
      plane_pt.y = y;
    } 
    catch(const larbys& err) { return false; }
    return true;
  }
}

#endif
