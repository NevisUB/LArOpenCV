#ifndef __PIZEROFILTERV_CXX__
#define __PIZEROFILTERV_CXX__

#include "PiZeroFilterV.h"


namespace larcv{

  void PiZeroFilterV::_Configure_(const ::fcllite::PSet &pset)
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

  Cluster2DArray_t PiZeroFilterV::_Process_(const larcv::Cluster2DArray_t& clusters,
					      const ::cv::Mat& img,
					      larcv::ImageMeta& meta)
  { 

    Cluster2DArray_t OutputClusters;
    Cluster2DArray_t Vertex_Clusters;
    std::vector<int> OutputClustersID;

    std::map<int,std::vector<int> > vclusters;
    if ( meta.plane() == 0 ) { _verts_v.clear(); _verts_v.resize(3); }
    
    auto& _verts =  _verts_v[ meta.plane() ];
    _verts.reserve(clusters.size() / 2);
    
    if(clusters.empty())
    {
      return clusters;
    }

    for(int i = 0; i < clusters.size(); i++)
    {
	int numHits = clusters.at(i)._numHits;

	//If cluster has hits >= defined minimum number of hits fill new cluster array
	if(numHits >= _nhits_cut)
	{
	  for(int j = 0; j < clusters.size(); j++)
          {

	    if ( i == j ) continue;
	    int next_numHits = clusters[j]._numHits;
	    
	    if(next_numHits >= _nhits_cut)
	    {
	    
	      double ipointX = clusters[i]._eigenVecFirst.x+clusters[i]._startPt.x;
	      double ipointY = clusters[i]._eigenVecFirst.y+clusters[i]._startPt.y;

	      double jpointX = clusters[j]._eigenVecFirst.x+clusters[j]._startPt.x;
	      double jpointY = clusters[j]._eigenVecFirst.y+clusters[j]._startPt.y;

	      Point2D iPoint  (ipointX,ipointY);
	      Point2D jPoint  (jpointX,jpointY);

	      Point2D sharedPoint = backprojectionpoint(iPoint, clusters[i]._startPt,
				    			jPoint, clusters[j]._startPt);

	      double distance1 = distance2D(sharedPoint, clusters[i]._startPt, _width, _height);
              double distance2 = distance2D(sharedPoint, clusters[j]._startPt, _width, _height);


	      //Make sure distance from back projected vertex from PCA
	      //is equal to or shorter than maximum radiation length
	      if(std::abs(distance1) <=  _max_rad_length && std::abs(distance2) <= _max_rad_length)
	      {

		//key exists
		if ( vclusters.find(j) != vclusters.end() )
		  // is i inside j
		  if( std::find(vclusters[j].begin(), vclusters[j].end(), i) != vclusters[j].end())
		    //ok it was, don't push me
		    continue;
		
		Vertex2D v(distance1,distance2,sharedPoint,i,j);
		v.first  = &clusters[ i ];
		v.second = &clusters[ j ];
		
		_verts.emplace_back(v);
		  
		vclusters[i].push_back(j);
	      } // end distance
	      
	    }
          }	
	}
    }
    // vertex objects created at this point
    
    
    //list of connections
    if ( vclusters.size() > 0){ 
      for( auto& c : vclusters ) {
	std::cout << "" <<  c.first << " : {";
	for(const auto& s : c.second) std::cout << s << ",";
	std::cout << "}\n";
      }
      std::cout << "\n";
    }
    return clusters;
  }


  Point2D PiZeroFilterV::backprojectionpoint( Point2D point1, Point2D point2, Point2D point3, Point2D point4)
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

  double PiZeroFilterV::distance2D( Point2D point1, Point2D point2, double width, double height)
  {
    double length = 0;
    double length2 = (point2.x - point1.x)*(point2.x - point1.x)*(width*width) 
		   + (point2.y - point1.y)*(point2.y - point1.y)*(height*height);

    if(length2 != 0){length = std::sqrt(length2);}

    return length;
  }

  Cluster2DArray_t PiZeroFilterV::merging(Cluster2DArray_t filtered)
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
