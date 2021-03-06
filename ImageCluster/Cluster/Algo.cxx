#ifndef __ALGO_CXX__
#define __ALGO_CXX__

#include "Algo.h"
#include "VicData.h"

namespace larocv {

  void Algo::clear()
  {
    _mip_ctor_v.clear();
    _hip_ctor_v.clear();
    _all_ctor_v.clear();
    _hullpts_v.clear();
    _defects_v.clear();
    _split_defects_v.clear();
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
    
    ::cv::imwrite("shitty.png",img);
    
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
    
    ContourArray_t hip_ctor_v_tmp;
    hip_ctor_v_tmp.reserve(_hip_ctor_v.size());
    
    for (const auto& hip_ctor : _hip_ctor_v)
      if ( hip_ctor.size() > min_hip_size)
	hip_ctor_v_tmp.emplace_back(hip_ctor);
    
    //swap them out -- the thresholded hips and all hips
    //_hip_ctor_v == HIP contours
    std::swap(_hip_ctor_v,hip_ctor_v_tmp);
    
    ///////////////////////////////////////////////
    //Masking away the hip in the original image
    //find the non zero pixels in the hip contours. Mask them out of the MIP image.

    //clone the current mip image
    ::cv::Mat mip_thresh_m = img_thresh_m.clone();
    
    // get the non zero points of the mip
    Contour_t all_locations;
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
     ContourArray_t mip_ctor_v_tmp;
     mip_ctor_v_tmp.reserve(_mip_ctor_v.size());
     
     for (const auto& mip_ctor : _mip_ctor_v)
       if (mip_ctor.size() > min_mip_size)
	 mip_ctor_v_tmp.emplace_back(mip_ctor);

     //swap them out -- the thresholded mips and all mips
     //hip_ctor_v == HIP contours
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
     ContourArray_t _atomic_ctor_v;

     //contours which may be broken up
     ContourArray_t _break_ctor_v;

     //loop through all the contours, and close them up
     _break_ctor_v = _all_ctor_v;

     for(auto& ctor : _break_ctor_v)
       ctor.emplace_back(ctor.at(0));

     //for each contour lets do the breaking
     std::cout << "ctors to break: " << _break_ctor_v.size() << "\n";
     int nbreaks=0;
     while( _break_ctor_v.size() != 0 and nbreaks<=20) {

       //get a contour out off the front
       auto  ctor_itr = _break_ctor_v.begin();
       auto& ctor     = *ctor_itr;
       
       cv::approxPolyDP(cv::Mat(ctor), ctor, 3, true);
  
       std::cout << "\t====>breakctor : " << _break_ctor_v.size() << ", atomic_ctor : " << _atomic_ctor_v.size() << "\n";
       std::cout << "\t===>Found contour of size : " << ctor.size() << "\n";

       if (ctor.size() <= 2) {
	 _break_ctor_v.erase(ctor_itr);
	 continue;
       }
       
       // get the hull and defects for this contour
       std::vector<int> hullpts;
       std::vector<::cv::Vec4i> defects;
       std::vector<float> defects_d;

       fill_hull_and_defects(ctor,hullpts,defects,defects_d);
       filter_defects(defects,defects_d,_min_defect_size);
       
       if ( !defects_d.size() ) { // it's atomic 
	 _atomic_ctor_v.emplace_back(ctor);
	 std::cout << "\t==> not breaking it\n";
	 _break_ctor_v.erase(ctor_itr);
	 continue;
       }

       std::cout << "\t=>Breaking it nbreaks: " << nbreaks << "\n";

       nbreaks+=1;
       
       //get the chosen edge
       auto chosen_edge = max_hull_edge(ctor,defects);
       std::cout << "Chosen edge pts: [" << ctor[chosen_edge[0]].x << "," << ctor[chosen_edge[1]].x << "],["
		 << ctor[chosen_edge[0]].y << "," << ctor[chosen_edge[1]].y << "]\n"; 
       // lets break this line up and find the line between the hull edge and
       // the defect point, which does not intersect the contour itself
       Line min_line = find_line_hull_defect(ctor,chosen_edge);

       Contour_t ctor1;
       Contour_t ctor2;

       split_contour(ctor,ctor1,ctor2,min_line);
       std::cout << "1 and 2: " << ctor1.size() << "," << ctor2.size() << "\n";
       _break_ctor_v.erase(ctor_itr);

       _break_ctor_v.emplace_back(ctor1);
       _break_ctor_v.emplace_back(ctor2);

       std::cout << "~~Next\n";
     }

     std::cout << "Done~\n";


     //debug

     for(auto& bc : _break_ctor_v) _atomic_ctor_v.emplace_back(bc);
     
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
  
  
  bool Algo::test_point_above(std::pair<::cv::Point2f,::cv::Point2f> segment,::cv::Point2f pt) { 
    
    ::cv::Point2f p1=segment.first;
    ::cv::Point2f p2=segment.second;
    
    float slope=(p2.y-p1.y)/(p2.x-p1.x);
    float yinter = -1.0*slope*p1.x+p1.y;
    
    if (pt.y > pt.x*slope+yinter)
      return true;
    
    return false;
    
  }

  bool Algo::on_line(const Line& line,::cv::Point pt) { 

    float eps=0.99;

    float xpos = pt.x;
    float ypos = pt.y;
    
    if ((ypos < xpos*line.slope + line.offset + eps) and
	(ypos > xpos*line.slope + line.offset - eps))
      return true;

    if ((ypos < (xpos+eps)*line.slope + line.offset) and
	(ypos > (xpos-eps)*line.slope + line.offset))
      return true;
    
    return false;
    
  }

  
  bool Algo::test_point_above(const Line& line,::cv::Point pt) { 
    
    if (pt.y > pt.x*line.slope + line.offset)
      return true;
    
    return false;
    
  }

  int Algo::four_pt_intersect(::cv::Point2f p1,
			      ::cv::Point2f p2,
			      ::cv::Point2f p3,
			      ::cv::Point2f p4) {
    
    float p0_x = p1.x;
    float p0_y = p1.y;
    
    float p1_x = p2.x;
    float p1_y = p2.y;
    
    float p2_x = p3.x;
    float p2_y = p3.y;

    float p3_x = p4.x;
    float p3_y = p4.y;
    
    float s1_x = p1_x - p0_x;
    float s1_y = p1_y - p0_y; 
    float s2_x = p3_x - p2_x;
    float s2_y = p3_y - p2_y;
    
    float s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    float t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);
    
    if (s >= 0 and s <= 1 and t >= 0 and t <= 1)
      return 1;
    
    return 0;
  }
  
  ::cv::Point2f Algo::intersect(float x1,float y1,
				float x2,float y2,
				float x3,float y3) {
    
    float k = ((y2-y1) * (x3-x1) - (x2-x1) * (y3-y1)) / (std::pow(y2-y1,2) + std::pow(x2-x1,2));
    float x4 = x3 - k * (y2-y1);
    float y4 = y3 + k * (x2-x1);
    return ::cv::Point2f(x4,y4);
  }

  double Algo::intersect_distance(float x1,float y1,float x2,float y2,float x3,float y3){
    float k = ((y2-y1) * (x3-x1) - (x2-x1) * (y3-y1)) / ( std::pow(y2-y1,2) + std::pow(x2-x1,2));
    float x4 = x3 - k * (y2-y1);
    float y4 = y3 + k * (x2-x1);
    return std::sqrt(std::pow(x4-x3,2)+std::pow(y4-y3,2));
  }
  
  ::cv::Point2f Algo::intersection_point(float x1,float x2,float y1,float y2,float x3,float x4,float y3,float y4){
    float denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    float xx = ( (x1*y2 - y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4) ) / denom;
    float yy = ( (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4) ) / denom;
    return ::cv::Point2f(xx,yy);
  }

  double Algo::distance(float x1,float x2,float y1,float y2){
    return std::sqrt(std::pow(x2-x1,2)+std::pow(y2-y1,2));
  }


  void Algo::FillClusterParams(Cluster2DArray_t& cluster2d_v,const ::cv::Mat& img) {
    
    for (auto& ocluster : cluster2d_v) {
      auto& contour = ocluster._contour;
      ocluster._minAreaBox  = ::cv::minAreaRect(contour);
      ocluster._boundingBox = ::cv::boundingRect(contour);
      auto& min_rect      = ocluster._minAreaBox;
      auto& bounding_rect = ocluster._boundingBox;
      ::cv::Point2f vertices[4];

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
    
    Contour_t all_locations;
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
  
  

  void Algo::fill_hull_and_defects(const Contour_t& ctor,
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
       
    for( int j = 0; j < defects.size(); ++j ) {
	defects_d[j]  = ( ( (float) defects[j][3] ) / 256.0 );
	// std::cout << "j : " << defects[j] << "\n";
      }

    
    std::cout << "\tHull is\n";
    std::cout << "[";
    for(auto hullpt : hullpts) {
      std::cout << ctor[hullpt].x << ",";
    }
    std::cout << "],[";
    for(auto hullpt : hullpts) {
      std::cout << ctor[hullpt].y << ",";
    }
    std::cout << "]\n";

    std::cout << "\tctor is\n";
    std::cout << "[";
    for(auto hullpt : ctor) {
      std::cout << hullpt.x << ",";
    }
    std::cout << "],[";
    for(auto hullpt : ctor) {
      std::cout << hullpt.y << ",";
    }
    std::cout << "]\n";
    
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
      std::cout << "*Defect of size : " << defects_d[i] << " found\n";
      defects_tmp.emplace_back(defects[i]);
      defects_d_tmp.emplace_back(defects_d[i]);
    }


    std::swap(defects_tmp  ,defects);
    std::swap(defects_d_tmp,defects_d);
    
  }

  Line Algo::find_line_hull_defect(const Contour_t& ctor, cv::Vec4i defect) {

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

    std::vector<::cv::Point2f> l_;
    l_.resize(npts);

    ::cv::Point2f dir_(x2-x1,y2-y1);
    
    // make the points on the hull edge
    for(int j=0;j<npts;++j) {
      float k = ( (float) j ) / ( (float) npts );
      l_[j] = dir_*k+::cv::Point2f(x1,y1);
    }
    
    ::cv::Point2f p3(x3,y3);
    ::cv::Point2f p4(-1,-1);

    float mdist = 9.e9;
    for ( const auto& l : l_ ) {

      // pont on hull
      auto x4 = l.x;
      auto y4 = l.y;

      //intersection counter
      int inters = 0;

      //min and max idx from start end
      auto minidx = std::min(defect[0],defect[1]);
      auto maxidx = std::max(defect[0],defect[1]);

      // point on hull as cv
      ::cv::Point2f pt4(x4,y4);

      // loop over portion of contour facing edge
      for(unsigned ix=minidx;ix<maxidx;++ix) {
	
	::cv::Point2f pt1 = ctor.at(ix);
	::cv::Point2f pt2 = ctor.at((ix+1)%pts_c);

	inters += SegmentSegmentTest(pt1,pt2,p3,pt4);
	
	if (inters>1) break;
	
      }

      if(inters>1) continue;
      
      float dd = std::sqrt( std::pow(pt4.x-p3.x,2) + std::pow(pt4.y-p3.y,2) );

      //std::cout << "dd : " << dd << "\n";
      if ( dd > mdist ) continue;

      mdist = dd;

      p4 = pt4;
      
    }

    if (p4.x == -1 || p4.y == -1) throw std::exception();
    
    float slope  = (p4.y-p3.y)/(p4.x-p3.x);
    float yinter = -1.0*slope*p4.x+p4.y;
    
    return Line(slope,yinter);
  }

  void Algo::split_contour(Contour_t& ctor,Contour_t& ctor1,Contour_t& ctor2, const Line& line) {

    //get the two intersection points of this contour and this line
    //one of these points is presumably on the contour
    //the other needs to be calculated

    //get the Y coordinate for the right most X co
    auto cs = ctor.size();

    std::cout << "\t==>called split_contour with size: " << cs << "\n";
    std::cout << "\t==>this line is slope " << line.slope << " with offset " << line.offset << "\n";
    
    Contour_t ctor_copy;
    ctor_copy.reserve(ctor.size());
    
    for(unsigned i=0; i<ctor.size(); ++i) {

      auto& p1 = ctor[ i   %cs];
      auto& p2 = ctor[(i+1)%cs];

      //std::cout << "Checking p1: " << p1 << " , p2: " << p2 << "\n";
      ctor_copy.emplace_back(p1);
      
      float x1=p1.x;
      float x2=p2.x;
      
      float y1=p1.y;
      float y2=p2.y;

      auto min_x = std::min(p1.x,p2.x);
      auto max_x = std::max(p1.x,p2.x);

      if (min_x == max_x) { min_x-=5; max_x+=5; }

      float x3=min_x;
      float y3=line.slope*x3 + line.offset;
      
      float x4=max_x;
      float y4=line.slope*x4 + line.offset;

      ::cv::Point2f p3(x3,y3);
      ::cv::Point2f p4(x4,y4);
      
      if ( ! four_pt_intersect(p1,p2,p3,p4) ) continue;

      //std::cout << "\t==>They intersect\n";
      // they intersect, get their intersection point
      auto ip = intersection_point( x1, x2, y1, y2, x3, x4, y3, y4);

      cv::Point inter_pt(ip.x,ip.y);

      //std::cout << "\t==>its @ " << inter_pt << "\n";
      
      if ( inter_pt == p1 or inter_pt == p2 ) continue;

      //std::cout << "\t==>Putting it in\n";
      ctor_copy.emplace_back(inter_pt);
      
    }

    //std::cout << "ctor.size(): " << ctor.size() << " copy size is " << ctor_copy.size() << "\n";
    
    for(auto& pt : ctor_copy) {

      if ( on_line(line,pt) )
	{ ctor1.emplace_back(pt); ctor2.emplace_back(pt); continue; }
      
      if ( test_point_above(line,pt) )
	{ ctor1.emplace_back(pt); continue; }
      
      ctor2.emplace_back(pt);
      
    }
    
    
  }

  cv::Vec4i Algo::max_hull_edge(const Contour_t& ctor, std::vector<cv::Vec4i> defects) {

    auto ctor_size = ctor.size();

    float max_dist = -1;
    int max_idx = -1;
    
    for(unsigned i=0;i<defects.size();++i) {

      auto d1=defects[i];
      
      float ll=std::sqrt(std::pow(ctor[d1[0]].x-ctor[d1[1]].x,2)+std::pow(ctor[d1[0]].y-ctor[d1[1]].y,2));

      if (ll > max_dist) {
	max_dist=ll;
	max_idx=i;
      }
      
    }

    return defects.at(max_idx);
      
  }

  float Algo::Signed2DTriArea(const ::cv::Point2f& a,const cv::Point2f& b, const cv::Point2f& c)
  {
    return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
  }

  int Algo::SegmentSegmentTest(const ::cv::Point2f& a, const ::cv::Point2f& b, const ::cv::Point2f& c, const ::cv::Point2f& d) {
    float t;
    float a1 = Signed2DTriArea(a,b,d);
    float a2 = Signed2DTriArea(a,b,c);
    if (a1 * a2 < 0.0f) {
      float a3 = Signed2DTriArea(c,d,a);
      float a4 = a3 + a2 - a1;
      if (a3 * a4 < 0.0f) {
	t = a3 / (a3 - a4);
	return 1;
      }
    }
    return 0;
  }

  
}

#endif
