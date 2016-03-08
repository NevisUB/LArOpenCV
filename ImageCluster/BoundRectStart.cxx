#ifndef __BOUNDRECTSTART_CXX__
#define __BOUNDRECTSTART_CXX__

#include "BoundRectStart.h"
#include "PCAUtilities.h"
#include "StatUtilities.h"

namespace larcv{

  void BoundRectStart::_Configure_(const ::fcllite::PSet &pset)
  {
    _nHitsCut  = pset.get<int> ( "NHitsCut"  );
    _nDivWidth = pset.get<int> ( "NDivWidth" );

    if ( _nDivWidth%2 != 0 ) { std::cout << "\n\tNDivWidth must be even!\n"; throw std::exception(); }
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

      // handles to few variables
      auto& verts   = ocluster._minAreaRect;
      auto& center  = ocluster._centerPt;
      auto& angle   = bbox.angle;

      std::vector<::cv::RotatedRect> divisions; divisions.resize(_nDivWidth);

      double angle_deg = bbox.angle;
      double height ( bbox.size.height ), width( bbox.size.width );

      bool swapped = false;

      if (bbox.size.width < bbox.size.height) {
	angle_deg += 90.;
	width  = bbox.size.height;
	height = bbox.size.width;
	swapped = true;
      }

      double N = _nDivWidth;
      
      // divide the box up
      double w_div = width  / (double) N;
      double h_div = height / (double) N;

      // what is the step
      auto dx = w_div * std::cos(angle_deg * _deg2rad);
      auto dy = w_div * std::sin(angle_deg * _deg2rad);

      // where do i start
      auto cx = center.x - (N/2-0.5) * dx;
      auto cy = center.y - (N/2-0.5) * dy;

      auto& vvv = ocluster._verts;
      vvv.clear(); vvv.resize(N);
	
      for(unsigned i = 0; i < N; ++i) {
	auto& vv = vvv[i];

	::cv::Size2f s = swapped ? ::cv::Size2f(height,w_div) : ::cv::Size2f(w_div,height);

	// step and make rotated rect
	divisions[i] = ::cv::RotatedRect(::cv::Point2f(cx + i*dx,cy + i*dy),s,angle);
	
	::cv::Point2f ver[4];
	divisions[i].points(ver);
      	v.clear(); v.resize(4);
	for(int r=0;r<4;++r) v[r] = ver[r];
	std::swap(vv,v);
      }


      // do something with the segments...
      std::vector<Contour_t> insides(N); insides.clear();
      std::vector<double>    tot_charge(N); tot_charge.clear();

      // hits in the cluster...
      for (auto& h : ocluster._insideHits) {      

	//which ones are in this segment
	for( unsigned i = 0; i < divisions.size(); ++i ) {
	  //auto& div = divisions[i];
	  auto& div = ocluster._verts[i];
	  
	  if ( ::cv::pointPolygonTest(div,h,false) >= 0 ) {
	    tot_charge[i] += (int) img.at<uchar>(h.y,h.x);
	    insides   [i].push_back(h);
	  }
	  
	}
      }

      std::vector<double> roi_covs(N); roi_covs.clear();
      for(int i=0;i<N;++i)
	{ roi_covs[i] = roi_cov(insides[i]); ocluster._something.push_back(roi_covs[i]); }
	

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
