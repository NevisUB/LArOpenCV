#ifndef __DEFECTCLUSTER_CXX__
#define __DEFECTCLUSTER_CXX__

#include "DefectCluster.h"
#include "Core/Geo2D.h"
namespace larocv {

  /// Global larocv::DefectClusterFactory to register AlgoFactory
  static DefectClusterFactory __global_DefectClusterFactory__;

  void DefectCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _min_defect_size      = 5;
    _hull_edge_pts_split  = 50;
    set_verbosity(msg::kDEBUG);    
  }

  larocv::Cluster2DArray_t DefectCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
						    const ::cv::Mat& img,
						    larocv::ImageMeta& meta,
						    larocv::ROI& roi)
  {
    if(this->ID()==0) throw larbys("DefectCluster should not be run 1st!");
    auto& defectcluster_data = AlgoData<larocv::DefectClusterData>();
    defectcluster_data._input_id = (this->ID() - 1);
    ////////////////////////////////////////////
    // Take a single contour, find the defect on the side with 
    // longest hull edge. Break the contour into two. Re insert into master
    // contour list to re-breakup.
    
    //contours which can not be broken up further
    GEO2D_ContourArray_t atomic_ctor_v;
    
    //contours which may be broken up
    GEO2D_ContourArray_t break_ctor_v;
    break_ctor_v.reserve(clusters.size());
    
    //associated indicies
    std::vector<size_t> atomic_ctor_ass_v;
    
    int nbreaks=0;
    
    std::vector<int> hullpts;
    std::vector<::cv::Vec4i> defects;
    std::vector<float> defects_d;

    LAROCV_DEBUG() << "On plane : " << meta.plane() << std::endl;
    //for each initial cluster
    for(unsigned ic=0;ic<clusters.size();++ic) {

       LAROCV_DEBUG()<< "Atomic cluster: " << ic << "\n";

       auto contour = clusters[ic]._contour;

       break_ctor_v.clear();
       
       //put the first contour in the fifo for breaking
       break_ctor_v.emplace_back(std::move(contour));

       while( break_ctor_v.size() != 0 and nbreaks<=10) {

	 // get a contour out off the front
	 auto  ctor_itr = break_ctor_v.begin();
	 auto& ctor     = *ctor_itr;

	 // cv::approxPolyDP(cv::Mat(ctor), ctor, 0.1, true);
	 /*
	 ContourArray_t actor_v;
	 std::vector<::cv::Vec4i> cv_hierarchy_v;
	 actor_v.clear(); cv_hierarchy_v.clear();
	 ::cv::findContours(cv::Mat(ctor),actor_v,cv_hierarchy_v,
			    CV_RETR_EXTERNAL,
			    CV_CHAIN_APPROX_SIMPLE);
	 ctor = actor_v[0];
	 if(actor_v.size()>1) {
	   throw larbys("FUCK");
	 }
	 */
	 
	 LAROCV_DEBUG() << "Break vector size " << break_ctor_v.size() << "... this contour size " << ctor.size() << "\n";
	 LAROCV_DEBUG() << "This contour size : " << ctor.size() << "\n";
	 
	 //this contour contains only two points. it's a line. erase and ignore
	 if (ctor.size() <= 2) {
	     LAROCV_DEBUG() << "Contour too small" << std::endl;
	     break_ctor_v.erase(ctor_itr);
	     continue;
	   }
	 
	 // clear the hull and defects for this contour
	 hullpts.clear();
	 defects.clear();
	 defects_d.clear();
	 
	 // fill the hull and defects
	 fill_hull_and_defects(ctor,hullpts,defects,defects_d);
	 
	 // filter the hull and defects based on fcl minimum size
	 filter_defects(defects,defects_d,_min_defect_size);
	 
	 // no defects of minimum size found! the contour is atomic
	 if ( !defects_d.size() ) {
	   LAROCV_DEBUG() << "No defects found for this ctor" << std::endl;
	   atomic_ctor_v.emplace_back(ctor);
	   atomic_ctor_ass_v.push_back(ic);
	   break_ctor_v.erase(ctor_itr);
	   continue;
	 }

	 nbreaks+=1;
	 LAROCV_DEBUG() << "Breaking. Number of total breaks : " << nbreaks << std::endl;
	 
	 //get the chosen edge, currently take the defect facing the longest hull edge
	 auto chosen_edge = max_hull_edge(ctor,defects);
	 LAROCV_DEBUG() << "HullEdge: " << chosen_edge << std::endl;

	 // Check the sanity of a chosen edge
	 //size_t minidx = std::min(chosen_edge[0],chosen_edge[1]);
	 //size_t maxidx = std::max(chosen_edge[0],chosen_edge[1]);
	 /*
	 if(chosen_edge[2] < minidx || maxidx < chosen_edge[2] ) {
	   LAROCV_CRITICAL() << "HullEdge chosen" << chosen_edge << " has defect outside the range...." << std::endl;
	   std::stringstream ss;
	   for(size_t pt_idx=0; pt_idx < ctor.size(); ++pt_idx) 
	     ss << "Pt " << pt_idx << " " << ctor[pt_idx] << std::endl;
	   LAROCV_CRITICAL() << "Dumping points..." << std::endl
			     << ss.str() << std::endl;
	   throw larbys();
	 }
	 */
	 
	 // segment the hull line and find the line between the hull edge and
	 // the defect point, which does not intersect the contour itself, other than at the defect point
	 geo2d::Line<float> min_line;
	 bool broken=false;
	 try{	   
	   min_line = find_line_hull_defect(ctor,chosen_edge);
	   broken=true;
	 }catch(const larbys& err) {
	   LAROCV_INFO() << "Could not break the contour by moving hull line ..."<<std::endl;
	 }
	 if(!broken) {
	   try{
	     min_line = scan_breaker(ctor, chosen_edge);
	     broken = true;
	   }catch(const larbys& err) {
	     LAROCV_INFO() << "Could not break the contour by scanning the perimeter..." << std::endl;
	   }
	 }
	 if(!broken) {
	   LAROCV_NORMAL() << "Registering un-breakable contour into an 'atomic' contour list..." << std::endl;
	   atomic_ctor_v.emplace_back(ctor);
	   atomic_ctor_ass_v.push_back(ic);
	   break_ctor_v.erase(ctor_itr);
	   continue;	   
	 }
	   
	 LAROCV_DEBUG() << "MinLine: " << min_line.pt << " => " << min_line.dir << std::endl;

	 defectcluster_data._split_line_v_v[meta.plane()].emplace_back(min_line);
	 
	 GEO2D_Contour_t ctor1,ctor2;
	 
	 // split the contour into two by this line
	 split_contour(ctor,ctor1,ctor2,min_line);

	 LAROCV_DEBUG() << "Split contour of size " << ctor.size() << " into " << ctor1.size() << " + " << ctor2.size() << " = " << ctor1.size()+ctor2.size() << std::endl;
	 
	 //remove this contour
	 break_ctor_v.erase(ctor_itr);
	 
	 //put inside
	 break_ctor_v.emplace_back(std::move(ctor1));
	 break_ctor_v.emplace_back(std::move(ctor2));

       }
       
       //atomic_contour_v is filled, break_ctor_v should be clear;
       if ( break_ctor_v.size() ) {
	 LAROCV_CRITICAL() << "Max break condition found, not all contours atomic" << std::endl;

	 for (auto& break_ctor : break_ctor_v) {
	   if (break_ctor.size() == 0) continue;
	   LAROCV_CRITICAL() << "Putting defect ctor of size : " << break_ctor.size() << " into atomic_atomic_ctor_v "<< std::endl;
	   atomic_ctor_v.emplace_back(std::move(break_ctor));
	   atomic_ctor_ass_v.push_back(ic);
	 }

       }
     }
   

    LAROCV_DEBUG() << "Plane " << meta.plane() << " associations : " << std::endl;
    for(unsigned i=0;i<atomic_ctor_ass_v.size();++i)
      LAROCV_DEBUG() << "defect : " << i << " : original idx/total " << atomic_ctor_ass_v[i] << "/" << clusters.size() << std::endl;
     
     defectcluster_data.set_data(clusters.size(),atomic_ctor_v,atomic_ctor_ass_v,meta.plane());
     
     Cluster2DArray_t oclusters_v;
     for(auto& atomic_ctor : atomic_ctor_v) {
       
       Cluster2D ocluster;
       std::swap(ocluster._contour,atomic_ctor);
       
       oclusters_v.emplace_back(std::move(ocluster));
     }
     LAROCV_DEBUG() << "Plane " << meta.plane() << " complete" << std::endl;
     return oclusters_v;
  }

  bool DefectCluster::on_line(const geo2d::Line<float>& line,::cv::Point pt) { 

    float eps = 0.9;

    float xpos = pt.x;
    float ypos = pt.y;
    
    if ((ypos < line.y(xpos) + eps) and
	(ypos > line.y(xpos) - eps))
      return true;
    
    if ((ypos < line.y(xpos + eps)) and
    	(ypos > line.y(xpos - eps)))
    return true;
    
    return false;
    
  }


  void DefectCluster::fill_hull_and_defects(const GEO2D_Contour_t& ctor,
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
  void DefectCluster::filter_defects(std::vector<cv::Vec4i>& defects,
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

  geo2d::Line<float> DefectCluster::find_line_hull_defect(const GEO2D_Contour_t& ctor, cv::Vec4i defect_info) {

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
	
	if(dist_defect_to_hull < 1) throw larbys("Could not locate a breaking line");
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
    return geo2d::Line<float>(geo2d::Vector<float>(0,yinter),geo2d::Vector<float>(1,dir));
  }

  geo2d::Line<float> DefectCluster::scan_breaker(const GEO2D_Contour_t& ctor, cv::Vec4i defect_info) {

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
	  if ( scan_edge2 < 1 ) throw larbys("LOGIC ERROR");
	  if ( ctor_idx_v[(scan_edge2-1)%(ctor.size())] == defect_info[2] )  
	    throw larbys("Scan reached the defect point. Could not locate the breaking line...");
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
  
  void DefectCluster::split_contour(const GEO2D_Contour_t& ctor,
				    GEO2D_Contour_t& ctor1,
				    GEO2D_Contour_t& ctor2,
				    const geo2d::Line<float>& line) {
    
    //get the two intersection points of this contour and this line
    //one of these points is presumably on the contour
    //the other needs to be extended through the contour, and calculated
    
    auto pts_c = ctor.size();

    GEO2D_Contour_t ctor_copy;
    ctor_copy.reserve(ctor.size());

    geo2d::LineSegment<float> ctor_segment(-1,-1,-1,-1);
    geo2d::LineSegment<float> span_segment(-1,-1,-1,-1);

    uint n_inters=0;
    uint n_new_inters=0;
    
    for(unsigned i=0; i<ctor.size(); ++i) {
      
      auto p1 = cv::Point2f(ctor[ i   %pts_c]);
      auto p2 = cv::Point2f(ctor[(i+1)%pts_c]);
      
      ctor_segment.pt1 = p1;
      ctor_segment.pt2 = p2;
      
      // put the first point into the copy contour
      ctor_copy.emplace_back(p1);

      //make a line segment which spans the full range of
      //X from this contour segment
      
      auto min_x = std::min(p1.x,p2.x);
      auto max_x = std::max(p1.x,p2.x);

      if (min_x == max_x) { min_x-=1; max_x+=1; }
      
      geo2d::Vector<float> p3(min_x,line.y(min_x));
      geo2d::Vector<float> p4(max_x,line.y(max_x));

      span_segment.pt1 = p3;
      span_segment.pt2 = p4;
      
      // intersection points for the two segments, one I made
      // and the other is the contour itself
      geo2d::Vector<float> ip(0,0);

      // do they intersect
      if ( ! geo2d::IntersectionPoint(ctor_segment,span_segment,ip) ) continue;

      //they intersect, cast to int
      cv::Point inter_pt(ip.x,ip.y);
      
      //this point is already a part of the contour, don't put a duplicate inside
      if ( inter_pt == cv::Point(p1) or inter_pt == cv::Point(p2) ){
	LAROCV_DEBUG() << "inter_pt " << inter_pt << " is either p1 " << p1 << " or " << p2 << std::endl;
	n_inters++;
	continue;
      }

      LAROCV_DEBUG() << "Inserting a new point: " << inter_pt << std::endl;
      ctor_copy.emplace_back(std::move(inter_pt));// this cv::Point2f has been floored with typecast
      n_new_inters++;
      n_inters++;
    }

    LAROCV_DEBUG() << "New intersections inserted " << n_new_inters << " total intersections " << n_inters << std::endl;
      
    // for each point in the copy contour
    for(auto& pt : ctor_copy) {

      // // point on the line (with tolerance) --  we should put it into both output contours
      // if ( on_line(line,pt) )
      // 	{ ctor1.emplace_back(pt); ctor2.emplace_back(pt); continue; }
      
      // point is above the line -- put in contour 1
      if ( pt.y > line.y(pt.x) )
	{ ctor1.emplace_back(pt); continue; }

      // it's below the line -- put in contour 2
      ctor2.emplace_back(pt);
    }
    /*
    int xmin,xmax,ymin,ymax;

    xmin=xmax=ymin=ymax=-1;
    for(auto const& pt : ctor1) {
      if(xmin<0) {
	xmin = xmax = pt.x;
	ymin = ymax = pt.y;
      }
      if(pt.x < xmin) xmin = pt.x;
      if(pt.x > xmax) xmax = pt.x;
      if(pt.y < ymin) ymin = pt.y;
      if(pt.y > ymax) ymax = pt.y;
    }

    cv::Mat img1(ymax-ymin+1,xmax-xmin+1,CV_8UC1,cvScalar(0.));
    for(auto const& pt : ctor1) 
      img1.at<unsigned char>((size_t)(pt.y-ymin),(size_t)(pt.x-xmin)) = (unsigned char)255;
    auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(5,5));
    ::cv::dilate(img1,img1,kernel,::cv::Point(-1,-1),1);
    ::cv::blur(img1,img1,::cv::Size(5,5));

    cv::imwrite("poop1.png",img1);

    // find contours again
    ContourArray_t actor_v;
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    actor_v.clear(); cv_hierarchy_v.clear();
    ::cv::findContours(img1,actor_v,cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    ctor1 = actor_v[0];
    if(actor_v.size()>1) {
      std::cout<<actor_v.size()<<std::endl;
      throw larbys("FUCK");
    }
    for(auto& pt : ctor1) { pt.x += xmin; pt.y += ymin; }

    xmin=xmax=ymin=ymax=-1;
    for(auto const& pt : ctor2) {
      if(xmin<0) {
	xmin = xmax = pt.x;
	ymin = ymax = pt.y;
      }
      if(pt.x < xmin) xmin = pt.x;
      if(pt.x > xmax) xmax = pt.x;
      if(pt.y < ymin) ymin = pt.y;
      if(pt.y > ymax) ymax = pt.y;
    }

    cv::Mat img2(xmax-xmin+1,ymax-ymin+1,CV_8UC1,cvScalar(0.));
    for(auto const& pt : ctor2) 
      img2.at<unsigned char>((size_t)(pt.y - ymin),(size_t)(pt.x - xmin)) = (unsigned char)255;
    //auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(5,5));
    ::cv::dilate(img2,img2,kernel,::cv::Point(-1,-1),1);
    ::cv::blur(img2,img2,::cv::Size(5,5));
    cv::imwrite("poop2.png",img2);

    // find contours again
    actor_v.clear(); cv_hierarchy_v.clear();
    ::cv::findContours(img2,actor_v,cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    ctor2 = actor_v[0];
    if(actor_v.size()>1) {
      std::cout<<actor_v.size()<<std::endl;
      throw larbys("FUCK");
    }
    for(auto& pt : ctor2) { pt.x += xmin; pt.y += ymin; }

    */
    
    // Do contour finding on these points
    
    /*
    if(ctor1.size()>2 && ctor1.back() != ctor1.front())
      ctor1.push_back(ctor1[0]);
    
    if(ctor2.size()>2 && ctor2.back() != ctor2.front())
      ctor2.push_back(ctor2[0]);
    */

    /*
    for(size_t i=0; i<ctor1.size(); ++i) {
      for(size_t j=i; j<ctor1.size(); ++j) {
	if(ctor1[i] == ctor1[j]) throw larbys("RUI");
      }
    }

    for(size_t i=0; i<ctor2.size(); ++i) {
      for(size_t j=i; j<ctor2.size(); ++j) {
	if(ctor2[i] == ctor2[j]) throw larbys("RUI");
      }
    }
    
    if(ctor1.size()>3)
      geo2d::UntanglePolygon(ctor1);
    if(ctor2.size()>3)
      geo2d::UntanglePolygon(ctor2);
    */
  }

  cv::Vec4i DefectCluster::max_hull_edge(const GEO2D_Contour_t& ctor, std::vector<cv::Vec4i> defects) {

    float max_dist = -1;
    int max_idx    = -1;
    
    for(unsigned i=0;i<defects.size();++i) {

      const auto& d1=defects[i];
      
      float ll=std::sqrt(std::pow(ctor[d1[0]].x-ctor[d1[1]].x,2)
			 +
			 std::pow(ctor[d1[0]].y-ctor[d1[1]].y,2));

      if (ll > max_dist) {
	max_dist = ll;
	max_idx  = i;
      }
      
    }

    if (max_idx == -1) throw larbys("Maximum hull edge could not be calculated\n");
    
    return defects.at(max_idx);
      
  }


}
#endif
