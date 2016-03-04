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
	
      // partition the points, which side has the most [whatever you want]
      float edge1_d = std::sqrt(pow(verts[0].x-verts[1].x,2) + pow(verts[0].y - verts[1].y,2));
      float edge2_d = std::sqrt(pow(verts[2].x-verts[1].x,2) + pow(verts[2].y - verts[1].y,2));
      
      float le_d(0),se_d(0);

      int se[2][2]; //se == short edge
      int le[2][2]; // le == long edge

      if( edge1_d > edge2_d ){

	le[0][0] = 1; le[0][1] = 0;
	le[1][0] = 3; le[1][1] = 2;

	se[0][0] = 2; se[0][1] = 1;
	se[1][0] = 3; se[1][1] = 0;
	
	le_d = edge1_d;
	se_d = edge2_d;
	
      } else {

	le[0][0] = 2; le[0][1] = 1;
	le[1][0] = 3; le[1][1] = 0;

	se[0][0] = 1; se[0][1] = 0;
	se[1][0] = 3; se[1][1] = 2;
	
	le_d = edge2_d;
	se_d = edge1_d;
	
      }

      std::vector<::cv::Point2f> f_half,s_half,ipoints;
      
      for ( unsigned i = 0 ; i < 2; ++i) {
	//get intersection points
	auto sed = Point2D(verts[se[i][0]].x-verts[se[i][1]].x,
			   verts[se[i][0]].y-verts[se[i][1]].y);
	
	auto ipoint = intersection_point(center.x,center.x+sed.x,center.y,center.y+sed.y,
					 verts[le[i][0]].x,verts[le[i][1]].x,
					 verts[le[i][0]].y,verts[le[i][1]].y);

	ipoints.push_back(::cv::Point2f(ipoint.x,ipoint.y));
	ocluster._m.emplace_back(ipoint);
      }


      // It has to be this way: put one of the verticies first
      // then we need to build the box by stepping to the closest point
      // then to the other point on the divide, then back. This is to
      // segment the bounding area into 2 boxes, trust me the following lines work
      // to do the segmentation
      
      f_half.emplace_back(verts[ se[0][0] ].x,
      			  verts[ se[0][0] ].y);
	
	
      s_half.emplace_back(verts[ se[1][0] ].x,
      			  verts[ se[1][0] ].y);

      // check the distance for the first half
      if ( dist(f_half[0].x,ipoints[0].x,f_half[0].y,ipoints[0].y) <
      	   dist(f_half[0].x,ipoints[1].x,f_half[0].y,ipoints[1].y) )
      	{
      	  f_half.emplace_back( ipoints[0] );
      	  f_half.emplace_back( ipoints[1] );
      	  f_half.emplace_back(verts[ se[0][1] ].x,
      			      verts[ se[0][1] ].y);

      	}
      else {
      	f_half.emplace_back( ipoints[1] );
      	f_half.emplace_back( ipoints[0] );	
      	f_half.emplace_back(verts[ se[0][1] ].x,
      			    verts[ se[0][1] ].y);

	  
      }
      
      //check the distance for the second half
      if ( dist(s_half[0].x,ipoints[0].x,s_half[0].y,ipoints[0].y) <
      	   dist(s_half[0].x,ipoints[1].x,s_half[0].y,ipoints[1].y) )
      	{
      	  s_half.emplace_back(ipoints[0]);
      	  s_half.emplace_back(ipoints[1]);
      	  s_half.emplace_back(verts[ se[1][1] ].x,
      			      verts[ se[1][1] ].y);

      	}
      else {
      	s_half.emplace_back(ipoints[1]);
      	s_half.emplace_back(ipoints[0]);	
      	s_half.emplace_back(verts[ se[1][1] ].x,
      			    verts[ se[1][1] ].y);

      }



      
      int tot_charge[2] = {0,0};
      Contour_t insides[2];
      
      for (auto& h : ocluster._insideHits) {
	if ( ::cv::pointPolygonTest(f_half,h,false) >= 0 ) {
	  tot_charge[0] += (int) img.at<uchar>(h.y,h.x);
	  insides[0].push_back(h);
	}

	if ( ::cv::pointPolygonTest(s_half,h,false) >= 0 ) {
	  tot_charge[1] += (int) img.at<uchar>(h.y,h.x);
	  insides[1].push_back(h);
	}
      }
      

      double roi_covs[2] = {0,0};

      roi_covs[0] = roi_cov(insides[0]);
      roi_covs[1] = roi_cov(insides[1]);

      int cidx = 0;

      // charge * linearity
      //if ( tot_charge[0]*roi_covs[0] > tot_charge[1]*roi_covs[1] )

      // which side is more linear? Ok choose this side
      if ( roi_covs[0] > roi_covs[1] )
	{ ocluster._chosen = f_half; cidx = 0;}
      else
	{ ocluster._chosen = s_half; cidx = 1;}


      ::cv::Point* farthest;
      double far = 0;
      for( auto& h : insides[cidx] ){
	auto d = dist(h.x,center.x,h.y,center.y);
	if ( d > far ) { far = d;  farthest = &h; }
      }
      
      ocluster._startPt = Point2D(farthest->x,farthest->y);
      oclusters.emplace_back(ocluster);
      
    }
    
    return oclusters;
  }

}
#endif
