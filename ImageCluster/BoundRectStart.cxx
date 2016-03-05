#ifndef __BOUNDRECTSTART_CXX__
#define __BOUNDRECTSTART_CXX__

#include "BoundRectStart.h"
#include "PCAUtilities.h"
#include "StatUtilities.h"

namespace larcv{

  void BoundRectStart::_Configure_(const ::fcllite::PSet &pset)
  {
    _nHitsCut = pset.get<int> ("NHitsCut");
  }

  Cluster2DArray_t BoundRectStart::_Process_(const larcv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larcv::ImageMeta& meta)
  {
    Cluster2DArray_t oclusters; oclusters.reserve( clusters.size() );

    Contour_t all_locations;
    ::cv::findNonZero(img, all_locations); // get the non zero points
    
    for(size_t k = 0; k < clusters.size(); k++) {
      
      Cluster2D ocluster = clusters[k];       
      auto& contour      = ocluster._contour;


      for(const auto& loc: all_locations) {
	if ( ::cv::pointPolygonTest(ocluster._contour,loc,false) < 0 ) continue;
	ocluster._insideHits.emplace_back(loc.x, loc.y);
      }

      ocluster._numHits = ocluster._insideHits.size();
      
      if ( ocluster._numHits < _nHitsCut ) continue;
      
      // use bounding box considerations to define start point, fill the vertices vector
      auto bbox = ::cv::minAreaRect(contour);
      ::cv::Point2f verticies[4];
      bbox.points(verticies);

      // turn verticies vector into std::vector
      std::vector<::cv::Point2f> v; v.resize(4);
      for(int r=0;r<4;++r) v[r] = verticies[r];

      //give ocluster the minimum area rectangle
      std::swap(v,ocluster._minAreaRect);

      //set the center points as the center of this box
      ocluster._centerPt  = Point2D(bbox.center.x,bbox.center.y);


      ////////////////////////////////

      auto& verts        = ocluster._minAreaRect;
      auto& center       = ocluster._centerPt;
      auto& angle        = bbox.angle;
	
      const static unsigned int N = 8;
      ::cv::RotatedRect divisions[N];

      float blob_angle_deg = bbox.angle;
      double height ( bbox.size.height ), width( bbox.size.width );
      bool swapped = false;

      if (bbox.size.width < bbox.size.height) {
	blob_angle_deg += 90.;
	width  = bbox.size.height;
	height = bbox.size.width;
	swapped = true;
      }

      
      double w_div = width  / (double) N;
      double h_div = height / (double) N;

      double deg2rad = 3.14159 / 180.0;
      
      auto dx = w_div * std::cos(blob_angle_deg * deg2rad);
      auto dy = w_div * std::sin(blob_angle_deg * deg2rad);

      auto cx = center.x - (N/2-0.5) * dx;
      auto cy = center.y - (N/2-0.5) * dy;


      auto& vvv = ocluster._verts;
      vvv.resize(N);
	
      for(unsigned i = 0; i < N; ++i) {
	auto& vv = vvv[i];

	::cv::Size2f s = swapped ? ::cv::Size2f(height,w_div) : ::cv::Size2f(w_div,height);
	
	divisions[i] = ::cv::RotatedRect(::cv::Point2f(cx + i*dx,cy + i*dy),
					 s,
					 angle);
	
	::cv::Point2f ver[4];
	divisions[i].points(ver);
      	v.clear(); v.resize(4);
	for(int r=0;r<4;++r) v[r] = ver[r];
	std::swap(vv,v);
	
      }
      
      
      
      
      // int tot_charge[2] = {0,0};
      // Contour_t insides[2];
      
      // for (auto& h : ocluster._insideHits) {
      // 	if ( ::cv::pointPolygonTest(f_half,h,false) >= 0 ) {
      // 	  tot_charge[0] += (int) img.at<uchar>(h.y,h.x);
      // 	  insides[0].push_back(h);
      // 	}

      // 	if ( ::cv::pointPolygonTest(s_half,h,false) >= 0 ) {
      // 	  tot_charge[1] += (int) img.at<uchar>(h.y,h.x);
      // 	  insides[1].push_back(h);
      // 	}
      // }
      

      // double roi_covs[2] = {0,0};

      // roi_covs[0] = roi_cov(insides[0]);
      // roi_covs[1] = roi_cov(insides[1]);

      // int cidx = 0;

      // // charge * linearity
      // //if ( tot_charge[0]*roi_covs[0] > tot_charge[1]*roi_covs[1] )

      // // which side is more linear? Ok choose this side
      // if ( roi_covs[0] > roi_covs[1] )
      // 	{ ocluster._chosen = f_half; cidx = 0;}
      // else
      // 	{ ocluster._chosen = s_half; cidx = 1;}


      // ::cv::Point* farthest;
      // double far = 0;
      // for( auto& h : insides[cidx] ){
      // 	auto d = dist(h.x,center.x,h.y,center.y);
      // 	if ( d > far ) { far = d;  farthest = &h; }
      // }
      
      // ocluster._startPt = Point2D(farthest->x,farthest->y);
      oclusters.emplace_back(ocluster);
      
    }
    
    return oclusters;
  }
    
  
}
#endif
