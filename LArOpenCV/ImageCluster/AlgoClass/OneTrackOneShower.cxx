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
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"

namespace larocv {

  void OneTrackOneShower::Reset()
  { _cand_vertex_v.clear(); }

  void OneTrackOneShower::Configure(const Config_t &pset)
  {

    //
    // AlgoClass module configuration
    //
    
    auto this_verbosity = (msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level())));
    this->set_verbosity(this_verbosity);

    // Pixel intensity threshold for the image
    _pi_threshold = pset.get<float>("PiThreshold",10);
    // Default radius size to determine a crossing point
    _circle_default_radius = pset.get<float>("CircleDefaultRadius",10);
    // Use the radius stored in the VertexSeed2D to determine a crossing point
    _use_seed_radius = pset.get<bool>("UseSeedRadius",false);
    // Optionally refine a crossing point merging with crossing points part of the same charge blob
    _refine_qpoint = pset.get<bool>("RefineQPoint",false);
    if(_refine_qpoint)
      // Specify how many pixels to maskout for crossing point refinement
      _refine_qpoint_maskout = pset.get<float>("RefineQPointMaskout");

    // Optionally graduate a circle, computing crossing points along the way
    _grad_circle = pset.get<bool> ("GraduateCircle",false);
    if (_grad_circle) {
      _grad_circle_min = pset.get<float>("GraduateCircleMin");
      _grad_circle_max = pset.get<float>("GraduateCircleMax");
      _grad_circle_step = pset.get<float>("GraduateCircleStep");
      for(float step = _grad_circle_min;
	  step <= _grad_circle_max;
	  step += _grad_circle_step)
	_grad_circle_rad_v.push_back(step);
    }

    // Require there by a unique (==1) crossing point to claim a shower-exists on this plane
    _require_unique = pset.get<bool>("RequireUnique",true);

    // Number of planes (images) to search for the shower
    _num_planes = 3;

    // Required number of non-zero pixels between 2D vertex, and the crossing point to claim a valid
    // crossing point
    _valid_xs_npx = pset.get<size_t>("ValidXsNpx",1);

    // Check if gap exists between the center of the circle and the crossing point
    _path_exists_check = pset.get<bool>("PathExistsCheck",false);
    if (_path_exists_check) {
      _pi_thr        = pset.get<float>("GapPiThreshold",10.0);
      _pi_tol        = pset.get<float>("GapPiTolerance",2.0);
      _d_thresh      = pset.get<float>("GapDistThreshold",5.0);
      _min_ctor_size = pset.get<uint>("GapMinContourSize",2);
    }
    
    // Configure LArPlaneGeo class
    _geo_algo.Configure(pset.get<Config_t>("LArPlaneGeo"));

    _try_groups = pset.get<bool>("TryGroups",false);

  }

  bool OneTrackOneShower::ValidateTrackEdge(const cv::Mat& img, 
					    const geo2d::Vector<float>& pt) {
    
    if (_grad_circle_rad_v.empty()) throw larbys("grad circle not configured");

    auto xs_pt_vv = QPointArrayOnCircleArray(img,pt,_grad_circle_rad_v);
    
    for(size_t rad_id=0; rad_id < xs_pt_vv.size(); ++rad_id) {
      const auto& xs_pt_v = xs_pt_vv[rad_id];
      if (xs_pt_v.empty()) return false;
    }

    return true;
  }


  void OneTrackOneShower::SetPlaneInfo(const ImageMeta& meta)
  { _geo_algo.ResetPlaneInfo(meta); }


  // Determine if the crossing point is valid
  geo2d::VectorArray<float>
  OneTrackOneShower::ValidShowerPointOnCircle(::cv::Mat& img,
					      const geo2d::Circle<float>& circle,
					      const std::vector<geo2d::Vector<float> >& xs_pts) const
  {

    // Check for a gap in pixels between center of the circle and crossing point
    if(_path_exists_check) {
      LAROCV_DEBUG() << "User opted to check path existence" << std::endl;
      geo2d::VectorArray<float> res_v;
      for(auto const& xs_pt : xs_pts) {
	// 2 pixel tolerance outside the circle
	auto pathexists = PathExists(MaskImage(img,circle,_pi_tol,false),
				     circle.center,  // pt1
				     xs_pt,          // pt2
				     _d_thresh,      // threshold pt to contour distance
				     _pi_thr,        // pixel intensity threshold
				     _min_ctor_size);// minimum contour size
	if (pathexists) res_v.push_back(xs_pt);
      }
      LAROCV_DEBUG() << "... " << xs_pts.size() << " crossing points reduced to " << res_v.size() << std::endl;
      return res_v;
    }

    // Dont check for a gap, but require significant # pixels to have a charge
    // when drawing a line from the center to the xs point
    static std::vector<geo2d::Vector<int> > check_pt_v;
    check_pt_v.clear();

    static geo2d::VectorArray<float> res_v;
    res_v.clear();

    geo2d::Vector<int> pt;
    
    for(auto const& xs_pt : xs_pts) {
      LAROCV_DEBUG() << "@xs_pt=" << xs_pt << std::endl;
      auto const dir = geo2d::dir(circle.center,xs_pt);
      if(dir.x == 0 && dir.y ==0) throw larbys("No direction found!");
      check_pt_v.clear();
      // fill point coordinates to be checked
      if(dir.x==0) {
	for(size_t i=0; i<check_pt_v.size(); ++i) {
	  if(circle.center.y>0)
	    { pt.x = circle.center.x; pt.y = circle.center.y+i; }
	  else
	    { pt.x = circle.center.x; pt.y = circle.center.y-i; }
	  LAROCV_DEBUG() << " ... check point " << check_pt_v.size() << " @ " << pt << std::endl;
	  check_pt_v.push_back(pt);
	}
      }else if(dir.y==0) {
	for(size_t i=0; i<check_pt_v.size(); ++i) {
	  if(circle.center.x>0)
	    { pt.x = circle.center.x+i; pt.y = circle.center.y; }
	  else
	    { pt.x = circle.center.x-i; pt.y = circle.center.y; }
	  LAROCV_DEBUG() << " ... check point " << check_pt_v.size() << " @ " << pt << std::endl;
	  check_pt_v.push_back(pt);
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
	  for(auto const check_pt : check_pt_v) {
	    if(check_pt.x != pt.x || check_pt.y != pt.y) continue;
	    included = true;
	    break;
	  }
	  if(!included) {
	    check_pt_v.push_back(pt);
	    LAROCV_DEBUG() << " ... check point " << check_pt_v.size() << " @ " << pt << std::endl;
	  }
	}
	for(size_t i=0; i<dx; ++i) {
	  float step = i*dxd;
	  pt.y = (size_t)(l.y(circle.center.x + step)+0.5);
	  pt.x = (size_t)(circle.center.x + step + 0.5);
	  bool included=false;
	  for(auto const check_pt : check_pt_v) {
	    if(check_pt.x != pt.x || check_pt.y != pt.y) continue;
	    included = true;
	    break;
	  }
	  if(!included) {
	    check_pt_v.push_back(pt);
	    LAROCV_DEBUG() << " ... check point " << check_pt_v.size() << " @ " << pt << std::endl;
	  }
	}
      }

      // Now check coordinates registered
      size_t valid_px_count=0;
      for(auto const& pt : check_pt_v) {
	if (pt.x<0         or pt.y<0)         continue;
	if (pt.x>=img.cols or pt.y>=img.rows) continue;
	if( ((float)(img.at<uchar>(pt.y,pt.x))) > _pi_threshold )
	  ++valid_px_count;
      }

      bool skip_xs_pt = (valid_px_count < _valid_xs_npx);

      LAROCV_INFO() << "For xs @ " << xs_pt
		    << " found " << valid_px_count << "/" << check_pt_v.size()
		    << " pixels above threshold ("
		    << (skip_xs_pt ? "skipping)" : "keeping)")
		    << std::endl;

      if(!skip_xs_pt) res_v.push_back(xs_pt);
    }
    return res_v;
  }

  // Given an image array, and vertexseed3d array, make a vertex3d and associated circle vertex
  std::vector<data::Vertex3D>
  OneTrackOneShower::ListShowerVertex(std::vector<cv::Mat>& img_v,
				      const std::vector<data::VertexSeed3D>& cand_v) const
  {
    std::vector<data::Vertex3D> res_vtx3d_v;
    res_vtx3d_v.clear();

    // Make circle to search for a crossing point in the image
    geo2d::Circle<float> circle;
    circle.radius = _circle_default_radius;

    // Search each 3D seed for a crossing point in the image
    LAROCV_INFO() << "Candidate 3D vertex seeds " << cand_v.size() << std::endl;
    for(size_t vtx_idx=0; vtx_idx<cand_v.size(); ++vtx_idx) {
      auto const& cand_vtx = cand_v[vtx_idx];
      LAROCV_INFO() << "Inspecting vertex seed " << vtx_idx
		    << " @ (" << cand_vtx.x << "," << cand_vtx.y << "," << cand_vtx.z << ")" << std::endl;
      if (cand_vtx.vtx2d_v.size() != _num_planes) {
	LAROCV_CRITICAL() << " listing invalid vertex " << std::endl;
	throw larbys();
      }

      // Possibly one circle vertex per plane
      std::vector<data::CircleVertex> cvtx_v;
      cvtx_v.resize(_num_planes);
      for(size_t plane=0; plane<img_v.size(); ++plane) {
	auto& cvtx = cvtx_v[plane];
	
	circle.center.x = cand_vtx.vtx2d_v.at(plane).x;
	circle.center.y = cand_vtx.vtx2d_v.at(plane).y;
	circle.radius   = _use_seed_radius ? cand_vtx.vtx2d_v.at(plane).radius : circle.radius;
	
	if (circle.radius==kINVALID_FLOAT)
	  circle.radius=_circle_default_radius;
	
	cvtx.center.x = circle.center.x;
	cvtx.center.y = circle.center.y;
	cvtx.radius   = circle.radius;

	auto& img = img_v[plane];

	ValidateCircleVertex(img,cvtx);
	
      } // end plane
      
      data::Vertex3D vtx3d;
      vtx3d.x = cand_vtx.x;
      vtx3d.y = cand_vtx.y;
      vtx3d.z = cand_vtx.z;
      vtx3d.vtx2d_v = Seed2Vertex(cand_vtx.vtx2d_v);
      vtx3d.cvtx2d_v = cvtx_v;
      vtx3d.type = data::VertexType_t::kShower;
      res_vtx3d_v.emplace_back(std::move(vtx3d));

    } // end vertex
    
    return res_vtx3d_v;
  }
  
  // Given an image, and circle vertex make crossing points, fill out PCA information if possible
  void
  OneTrackOneShower::ValidateCircleVertex(cv::Mat& img,
					  data::CircleVertex& cvtx) const {
    
    geo2d::VectorArray<float> res_xs_pt_v;

    // Determine crossing points by changing the circle size, scan until the number
    // of crossing points increases
    if (_grad_circle) {
      LAROCV_DEBUG() << "Graduating circle @ " << cvtx.center << std::endl;
      auto xs_pt_vv = QPointArrayOnCircleArray(img,cvtx.center,_grad_circle_rad_v,_pi_threshold);
      
      for(size_t rad_id=0;rad_id<xs_pt_vv.size();++rad_id) {
	cvtx.radius = _grad_circle_rad_v[rad_id];
	auto& xs_pt_v = xs_pt_vv[rad_id];
	LAROCV_DEBUG() << "Inspect XS " << xs_pt_v.size() << " @ rad " << cvtx.radius << std::endl;
	xs_pt_v = this->ValidShowerPointOnCircle(img, cvtx.as_circle(), xs_pt_v);

	if(_refine_qpoint && !xs_pt_v.empty()) {
	  xs_pt_v = QPointOnCircleRefine(img,cvtx.as_circle(),xs_pt_v,_refine_qpoint_maskout);
	}
      }

      auto last_xs = xs_pt_vv.front().size();

      for(size_t rad_id=0; rad_id < xs_pt_vv.size(); ++rad_id) {
	const auto& xs_pt_v = xs_pt_vv[rad_id];
	if (xs_pt_v.empty()) break;
	if (xs_pt_v.size() > last_xs) break;
	
	cvtx.radius = _grad_circle_rad_v[rad_id];
	res_xs_pt_v = xs_pt_v;
	last_xs = xs_pt_v.size();
      }
      
    } // end graduate circle

    else {
      LAROCV_DEBUG() << "Finding crossing points using regular QPoint method" << std::endl;
      LAROCV_DEBUG() << "@cvtx pt=" << cvtx.center  << " rad=" << cvtx.radius << " pth=" << _pi_threshold << std::endl;
      geo2d::VectorArray<float> xs_pt_v;

      if (_try_groups)
	xs_pt_v = OnCircleGroups(img,cvtx.as_circle());
      else
	xs_pt_v = QPointOnCircle(img,cvtx.as_circle(),_pi_threshold);

      LAROCV_DEBUG() << "... " << xs_pt_v.size() << " xs found" << std::endl;
      if(_refine_qpoint && !xs_pt_v.empty()) {
	LAROCV_DEBUG() << "Refining qpoint" << std::endl;
	xs_pt_v = QPointOnCircleRefine(img,cvtx.as_circle(),xs_pt_v,_refine_qpoint_maskout);
	LAROCV_DEBUG() << "... "<< xs_pt_v.size() << " xs refined" << std::endl;
      }
      
      xs_pt_v = this->ValidShowerPointOnCircle(img, cvtx.as_circle(), xs_pt_v);
      LAROCV_DEBUG() << "... " << xs_pt_v.size() << " xs validated"  << std::endl;

      res_xs_pt_v = xs_pt_v;
    }

    for(auto const& xs_pt : res_xs_pt_v) {
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

  // Given an image, use internal vertex seeds to generate true vertices
  std::vector<data::Vertex3D>
  OneTrackOneShower::CreateSingleShower(std::vector<cv::Mat>& img_v)
  {
    // Construct a single list of vertex candidates (ltrack=>vtrack=>vedge)
    LAROCV_INFO() << "# Vertex3D after combining all: " << _cand_vertex_v.size() << std::endl;

    std::vector<data::Vertex3D> result;

    // Search for a single shower
    _shower_vtx3d_v.clear();
    _shower_vtx3d_v = ListShowerVertex(img_v,_cand_vertex_v);
    LAROCV_INFO() << "# Vertex3D candidate for single shower: " << _shower_vtx3d_v.size() << std::endl;

    // For each returned shower, count the number of crossing points to determine if
    // there really is one shower 
    for(size_t i=0; i<_shower_vtx3d_v.size(); ++i) {

      auto const& vtx3d = _shower_vtx3d_v[i];
      LAROCV_INFO() << "Inspecting 3D vertex ("
		    << vtx3d.x << ","
		    << vtx3d.y << ","
		    << vtx3d.z << ")" << std::endl;
      
      // Accept if 2 planes have 2D circle w/ only 1 crossing
      std::vector<size_t> num_xs_v(_num_planes+1,0);
      size_t num_plane_unique_xs = 0;
      for(size_t plane=0; plane<vtx3d.vtx2d_v.size(); ++plane) {
	auto const& cvtx2d = vtx3d.cvtx2d_v[plane];
	LAROCV_DEBUG() << "  2D vertex @ " << cvtx2d.center
		       << " ... # crossing points = " << cvtx2d.xs_v.size() << std::endl;
	num_xs_v[plane] = cvtx2d.xs_v.size();
	if(_require_unique && cvtx2d.xs_v.size() == 1) {
	  ++num_plane_unique_xs;
	}
	else {
	  if(cvtx2d.xs_v.size()) {
	    ++num_plane_unique_xs;
	  }
	}
	    
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
