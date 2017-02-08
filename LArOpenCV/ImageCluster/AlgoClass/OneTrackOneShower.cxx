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
#include "LArOpenCV/ImageCluster/AlgoData/SingleShowerData.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexClusterData.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrackClusterData.h"

namespace larocv {

  void OneTrackOneShower::Reset()
  {
    _geo_algo.Reset();
    _geo_algo._xplane_tick_resolution = _xplane_tick_resolution;
    _geo_algo._num_planes = _num_planes;
  }

  void OneTrackOneShower::Configure(const Config_t &pset)
  {
    auto this_verbosity = (msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level())));
    this->set_verbosity(this_verbosity);
    _par_algo.Configure(pset);
    _par_algo._pi_threshold = 5;
    _par_algo.set_verbosity(this_verbosity);
      
    _pi_threshold = 5;
    _circle_default_radius = pset.get<float>("CircleDefaultRadius",10);//10

    _xplane_tick_resolution = 3;
    _num_planes = 3;

    _valid_xs_npx = pset.get<size_t>("ValidXsNpx",1);
  }

  void OneTrackOneShower::SetPlaneInfo(const ImageMeta& meta)
  { _geo_algo.ResetPlaneInfo(meta); }

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
  
  data::ShowerCluster
  OneTrackOneShower::SingleShowerHypothesis(const ::cv::Mat& img,
					    const data::CircleVertex& vtx,
					    const GEO2D_Contour_t& super_cluster)
  {

    if(vtx.xs_v.size()!=1) {
      LAROCV_CRITICAL() << "Only 1 crossing-point CircleVertex can be provided..." << std::endl;
      throw larbys();
    }
    
    auto res_ctor_v = _par_algo.CreateParticleCluster(img,vtx,super_cluster);
    
    data::ShowerCluster shower;

    auto& contour = shower.ctor;
    if ( res_ctor_v.empty() ) {
      LAROCV_INFO() << "No single shower can be identified" << std::endl;
    }
    else if (res_ctor_v.size() == 1) {
      LAROCV_INFO() << "Single shower identified" << std::endl;
      contour = res_ctor_v[0];
    }
    else {
      LAROCV_CRITICAL() << "Implement choosing largest area contour?" << std::endl;
      throw larbys();
    }
      
    if(shower.ctor.size()<=2)
      shower.ctor.clear();
    else
      shower.start = vtx.center;

    return shower;
  }

  void OneTrackOneShower::ListShowerVertex(const std::vector<const cv::Mat>& img_v,
					   const std::vector<data::Vertex3D>& cand_v,
					   std::vector<data::Vertex3D>& res_vtx_v,
					   std::vector<std::vector<data::CircleVertex> >& res_cvtx_v) const
  {
    res_vtx_v.clear();
    res_cvtx_v.clear();
    
    geo2d::Circle<float> circle;
    circle.radius = _circle_default_radius;

    for(size_t vtx_idx=0; vtx_idx<cand_v.size(); ++vtx_idx) {
      auto const& cand_vtx = cand_v[vtx_idx];
      LAROCV_INFO() << "Inspecting vertex " << vtx_idx
		    << " @ (" << cand_vtx.x << "," << cand_vtx.y << "," << cand_vtx.z << ")" << std::endl;
      std::vector<data::CircleVertex> cvtx_v;
      cvtx_v.resize(_num_planes);
      for(size_t plane=0; plane<img_v.size(); ++plane) {
	data::ShowerCluster scluster;
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
	  data::PointPCA pca_pt;
	  pca_pt.pt = xs_pt;
	  pca_pt.line = SquarePCA(img, pca_pt.pt, 5, 5);
	  cvtx.xs_v.push_back(pca_pt);
	}
      }

      res_vtx_v.push_back(cand_vtx);
      res_cvtx_v.push_back(cvtx_v);
    }
  }
  
  std::vector<data::SingleShower> OneTrackOneShower::CreateSingleShower(const std::vector<const cv::Mat>& img_v)
  {
    // Construct a single list of vertex candidates (ltrack=>vtrack=>vedge)
    LAROCV_INFO() << "# Vertex3D after combining all: " << _cand_vertex_v.size() << std::endl;

    // Now search for a single shower
    _shower_vtx3d_v.clear();
    _shower_vtx2d_vv.clear();
    
    ListShowerVertex(img_v,_cand_vertex_v, _shower_vtx3d_v, _shower_vtx2d_vv);
    LAROCV_INFO() << "# Vertex3D candidate for single shower: " << _shower_vtx3d_v.size() << std::endl;
    
    std::vector<data::SingleShower> shower_v;

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

      data::SingleShower shower;
      for(size_t plane=0; plane<img_v.size(); ++plane) {
	auto const& img  = img_v[plane];
	auto const& cvtx = vtx2d_v[plane];
	if(cvtx.xs_v.size()!=1) continue;
	LAROCV_INFO() << "Clustering attempt on plane " << plane << " with circle @ " << cvtx.center << std::endl;
	shower.insert(plane,SingleShowerHypothesis(img, cvtx));
      }
      size_t num_good_plane=0;
      for(auto const& scluster: shower.get_clusters()) {
	if(scluster.ctor.size()<3) continue;
	++num_good_plane;
      }
      
      if(num_good_plane<2) continue;

      shower.set_vertex(vtx3d);

      shower_v.emplace_back(std::move(shower));
    }

    // done
    return shower_v;
  }

}
#endif
