#ifndef __PIZEROFILTER_CXX__
#define __PIZEROFILTER_CXX__

#include "PiZeroFilter.h"

namespace larcv{

  void PiZeroFilter::_Configure_(const ::fcllite::PSet &pset)
  {

    _nhits_cut = pset.get<int> ("NHitsCut");
    _back_startPt = pset.get<double> ("BackStartPoint");

  }

  Cluster2DArray_t PiZeroFilter::_Process_(const larcv::Cluster2DArray_t& clusters,
					   const ::cv::Mat& img,
					   larcv::ImageMeta& meta)
  { 

    Cluster2DArray_t OutputClusters;
    std::vector<Point2D> eigenVecFirstArray;
    std::vector<Point2D> eigenVecCenterArray;

    //for(int clust = 0; clust < clusters.size(); clust++)
    for(auto& clust : clusters)
    {

	int numHits = clust._numHits;

	//If cluster has hits >= defined minimum number of hits fill new cluster array
	if(numHits >= _nhits_cut)
	{

          //eigenVecFirstArray.push_back(clust._eigenVecFirst); 
	  //eigenVecCenterArray.push_back(clust._centerPt);


          for(auto& next_clust : clusters)
          {
	    int next_numHits = next_clust._numHits;
	    
	    if(next_numHits >= _nhits_cut)
	    {

	      Point2D sharedPoint = backprojectionpoint(clust._eigenVecFirst, clust._centerPt, next_clust._eigenVecFirst, next_clust._centerPt);
	      
	      double distance1 = distance2D(sharedPoint, clust._startPt);
              double distance2 = distance2D(sharedPoint, next_clust._startPt);


	      if(std::abs(distance1 - distance2) <= _back_startPt){OutputClusters.push_back(clust);}

	    }
          }	
	}
      }
 
    return OutputClusters;

  }


  Point2D PiZeroFilter::backprojectionpoint( Point2D point1, Point2D point2, Point2D point3, Point2D point4)
  {


    //Points 1 and 2 associated to line 1
    //Points 3 and 4 associated to line 2

    //Get Slopes
    double slope1 = ((point2.y - point1.y)/(point2.x - point1.x));
    double slope2 = ((point4.y - point3.y)/(point4.x - point3.x));
   

    //Get line intercept
    double intercept1 = (point1.y - (slope1*point1.x));
    double intercept2 = (point3.y - (slope2*point3.x));

    //Find x intersection point
    double x = ((intercept2 - intercept1)/(slope1 - slope2));
    //Find y intersection point
    double y = ((slope1*intercept2 - slope2*intercept1)/(slope1 - slope2));   

    Point2D intersection(x,y);

    return intersection;

  } 

  double PiZeroFilter::distance2D( Point2D point1, Point2D point2)
  {

    double length = std::sqrt( (point2.x - point1.x)*(point2.x - point1.x) + (point2.y - point1.y)*(point2.y - point1.y));

    return length;

  }

}



#endif
