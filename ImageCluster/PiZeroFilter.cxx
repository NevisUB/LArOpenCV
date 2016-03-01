#ifndef __PIZEROFILTER_CXX__
#define __PIZEROFILTER_CXX__

#include "PiZeroFilter.h"

namespace larcv{

  void PiZeroFilter::_Configure_(const ::fcllite::PSet &pset)
  {

    _nhits_cut = pset.get<int> ("NHitsCut");


  }

  Cluster2DArray_t PiZeroFilter::_Process_(const larcv::Cluster2DArray_t& clusters,
					   const ::cv::Mat& img,
					   larcv::ImageMeta& meta)
  { 

    Cluster2DArray_t OutputClusters;

    //for(int clust = 0; clust < clusters.size(); clust++)
    for(auto& clust : clusters)
    {

	int numHits = clust._numHits;

	//If cluster has hits >= defined minimum number of hits fill new cluster array
	if(numHits >= _nhits_cut)
	{

	  OutputClusters.push_back(clust);

	} 
	
    }

	return OutputClusters;
   }

}
#endif
