#ifndef __ALGO_CXX__
#define __ALGO_CXX__

#include "Algo.h"
#include "VicData.h"
#include "Core/Geo2D.h"

namespace larocv {

  void Algo::clear()
  {
    _mip_ctor_v.clear();
    _hip_ctor_v.clear();
    _all_ctor_v.clear();
    _hullpts_v.clear();
    _defects_v.clear();
    _ocluster_v.clear();
  }

  void Algo::_Configure_(const ::fcllite::PSet &pset)
  {
    _min_hip_cluster_size = 6;
    _min_mip_cluster_size = 6;
    _min_defect_size      = 5;
    _hull_edge_pts_split  = 50;
    _mip_thresh = 1;
    _hip_thresh = 255;
  }

  larocv::Cluster2DArray_t Algo::_Process_(const larocv::Cluster2DArray_t& clusters,
					   const ::cv::Mat& img,
					   larocv::ImageMeta& meta,
					   larocv::ROI& roi)
  {

    clear();
    if ( clusters.size() )
      throw larbys("This algo can only be executed first in algo chain!");
    
    int THRESH_LOWER = _mip_thresh;
    int HIP_LEVEL    = _hip_thresh;
    
    ///////////////////////////////////////////////
    //Threshold the input image to certain ADC value, this is our MIP
    ::cv::Mat img_thresh_m;
    threshold(img, img_thresh_m,THRESH_LOWER,255,0);
    
    ///////////////////////////////////////////////
    //Threshold the input image to HIP ADC value, this is our HIP
    ::cv::Mat hip_thresh_m;
    threshold(img, hip_thresh_m,HIP_LEVEL,255,0);

    ///////////////////////////////////////////////
    //Contour finding on the HIP
    std::vector<::cv::Vec4i> hip_cv_hierarchy_v;
    _hip_ctor_v.clear();
    hip_cv_hierarchy_v.clear();
    
    ::cv::findContours(hip_thresh_m,_hip_ctor_v,
		       hip_cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    
    ///////////////////////////////////////////////
    //Filter the HIP contours to a minimum size
    int min_hip_size = _min_hip_cluster_size;
    
    //ContourArray_t hip_ctor_v_tmp;
    GEO2D_ContourArray_t hip_ctor_v_tmp;
    hip_ctor_v_tmp.reserve(_hip_ctor_v.size());
    
    for (const auto& hip_ctor : _hip_ctor_v)
      if ( hip_ctor.size() > min_hip_size)
	hip_ctor_v_tmp.emplace_back(hip_ctor);
    
    //swap them out -- the thresholded hips and all hips
    std::swap(_hip_ctor_v,hip_ctor_v_tmp);
    
    ///////////////////////////////////////////////
    //Masking away the hip in the original image
    //find the non zero pixels in the hip contours. Mask them out of the MIP image.

    //clone the current mip image
    ::cv::Mat mip_thresh_m = img_thresh_m.clone();
    
    // get the non zero points of the mip
    std::vector<geo2d::Vector2D<int> > all_locations;
    ::cv::findNonZero(mip_thresh_m, all_locations); 
    
     for( const auto& loc: all_locations ) {
       for( size_t i = 0; i < _hip_ctor_v.size(); i++ ) {
	 if ( ::cv::pointPolygonTest(_hip_ctor_v[i],loc,false) < 0 )  // check if point in HIP contour
	   continue;
	 
	 //Something here. Zero out this pixel.
	 mip_thresh_m.at<uchar>(loc.y, loc.x) = 0;
       }
     }

     ///////////////////////////////////////////////
     //run the contour finder on the MIPs
     std::vector<::cv::Vec4i> mip_cv_hierarchy_v;
     _mip_ctor_v.clear();
     mip_cv_hierarchy_v.clear();
     
     ::cv::findContours(mip_thresh_m,_mip_ctor_v,mip_cv_hierarchy_v,
			CV_RETR_EXTERNAL,
			CV_CHAIN_APPROX_SIMPLE);

     ///////////////////////////////////////////////
     //Filter the MIP contours to a minimum size, the ones that are
     int min_mip_size = _min_mip_cluster_size;
     GEO2D_ContourArray_t mip_ctor_v_tmp;
     mip_ctor_v_tmp.reserve(_mip_ctor_v.size());
     
     for (const auto& mip_ctor : _mip_ctor_v)
       if (mip_ctor.size() > min_mip_size)
	 mip_ctor_v_tmp.emplace_back(mip_ctor);

     //swap them out -- the thresholded mips and all mips
     std::swap(_mip_ctor_v,mip_ctor_v_tmp);
     
     ///////////////////////////////////////////////
     //unify the contours into a single array
     for (const auto& mip_ctor : _mip_ctor_v) {

       if (mip_ctor.size() > 0) 

     	 _all_ctor_v.emplace_back(mip_ctor);
       
     }

     for (const auto& hip_ctor : _hip_ctor_v) {

       if (hip_ctor.size() > 0)

	 _all_ctor_v.emplace_back(hip_ctor);
       
     }

     ////////////////////////////////////////////
     // Take a single contour, find the defect on the side with 
     // longest hull edge. Break the contour into two. Re insert into master
     // contour list to re-breakup.

     //contours which can not be broken up further
     GEO2D_ContourArray_t _atomic_ctor_v;

     //contours which may be broken up
     GEO2D_ContourArray_t _break_ctor_v;

     //loop through all the contours, and close them up
     _break_ctor_v = _all_ctor_v;

     for(auto& ctor : _break_ctor_v)
       ctor.emplace_back(ctor.at(0));

     //for each contour lets do the breaking
     LAROCV_DEBUG((*this)) << "ctors to break: " << _break_ctor_v.size() << "\n";

     int nbreaks=0;

     std::vector<int> hullpts;
     std::vector<::cv::Vec4i> defects;
     std::vector<float> defects_d;

     while( _break_ctor_v.size() != 0 and nbreaks<=20) {

       // get a contour out off the front
       auto  ctor_itr = _break_ctor_v.begin();
       auto& ctor     = *ctor_itr;

       // approximate contour with `more simple` polygon
       cv::approxPolyDP(cv::Mat(ctor), ctor, 3, true);

       LAROCV_DEBUG((*this)) << "\t====>_breakctor : " << _break_ctor_v.size() << ", atomic_ctor : " << _atomic_ctor_v.size() << "\n";
       LAROCV_DEBUG((*this)) << "\t===>Found contour of size : " << ctor.size() << "\n";

       //this contour contains only two points. it's a line. erase and ignore
       if (ctor.size() <= 2)
	 { _break_ctor_v.erase(ctor_itr); continue; }
       
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
	 _atomic_ctor_v.emplace_back(ctor);
	 _break_ctor_v.erase(ctor_itr);
	 LAROCV_DEBUG((*this)) << "\t==> atomic found: not breaking it\n";
	 continue;
       }

       LAROCV_DEBUG((*this)) << "\t=>Breaking it nbreaks: " << nbreaks << "\n";

       nbreaks+=1;
       
       //get the chosen edge, currently take the defect facing the longest hull edge
       auto chosen_edge = max_hull_edge(ctor,defects);
       LAROCV_DEBUG((*this)) << "Chosen edge pts: [" << ctor[chosen_edge[0]].x << "," << ctor[chosen_edge[1]].x << "],["
			   << ctor[chosen_edge[0]].y << "," << ctor[chosen_edge[1]].y << "]\n"; 
       
       // segment the hull line and find the line between the hull edge and
       // the defect point, which does not intersect the contour itself, other than at the defect point
       auto min_line = find_line_hull_defect(ctor,chosen_edge);

       GEO2D_Contour_t ctor1,ctor2;

       // split the contour into two by this line
       split_contour(ctor,ctor1,ctor2,min_line);
       
       LAROCV_DEBUG((*this)) << "1 and 2: " << ctor1.size() << "," << ctor2.size() << "\n";

       //remove this contour
       _break_ctor_v.erase(ctor_itr);

       //put inside
       _break_ctor_v.emplace_back(ctor1);
       _break_ctor_v.emplace_back(ctor2);

       LAROCV_DEBUG((*this)) << "~~Next\n";
     }

     LAROCV_DEBUG((*this)) << "Done~\n";


     // debug
     for(auto& bc : _break_ctor_v) throw larbys("Max break condition found, not all contours atomic\n");//_atomic_ctor_v.emplace_back(bc);
     
     for(auto& atomic_ctor : _atomic_ctor_v) {
       
       Cluster2D ocluster;
       ocluster._contour = atomic_ctor;

       _ocluster_v.emplace_back(ocluster);
     }
     
     FillClusterParams(_ocluster_v,img);

     // auto& mydata = AlgoData<larocv::VicData>();
     // mydata.num_clusters = _ocluster_v.size();
     
     return _ocluster_v;
  }
  

  bool Algo::on_line(const geo2d::Line<float>& line,::cv::Point pt) { 

    float eps = 0.99;

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


  void Algo::FillClusterParams(Cluster2DArray_t& cluster2d_v,const ::cv::Mat& img) {
    
    for (auto& ocluster : cluster2d_v) {
      auto& contour = ocluster._contour;
      ocluster._minAreaBox  = ::cv::minAreaRect(contour);
      ocluster._boundingBox = ::cv::boundingRect(contour);
      auto& min_rect      = ocluster._minAreaBox;
      auto& bounding_rect = ocluster._boundingBox;
      geo2d::Vector2D<float> vertices[4];

      //rotated rect coordinates
      min_rect.points(vertices);
      ocluster._minAreaRect  = {vertices[0],vertices[1],vertices[2],vertices[3]};

      //axis aligned rect coordinates
      ocluster._minBoundingRect = {bounding_rect.br(),bounding_rect.tl()};
      
      auto rect = min_rect.size;
      ocluster._area      = ::cv::contourArea(contour) ;
      ocluster._perimeter = ::cv::arcLength(contour,1);
      ocluster._length    = rect.height > rect.width ? rect.height : rect.width;
      ocluster._width     = rect.height > rect.width ? rect.width  : rect.height;
      ocluster._sumCharge = 0 ;
      ocluster._angle2D   = min_rect.angle;
      ocluster._centerPt  = Point2D(min_rect.center.x,min_rect.center.y);
    }
    
    std::vector<geo2d::Vector2D<int> > all_locations;
    ::cv::findNonZero(img, all_locations); // get the non zero points
    
    for( auto& loc: all_locations ) {
      for( size_t i = 0; i < cluster2d_v.size(); i++ ) {
	
	if ( ::cv::pointPolygonTest(cluster2d_v[i]._contour,loc,false) < 0 ) 
	  continue;
	
	cluster2d_v[i]._insideHits.emplace_back(loc.x, loc.y);
	cluster2d_v[i]._sumCharge += (int) img.at<uchar>(loc.y, loc.x);
      }   
    }
  }
  
  

  void Algo::fill_hull_and_defects(const GEO2D_Contour_t& ctor,
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

    // LAROCV_DEBUG((*this)) << "\tHull is\n";
    // LAROCV_DEBUG((*this)) << "[";
    // for(auto hullpt : hullpts) {
    //   LAROCV_DEBUG((*this)) << ctor[hullpt].x << ",";
    // }
    // LAROCV_DEBUG((*this)) << "],[";
    // for(auto hullpt : hullpts) {
    //   LAROCV_DEBUG((*this)) << ctor[hullpt].y << ",";
    // }
    // LAROCV_DEBUG((*this)) << "]\n";

    // LAROCV_DEBUG((*this)) << "\tctor is\n";
    // LAROCV_DEBUG((*this)) << "[";
    // for(auto hullpt : ctor) {
    //   LAROCV_DEBUG((*this)) << hullpt.x << ",";
    // }
    // LAROCV_DEBUG((*this)) << "],[";
    // for(auto hullpt : ctor) {
    //   LAROCV_DEBUG((*this)) << hullpt.y << ",";
    // }
    // LAROCV_DEBUG((*this)) << "]\n";
    
  }
  void Algo::filter_defects(std::vector<cv::Vec4i>& defects,
			    std::vector<float>& defects_d,
			    float min_defect_size){

    std::vector<cv::Vec4i> defects_tmp;
    defects_tmp.reserve(defects.size());

    std::vector<float> defects_d_tmp;
    defects_d_tmp.reserve(defects.size());

    for(unsigned i=0;i<defects.size();++i) {

      if (defects_d[i] < min_defect_size) continue;
      //LAROCV_DEBUG((*this)) << "*Defect of size : " << defects_d[i] << " found\n";
      defects_tmp.emplace_back(defects[i]);
      defects_d_tmp.emplace_back(defects_d[i]);
    }


    std::swap(defects_tmp  ,defects);
    std::swap(defects_d_tmp,defects_d);
    
  }

  geo2d::Line<float> Algo::find_line_hull_defect(const GEO2D_Contour_t& ctor, cv::Vec4i defect) {

    //number of points in contour
    int pts_c = ctor.size();
    
    auto start = ctor[defect[0]];
    auto end   = ctor[defect[1]];
    auto far   = ctor[defect[2]];
    
    float x1 = start.x;
    float x2 = end.x;
    float x3 = far.x;
    float y1 = start.y;
    float y2 = end.y;
    float y3 = far.y;
    
    int npts=_hull_edge_pts_split;

    std::vector< geo2d::Vector2D<float> > l_;
    l_.resize(npts);

    geo2d::Vector2D<float> dir_(x2-x1,y2-y1);
    
    // make the points on the hull edge
    for(int j=0;j<npts;++j) {
      float k = ( (float) j ) / ( (float) npts );
      l_[j] = dir_*k+geo2d::Vector2D<float>(x1,y1);
    }
    
    geo2d::Vector2D<float> p3(x3,y3);
    geo2d::Vector2D<float> p4(-1,-1);

    float mdist = 9.e9;
    
    for ( const auto& l : l_ ) {

      // number intersections counter
      int inters = 0;

      // min and max index from contour start to end, so we know how to order the loop
      auto minidx = std::min(defect[0],defect[1]);
      auto maxidx = std::max(defect[0],defect[1]);

      // this point on segmented hull
      const auto& pt4 = l;

      // loop over portion of contour facing edge
      for(unsigned ix=minidx;ix<maxidx;++ix) {
	
	geo2d::Vector2D<float> pt1 = ctor[  ix         ];
	geo2d::Vector2D<float> pt2 = ctor[ (ix+1)%pts_c];

	inters += geo2d::SegmentIntersection(pt1,pt2,p3,pt4);

	//There is more than one intersection for this line
	//between the contour and the hull
	if (inters > 1) break;
	
      }

      //the line cut through the contour
      if(inters>1) continue;
      
      float dd = geo2d::dist(p3,pt4);

      if ( dd > mdist ) continue;

      mdist = dd;

      p4 = pt4;
      
    }

    if (p4.x == -1 || p4.y == -1) throw larbys("Could not find hull defect, p4 uninitialized\n");
    
    float dir    = (p4.y-p3.y) / (p4.x-p3.x);
    float yinter = -1.0*dir*p4.x+p4.y;
    
    return geo2d::Line<float>(geo2d::Vector2D<float>(0,yinter),
			      geo2d::Vector2D<float>(1,dir));
  }

  void Algo::split_contour(const GEO2D_Contour_t& ctor,GEO2D_Contour_t& ctor1,GEO2D_Contour_t& ctor2, const geo2d::Line<float>& line) {

    //get the two intersection points of this contour and this line
    //one of these points is presumably on the contour
    //the other needs to be extended through the contour, and calculated

    auto pts_c = ctor.size();

    GEO2D_Contour_t ctor_copy;
    ctor_copy.reserve(ctor.size());
    
    for(unsigned i=0; i<ctor.size(); ++i) {
      
      auto p1 = cv::Point2f(ctor[ i   %pts_c]);
      auto p2 = cv::Point2f(ctor[(i+1)%pts_c]);

      // put the first point into the copy contour
      ctor_copy.emplace_back(p1);

      //make a line segment which spans the full range of
      //X from this contour segment
      
      auto min_x = std::min(p1.x,p2.x);
      auto max_x = std::max(p1.x,p2.x);

      if (min_x == max_x) { min_x-=5; max_x+=5; }

      geo2d::Vector2D<float> p3(min_x,line.y(min_x));
      geo2d::Vector2D<float> p4(max_x,line.y(max_x));

      // intersection points for the two segments, one I made
      // and the other is the contour itself
      geo2d::Vector2D<float> ip(0,0);


      // do they intersect
      if ( ! geo2d::SegmentIntersection(p1,p2,p3,p4,ip) ) continue;

      //they intersect, cast to int
      cv::Point inter_pt(ip.x,ip.y);

      //this point is already a part of the contour, don't put a duplicate inside
      //avoid this step by putting it in, then calling approxPolyDP(...)
      if ( inter_pt == cv::Point(p1) or inter_pt == cv::Point(p2) ) continue;

      ctor_copy.emplace_back(std::move(inter_pt));
    }


    // for each point in the copy contour
    for(auto& pt : ctor_copy) {

      // point on the line (with tolerance) --  we should put it into both output contours
      if ( on_line(line,pt) )
	{ ctor1.emplace_back(pt); ctor2.emplace_back(pt); continue; }

      // point is above the line -- put in contour 1
      if ( pt.y > line.y(pt.x) )
	{ ctor1.emplace_back(pt); continue; }

      // it's below the line -- put in contour 2
      ctor2.emplace_back(pt);
    }
    
  }

  cv::Vec4i Algo::max_hull_edge(const GEO2D_Contour_t& ctor, std::vector<cv::Vec4i> defects) {

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

    if (max_idx == -1) throw larbys("maximum hull edge could not be calculated\n");
    
    return defects.at(max_idx);
      
  }
  
}

#endif
