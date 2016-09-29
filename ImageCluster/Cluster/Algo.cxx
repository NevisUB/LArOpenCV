#ifndef __ALGO_CXX__
#define __ALGO_CXX__

#include "Algo.h"

namespace larocv {


  void Algo::_Configure_(const ::fcllite::PSet &pset)
  {
  }

  larocv::Cluster2DArray_t Algo::_Process_(const larocv::Cluster2DArray_t& clusters,
					   const ::cv::Mat& img,
					   larocv::ImageMeta& meta,
					   larocv::ROI& roi)
  {

    if ( clusters.size() )
      throw larbys("This algo can only be executed first in algo chain!");

    ///////////////////////////////////////////////
    //Threshold the input image to certain ADC value, this is the working image
    int THRESH_LOWER=20;
    ::cv::Mat img_thresh_m;
    threshold(img, img_thresh_m,THRESH_LOWER,255,0);
    
    ///////////////////////////////////////////////
    //Threshold the input image to HIP ADC value, this is our HIP
    int HIP_LEVEL=80;
    ::cv::Mat hip_thresh_m;
    threshold(img, hip_thresh_m,HIP_LEVEL,255,0);

    ///////////////////////////////////////////////
    //Contour finding on the HIP
    ContourArray_t hip_ctor_v;
    std::vector<::cv::Vec4i> hip_cv_hierarchy_v;
    hip_ctor_v.clear();
    hip_cv_hierarchy_v.clear();
    
    ::cv::findContours(hip_thresh_m,hip_ctor_v,hip_cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);

    ///////////////////////////////////////////////
    //Filter the HIP contours to a minimum size
    int min_hip_size = 6;
    ContourArray_t hip_ctor_v_tmp;
    hip_ctor_v_tmp.reserve(hip_ctor_v.size());
    
    for (const auto& hip_ctor : hip_ctor_v)
      if ( hip_ctor.size() > min_hip_size)
	hip_ctor_v_tmp.emplace_back(hip_ctor);

    // swap them b.c. I don't like _tmp stuffs
    std::swap(hip_ctor_v,hip_ctor_v_tmp);
    
    ///////////////////////////////////////////////
    //Masking away the hip in the original image

    //lets find the non zero pixels hiding in the hip contours. Lets mask them out of the original image
    //OK this means lets identify the point inside the HIP contour, then go straight into the
    //original image and zero that guy out to create a MIP image
    ::cv::Mat mip_thresh_m = img_thresh_m.clone();

    Contour_t all_locations;
    ::cv::findNonZero(mip_thresh_m, all_locations); // get the non zero points
    
     for( const auto& loc: all_locations ) {
       for( size_t i = 0; i < hip_ctor_v.size(); i++ ) {
	 if ( ::cv::pointPolygonTest(hip_ctor_v[i],loc,false) < 0 )  // not inside
	   continue;
	 
	 //something here can we zero out in the MIP image
	 
	 mip_thresh_m.at<uchar>(loc.y, loc.x) = 0;
       }
     }

     ///////////////////////////////////////////////
     //run the contour finder on the MIP

     ContourArray_t mip_ctor_v;
     std::vector<::cv::Vec4i> mip_cv_hierarchy_v;
     mip_ctor_v.clear();
     mip_cv_hierarchy_v.clear();
     
     ::cv::findContours(mip_thresh_m,mip_ctor_v,mip_cv_hierarchy_v,
			CV_RETR_EXTERNAL,
			CV_CHAIN_APPROX_SIMPLE);     

     ///////////////////////////////////////////////
     //Filter the MIP contours to a minimum size, the ones that are
     int min_mip_size = 6;
     ContourArray_t mip_ctor_v_tmp;
     mip_ctor_v_tmp.reserve(mip_ctor_v.size());
     
     for (const auto& mip_ctor : mip_ctor_v)
       if (mip_ctor.size() > min_mip_size)
	 mip_ctor_v_tmp.emplace_back(mip_ctor);
     
     // swap them b.c. I don't like _tmp stuffs either
     std::swap(mip_ctor_v,mip_ctor_v_tmp);
     
     ///////////////////////////////////////////////
     //unify the contours
     ContourArray_t all_ctor_v;
     all_ctor_v.reserve(mip_ctor_v.size() + hip_ctor_v.size());
     all_ctor_v.insert( all_ctor_v.end(), mip_ctor_v.begin(), mip_ctor_v.end());
     all_ctor_v.insert( all_ctor_v.end(), hip_ctor_v.begin(), hip_ctor_v.end() );

     ///////////////////////////////////////////////
     //convex hull on each contour, lets save the output
     
     //with the MIP and HIP clusters in hand, lets calculate convex hull per cluster and find defects
     std::vector<std::vector<int> > hullpts_v;
     hullpts_v.resize(all_ctor_v.size());

     std::vector<std::vector<::cv::Vec4i> > defects_v;
     defects_v.resize(all_ctor_v.size());

     std::vector<std::vector<double> > defects_dist_v;
     defects_dist_v.resize(all_ctor_v.size());
     
     for (unsigned i = 0; i < all_ctor_v.size(); ++i ) {

       auto& cluster   = all_ctor_v[i];
       auto& hullpts   = hullpts_v[i];
       auto& defects   = defects_v[i];
       auto& defects_d = defects_dist_v[i];

       //make the hull
       ::cv::convexHull(cluster, hullpts);
       //close the hull up
       hullpts.push_back(hullpts.at(0));

       //make the contour that olds actual hull points, instead of indicies
       std::vector<::cv::Point> hullcontour;
       hullcontour.resize(hullpts.size()-1);
       
       for(unsigned u=0;u<hullcontour.size();++u) 
	 hullcontour[u] = cluster[ hullpts[u] ];

       //compute the defects
       ::cv::convexityDefects(cluster,hullpts,defects);
       
       //lets put the defects into bonafide vector of doubles
       defects_d.resize( defects.size() );
       
       for( int j = 0; j < defects.size(); ++j )
	 defects_d[j]  = ( ( (float) defects[j][3] ) / 256.0 ); 
       
     }

     ////////////////////////////////////////////
     //Create the result contour array vector for output 
     Cluster2DArray_t ocluster_v;
     ocluster_v.resize(all_ctor_v.size());

     for(unsigned i=0;i<all_ctor_v.size();++i) 
       ocluster_v[i]._contour = all_ctor_v[i];

     ////////////////////////////////////////////
     //Fill cluster parameters
     FillClusterParams(ocluster_v,img);

     ////////////////////////////////////////////
     //Now lets go through and do the breaking

     // array for each contour
     // array for set of defects per contour
     // pair of location @ defect point, and intersection of minimum line to convex hull
     std::vector< std::vector<std::pair<::cv::Point2f,::cv::Point2f> > >split_defects_v;
     split_defects_v.resize(ocluster_v.size());
     
     float MIN_DEFECT_SIZE=5;
     
     for(unsigned i=0;i<ocluster_v.size();++i) {
       auto& contour   = ocluster_v[i]._contour;
       auto& defects   = defects_v[i];
       auto& defects_d = defects_dist_v[i];
       auto& split_defect = split_defects_v[i];
       
       for(unsigned ig=0;ig<defects_d.size();++ig) {

	 if (defects_d[ig] < MIN_DEFECT_SIZE)
	   continue;
	 
	 
	 //defects gives us indicies of location @ contour
	 ::cv::Point start  = contour.at((size_t)defects[ig][0]);
	 ::cv::Point end    = contour.at((size_t)defects[ig][1]);
	 ::cv::Point far    = contour.at((size_t)defects[ig][2]);

	 ::cv::Point2f kx(start.x,end.x);
	 ::cv::Point2f ky(start.y,end.y);
	 ::cv::Point2d kz(far.x  ,far.y);
	 
	 float x1=kx.x;
	 float x2=kx.y;
	 float x3=kz.x;
	 float y1=ky.x;
	 float y2=ky.y;
	 float y3=kz.y;

	 int npts=50;
	 std::vector<::cv::Point2f> l_;
	 l_.resize(npts);
	 for(int j=0;j<npts;++j){
	   ::cv::Point2f dir_=::cv::Point2f(x2-x1,y2-y1);
	   float k = ( (float) j ) / ( (float) npts );
	   l_[j] = dir_*k+::cv::Point2f(x1,y1);
	 }
       
	 ::cv::Point2f p3(x3,y3);
	 std::vector<std::pair<::cv::Point2f,::cv::Point2f> > ss;
	 ss.reserve(l_.size()/10);
	 
	 for ( const auto& l : l_ ) {

	   ::cv::Point2f p4=intersect(x1,y1,x2,y2,l.x,l.y);
	   auto x4=l.x;
	   auto y4=l.y;
	   int pts_c = contour.size();
	   int inters=0;

	   for(unsigned ix=defects[ig][0];ix<defects[ig][1];++ix) {

	     ::cv::Point2f pt1=contour.at(ix);
	     ::cv::Point2f pt2=contour.at((ix+1)%pts_c);
	     ::cv::Point2f pt3(x3,y3);
	     ::cv::Point2f pt4(x4,y4);

	     inters += four_pt_intersect(pt1,pt2,pt3,pt4);
	      
	     if (inters>=3) break;
	   }//end loop over this region of contour next to hull
	   
	   if(inters>=3) continue;
	   ss.emplace_back(p4,p3);
	 }//end loop over segmented hull edge
	 
	 //get the minimum index....
	 int mindex=-1;
	 float mdist=9e9;
	 for(unsigned ih=0;ih<ss.size();++ih) {
	   auto _p4=ss[ih].first;
	   auto _p3=ss[ih].second;
	   float _d=std::sqrt( std::pow(_p4.x-_p3.x,2) + std::pow(_p4.y-_p3.y,2));
	   if (_d < mdist) {
	     mdist  = _d;
	     mindex = ih;
	   }
	 }
	 if (mindex==-1) { std::cout << " mindex can not be -1!"; throw std::exception(); }

	 split_defect.emplace_back(ss[mindex]);
	 	 
       }//end loop over this defect

     }//end loop over this contour


     //////////////////////////////////////////
     // now that we have the locations on the hull which connect to the defects
     // we can do the splitting

     ContourArray_t ocluster_v_tmp;
     ocluster_v_tmp.reserve(ocluster_v.size());       
     
     for(unsigned i=0;i<ocluster_v.size();++i) {

       auto& contour   = ocluster_v[i]._contour;
       auto& defects   = defects_v[i];
       auto& defects_d = defects_dist_v[i];
       auto& split_defect = split_defects_v[i];

       if (split_defect.size()==0) {
	 ocluster_v_tmp.emplace_back(contour);
	 continue;
       }
       
       std::sort(std::begin(split_defect),std::end(split_defect),
		 [](const std::pair<::cv::Point2f,::cv::Point2f>& p1,
		    const std::pair<::cv::Point2f,::cv::Point2f>& p2)
		 { return p1.second.y < p2.second.y; } );

       std::vector< std::vector<::cv::Point> > split_ctors;
       split_ctors.resize(split_defect.size()+1);
       
       for(unsigned id=0;id<split_defect.size();++id) {

	 //this one is broken
	 auto dp1 = split_defect.at(id);
	 auto dp2 = split_defect.at((id+1)%split_defect.size());
	 
	 //loop over the contour and assign it to top bottom etc
	 //for(const auto& pt : contour) {
	 
	 for(unsigned iq=0;iq<contour.size();++iq) {	 

	   auto pt=contour[iq];
	   
	   if (iq==0) {
	     if (! test_point_above(dp1,pt) ) {
	       split_ctors[id].emplace_back(pt);
	     }
	   }
	   
	   if (iq==split_defect.size()-1) {
	     if ( test_point_above(dp1,pt) ) {
	       split_ctors[id+1].emplace_back(pt);
	       continue;
	     }
	   }
	   
	   if( !test_point_above(dp2,pt) and test_point_above(dp1,pt)) {
	     split_ctors[id+1].emplace_back(pt);
	   }
	   
	 }//end loop over this contour points

       }//end loop over the defects for this contour

       for (const auto& split_ctor : split_ctors)
	 ocluster_v_tmp.emplace_back(split_ctor);

     }//end loop over ocluster_v contours


     Cluster2DArray_t ocluster_v_new;
     ocluster_v_new.resize(ocluster_v_tmp.size());

     for(unsigned ob=0;ob<ocluster_v_tmp.size();++ob)
       ocluster_v_new[ob]._contour = ocluster_v_tmp[ob];

     FillClusterParams(ocluster_v_new,img);
     
     std::swap(ocluster_v,ocluster_v_new);
     return ocluster_v;
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
  
  bool Algo::four_pt_intersect(::cv::Point2f p1,
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
  
}

#endif
