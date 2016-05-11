#ifndef __TRIANGLECLUSTEREXT_CXX__
#define __TRIANGLECLUSTEREXT_CXX__

#include "TriangleClusterExt.h"
#include "PCAUtilities.h"

namespace larcv{


  
  void TriangleClusterExt::_Configure_(const ::fcllite::PSet &pset)
  {
    _N = pset.get<int>("Keep");
    // _trilen   = pset.get<float>("TriLen");
    // _triangle = pset.get<float>("TriAngle"); //get it...
      
  }

  Cluster2DArray_t TriangleClusterExt::_Process_(const larcv::Cluster2DArray_t& clusters,
						 const ::cv::Mat& img,
						 larcv::ImageMeta& meta)
  {

    /* Once again, copy Ariana. Lets take the TWO (fcl...) largest clusters no matter what the size
       and drop the other contours in ditch effort to scoop up charge. We will rely on the PCA for
       the over all direction and trust the start point. I hope to build a polygon that looks
       like a flashlight. A bounding rectangle at the start, then trapezoid at the end. In this scheme
       our contour should be convex and not require sophisticated untangling
    */

    
    Cluster2DArray_t ccopy = clusters; //we need to make a copy to do the sorting
    Cluster2DArray_t oclusters;
    oclusters.resize(_N);
      
    //sort the clusters by number of hits
    std::sort(std::begin(ccopy),std::end(ccopy),
	      [](const Cluster2D& c1, const Cluster2D& c2)
	      { return c1._numHits > c2._numHits; } );
    

    //take it from the copy and put it in ocluster to be manipulated
    for(unsigned i = 0; i < _N; ++i) std::swap(oclusters[i],ccopy[i]);

    for(auto& ocluster : oclusters) {

      auto& ostart = ocluster._startPt;
      auto& oend   = ocluster._endPt;
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
      // i make a copy because I want to sort it
      auto minrect = ocluster._minAreaRect; //is std::vector<cv::Point2f>
      
      // find the two verticies that are farthest from the start point, this is other edge
      // just a note that ::cv:: hates ::cv::Point2f pointers what the hell, make wasteful copy for now
      ::cv::Point2f stop, sbot, etop, ebot;

      //sort based on distance to the ostart, far away means it's on the other side
      //if I was smart I would check the distance to the end point but im stupid
    
      std::sort(std::begin(minrect),std::end(minrect),
		[&ostart](const ::cv::Point2f& p1, const ::cv::Point2f& p2)
		{ return dist(p1,ostart) > dist(p2,ostart); } );


      //set the top and bottom end
      etop = minrect[0].y > minrect[1].y ? minrect[0] : minrect[1];
      ebot = etop == minrect[0] ? minrect[1] : minrect[0];

      //set the top and bottom start
      stop = minrect[2].y > minrect[3].y ? minrect[2] : minrect[3];
      sbot = stop == minrect[2] ? minrect[3] : minrect[2];

      //now we have the top and bottom points to do the projection outwards. First find out how long we are...
      const auto& obbox = ocluster._boundingBox;
      
      auto olength = obbox.width > obbox.height ? obbox.width : obbox.height;

      auto dx =  etop.x - stop.x; 
      auto dy =  etop.y - etop.y;

      float odout  = 50.0;
      float oangle = 45 * 3.14159/180.;
      float bangle = std::atan(dy/dx);

      ::cv::Point2f p1(1.0*odout*std::cos(oangle + bangle) * sgn(dx),
		       1.0*odout*std::sin(oangle + bangle) );
      
      ::cv::Point2f p2(1.0*odout*std::cos(oangle - bangle) * sgn(dx),
		       -1.0*odout*std::sin(oangle - bangle));

      // super sad += is not function
      p1 = p1 + etop;
      p2 = p2 + ebot;

      if ( p1.x < 0 ) p1.x = 0;
      if ( p1.y < 0 ) p1.y = 0;
      if ( p2.x < 0 ) p2.x = 0;
      if ( p2.y < 0 ) p2.y = 0;
      if ( stop.x<0 ) stop.x=0;
      if ( stop.y<0 ) stop.y=0;
      if ( etop.x<0 ) etop.x=0;
      if ( etop.y<0 ) etop.y=0;
      if ( sbot.x<0 ) sbot.x=0;
      if ( sbot.y<0 ) sbot.y=0;
      if ( ebot.x<0 ) ebot.x=0;
      if ( ebot.y<0 ) ebot.y=0;

      std::cout << "p1: " << p1 << "\n";
      std::cout << "stop: " << stop << "\n";
      std::cout << "etop: " << etop << "\n";
      std::cout << "ebot: " << ebot << "\n";
      std::cout << "sbot: " << sbot << "\n";
      std::cout << "p2: " << p2 << "\n";
      
      //lets do something idiotic first and just check that we can make this polygon reasonably;
      Contour_t aho_contour;
      aho_contour.reserve(4);

      aho_contour.emplace_back(std::move(sbot));
      aho_contour.emplace_back(std::move(stop));

      aho_contour.emplace_back(std::move(etop));
      aho_contour.emplace_back(std::move(p1));

      aho_contour.emplace_back(std::move(p2));
      aho_contour.emplace_back(std::move(ebot));

      std::swap(aho_contour,ocluster._contour);
      
    }
    return oclusters;
  }
  
}
#endif
