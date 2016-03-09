//by vic
#ifndef __STARTENDMERGE_CXX__
#define __STARTENDMERGE_CXX__

#include "StartEndMerge.h"
#include "PCAUtilities.h"

namespace larcv{

  void StartEndMerge::_Configure_(const ::fcllite::PSet &pset)
  {
    _max_start_end_d = pset.get<double>("MaxStartEndDistance");
 
    _w = 0.0557;
    _h = 0.3;
  }

  Cluster2DArray_t StartEndMerge::_Process_(const larcv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larcv::ImageMeta& meta)
  { 
    Cluster2DArray_t oclusters; oclusters.reserve(clusters.size());


    // we should just traverse graph. Start at one end point, try and jump to another cluster, if next cluster
    // end point is close to another, jump to it, if not reel back one and jump again until all connections are
    // made. Then merge the cluster. We need distance (if less than cut) from one end point to start point
    // we can also consider using PCA axis to assist this connecting. We will make this function generic later
    
    std::map<size_t,std::vector<size_t> > con_start_end; //graph to be traversed based on distance from start to end
    std::map<size_t,std::vector<size_t> > com_start_end; //accepted graph used to combine clusters (should be unique)

    for(unsigned i = 0; i < clusters.size(); ++i){

      auto& s1 = clusters[i]._startPt;
      
      for(unsigned j = 0; j < clusters.size(); ++j){

	if ( i == j ) continue;

	auto& e2 = clusters[j]._endPt;

	auto d = dist(s1.x * _w, e2.x * _w,
		      s1.y * _h, e2.y * _h);
	
	if ( d <= _max_start_end_d ) con_start_end[i].push_back(j);
      }
    }
    
    // traverse the graph, not in the sense of PCA segmentation
    // since it was 100% symetric, 

    //seen cluster
    std::map<int,bool> used; for(int i=0;i<clusters.size();++i) used[i] = false;
    
    for(unsigned i = 0; i < clusters.size(); ++i) {

      if ( used[i] ) continue;

      const auto& c1 = clusters[i];

      // used[i] = true;
      
      traverse(c1,clusters,used,con_start_end,com_start_end,i,i);
    }

    std::cout << "\t==>con_start_end\n";
    for( const auto& c : con_start_end ) {
      std::cout << "[" << c.first << "] : {";
      for( const auto& cc : c.second ) std::cout << cc << ",";
      std::cout << "}\n";
    }
    std::cout << "\t==>com_start_end\n";
    for( const auto& c : com_start_end ) {
      std::cout << "[" << c.first << "] : {";
      for( const auto& cc : c.second ) std::cout << cc << ",";
      std::cout << "}\n";
    }
    
    
    //just let it pass through for now
    return clusters;
  }

  //straight rip of PCAsegmentation function
  //I wrote a while back
  void StartEndMerge::traverse(const Cluster2D& c1,         // the input cluster
			       const Cluster2DArray_t& ca,  // all the clusters
			       std::map<int,bool >& used,   //which one is used
			       const std::map<size_t,std::vector<size_t> >& cnse, //neighbors
			       std::map<size_t,std::vector<size_t> >&       cmse, //traversed and unique graph
			       int i,int k)
  {
    std::cout << "i: " << i << " k: " << k << "\n";

    if ( cnse.count(k) <= 0 ) 
      return;
    std::cout << "pass cnse.count(k)\n";
    
    for(const auto& n : cnse.at(k)) { // std::vector of neighbors
      std::cout << "n: " << n << "\n";
      if ( used[n] ) continue;
      std::cout << "passed used[n]\n";
      
      const auto& c2 = ca.at(n);

      //here we could check the PCA dot product which might help us do merging
      // if ( !compatible(box,box2) )
      // 	continue;
      
      cmse[i].push_back(n);
      used[n] = true;
      
      traverse(c2,ca,used,cnse,cmse,i,n);
    }
  }

  
  
}
#endif
