#ifndef __ONETRACKONESHOWER_CXX__
#define __ONETRACKONESHOWER_CXX__

#include "Geo2D/Core/VectorArray.h"
#include "Geo2D/Core/Geo2D.h"
#include "Geo2D/Core/Line.h"
#include "OneTrackOneShower.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/SpectrumAnalysis.h"

namespace larocv {

  void OneTrackOneShower::Reset()
  { _cand_vertex_v.clear(); }

  void OneTrackOneShower::Configure(const Config_t &pset)
  {
    auto this_verbosity = (msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level())));
    this->set_verbosity(this_verbosity);
      
    _pi_threshold = 5;
    _circle_default_radius = pset.get<float>("CircleDefaultRadius",10);//10

    _xplane_tick_resolution = 3;
    _num_planes = 3;

    _valid_xs_npx = pset.get<size_t>("ValidXsNpx",1);
  }

  void OneTrackOneShower::SetPlaneInfo(const ImageMeta& meta)
  {}

  geo2d::VectorArray<float>
  OneTrackOneShower::ValidShowerPointOnCircle(const ::cv::Mat& img,
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
  
  std::vector<data::Vertex3D>
  OneTrackOneShower::ListShowerVertex(const std::vector<const cv::Mat>& img_v,
				      const std::vector<data::VertexSeed3D>& cand_v) const
  {
    std::vector<data::Vertex3D> res_vtx3d_v;
    res_vtx3d_v.clear();
    geo2d::Circle<float> circle;
    circle.radius = _circle_default_radius;
    LAROCV_INFO() << "Candidate 3D vertex seeds " << cand_v.size() << std::endl;
    for(size_t vtx_idx=0; vtx_idx<cand_v.size(); ++vtx_idx) {
      auto const& cand_vtx = cand_v[vtx_idx];
      LAROCV_INFO() << "Inspecting vertex seed " << vtx_idx
		    << " @ (" << cand_vtx.x << "," << cand_vtx.y << "," << cand_vtx.z << ")" << std::endl;
      if (cand_vtx.vtx2d_v.size() != _num_planes) {
	LAROCV_CRITICAL() << " listing invalid vertex " << std::endl;
	throw larbys();
      }
      std::vector<data::CircleVertex> cvtx_v;
      cvtx_v.resize(_num_planes);
      for(size_t plane=0; plane<img_v.size(); ++plane) {
	auto& cvtx = cvtx_v[plane];
	cvtx.radius = _circle_default_radius;
	circle.center.x = cand_vtx.vtx2d_v.at(plane).pt.x;
	circle.center.y = cand_vtx.vtx2d_v.at(plane).pt.y;
	cvtx.center.x = circle.center.x;
	cvtx.center.y = circle.center.y;
	auto const& img = img_v[plane];
	auto xs_pt_v = QPointOnCircle(img,circle,_pi_threshold);
	LAROCV_INFO() << "Inspecting plane " << plane
		      << " ... " << xs_pt_v.size()
		      << " crossing points on circle @ " << circle.center << " w/ rad " << circle.radius << std::endl;
	xs_pt_v = this->ValidShowerPointOnCircle(img, circle, xs_pt_v);
	for(auto const& xs_pt : xs_pt_v) {
	  LAROCV_DEBUG() << "Determining PCA @ " << xs_pt << std::endl;
	  data::PointPCA pca_pt;
	  pca_pt.pt = xs_pt;
	  try {
	    pca_pt.line = SquarePCA(img, pca_pt.pt, 5, 5);
	  }
	  catch(const larbys& lar) {
	    LAROCV_WARNING() << "Local pca assignment fails for xs @ " << xs_pt << "... skip!" << std::endl;
	    continue;
	  }
	  cvtx.xs_v.push_back(pca_pt);
	}
      }
      data::Vertex3D vtx3d;
      vtx3d.x = cand_vtx.x;
      vtx3d.y = cand_vtx.y;
      vtx3d.z = cand_vtx.z;
      vtx3d.vtx2d_v = cand_vtx.vtx2d_v;
      vtx3d.cvtx2d_v = cvtx_v;
      vtx3d.type=data::VertexType_t::kShower;
      res_vtx3d_v.emplace_back(std::move(vtx3d));
    }
    return res_vtx3d_v;
  }
  
  std::vector<data::Vertex3D>
  OneTrackOneShower::CreateSingleShower(const std::vector<const cv::Mat>& img_v)
  {
    // Construct a single list of vertex candidates (ltrack=>vtrack=>vedge)
    LAROCV_INFO() << "# Vertex3D after combining all: " << _cand_vertex_v.size() << std::endl;

    // Now search for a single shower
    _shower_vtx3d_v.clear();

    std::vector<data::Vertex3D> result;
    
    _shower_vtx3d_v = ListShowerVertex(img_v,_cand_vertex_v);
    LAROCV_INFO() << "# Vertex3D candidate for single shower: " << _shower_vtx3d_v.size() << std::endl;
    
    for(size_t i=0; i<_shower_vtx3d_v.size(); ++i) {

      auto const& vtx3d = _shower_vtx3d_v[i];
      LAROCV_INFO() << "Inspecting 3D vertex ("
		    << vtx3d.x << ","
		    << vtx3d.y << ","
		    << vtx3d.z << ")" << std::endl;
      
      // accept if 2 planes have 2D circle w/ only 1 crossing
      std::vector<size_t> num_xs_v(_num_planes+1,0);
      size_t num_plane_unique_xs = 0;
      for(size_t plane=0; plane<vtx3d.vtx2d_v.size(); ++plane) {
	auto const& cvtx2d = vtx3d.cvtx2d_v[plane];
	LAROCV_DEBUG() << "  2D vertex @ " << cvtx2d.center
		       << " ... # crossing points = " << cvtx2d.xs_v.size() << std::endl;
	num_xs_v[plane] = cvtx2d.xs_v.size();
	if(cvtx2d.xs_v.size() == 1) ++num_plane_unique_xs;
      }
      if(num_plane_unique_xs<2) {
	LAROCV_DEBUG() << "Skipping this candidate... (# of planes w/ unique-circle-xs-pt is <2)" << std::endl;
	continue;
      }

      result.push_back(vtx3d);
    }
    return result;
  }

}
#endif
