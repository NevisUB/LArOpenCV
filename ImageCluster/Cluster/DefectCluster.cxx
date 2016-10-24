#ifndef __DEFECTCLUSTER_CXX__
#define __DEFECTCLUSTER_CXX__

#include "DefectCluster.h"

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
    
    //for each initial cluster
     for(unsigned ic=0;ic<clusters.size();++ic) {

       LAROCV_DEBUG()<< "\t==>On cluster ic: " << ic << "\n";

       auto contour = clusters[ic]._contour;

       break_ctor_v.clear();
       
       //put the first contour in the fifo for breaking
       break_ctor_v.emplace_back(std::move(contour));

       while( break_ctor_v.size() != 0 and nbreaks<=10) {

	 // get a contour out off the front
	 auto  ctor_itr = break_ctor_v.begin();
	 auto& ctor     = *ctor_itr;

	 // cv::approxPolyDP(cv::Mat(ctor), ctor, 0.1, true);

	 LAROCV_DEBUG() << "\t==>break_ctor_v.size() : " << break_ctor_v.size() << "\n";
	 LAROCV_DEBUG() << "\t==>contour size : " << ctor.size() << "\n";
	 LAROCV_DEBUG() << "\t==>contour dump\n";

	 // LAROCV_DEBUG()<<"plt.plot([";
	 // for(auto&c:ctor)LAROCV_DEBUG()<<c.x<<",";
	 // LAROCV_DEBUG()<<"],\n[";
	 // for(auto&c:ctor)LAROCV_DEBUG()<<c.y<<",";
	 // LAROCV_DEBUG()<<"],\n'-o',lw=2)\n";
	 
	 //this contour contains only two points. it's a line. erase and ignore
	 if (ctor.size() <= 2)
	   {  LAROCV_DEBUG() << "\t==>Contour too small\n"; break_ctor_v.erase(ctor_itr); continue; }
	 
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
	   LAROCV_DEBUG() << "\t==> no defects found\n";
	   atomic_ctor_v.emplace_back(ctor);
	   atomic_ctor_ass_v.push_back(ic);
	   break_ctor_v.erase(ctor_itr);
	   continue;
	 }

	 nbreaks+=1;
	 LAROCV_DEBUG() << "\t==>breaking nbreaks : " << nbreaks << "\n";
	 
	 //get the chosen edge, currently take the defect facing the longest hull edge
	 auto chosen_edge = max_hull_edge(ctor,defects);
	 
	 // segment the hull line and find the line between the hull edge and
	 // the defect point, which does not intersect the contour itself, other than at the defect point
	 auto min_line = find_line_hull_defect(ctor,chosen_edge);

	 defectcluster_data._split_line_v_v[meta.plane()].emplace_back(min_line);
	 
	 GEO2D_Contour_t ctor1,ctor2;
	 
	 // split the contour into two by this line
	 split_contour(ctor,ctor1,ctor2,min_line);
	 
	 //remove this contour
	 break_ctor_v.erase(ctor_itr);
	 
	 //put inside
	 break_ctor_v.emplace_back(std::move(ctor1));
	 break_ctor_v.emplace_back(std::move(ctor2));

       }
       
       //atomic_contour_v is filled, break_ctor_v should be clear;
       //if ( break_ctor_v.size() ) throw larbys("Max break condition found, not all contours atomic\n");
       
     }
     
     
     defectcluster_data.set_data(clusters.size(),atomic_ctor_v,atomic_ctor_ass_v,meta.plane());
     
     Cluster2DArray_t oclusters_v;
     for(auto& atomic_ctor : atomic_ctor_v) {
       
       Cluster2D ocluster;
       std::swap(ocluster._contour,atomic_ctor);
       
       oclusters_v.emplace_back(std::move(ocluster));
     }
     
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

  geo2d::Line<float> DefectCluster::find_line_hull_defect(const GEO2D_Contour_t& ctor, cv::Vec4i defect) {

    //number of points in contour
    int pts_c = ctor.size();

    // contour segment
    geo2d::LineSegment<float> ctor_segment(-1,-1,-1,-1);

    // hull defect segment
    geo2d::LineSegment<float> hull_defect_segment(-1,-1,-1,-1);

    auto start = ctor[defect[0]];
    auto end   = ctor[defect[1]];
    auto far   = ctor[defect[2]];
    
    float x1 = start.x;
    float x2 = end.x;
    float x3 = far.x;
    float y1 = start.y;
    float y2 = end.y;
    float y3 = far.y;

    LAROCV_DEBUG() << "\t==>Inspecting hull defect\n";
    LAROCV_DEBUG() << "\t==>start and end of edge\n";
    LAROCV_DEBUG() << "plt.plot(["<<start.x<<","<<end.x<<"],["<<start.y<<","<<end.y<<"],'-',lw=5)\n";

    auto minidx = std::min(defect[0],defect[1]);
    auto maxidx = std::max(defect[0],defect[1]);

    int npts=_hull_edge_pts_split;

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

    // LAROCV_DEBUG() << "\t==>Line points\n";
    // LAROCV_DEBUG() << "plt.plot([";
    // for ( const auto& l : l_ )
    //   LAROCV_DEBUG() << l.x << ",";
    // LAROCV_DEBUG() << "]\n,[";
    // for ( const auto& l : l_ )
    //   LAROCV_DEBUG() << l.y << ",";
    // LAROCV_DEBUG() << "],'o',markersize=5)\n";

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
      for(unsigned ix=minidx;ix<maxidx;++ix) {

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

    if (p4.x == -1 || p4.y == -1) throw larbys("Could not find hull defect, p4 uninitialized\n");
    
    float dir    = (p4.y-p3.y) / (p4.x-p3.x);
    float yinter = -1.0*dir*p4.x+p4.y;
    
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

    LAROCV_DEBUG() << "\t==>Intersections\n";    

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
      if ( inter_pt == cv::Point(p1) or inter_pt == cv::Point(p2) ) continue;

      LAROCV_DEBUG() << "plt.plot(["<< p3.x << "," << p4.x << "],["<<p3.y<<","<<p4.y<<"],'-o',lw=2)\n";
      LAROCV_DEBUG() << "plt.plot(" << ip.x << "," << ip.y << ",'o',markersize=10)\n";

      ctor_copy.emplace_back(std::move(inter_pt));// this cv::Point2f has been floored with typecast
    }

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
