#ifndef DEFECTBREAKER_CXX
#define DEFECTBREAKER_CXX

#include "DefectBreaker.h"
#include "Geo2D/Core/Geo2D.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LArOpenCV/Core/larbys.h"

namespace larocv {

  void DefectBreaker::Configure(const Config_t &pset) {
    _min_defect_size      = pset.get<int>("MinDefectSize",5);
    _hull_edge_pts_split  = pset.get<int>("NHullEdgePoints",50);
    _n_allowed_breaks     = pset.get<int>("NAllowedBreaks",10);
    int log_level         = pset.get<int>("Verbosity",0);
    _logger->set((larocv::msg::Level_t)log_level);
  }
  
  cv::Vec4i DefectBreaker::max_hull_edge(const GEO2D_Contour_t& ctor,
					 std::vector<cv::Vec4i> defects) {
    
    float max_dist = -1;
    int max_idx    = -1;
    
    for(uint i=0 ; i<defects.size(); ++i) {
      
      const auto& d1 = defects[i];
      
      float ll=std::sqrt(std::pow(ctor[d1[0]].x-ctor[d1[1]].x,2)
			 +
			 std::pow(ctor[d1[0]].y-ctor[d1[1]].y,2));
      
      if (ll > max_dist) {
	max_dist = ll;
	max_idx  = i;
      }
      
    }
    
    if (max_idx == -1) throw larocv::larbys("Maximum hull edge could not be calculated\n");
    
    return defects[max_idx];
  }
  
  
  void DefectBreaker::split_contour(const GEO2D_Contour_t& ctor,
				    GEO2D_Contour_t& ctor1,
				    GEO2D_Contour_t& ctor2,
				    const geo2d::Line<float>& line) {
    
    //get the two intersection points of this contour and this line
    //one of these points is presumably on the contour
    //the other needs to be extended through the contour, and calculated
    
    auto pts_c = ctor.size();
    
    GEO2D_Contour_t ctor_copy;
    ctor_copy.reserve(ctor.size());
    
    geo2d::LineSegment<float> ctor_s(-1,-1,-1,-1);
    geo2d::LineSegment<float> span_s(-1,-1,-1,-1);
    
    uint n_inters=0;
    uint n_new_inters=0;
    
    for(unsigned i=0; i<ctor.size(); ++i) {

      auto p1 = cv::Point2f(ctor[ i   %pts_c]);
      auto p2 = cv::Point2f(ctor[(i+1)%pts_c]);

      LAROCV_DEBUG() << "Scan p1: " << p1 << ". p2: " << p2 << std::endl;
      
      ctor_s.pt1 = p1;
      ctor_s.pt2 = p2;
      
      // put the first point into the copy contour
      ctor_copy.emplace_back(p1);
      
      //make a line segment which spans the full range of
      //X from this contour segment
      
      auto min_x = std::min(p1.x,p2.x);
      auto max_x = std::max(p1.x,p2.x);
      
      if (min_x == max_x) { min_x-=1; max_x+=1; }
      
      geo2d::Vector<float> p3(min_x,line.y(min_x));
      geo2d::Vector<float> p4(max_x,line.y(max_x));
      
      span_s.pt1 = p3;
      span_s.pt2 = p4;
      
      // intersection points for the two segments, one I made
      // and the other is the contour itself
      geo2d::Vector<float> ip(0,0);
      
      // do they intersect
      if ( ! geo2d::IntersectionPoint(ctor_s,span_s,ip) ) continue;
      
      //they intersect, cast to int
      cv::Point inter_pt(ip.x,ip.y);

      LAROCV_DEBUG() << "Calculated inter pt: " << inter_pt << std::endl;
      
      // calculate first and second point
      // the two points before & after inter pt
      cv::Point first_pt(kINVALID_INT,kINVALID_INT);
      cv::Point second_pt(kINVALID_INT,kINVALID_INT);
      
      bool valid_first  = true;
      bool valid_second = true;
      
      //not vertical
      if (p2.x != p1.x) {
	//Make a point before
	int leftx = inter_pt.x - 1;
	int lefty;
	try { lefty = ctor_s.y(leftx); }
	catch(...) { valid_first = false; }
      
	if (valid_first) first_pt = cv::Point(leftx,lefty);
	
	//Make a point after
	int rightx = inter_pt.x + 1;
	int righty;
	try { righty = ctor_s.y(rightx); }
	catch(...) { valid_second = false; }

	if (valid_second) second_pt = cv::Point(rightx,righty);
	
	if ( p2.x < p1.x ) {
	  std::swap(first_pt,second_pt);
	  std::swap(valid_first,valid_second);
	}
	
      }
      //is vertical
      else {
	//Make a point above
	int topy = inter_pt.y + 1;
	int topx;
	try { topx = ctor_s.x(topy); }
	catch(...) { valid_first = false; }

	if (valid_first) first_pt = cv::Point(topx,topy);
	
	//Make a point below
	int boty = inter_pt.y - 1;
	int botx;
	try { botx = ctor_s.x(boty); }
	catch(...) { valid_second = false; }

	if (valid_second) second_pt = cv::Point(botx,boty);
	
	if ( p2.y < p1.y )  {
	  std::swap(first_pt,second_pt);
	  std::swap(valid_first,valid_second);
	}
      }
      
      
      //this point is already a part of the contour, don't put a duplicate inside
      //if ( inter_pt == cv::Point(p1) or inter_pt == cv::Point(p2) ){
      if ( inter_pt == cv::Point(p1) ){
	LAROCV_DEBUG() << "inter pt: " << inter_pt << " is p1: " << cv::Point(p1) << std::endl;
	
	//Lets add a second point on this contour
	if (valid_second && second_pt != cv::Point(p2)) {
	  LAROCV_DEBUG() << "Insert new pt: " << second_pt << std::endl;
	  ctor_copy.emplace_back(std::move(second_pt));
	}
      
	n_inters++;
	continue;
      }

      if ( inter_pt == cv::Point(p2) ){
	LAROCV_DEBUG() << "inter pt: " << inter_pt << " is p2: " << cv::Point(p2) << std::endl;
	
	//Lets add a second point on this contour
	if (valid_first&& first_pt != cv::Point(p1)) {
	  LAROCV_DEBUG() << "Insert new pt: " << first_pt << std::endl;
	  ctor_copy.emplace_back(std::move(first_pt));
	}
      
	n_inters++;
	continue;
      }

      
      if (valid_first && first_pt != cv::Point(p1)) {
	LAROCV_DEBUG() << "Insert new pt: " << first_pt << std::endl;
	ctor_copy.emplace_back(std::move(first_pt));
      }
      
      LAROCV_DEBUG() << "Insert inter pt: " << inter_pt << std::endl;
      ctor_copy.emplace_back(std::move(inter_pt));// this cv::Point2f has been floored with typecast
      
      if (valid_second && second_pt != cv::Point(p2)) {
	LAROCV_DEBUG() << "Insert new pt: " << second_pt << std::endl;
	ctor_copy.emplace_back(std::move(second_pt));
      }
      
      n_new_inters++;
      n_inters++;
    }
    
    LAROCV_DEBUG() << "New intersections inserted " << n_new_inters << " total intersections " << n_inters << std::endl;
    
    // for each point in the copy contour
    for(auto& pt : ctor_copy) {
      
      // point is above the line -- put in contour 1
      if ( pt.y > line.y(pt.x) )
	{ ctor1.emplace_back(pt); continue; }
      
      // it's below the line -- put in contour 2
      ctor2.emplace_back(pt);
    }
    
  }
  void DefectBreaker::fill_hull_and_defects(const GEO2D_Contour_t& ctor,
					    std::vector<int>& hullpts,
					    std::vector<cv::Vec4i>& defects,
					    std::vector<float>& defects_d) {
    
    //Make this hull
    ::cv::convexHull(ctor, hullpts);
    
    //Close this hull up
    hullpts.push_back(hullpts.at(0));
    
    //Compute the defects
    ::cv::convexityDefects(ctor,hullpts,defects);
    
    //Put the defects distances specially into vector of doubles
    defects_d.resize( defects.size() );
    
    for( int j = 0; j < defects.size(); ++j )
      defects_d[j]  = ( ( (float) defects[j][3] ) / 256.0 );
    
  }
  void DefectBreaker::filter_defects(std::vector<cv::Vec4i>& defects,
				     std::vector<float>& defects_d,
				     float min_defect_size){
    
    std::vector<cv::Vec4i> defects_tmp;
    defects_tmp.reserve(defects.size());
    
    std::vector<float> defects_d_tmp;
    defects_d_tmp.reserve(defects.size());
    
    for(unsigned i=0;i<defects.size();++i) {
      if (defects_d[i] < min_defect_size) continue;
      defects_tmp.emplace_back(defects[i]);
      defects_d_tmp.emplace_back(defects_d[i]);
    }
    
    std::swap(defects_tmp  ,defects);
    std::swap(defects_d_tmp,defects_d);
  }
  geo2d::Line<float> DefectBreaker::find_line_hull_defect(const GEO2D_Contour_t& ctor, cv::Vec4i defect_info) {
    
    //number of points in contour
    int pts_c = ctor.size();
    
    // contour segment
    geo2d::LineSegment<float> ctor_segment(-1,-1,-1,-1);
    
    // hull defect segment
    geo2d::LineSegment<float> hull_defect_segment(-1,-1,-1,-1);
    
    // Line that represent the side of hull
    geo2d::Vector<float> start, end, defect, pt1, pt2;
    defect.x = ctor[defect_info[2]].x;  defect.y = ctor[defect_info[2]].y;
    start.x  = ctor[defect_info[0]].x;  start.y  = ctor[defect_info[0]].y;
    end.x    = ctor[defect_info[1]].x;  end.y    = ctor[defect_info[1]].y;
    
    auto hull_line = geo2d::Line<float>(start,end-start);
    
    float dist_defect_to_hull = geo2d::ClosestPoint(hull_line, defect, pt1, pt2);
    
    const geo2d::Vector<float> hull_to_defect = geo2d::dir(pt2,pt1);
    
    LAROCV_DEBUG() << "Defect: " << defect
		   << " ... hull line: " << start << " => " << end
		   << " (dist=" << dist_defect_to_hull << ")" << std::endl;
    LAROCV_DEBUG() << "Hull to defect dir : " << hull_to_defect <<  std::endl;
    
    /*
      auto minidx = std::min(defect_info[0],defect_info[1]);
      auto maxidx = std::max(defect_info[0],defect_info[1]);
    */
    std::vector<size_t> ctor_idx_v;
    ctor_idx_v.reserve(ctor.size());
    if(defect_info[1] > defect_info[0]) {
      auto idx = defect_info[0];
      while(1) {
	ctor_idx_v.push_back(idx);
	if(idx == defect_info[1]) break;
	++idx;
      }
    }else{
      auto idx = defect_info[0];
      while(1) {
	ctor_idx_v.push_back(idx%ctor.size());
	if((idx%ctor.size()) == defect_info[1]) break;
	++idx;
      }
    }
    
    int npts=_hull_edge_pts_split;
    
    // These are parameters to be computed (for return)
    float yinter=0.;
    float dir=0.;
    
    // Loop sequence control variable
    bool first = true;
    while(1) {
      
      if(!first) {
	
	LAROCV_DEBUG() << "Could not find a breaker line ... trying dist: " << dist_defect_to_hull
		       << " => " << dist_defect_to_hull/2. << std::endl;
	dist_defect_to_hull /= 2.;
	start += hull_to_defect * dist_defect_to_hull;
	end   += hull_to_defect * dist_defect_to_hull;
	
	if(dist_defect_to_hull < 1) throw larocv::larbys("Could not locate a breaking line");
      }
      first=false;
      
      // start
      float x1 = start.x;
      float y1 = start.y;
      // end
      float x2 = end.x;
      float y2 = end.y;
      // defect
      float x3 = defect.x;
      float y3 = defect.y;
      
      //LAROCV_DEBUG() << "Inspecting hull defect\n";
      //LAROCV_DEBUG() << "start and end of edge\n";
      //LAROCV_DEBUG() << "plt.plot(["<<start.x<<","<<end.x<<"],["<<start.y<<","<<end.y<<"],'-',lw=5)\n";
      
      
      std::vector< geo2d::Vector<float> > l_;
      l_.resize(npts);
      
      geo2d::Vector<float> dir_(x2-x1,y2-y1);
      
      // make the points on the hull edge
      for(int j=0;j<npts;++j) {
	float k = ( (float) j ) / ( (float) npts );
	l_[j] = dir_*k+geo2d::Vector<float>(x1,y1);
      }
      
      geo2d::Vector<float> p3(x3,y3);
      geo2d::Vector<float> p4(-1,-1);
      
      float mdist = 9.e9;
      
      for ( const auto& l : l_ ) {
	
	// number intersections counter
	int inters = 0;
	
	// min and max index from contour start to end, so we know how to order the loop
	hull_defect_segment.pt1.x = p3.x;
	hull_defect_segment.pt1.y = p3.y;
	hull_defect_segment.pt2.x = l.x;
	hull_defect_segment.pt2.y = l.y;
	
	// loop over portion of contour facing edge
	//LAROCV_DEBUG() << "\n\n\n\n";
	for(auto const& ix : ctor_idx_v) {
	  
	  ctor_segment.pt1 = ctor[ ix         ];
	  ctor_segment.pt2 = ctor[(ix+1)%pts_c];
	  
	  inters += geo2d::Intersect(ctor_segment,hull_defect_segment);
	  
	  //LAROCV_DEBUG() << ix << " " << (ix+1)%pts_c << " " << ctor[ix] << " " << ctor[(ix+1)%pts_c] << " " << inters << "\n";
	  
	  //There is more than one intersection for this line
	  //between the contour and the hull
	  
	  if (inters > 2) break;
	  
	}
	
	//the line cut through the contour
	if(inters > 2) continue;
	
	float dd = geo2d::length(hull_defect_segment);
	
	if ( dd > mdist ) continue;
	
	mdist = dd;
	
	p4 = l;
      }
      
      if (p4.x == -1 || p4.y == -1) continue;
      
      // Found the breaking point
      dir    = (p4.y-p3.y) / (p4.x-p3.x);
      yinter = -1.0*dir*p4.x+p4.y;
      break;
    }
    return geo2d::Line<float>(geo2d::Vector<float>(0,yinter),
			      geo2d::Vector<float>(1,dir));
  }
  geo2d::Line<float> DefectBreaker::scan_breaker(const GEO2D_Contour_t& ctor, cv::Vec4i defect_info) {
    
    //number of points in contour
    int pts_c = ctor.size();
    
    // contour segment
    geo2d::LineSegment<float> ctor_segment(-1,-1,-1,-1);
    
    // hull defect segment
    geo2d::LineSegment<float> hull_defect_segment(-1,-1,-1,-1);
    
    // Line that represent the side of hull
    geo2d::Vector<float> start, end, defect;
    defect.x = ctor[defect_info[2]].x;  defect.y = ctor[defect_info[2]].y;
    start.x  = ctor[defect_info[0]].x;  start.y  = ctor[defect_info[0]].y;
    end.x    = ctor[defect_info[1]].x;  end.y    = ctor[defect_info[1]].y;
    
    //auto const minidx = std::min(defect_info[0],defect_info[1]);
    //auto const maxidx = std::max(defect_info[0],defect_info[1]);
    
    std::vector<size_t> ctor_idx_v;
    ctor_idx_v.reserve(ctor.size());
    if(defect_info[1] > defect_info[0]) {
      auto idx = defect_info[0];
      while(1) {
	ctor_idx_v.push_back(idx);
	if(idx == defect_info[1]) break;
	++idx;
      }
    }else{
      auto idx = defect_info[0];
      while(1) {
	ctor_idx_v.push_back(idx%ctor.size());
	if((idx%ctor.size()) == defect_info[1]) break;
	++idx;
      }
    }
    
    int npts=_hull_edge_pts_split;
    
    bool scan_mode = true;
    size_t scan_edge1=0;
    size_t scan_edge2=ctor_idx_v.size()-1;
    
    // These are parameters to be computed (for return)
    float yinter=0.;
    float dir=0.;
    
    // Loop sequence control variable
    bool first = true;
    while(1) {
      
      if(!first) {
	if( !scan_mode ) {
	  if ( scan_edge2 < 1 ) throw larocv::larbys("LOGIC ERROR");
	  if ( ctor_idx_v[(scan_edge2-1)%(ctor.size())] == defect_info[2] )  
	    throw larocv::larbys("Scan reached the defect point. Could not locate the breaking line...");
	  --scan_edge2;
	}else{
	  if ( ctor_idx_v[(scan_edge1+1)%(ctor.size())] == defect_info[2] )  {
	    // Flipping the scan mode
	    scan_edge1 = 0;
	    scan_edge2 = ctor_idx_v.size()-1;
	    scan_mode = false;
	  }else
	    ++scan_edge1;
	}
      }
      first=false;
      
      start = ctor[ctor_idx_v[scan_edge1%(ctor.size())]];
      end   = ctor[ctor_idx_v[scan_edge2%(ctor.size())]];
      
      // start
      float x1 = start.x;
      float y1 = start.y;
      // end
      float x2 = end.x;
      float y2 = end.y;
      // defect
      float x3 = defect.x;
      float y3 = defect.y;
      
      //LAROCV_DEBUG() << "Inspecting hull defect\n";
      //LAROCV_DEBUG() << "start and end of edge\n";
      //LAROCV_DEBUG() << "plt.plot(["<<start.x<<","<<end.x<<"],["<<start.y<<","<<end.y<<"],'-',lw=5)\n";
      
      
      std::vector< geo2d::Vector<float> > l_;
      l_.resize(npts);
      
      geo2d::Vector<float> dir_(x2-x1,y2-y1);
      
      // make the points on the hull edge
      for(int j=0;j<npts;++j) {
	float k = ( (float) j ) / ( (float) npts );
	l_[j] = dir_*k+geo2d::Vector<float>(x1,y1);
      }
      
      geo2d::Vector<float> p3(x3,y3);
      geo2d::Vector<float> p4(-1,-1);
      
      float mdist = 9.e9;
      
      for ( const auto& l : l_ ) {
	
	// number intersections counter
	int inters = 0;
	
	// min and max index from contour start to end, so we know how to order the loop
	hull_defect_segment.pt1.x = p3.x;
	hull_defect_segment.pt1.y = p3.y;
	hull_defect_segment.pt2.x = l.x;
	hull_defect_segment.pt2.y = l.y;
	
	// loop over portion of contour facing edge
	//LAROCV_DEBUG() << "\n\n\n\n";
	for(auto const& ix : ctor_idx_v) {
	  
	  ctor_segment.pt1 = ctor[ ix         ];
	  ctor_segment.pt2 = ctor[(ix+1)%pts_c];
	  
	  inters += geo2d::Intersect(ctor_segment,hull_defect_segment);
	  
	  //LAROCV_DEBUG() << ix << " " << (ix+1)%pts_c << " " << ctor[ix] << " " << ctor[(ix+1)%pts_c] << " " << inters << "\n";
	  
	  //There is more than one intersection for this line
	  //between the contour and the hull
	  
	  if (inters > 2) break;
	  
	}
	
	//the line cut through the contour
	if(inters > 2) continue;
	
	float dd = geo2d::length(hull_defect_segment);
	
	if ( dd > mdist ) continue;
	
	mdist = dd;
	
	p4 = l;
      }
      
      if (p4.x == -1 || p4.y == -1) continue;
      
      // Found the breaking point
      dir    = (p4.y-p3.y) / (p4.x-p3.x);
      yinter = -1.0*dir*p4.x+p4.y;
      break;
    }
    return geo2d::Line<float>(geo2d::Vector<float>(0,yinter),geo2d::Vector<float>(1,dir));
  }
  
  void DefectBreaker::AssociateDefects(const larocv::data::ClusterCompound& cluscomp,
				       const larocv::data::AtomicContour& parent,
				       const larocv::data::ContourDefect& defect,
				       larocv::data::AtomicContour& child1,
				       larocv::data::AtomicContour& child2)
  {
    
    if(this->logger().level() <= ::larocv::msg::kDEBUG) {
      std::stringstream ss;
      ss << "Parent defect points:";
      for(auto const& defect_id : parent.associated_defects())
	ss << " ID " << defect_id << " @ " << cluscomp.get_defect(defect_id)._pt_defect;
      ss << " ... new defect ID " << defect.id() << " @ " << defect._pt_defect;
      LAROCV_DEBUG() << std::string(ss.str()) << std::endl;
    }
    // Three cases to handle
    // case 0) parent is the "original" (i.e. no associated defect)
    // case 1) parent is an "edge" (i.e. 1 associated defect)
    // case 2) parent is "non-edge" (i.e. 2 associated defects)
    
    // case 0) simply associate both children to the defect
    if(parent.associated_defects().empty()) {
      LAROCV_DEBUG() << "Case 0: defect " << defect.id() << " is associated with both children" << std::endl;
      child1.associate(defect.id());
      child2.associate(defect.id());
      return;
    }
    
    // case 1 and 2 both require to compute the "closest defects from history" to each child
    
    // Look for the closest defect in the parent (parent) for child 1
    double closest_defect1_dist1 = 1e20;
    double closest_defect1_dist2 = 1e20;
    size_t closest_defect1_id1 = larocv::kINVALID_SIZE;
    size_t closest_defect1_id2 = larocv::kINVALID_SIZE;
    for(auto const& defect_id : parent.associated_defects()) {
      auto const& defect_pt = cluscomp.get_defect(defect_id)._pt_defect;
      double dist = pointPolygonTest(child1,defect_pt,true) * -1.;
      if(dist <  closest_defect1_dist1) {
	// update 2nd closest defect
	closest_defect1_dist2 = closest_defect1_dist1;
	closest_defect1_id2   = closest_defect1_id1;
	closest_defect1_dist1 = dist;
	closest_defect1_id1 = defect_id;
      }else if(dist < closest_defect1_dist2) {
	closest_defect1_dist2 = dist;
	closest_defect1_id2   = defect_id;
      }
    }
    
    LAROCV_DEBUG() << "Child1 atom size " << child1.size()
		   << " closest defect " << closest_defect1_id1
		   << " @ " << cluscomp.get_defect(closest_defect1_id1)._pt_defect
		   << " dist = " << closest_defect1_dist1 << std::endl;
    
    // Look for the closest defect in the parent (parent) for child 2
    double closest_defect2_dist1 = 1e20;
    double closest_defect2_dist2 = 1e20;
    size_t closest_defect2_id1 = larocv::kINVALID_SIZE;
    size_t closest_defect2_id2 = larocv::kINVALID_SIZE;
    for(auto const& defect_id : parent.associated_defects()) {
      auto const& defect_pt = cluscomp.get_defect(defect_id)._pt_defect;
      double dist = pointPolygonTest(child2,defect_pt,true) * -1.;
      if(dist < closest_defect2_dist1) {
	// update 2nd closest defect
	closest_defect2_dist2 = closest_defect2_dist1;
	closest_defect2_id2   = closest_defect2_id1;
	closest_defect2_dist1 = dist;
	closest_defect2_id1   = defect_id;
      }else if(dist < closest_defect2_dist2) {
	closest_defect2_dist2 = dist;
	closest_defect2_id2   = defect_id;
      }
    }
    LAROCV_DEBUG() << "Child2 atom size " << child2.size()
		   << " closest defect " << closest_defect2_id1
		   << " @ " << cluscomp.get_defect(closest_defect2_id1)._pt_defect
		   << " dist = " << closest_defect2_dist1 << std::endl;
    
    // case 1) parent is an "edge"
    // In this case one of children must inherit "edge" feature.
    // The edge child should have a larger distance to the "closest" defect in the history (history = 1 defect anyway...)
    if(parent.associated_defects().size() == 1) {
      if(closest_defect1_dist1 > closest_defect2_dist1) {
	child1.associate(defect.id());
	child2.associate(defect.id());
	child2.associate(closest_defect2_id1);
	LAROCV_DEBUG() << "Case 1: new edge @ defect " << defect.id()
		       << " and non-edge defect " << closest_defect2_id1
		       << std::endl;
      }else{
	child1.associate(defect.id());
	child1.associate(closest_defect1_id1);
	child2.associate(defect.id());
	LAROCV_DEBUG() << "Case 1: new edge @ defect " << defect.id()
		       << " and non-edge defect " << closest_defect1_id1
		       << std::endl;
      }
      return;
    }
    
    // case 2) parent is "non-edge"
    child1.associate(defect.id());
    child1.associate(closest_defect1_id1);
    
    child2.associate(defect.id());
    if(closest_defect1_id1 == closest_defect2_id1) {
      child2.associate(closest_defect2_id2);
      LAROCV_DEBUG() << "Case 2: defect pairs (" << defect.id() << "," << closest_defect1_id1 << ")"
		     << " and (" << defect.id() << "," << closest_defect2_id2 << ")" << std::endl;
    }
    else {
      child2.associate(closest_defect2_id1);
      LAROCV_DEBUG() << "Case 2: defect pairs (" << defect.id() << "," << closest_defect1_id1 << ")"
		     << " and (" << defect.id() << "," << closest_defect2_id1 << ")" << std::endl;
    }    
    return;
  }
  
  larocv::data::ClusterCompound DefectBreaker::BreakContour(const larocv::GEO2D_Contour_t& in_ctor) {
    
    ////////////////////////////////////////////
    // Take a single contour, find the defect on the side with 
    // longest hull edge. Break the contour into two. Re insert into queue
    // repeat operation on queue
    
    larocv::data::ClusterCompound cluscomp;
    
    //contours which may be broken up are put in this queue
    std::vector<larocv::data::AtomicContour> candidate_ctor_v;
    std::vector<bool> deprecate_ctor_v;
    
    int nbreaks=0;
    
    std::vector<int> hullpts;
    std::vector<::cv::Vec4i> defects;
    std::vector<float> defects_d;
    
    LAROCV_DEBUG() << "Original contour size: " << in_ctor.size() << std::endl;
    
    larocv::data::AtomicContour first_a_ctor;
    first_a_ctor = in_ctor;
    candidate_ctor_v.push_back(first_a_ctor);
    deprecate_ctor_v.push_back(false);
    
    while(nbreaks<=_n_allowed_breaks) {
      
      // get a contour out off the list
      size_t target_ctor_idx=larocv::kINVALID_SIZE;
      for(size_t candidate_idx=0; candidate_idx < candidate_ctor_v.size(); ++candidate_idx) {
	if(deprecate_ctor_v[candidate_idx]) continue;
	target_ctor_idx = candidate_idx;
	break;
      }
      
      if(target_ctor_idx == larocv::kINVALID_SIZE) {
	LAROCV_DEBUG() << "No more cluster to break..." << std::endl;
	break;
      }
      
      auto& a_ctor = candidate_ctor_v[target_ctor_idx];
      auto& ctor   = a_ctor;
      
      LAROCV_INFO() << "Next target cluster for breaking: candidate index = " << target_ctor_idx
		    << " out of " << candidate_ctor_v.size() << " candidates ... size = " << ctor.size() << std::endl;
      
      //this contour contains only two points. it's a line. should not exist.
      if (ctor.size() <= 2) {
	LAROCV_CRITICAL() << "Contour too small (size<=2)... should not appear!" << std::endl;
	throw larocv::larbys();
	//deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	//continue;
      }
      
      //this contour cannot have a defect by definition
      if (ctor.size() == 3) {
	LAROCV_DEBUG() << "Contour too small to look for a defect (size=3)" << std::endl;
	auto& atomic = cluscomp.make_atom();
	for(auto const& defect_id : a_ctor.associated_defects())
	  atomic.associate(defect_id);
	atomic = ctor;
	LAROCV_DEBUG() << "This atomic id is made: " << atomic.id() << std::endl;
	LAROCV_INFO() << "Found atomic. Currently # atoms = "<< cluscomp.get_atoms().size()
		      << " ... # defects = " << cluscomp.get_defects().size()
		      << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;
      }
      
      // clear the hull and defects for this contour
      hullpts.clear();
      defects.clear();
      defects_d.clear();
      
      // fill the hull and defects
      try{
	fill_hull_and_defects(ctor,hullpts,defects,defects_d);
      }catch(...) {
	LAROCV_NORMAL() << "Failed to compute defects point (cv::convexityDefects)" << std::endl
			<< "Size of hullpts: " << hullpts.size() << std::endl
			<< "Size of defects: " << defects.size() << std::endl
			<< "Size of contour: " << ctor.size() << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;
      }
      
      // filter the hull and defects based on input minimum size
      filter_defects(defects,defects_d,_min_defect_size);
      
      // no defects of minimum size found! the contour is atomic
      if ( ! defects_d.size() ) {
	auto& atomic = cluscomp.make_atom();
	for(auto const& defect_id : a_ctor.associated_defects())
	  atomic.associate(defect_id);
	atomic = ctor;
	LAROCV_DEBUG() << "This atomic id is made: " << atomic.id() << std::endl;
	LAROCV_INFO() << "Found atomic. Currently # atoms = "<< cluscomp.get_atoms().size()
		      << " ... # defects = " << cluscomp.get_defects().size()
		      << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;
      }
      
      //get the chosen edge, currently take the defect facing the longest hull edge
      LAROCV_DEBUG() <<"Choosing edge for ctor..."<<std::endl;
      auto chosen_edge = max_hull_edge(ctor,defects);
      LAROCV_DEBUG() << "This chosen hull edge: " << chosen_edge << std::endl;
      
      LAROCV_DEBUG() <<ctor.size()<<" : "<<chosen_edge[0]<<" and "<<chosen_edge[1]<<std::endl;
      
      auto start = ctor[chosen_edge[0]];
      auto end   = ctor[chosen_edge[1]];
      auto diff  = end - start;
      
      if (std::abs(diff.x)<=3 and std::abs(diff.y)<=3) {
	LAROCV_DEBUG() << "This hull suspect... start : " <<  start << " end : " << end << " diff : " << diff << std::endl;
	auto& atomic = cluscomp.make_atom();
	for(auto const& defect_id : a_ctor.associated_defects())
	  atomic.associate(defect_id);
	atomic = ctor;
	LAROCV_DEBUG() << "This atomic id is made: " << atomic.id() << std::endl;
	LAROCV_INFO() << "Found atomic. Currently # atoms = "<< cluscomp.get_atoms().size()
		      << " ... # defects = " << cluscomp.get_defects().size()
		      << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;
      }
      
      nbreaks+=1;
      LAROCV_DEBUG() << "Breaking! Number of total breaks : " << nbreaks << std::endl;
      
      // segment the hull line and find the line between the hull edge and
      // the defect point, which does not intersect the contour itself, other than at the defect point
      geo2d::Line<float> min_line;
      bool broken=false;
      try{
	min_line = find_line_hull_defect(ctor,chosen_edge);
	broken=true;
      }catch(const larocv::larbys& err) {
	LAROCV_INFO() << "Could not break the contour by moving hull line ..."<<std::endl;
      }
      if(!broken) {
	try{
	  min_line = scan_breaker(ctor, chosen_edge);
	  broken = true;
	}catch(const larocv::larbys& err) {
	  LAROCV_INFO() << "Could not break the contour by scanning the perimeter..." << std::endl;
	}
      }
      
      if(!broken) {
	LAROCV_NORMAL() << "Registering un-breakable contour into an atomic contour list..." << std::endl;
	auto& atomic = cluscomp.make_atom();
	for(auto const& defect_id : a_ctor.associated_defects())
	  atomic.associate(defect_id);
	atomic = ctor;
	LAROCV_DEBUG() << "This atomic id is made: " << atomic.id() << std::endl;
	LAROCV_INFO() << "Found atomic. Currently # atoms = "<< cluscomp.get_atoms().size()
		      << " ... # defects = " << cluscomp.get_defects().size()
		      << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;	   
      }
      
      GEO2D_Contour_t ctor1, ctor2;
      // split the contour into two by this line
      split_contour(ctor,ctor1,ctor2,min_line);
      
      if(ctor1.size()<=2  && ctor2.size()<=2) {
	LAROCV_DEBUG() << "Contour got split into 2 too small contours (size<=2) ... storing original as an atom" << std::endl;
	auto& atomic = cluscomp.make_atom();
	for(auto const& defect_id : a_ctor.associated_defects())
	  atomic.associate(defect_id);
	atomic = ctor;
	LAROCV_DEBUG() << "This atomic id is made: " << atomic.id() << std::endl;
	LAROCV_INFO() << "Found atomic. Currently # atoms = "<< cluscomp.get_atoms().size()
		      << " ... # defects = " << cluscomp.get_defects().size()
		      << std::endl;
	deprecate_ctor_v[target_ctor_idx] = true;
	//ctor.clear();
	continue;
      }
      
      LAROCV_DEBUG() << "Split contour of size: " << ctor.size()
		     << " into " << ctor1.size() << " + " << ctor2.size()
		     << " = " << ctor1.size()+ctor2.size() << std::endl;
      
      if(ctor1.size() >2 && ctor2.size() >2) {
	auto& defect = cluscomp.make_defect();
	defect._pt_start  = ctor[chosen_edge[0]];
	defect._pt_end    = ctor[chosen_edge[1]];
	defect._pt_defect = ctor[chosen_edge[2]];
	defect._dist      = (float)chosen_edge[3] / 256.;
	defect._split_line = min_line;
	
	larocv::data::AtomicContour a_ctor1;
	a_ctor1 = ctor1;
	larocv::data::AtomicContour a_ctor2;
	a_ctor2 = ctor2;
	AssociateDefects(cluscomp,a_ctor,defect,a_ctor1,a_ctor2);
	
	candidate_ctor_v.push_back(a_ctor1);
	candidate_ctor_v.push_back(a_ctor2);
	deprecate_ctor_v.push_back(false);
	deprecate_ctor_v.push_back(false);
	
      } else {
	
	if(ctor1.size()<=2) { LAROCV_DEBUG() << "Broken contour too small (size<=2)... ignoring..." << std::endl; }
	else {
	  larocv::data::AtomicContour a_ctor1;
	  a_ctor1 = ctor1;
	  for(auto const& defect_id : a_ctor.associated_defects())
	    a_ctor1.associate(defect_id);
	  //candidate_ctor_v.emplace_back(std::move(a_ctor1));
	  candidate_ctor_v.push_back(a_ctor1);
	  deprecate_ctor_v.push_back(false);
	}
	
	if(ctor2.size()<=2) { LAROCV_DEBUG() << "Broken contour too small (size<=2)... ignoring..." << std::endl; }
	else {
	  larocv::data::AtomicContour a_ctor2;
	  a_ctor2 = ctor2;
	  for(auto const& defect_id : a_ctor.associated_defects())
	    a_ctor2.associate(defect_id);
	  //candidate_ctor_v.emplace_back(std::move(a_ctor2));
	  candidate_ctor_v.push_back(a_ctor2);
	  deprecate_ctor_v.push_back(false);
	}
      }
      
      deprecate_ctor_v[target_ctor_idx] = true;
      //ctor.clear();
      
      size_t num_remaining_candidates = 0;
      for(auto const& deprecate : deprecate_ctor_v) {
	if(!deprecate) ++num_remaining_candidates;
      }
      LAROCV_DEBUG() << "Valid candidate size " << num_remaining_candidates << " @ end of loop..." << std::endl;
      
    } //end of breaking
    
    //atomic_contour_v is filled, candidate_ctor_v should be all deprecated, if its not, put what's inside into atomics
    LAROCV_DEBUG() << "Checking deprecation" << std::endl;
    for (size_t target_ctor_idx=0; target_ctor_idx<candidate_ctor_v.size(); ++target_ctor_idx) {
      
      auto& candidate_ctor = candidate_ctor_v[target_ctor_idx];
      
      if(deprecate_ctor_v[target_ctor_idx]) continue;
      if(candidate_ctor.size() <= 2) continue;
      
      LAROCV_NORMAL() << "Putting defect ctor of size : " << candidate_ctor.size() << " into atomic_atomic_ctor_v "<< std::endl;
      auto& atomic = cluscomp.make_atom();
      for(auto const& defect_id : candidate_ctor.associated_defects())
	atomic.associate(defect_id);
      atomic = candidate_ctor;
      LAROCV_DEBUG() << "This atomic id is made: " << atomic.id() << std::endl;
      LAROCV_DEBUG() << "Currently # atoms = "<< cluscomp.get_atoms().size()
		     << " ... # defects = " << cluscomp.get_defects().size()
		     << std::endl;
    }
    
    // now perform correlation between defects and atomic contours.
    // this is not done in the loop above because it appears hard in the 1st pass.
    // we implement using a geometrical approximation.
    auto const& ctor_defect_v = cluscomp.get_defects();
    auto const& atomic_ctor_v = cluscomp.get_atoms();
    // make sure logic stands: # defects should be 1 less than # contours
    if(ctor_defect_v.size()+1 != atomic_ctor_v.size()) {
      LAROCV_CRITICAL() << "Logic error ... # of defects = " << ctor_defect_v.size()
			<< " ... "
			<< "# of atoms = " << atomic_ctor_v.size()
			<< std::endl;
      throw larocv::larbys();
    }
    
    
    // Loop over atoms to complete association for defects=>atom
    LAROCV_DEBUG() << "Completing association" << std::endl;
    for(auto const& atom : cluscomp.get_atoms()) {
      
      for(auto const& defect_id : atom.associated_defects())
	
	cluscomp.associate(atom.id(),defect_id);
      
    }
    
    // INFO report
    if(this->logger().level() <= ::larocv::msg::kINFO) {
      std::stringstream ss;
      ss << "Reporting the final set of atoms/defects" << std::endl;
      for(auto const& atom : cluscomp.get_atoms()) {
	
	ss << "    Atom ID " << atom.id() << " size = " << atom.size() << " ... associated defects: ";
	for(auto const& defect_id : atom.associated_defects())
	  
	  ss << defect_id << " @ " << cluscomp.get_defect(defect_id)._pt_defect << " ... ";
	
	ss << std::endl;
      }
      LAROCV_INFO() << std::string(ss.str()) << std::endl;
    }
    
    return cluscomp;
  }
  
  
  double DefectBreaker::DistanceAtom2Point(const larocv::data::AtomicContour& atom, const geo2d::Vector<float>& point) const
  {
    geo2d::Vector<float> pt;
    double point_dist = 0;
    double closest_dist = 1e10;
    for(auto const& contour_pt : atom) {
      pt.x = contour_pt.x;
      pt.y = contour_pt.y;
      point_dist = geo2d::dist2(pt,point);
      if(point_dist < closest_dist) closest_dist = point_dist;
    }
    return sqrt(closest_dist);
  }
  
  
  std::vector<size_t> DefectBreaker::OrderAtoms(const larocv::data::ClusterCompound& cluster,
						const geo2d::Vector<float>& start_) const
  {
    const auto& vtx2d = start_; 
    LAROCV_DEBUG() << "called for ClusterCompound ID " << cluster.id()
		   << " ... 2D vtx @ " << vtx2d << std::endl;
    // goal: order atoms in the cluster from start to the end of a trajectory
    // 0) find the atom closest to 2d vtx
    // 1) follow the association chain with defects to order the rest
    
    // Retrieve atoms
    auto const& atoms   = cluster.get_atoms();
    // Prepare result
    std::vector<size_t> result_v;
    result_v.reserve(atoms.size());
    
    // If no atoms, just return empty vector
    if(atoms.empty()) return result_v;
    result_v.resize(1);
    
    // Loop over atoms to find the closest to the 2D vertex on this plane
    double closest_atom_dist2vtx = 1e9;
    double start_atom_dist2vtx = 1e9;
    size_t closest_atom_id = larocv::kINVALID_SIZE;
    size_t start_atom_id = larocv::kINVALID_SIZE;
    double atom_dist2vtx = 0;
    LAROCV_DEBUG() << "Looping over " << atoms.size() << " atoms to find the closest one to the vtx" << std::endl;
    for(auto const& atom : atoms) {
      atom_dist2vtx = DistanceAtom2Point(atom, vtx2d);
      LAROCV_DEBUG() << "Atom " << atom.id()
		     << " distance = " << atom_dist2vtx
		     << " # defect = " << atom.associated_defects().size()
		     << std::endl;
      if(atom_dist2vtx < closest_atom_dist2vtx) {
	closest_atom_dist2vtx = atom_dist2vtx;
	closest_atom_id = atom.id();
      }
      if(atom.associated_defects().size() < 2) {
	if(atom_dist2vtx < start_atom_dist2vtx) {
	  start_atom_dist2vtx = atom_dist2vtx;
	  start_atom_id = atom.id();
	}
      }
    }
    LAROCV_DEBUG() << "Closest atom ID " << closest_atom_id << " distance " << closest_atom_dist2vtx << std::endl;
    LAROCV_DEBUG() << "Start   atom ID " << start_atom_id << " distance " << start_atom_dist2vtx << std::endl;
    
    result_v[0] = start_atom_id;
    
    // Do book keeping of used atom IDs
    size_t last_atom_id = result_v[0];
    std::vector<size_t> used_atom_id_v(atoms.size(),false);
    used_atom_id_v[last_atom_id] = true;
    
    // Loop over the rest of atoms to construct the chain
    while(result_v.size() < atoms.size()) {
      
      // retrieve last atom
      auto const& last_atom = cluster.get_atom(last_atom_id);
      // retrieve defects' id array
      auto const& defect_id_s = last_atom.associated_defects();
      
      LAROCV_DEBUG() << "Searching next atom (current ID=" << last_atom.id()
		     << ") asociated # defects = " << defect_id_s.size() << std::endl;
      
      // loop over associated defects, find the next associated atom
      size_t next_atom_id = larocv::kINVALID_SIZE;
      for(auto const& defect_id : defect_id_s) {
	LAROCV_DEBUG() << "  associated defect ID " << defect_id << std::endl;
	// retrieve a defect
	auto const& defect = cluster.get_defect(defect_id);
	// loop over associated atoms' id array
	for(auto const& ass_atom_id : defect.associated_atoms()) {
	  LAROCV_DEBUG() << "    association defect " << defect_id << " => atom " << ass_atom_id << std::endl;
	  // if found in "used atom id list", ignore
	  if(used_atom_id_v[ass_atom_id]) continue;
	  // else this atom is our target. make sure there's no duplication
	  if(next_atom_id != larocv::kINVALID_SIZE) {
	    LAROCV_CRITICAL() << "Detected a logic inconsistency in atom/defect association (duplicated association)" << std::endl;
	    throw larocv::larbys();
	  }
	  next_atom_id = ass_atom_id;
	  LAROCV_DEBUG() << "    ... found next atom ID " << next_atom_id << std::endl;
	  
	  // Note one can just "break" the loop here to save time.
	  // In this attempt we won't break to explicitly check the association's validity.
	}
	
      } // end looping over defects for this atom
      
      // make sure next atom is found
      if(next_atom_id == larocv::kINVALID_SIZE) {
	LAROCV_CRITICAL() << "Detected a logic inconsistency in atom/defect association (next atom not found)" << std::endl;
	throw larocv::larbys();
      }
      used_atom_id_v[next_atom_id] = true;
      result_v.push_back(next_atom_id);
      last_atom_id = next_atom_id;
    } // end looping over to order atoms
    
    return result_v;
  }
  
  std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > >
  DefectBreaker::AtomsEdge(const larocv::data::ClusterCompound& cluster,
			   const geo2d::Vector<float>& start_,
			   const std::vector<size_t> atom_order_v) const
  {
    const auto& vtx2d = start_;
    // goal: find the start/end of each atom
    // 0) loop over ordered atoms starting from the closest to the vertex
    // 1) for each atom compute the end by comparing w.r.t. the end of the last atom (1st one compare w/ vtx)
    // 2) for each atom compute the start by comparing its own end
    if(atom_order_v.size() != cluster.get_atoms().size()) {
      LAROCV_CRITICAL() << "Atom's ordering index array length != # atoms!" << std::endl;
      throw larocv::larbys();
    }
    
    // prepare result container
    std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > > result_v;
    result_v.resize(atom_order_v.size());
    // loop over atoms
    auto const& atoms = cluster.get_atoms();
    geo2d::Vector<float> pt, start, end, last_end;
    last_end = vtx2d;
    for(size_t order_index=0; order_index < atom_order_v.size(); ++order_index) {
      auto const& atom_index = atom_order_v[order_index];
      auto const& atom = atoms.at(atom_index);
      LAROCV_DEBUG() << "Inspecting order " << order_index
		     << " atom ID " << atom_index << std::endl;
      // loop over points to find the end
      double max_dist = 0;
      for(auto const& ctor_pt : atom) {
	pt.x = ctor_pt.x;
	pt.y = ctor_pt.y;
	auto dist = geo2d::dist(pt,last_end);
	if(dist > max_dist) {
	  max_dist = dist;
	  end = pt;
	  LAROCV_DEBUG() << "New end point candidate @ " << pt << " dist = " << max_dist << std::endl;
	}
      }
      // loop over points to find the start
      max_dist = 0;
      for(auto const& ctor_pt : atom) {
	pt.x = ctor_pt.x;
	pt.y = ctor_pt.y;
	auto dist = geo2d::dist(pt,end);
	if(dist > max_dist) {
	  max_dist = dist;
	  start = pt;
	  LAROCV_DEBUG() << "New start point candidate @ " << pt << " dist = " << max_dist << std::endl;
	}
      }
      // record start/end + update last_end
      last_end = end;
      result_v[atom_index].first  = start;
      result_v[atom_index].second = end;
      
      if(this->logger().level() <= larocv::msg::kINFO) {
	std::stringstream ss;
	ss << "Atom ID " << atom.id() << " order index " << order_index
	   << " start @ " << start
	   << " end @ " << end
	   << " # defects " << atom.associated_defects().size();
	for(auto const& defect_id : atom.associated_defects())
	  ss << " ... defect " << defect_id << " @ " << cluster.get_defect(defect_id)._pt_defect;
	LAROCV_INFO() << std::string(ss.str()) << std::endl;
      }
    }
    
    return result_v;
  }
  
  
  geo2d::Vector<float> DefectBreaker::ChooseStartPoint(larocv::data::ClusterCompound& cluscomp) {
    
    geo2d::Vector<float> start(larocv::kINVALID_FLOAT,larocv::kINVALID_FLOAT);
    
    if (cluscomp.size() > 1) {
      
      const larocv::data::AtomicContour* start_atomic = nullptr;
      
      size_t ass_defect_id = larocv::kINVALID_SIZE;
      
      //return the first atomic you see with 1 defect
      for(const auto& atomic_contour : cluscomp) {
	const auto& ass_defects_id_v = atomic_contour.associated_defects();
	
	//find the first contour that has a single defect
	if (ass_defects_id_v.size() == 1) {
	  start_atomic  = &atomic_contour;
	  ass_defect_id = ass_defects_id_v[0];
	  break;
	}
	
      }
      
      if ( !start_atomic)
	throw larocv::larbys("Could not choose starting atomic based on ass defects");
      
      const auto& pt_defect = cluscomp.get_defect(ass_defect_id);
      
      float max_dist = 0;
      //look for the farthest points away on the contour
      
      for(const auto& pt : *start_atomic) {
	geo2d::Vector<float> pt_f(pt.x,pt.y);
	auto dist = geo2d::dist(pt_f,pt_defect._pt_defect);
	if (dist > max_dist) {
	  max_dist = dist;
	  start = pt_f;
	}
      }
      
    }  else { // single cluster
      
      //Find the point farthest lowest in Y (wire)
      float maxy = larocv::kINVALID_FLOAT;
      for(const auto& pt : cluscomp.at(0))  {
	if (pt.y < maxy) {
	  maxy = pt.y;
	  start.x = (float) pt.x;
	  start.y = (float) pt.y;
	}
      }
      
      if (maxy == larocv::kINVALID_FLOAT)
	throw larocv::larbys("No maxy found");
    }
    
    
    if ( start.x == larocv::kINVALID_SIZE)
      throw larocv::larbys("Could not choose starting edge");
    
    return start;
  }
  
  larocv::data::ClusterCompound DefectBreaker::SplitContour(const GEO2D_Contour_t& in_ctor) {
    
    // break, create atomics, associate defects
    auto cluscomp = BreakContour(in_ctor);
    
    //need to determine starting point for ordering, find the atomic which has 1 associated defect
    auto start = ChooseStartPoint(cluscomp);
    
    // order atomics
    auto const ordered_atom_id_v = OrderAtoms(cluscomp,start);
    
    // get start/end
    auto atom_edges_v = AtomsEdge(cluscomp, start, ordered_atom_id_v);
    
    //do something with start end -- ensure each atomic has start and end point...
    for(uint atom_idx=0; atom_idx<cluscomp.size(); ++atom_idx) {
      auto& atom = cluscomp.at(atom_idx);
      auto& edges = atom_edges_v[atom_idx];
      atom.add_edge(edges.first);
      atom.add_edge(edges.second);
    }
    
    return cluscomp;
    
  }
}
#endif
  
