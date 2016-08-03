#ifndef __PIZEROFILTER_CXX__
#define __PIZEROFILTER_CXX__

#include "PiZeroFilter.h"

namespace larocv{

  void PiZeroFilter::_Configure_(const ::fcllite::PSet &pset)
  {

    _nhits_cut      = pset.get<int>    ("NHitsCut");
    _back_startPt   = pset.get<double> ("BackStartPoint");
    _min_area       = pset.get<double> ("MinArea");
    _max_rad_length = pset.get<double> ("MaxRadLength");
    _width          = pset.get<double> ("Width");
    _height         = pset.get<double> ("Height");

    _attempt_merging = pset.get<bool>   ("AttemptMerging");
    _small_dot_prod  = pset.get<double> ("SmallDotProduct");
    
  }

  Cluster2DArray_t PiZeroFilter::_Process_(const larocv::Cluster2DArray_t& clusters,
					   const ::cv::Mat& img,
					   larocv::ImageMeta& meta)
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

	      double distance1 = distance2D(sharedPoint, clusters.at(i)._startPt, _width, _height);
              double distance2 = distance2D(sharedPoint, clusters.at(j)._startPt, _width, _height);


	      //Make sure distance from back projected vertex from PCA
	      //is equal to or shorter than maximum radiation length
	      if(std::abs(distance1) <=  _max_rad_length && std::abs(distance2) <= _max_rad_length)
	      {
 	        std::cout << "Distances: " << distance1 << ", " << distance2 << std::endl;

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
      int idx = OutputClustersID.at(clust);
      auto a = clusters [ idx ];
      a._vertex_2D = Point2D(10,10);
      OutputClusters.emplace_back(a);
    }
    
    //return OutputClusters;

    Cluster2DArray_t Merge_Vertex_Clusters;
    if(_attempt_merging == false){ return Vertex_Clusters; }
    if(_attempt_merging == true) { merging(Vertex_Clusters); return Merge_Vertex_Clusters;}
  }


  Point2D PiZeroFilter::backprojectionpoint( Point2D point1, Point2D point2, Point2D point3, Point2D point4)
  {


    //Points 1 and 2 associated to line 1
    //Points 3 and 4 associated to line 2

    //Get Slopes
    double slope1 = ((point2.y - point1.y)/(point2.x - point1.x));
    double slope2 = ((point4.y - point3.y)/(point4.x - point3.x));

    //Get line intercept
    double intercept1 = point2.y-(slope1*point2.x);
    double intercept2 = point4.y-(slope2*point4.x);

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
    double length2 = (point2.x - point1.x)*(point2.x - point1.x)*(width*width) 
		   + (point2.y - point1.y)*(point2.y - point1.y)*(height*height);

    if(length2 != 0){length = std::sqrt(length2);}

    return length;
  }

  Cluster2DArray_t PiZeroFilter::merging(Cluster2DArray_t filtered)
  {

    //Check Dot Product
    int number_clusters = filtered.size();

    double v1v2;
    double v1;
    double v2;
    double phi;
    double tantheta1;
    double tantheta2;
    double ext_boxl1;
    double ext_boxl2;
    double ext_boxh1;
    double ext_boxh2;

    Cluster2DArray_t merged;

    for(int i = 0; i < number_clusters-1; i++)
    {
      for(int j = i+1 ; j < number_clusters; j++)
      {

        v1v2 =  (filtered.at(i)._eigenVecFirst.x*filtered.at(j)._eigenVecFirst.x) +
		(filtered.at(i)._eigenVecFirst.y*filtered.at(j)._eigenVecFirst.y);
	 
        v1 =  std::sqrt((filtered.at(i)._eigenVecFirst.x*filtered.at(i)._eigenVecFirst.x) +
			(filtered.at(i)._eigenVecFirst.y*filtered.at(i)._eigenVecFirst.y));
	v2 =  std::sqrt((filtered.at(j)._eigenVecFirst.x*filtered.at(j)._eigenVecFirst.x) +
			(filtered.at(j)._eigenVecFirst.y*filtered.at(j)._eigenVecFirst.y));

	phi = std::acos(v1v2/(v1*v2));
	if(phi <= _small_dot_prod)
	{
	  tantheta1 = (filtered.at(i)._width/(2*filtered.at(i)._length));
	  tantheta2 = (filtered.at(j)._width/(2*filtered.at(j)._length));

	  ext_boxl1 = filtered.at(i)._length + _max_rad_length/2;
	  ext_boxl2 = filtered.at(j)._length + _max_rad_length/2;

	  ext_boxh1 = ext_boxl1 * tantheta1;
	  ext_boxh2 = ext_boxl2 * tantheta2;

	}
      }
    }
    return merged;
  }
}



#endif
