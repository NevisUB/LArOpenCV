#ifndef __PIZEROFILTER_CXX__
#define __PIZEROFILTER_CXX__

#include "PiZeroFilter.h"

namespace larcv{

  void PiZeroFilter::_Configure_(const ::fcllite::PSet &pset)
  {

    _nhits_cut = pset.get<int> ("NHitsCut");
    _back_startPt = pset.get<double> ("BackStartPoint");
    _min_area = pset.get<double> ("MinArea");
    _max_rad_length = pset.get<double> ("MaxRadLength");

  }

  Cluster2DArray_t PiZeroFilter::_Process_(const larcv::Cluster2DArray_t& clusters,
					   const ::cv::Mat& img,
					   larcv::ImageMeta& meta)
  { 

    Cluster2DArray_t OutputClusters;
    Cluster2DArray_t Vertex_Clusters;
    std::vector<int> OutputClustersID;


    if(clusters.empty())
    {
      return clusters;
    }

    for(int i = 0; i < clusters.size()-1; i++)
    {
	int numHits = clusters.at(i)._numHits;

	//If cluster has hits >= defined minimum number of hits fill new cluster array
	if(numHits >= _nhits_cut)
	{
	  for(int j = i+1; j < clusters.size(); j++)
          {
	    int next_numHits = clusters.at(j)._numHits;
	    
	    if(next_numHits >= _nhits_cut)
	    {
	    
	      double ipointX = clusters.at(i)._eigenVecFirst.x+clusters.at(i)._startPt.x;
	      double ipointY = clusters.at(i)._eigenVecFirst.y+clusters.at(i)._startPt.y;
	      double jpointX = clusters.at(j)._eigenVecFirst.x+clusters.at(j)._startPt.x;
	      double jpointY = clusters.at(j)._eigenVecFirst.y+clusters.at(j)._startPt.y;


	      Point2D iPoint  (ipointX,ipointY);
	      Point2D jPoint  (jpointX,jpointY);

	      Point2D sharedPoint = backprojectionpoint(iPoint, clusters.at(i)._startPt,
				    			jPoint, clusters.at(j)._startPt);


	      double distance1 = distance2D(sharedPoint, clusters.at(i)._startPt, clusters.at(i).PixelWidth(), clusters.at(i).PixelHeight());
              double distance2 = distance2D(sharedPoint, clusters.at(j)._startPt, clusters.at(j).PixelWidth(), clusters.at(j).PixelHeight());


//	      std::cout << "Origin: " << clusters.at(i).Origin().x << ", " << clusters.at(i).Origin().y << std::endl;
/*	      std::cout << "Start i: " << clusters.at(i)._startPt.x << ", " << clusters.at(i)._startPt.y << std::endl;
	      std::cout << "Start j: " << clusters.at(j)._startPt.x << ", " << clusters.at(j)._startPt.y << std::endl;
	      std::cout << "Next i: " << iPoint.x << ", " << iPoint.y << std::endl;
	      std::cout << "Next j: " << jPoint.x << ", " << jPoint.y << std::endl;
	      std::cout << "Share Point: " << sharedPoint.x << ", " << sharedPoint.y << std::endl;
*///	      std::cout << clusters.at(i).PixelWidth() << ", " << clusters.at(i).PixelHeight() << std::endl;
//	      std::cout << clusters.at(j).PixelWidth() << ", " << clusters.at(j).PixelHeight() << std::endl;

//	      std::cout << "Distances: " << distance1 << ", " << distance2 << std::endl;


	      //Make sure distance from back projected vertex from PCA
	      //is equal to or shorter than maximum radiation length
	      if(std::abs(distance1) <=  _max_rad_length && std::abs(distance2) <= _max_rad_length)
	      {
 	        std::cout << "Distances: " << distance1 << ", " << distance2 << std::endl;
//	        std::cout << i << ", " << j << std::endl;
		OutputClustersID.push_back(i);
	        OutputClustersID.push_back(j);

	        Cluster2D OClusteri = clusters.at(i);
	        OClusteri._vertex_2D = sharedPoint;
	        Cluster2D OClusterj = clusters.at(j);
		OClusterj._vertex_2D = sharedPoint;
		Vertex_Clusters.push_back(OClusteri);
		Vertex_Clusters.push_back(OClusterj);

	      }
	    }
          }	
	}
      }

    if(!OutputClustersID.empty())
      std::cout << "Number of Cluster Pairs: " << OutputClustersID.size()/2 << std::endl;

    if(OutputClustersID.empty())
      return Cluster2DArray_t();
    
    std::sort(OutputClustersID.begin(), OutputClustersID.end());
    auto UniqueOutputClustersID = std::unique(OutputClustersID.begin(), OutputClustersID.end());
    OutputClustersID.erase(UniqueOutputClustersID, OutputClustersID.end());

    // std::cout << "Clusters.size(): " << clusters.size() << std::endl;
    // std::cout << "OutputClusterID.size(): " << OutputClustersID.size() << std::endl;
    
    for(int clust = 0; clust < OutputClustersID.size(); clust++)
    {
      // std::cout << "clus       : " << clust << std::endl;
      int idx = OutputClustersID.at(clust);
      // std::cout << "cluster idx: " << idx   << std::endl;
      auto a = clusters [ idx ];
      // std::cout << "this ID is... " << a.ClusterID() << std::endl;
      a._vertex_2D = Point2D(10,10);
      OutputClusters.emplace_back(a);
    }
    
    // for(auto & o : Vertex_Clusters ) {
    //   std::cout << o.ClusterID() << " "
    // 		<< o.PlaneID()   << " "
    // 		<< o._length     << " "
    // 		<< o._numHits    << "\n";
    // }
	
    //return OutputClusters;
    //return clusters;

    std::cout << Vertex_Clusters.size() << std::endl;
    std::cout << Vertex_Clusters.at(0)._vertex_2D.x << ", " << Vertex_Clusters.at(0)._vertex_2D.y << std::endl;
    return Vertex_Clusters;
  }


  Point2D PiZeroFilter::backprojectionpoint( Point2D point1, Point2D point2, Point2D point3, Point2D point4)
  {


    //Points 1 and 2 associated to line 1
    //Points 3 and 4 associated to line 2

    //Get Slopes
    double slope1 = ((point2.y - point1.y)/(point2.x - point1.x));
    double slope2 = ((point4.y - point3.y)/(point4.x - point3.x));
  
//    std::cout << "Slope: " << slope1 << ", " << slope2 << std::endl;
 

    //Get line intercept
//    double intercept1 = (point1.y - (slope1*point1.x));
//    double intercept2 = (point3.y - (slope2*point3.x));

    double intercept1 = point2.y-(slope1*point2.x);
    double intercept2 = point4.y-(slope2*point4.x);

//    std::cout << "Intercept: " << intercept1 << ", " << intercept2 << std::endl;

    //Find x intersection point
    double x = ((intercept2 - intercept1)/(slope1 - slope2));
    //Find y intersection point
    double y = slope1*x + intercept1;   

    Point2D intersection(x,y);

    return intersection;

  } 

  double PiZeroFilter::distance2D( Point2D point1, Point2D point2, double width, double height)
  {

    double length = 0;

    double length2 = (point2.x - point1.x)*(point2.x - point1.x)/(width*width) 
		   + (point2.y - point1.y)*(point2.y - point1.y)/(height*height);

//    std::cout << "POINT 2: " << point2.x << ", " << point2.y << std::endl;
//    std::cout << "POINT 1: " << point1.x << ", " << point1.y << std::endl;

    if(length2 != 0){length = std::sqrt(length2);}

    return length;

  }

}



#endif
