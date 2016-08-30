#ifndef __FLASHLIGHTMERGE_CXX__
#define __FLASHLIGHTMERGE_CXX__

#include "FlashlightMerge.h"
#include "PCAUtilities.h"

namespace larocv{

  void FlashlightMerge::_Configure_(const ::fcllite::PSet &pset)
  {
    _N = pset.get<int>("Keep");
    // _trilen   = pset.get<float>("TriLen");
    // _triangle = pset.get<float>("TriAngle"); //get it...
      
  }

  Cluster2DArray_t FlashlightMerge::_Process_(const larocv::Cluster2DArray_t& clusters,
						 const ::cv::Mat& img,
						 larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    /* Once again, copy Ariana. Lets take the TWO (fcl...) largest clusters no matter what the size
       and drop the other contours in ditch effort to scoop up charge. We will rely on the PCA for
       the over all direction and trust the start point. I hope to build a polygon that looks
       like a flashlight. A bounding rectangle at the start, then trapezoid at the end. In this scheme
       our contour should be convex and not require sophisticated untangling
    */

    
    Cluster2DArray_t ccopy = clusters; //we need to make a copy to do the sorting
    Cluster2DArray_t oclusters;

    auto n = _N <= clusters.size() ? _N : clusters.size();
    oclusters.resize(n);
      
    //sort the clusters by number of hits
    std::sort(std::begin(ccopy),std::end(ccopy),
	      [](const Cluster2D& c1, const Cluster2D& c2)
	      { return c1._numHits > c2._numHits; } );
    

    //take it from the copy and put it in ocluster to be manipulated
    for(unsigned i = 0; i < n; ++i) std::swap(oclusters[i],ccopy[i]);

    for(auto& ocluster : oclusters) {

      
      auto ostart  = roi.roivtx_in_image(meta);
      auto oend    = ocluster._startPt;
      
      auto& odir   = ocluster._eigenVecFirst;
      
      // We now have the top N clusters, lets set the direction of the PCA i.e from
      // start to end point
      if  ( ostart.x < oend.x )  {
        if ( odir.x < 0 ) {
          odir.x *= -1.0;
          odir.y *= -1.0;
        }
      }
      else {
        if ( odir.x > 0 ) {
          odir.x *= -1.0;
          odir.y *= -1.0;
        }
      }
    
    
      //Get the minimum rectangle (should already exist in Cluster2D)
      //make a copy because I want to sort it
      auto minrect = ocluster._minAreaRect; 
      
      // find the two verticies that are farthest from the start point, this is other edge
      // just a note that ::cv:: hates ::cv::Point2f pointers what the hell, make wasteful copy for now
      ::cv::Point2f stop, sbot, etop, ebot;

      //sort based on distance to the ostart, far away means it's on the other side
      //if I was smart I would check the distance to the end point but im stupid
    
      std::sort(std::begin(minrect),std::end(minrect),
		[&oend](const ::cv::Point2f& p1, const ::cv::Point2f& p2)
		{ return dist(p1,oend) > dist(p2,oend); } );


      //set the top and bottom end
      etop = minrect[0].y > minrect[1].y ? minrect[0] : minrect[1];
      ebot = etop == minrect[0] ? minrect[1] : minrect[0];

      //set the top and bottom start
      stop = minrect[2].y > minrect[3].y ? minrect[2] : minrect[3];
      sbot = stop == minrect[2] ? minrect[3] : minrect[2];

      //now we have the top and bottom points to do the projection outwards.
      auto dx =  etop.x - stop.x; 
      auto dy =  etop.y - stop.y;
      
      float odout  = 50.0;
      float bangle = 20 * 3.14159/180.;

      auto& bbox = ocluster._minAreaBox;
      float oangle = bbox.angle * 3.14159/180.0;

      if (bbox.size.width < bbox.size.height)
	oangle += 90 * 3.14159/180.0;

      ::cv::Point2f p1,p2;


      // im going to be very explicit since i've screwed up many times
      float p1_cos_a = oangle + bangle;
      float p1_sin_a = oangle + bangle;

      float p2_cos_a = oangle - bangle;
      float p2_sin_a = oangle - bangle;

      auto xx1 = odout*std::cos(p1_cos_a);
      auto yy1 = odout*std::sin(p1_sin_a);

      auto xx2 = odout*std::cos(p2_cos_a);
      auto yy2 = odout*std::sin(p2_sin_a);
	    
      p1 = ::cv::Point2f(xx1,yy1);
      p2 = ::cv::Point2f(xx2,yy2);

      if ( (dx < 0 and dy < 0) or (dx < 0 and dy > 0)) { 
	p1 = ebot - p1;     
	p2 = etop - p2;
	std::swap(p1,p2);
      }
      else {
	p1 = p1 + etop;
	p2 = p2 + ebot;
      }
      
      //lets do something idiotic first and just check that we can make this polygon reasonably;
      Contour_t out_contour;
      out_contour.reserve(4);

      out_contour.emplace_back(std::move(sbot));
      out_contour.emplace_back(std::move(stop));

      out_contour.emplace_back(std::move(etop));
      out_contour.emplace_back(std::move(p1));

      out_contour.emplace_back(std::move(p2));
      out_contour.emplace_back(std::move(ebot));


      std::swap(out_contour,ocluster._contour);

      
    }




    if ( meta.debug() ) {

      std::stringstream ss1, ss2;

      ::larlite::user_info uinfo{};
      ss1 << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_clusters";
      uinfo.store("ID",ss1.str());

      ss1.str(std::string());
      ss1.clear();

      LAROCV_DEBUG((*this)) << "Writing debug information for " << oclusters.size() << "\n";
    
      for(size_t i = 0; i < oclusters.size(); ++i){

	const auto& cluster = oclusters[i];

	////////////////////////////////////////////
	/////Contour points
	
	ss1  <<  "ClusterID_" << cluster.ClusterID() << "_contour_x";
	ss2  <<  "ClusterID_" << cluster.ClusterID() << "_contour_y";

	for(const auto& point : cluster._contour) {
	  double x = meta.XtoTimeTick(point.x);
	  double y = meta.YtoWire(point.y);
	
	  uinfo.append(ss1.str(),x);
	  uinfo.append(ss2.str(),y);
	}
	
	ss1.str(std::string());
	ss1.clear();
	ss2.str(std::string());
	ss2.clear();
      }

      meta.ev_user()->emplace_back(uinfo);
    }

    
    return oclusters;
  }
  
}
#endif
