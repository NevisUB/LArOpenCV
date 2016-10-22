#ifndef __FLASHLIGHTMERGE_CXX__
#define __FLASHLIGHTMERGE_CXX__

#include "FlashlightMerge.h"
#include "PCAUtilities.h"

namespace larocv{

  void FlashlightMerge::_Configure_(const ::fcllite::PSet &pset)
  {
    _N = pset.get<int>("Keep");
    _trilen   = pset.get<float>("TriLen");   // in pixels
    _triangle = pset.get<float>("TriAngle"); // in degrees
      
  }

  Cluster2DArray_t FlashlightMerge::_Process_(const larocv::Cluster2DArray_t& clusters,
					      const ::cv::Mat& img,
					      larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    /* Once again, copy Ariana. Lets take the TWO (fcl...) largest clusters no matter what the size
       and drop the other contours in ditch effort to scobop up charge. We will rely on the PCA for
       the over all direction and trust the start point. I hope to build a polygon that looks
       like a flashlight. A bounding rectangle at the start, then trapezoid at the end. In this scheme
       our contour should be convex and not require sophisticated untangling
    */
    
    Cluster2DArray_t oclusters = clusters;

    for(unsigned i=0; i < oclusters.size(); ++i){

      auto& ocluster = oclusters[i];
      auto& contour =  ocluster._contour;

      auto oend    = ocluster._startPt;
      
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

      //set the top and bottom end, the two verticies furthest away from the start point
      etop = minrect[0].y > minrect[1].y ? minrect[0] : minrect[1];
      ebot = etop == minrect[0] ? minrect[1] : minrect[0];

      //set the top and bottom start, the two points closest to the start point
      stop = minrect[2].y > minrect[3].y ? minrect[2] : minrect[3];
      sbot = stop == minrect[2] ? minrect[3] : minrect[2];

      //now we have the top and bottom points to do the projection outwards.
      auto dx =  etop.x - stop.x; 
      auto dy =  etop.y - stop.y;
      
      float odout  = _trilen;
      float bangle = _triangle * 3.14159/180.;

      auto& bbox = ocluster._minAreaBox;
      float oangle = bbox.angle * 3.14159/180.0;

      // have to do this for some reason I read on forums
      if (bbox.size.width < bbox.size.height)
	oangle += 90 * 3.14159/180.0;
      
      // im going to be very explicit since i've screwed up many times
      float p1_cos_a = oangle + bangle;
      float p1_sin_a = oangle + bangle;

      float p2_cos_a = oangle - bangle;
      float p2_sin_a = oangle - bangle;
      
      auto xx1 = odout*std::cos(p1_cos_a);
      auto yy1 = odout*std::sin(p1_sin_a);

      auto xx2 = odout*std::cos(p2_cos_a);
      auto yy2 = odout*std::sin(p2_sin_a);
      
      ::cv::Point2f p1(xx1,yy1);
      ::cv::Point2f p2(xx2,yy2);

      // if (meta.plane()==1) {
      // 	std::cout << "sbot: " << sbot << "\n";
      // 	std::cout << "stop: " << stop << "\n";
      // 	std::cout << "ebot: " << ebot << "\n";
      // 	std::cout << "etop: " << etop << "\n";
      // 	std::cout << "dx : " << dx << " dy : " << dy << "\n";
      // 	std::cout << "oangle: " << oangle << "\n";
      // }
	
      if ( (dx < 0 and dy < 0) or (dx < 0 and dy > 0)) { 

	p1 = ebot - p1;     
	p2 = etop - p2;
	std::swap(p1,p2);
      }
      //odd case
      else if ( dx==0 and dy<0 ) {
	p1 = ebot - p1;     
	p2 = etop - p2;
      }
      //another odd case
      else if ( dy == 0 and dx < 0) {
	p1 = ebot - p1;     
	p2 = etop - p2;
	std::swap(p1,p2);
      }
      else {
	p1 = p1 + etop;
	p2 = p2 + ebot;
      }
      
      //replace the contour with the flashlight
      contour.clear();
      contour.reserve(6);

      contour.emplace_back(std::move(sbot));
      contour.emplace_back(std::move(stop));

      contour.emplace_back(std::move(etop));
      contour.emplace_back(std::move(p1));

      contour.emplace_back(std::move(p2));
      contour.emplace_back(std::move(ebot));

    }

    //get the vertex
    auto pi0st = roi.roivtx_in_image(meta);

    //sort the clusters based on their distance from the vertex
    std::sort(std::begin(oclusters),std::end(oclusters),
	      [this,&pi0st](const Cluster2D& c1, const Cluster2D& c2) {
		return (this->d_to_vtx(c1._contour,pi0st) < this->d_to_vtx(c2._contour,pi0st));
	      });

    //do the combinations, find which ones overlap, put them into a map
    std::map<int,std::vector<int> > neighbors;

    for(unsigned c1 = 0; c1 < oclusters.size(); ++c1) {

      auto& ctor1 = oclusters[c1]._contour;

      for(unsigned c2 = 0; c2 < oclusters.size(); ++c2) {
	
	if ( c1 == c2 ) continue;
	auto& ctor2 = oclusters[c2]._contour;

	if ( overlap(ctor1,ctor2) )
	  neighbors[c1].push_back(c2);
      }
    }




    //fix connections
    //sometimes this happens
    // actual neighbors...

    //   i : 0 : 1,
    //   i : 1 : 10,
    //   i : 5 : 7,
    //   i : 9 : 10,
    //   i : 10 : 1,

    //unique neighbors...

    // i : 0 : 1,10,
    // i : 1 :
    // i : 2 :
    // i : 3 :
    // i : 4 :
    // i : 5 : 7,
    // i : 6 :
    // i : 7 :
    // i : 8 :
    // i : 9 :
    // i : 10 : 

    for( auto& neighbor : neighbors ) {

      //get the current index
      auto& idx = neighbor.first;

      //and my neighbors
      auto& vec = neighbor.second;
      
      //for each of my neighbors
      for(auto& v : vec) {

	if ( neighbors.count(v) <= 0 ) continue;

	//get their neighbors
	auto& ovec = neighbors[v];

	//if i'm in there, good, if not
	if ( std::find(ovec.begin(), ovec.end(), idx) != ovec.end() ) continue;

	//ad me to my neighbors!
	ovec.push_back(idx);
	
      }
    }

    
    //unravel it, meaning start with the contours that are closest to the vertex, and create
    //a unique vector for them

    //seen cluster
    std::vector<bool> used(oclusters.size(),false);
    
    std::vector< std::vector<int> > unique_neighbors;
    unique_neighbors.resize(clusters.size());
    
    for(unsigned i = 0; i < oclusters.size(); ++i) {
	
      if ( used[i] ) continue;

      unravel(used,neighbors,unique_neighbors,i,i);

    }
    
    
    //this is useful for debugging!
    // std::cout <<"\n\t" << "actual neighbors...\n\n";
    // for(auto n : neighbors){
    //   std::cout << "i : " << n.first << " : ";
    //   for(auto j : n.second)     
    //   	std::cout << j << ",";
    //   std::cout << "\n";
    // }

    

    // std::cout <<"\n\t" << "unique neighbors...\n\n";
    // for(int i=0;i<unique_neighbors.size();++i) {

    //   std::cout << "i : " << i << " : ";
    //   for(auto j : unique_neighbors[i])
    // 	std::cout << j << ",";

    //   std::cout << "\n";
    // }


    //lets use used again
    std::fill(used.begin(), used.end(), false);    

    Cluster2DArray_t result_clusters;
    result_clusters.reserve(oclusters.size());

    for (unsigned i=0; i < unique_neighbors.size(); ++i ) {

      if ( used[i] ) continue;

      used[i]=true;
      
      //make the overall contour
      std::vector<::cv::Point> overall_ctor;

      //get the parent contour
      auto& ctor1 = oclusters[i]._contour;

      //get it's overlaps
      auto & neighbors = unique_neighbors[i];

      //reerve 10 times the space of the number of overlaps
      overall_ctor.reserve(10*(neighbors.size()+1 ));

      //put the actual points themselves
      for(auto& pt : oclusters[i]._insideHits) overall_ctor.emplace_back(pt);

      //for good measure I want to include the end cap of the FIRST contour
      for (int iy=2;iy<=5;++iy)
	overall_ctor.emplace_back(ctor1[iy]);
      
      //loop over the overlapping contours
      for ( auto& idx : neighbors ) {

	used[idx] = true;

	//get one overlapping contour
	auto& ctor2 = oclusters[idx]._contour;

	//put the points from this overlapping contour into this overall_contour;
	for(auto& pt : ctor2) overall_ctor.emplace_back(pt);
      }

      //make a new hull, give it some space
      std::vector<int> hullpts;
      hullpts.reserve(overall_ctor.size());

      //calculate the hull
      ::cv::convexHull(overall_ctor, hullpts);

      //make a new cluster
      Cluster2D cluster;

      //with a contour of fixed size...
      cluster._contour.resize(hullpts.size());

      //fill the contour with hull points!
      for(unsigned u=0;u<hullpts.size();++u) 
	cluster._contour[u] = overall_ctor[ hullpts[u] ];

      //set the cluster parameters to the parent
      cluster.copy_params(oclusters[i]);

      // the start point is (since we sorted oclusters) the closest point (in the BRS sense) to the
      // projected vertex, but we need the end point for matching
      
      //put this cluster back into the array
      result_clusters.emplace_back(std::move(cluster));
    }
  
    // std::cout << "result_clusters.size() : " << result_clusters.size() << "\n";

    // for the ones that are connected, lets make a convex hull out of the points we have
    if ( meta.debug() ) {

      std::stringstream ss1, ss2;

      ::larlite::user_info uinfo{};
      ss1 << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_clusters";
      uinfo.store("ID",ss1.str());

      ss1.str(std::string());
      ss1.clear();

      uinfo.store("N_hulls",(int)oclusters.size());

      LAROCV_DEBUG() << "Writing debug information for " << oclusters.size() << "\n";
    
      for(size_t i = 0; i < oclusters.size(); ++i){

	const auto& cluster = oclusters[i];

	////////////////////////////////////////////
	/////Contour points
	
	ss1  <<  "ClusterID_" << i << "_contour_x";
	ss2  <<  "ClusterID_" << i << "_contour_y";

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


    return result_clusters;
  }

  //straight rip of PCAsegmentation function
  //I wrote a while back
  void FlashlightMerge::unravel(std::vector<bool>& used,   //which one is used
				const std::map<int,std::vector<int> >& cnse, //neighbors
				std::vector< std::vector<int> >&       cmse, //traversed and unique graph
				int i,int k)
  {
    if ( cnse.count(k) <= 0 ) return; // key doesn't exist
    
    for(const auto& n : cnse.at(k)) { // std::vector of neighbors
      
      if ( used[n] ) continue;
      
      cmse[i].push_back(n);
      used[i] = true;
      used[n] = true;
      
      unravel(used,cnse,cmse,i,n);
    }
  }

  
  bool FlashlightMerge::overlap(const Contour_t& c1,
				const Contour_t& c2) {
    
    std::vector<int> c1_idx={2,3,4,5};  //will check 2->3, 3->4, 4->5
    std::vector<int> c2_idx={0,1,2}; //will check 0->1, 1->2

    for(unsigned i=0;i<c1_idx.size()-1;++i) {
      //check this segment on c1
      auto c11=c1_idx[i];
      auto c12=c1_idx[i+1];
      
      for(unsigned j=0;j<c2_idx.size()-1;++j) {
	auto c21=c2_idx[j];
	auto c22=c2_idx[j+1];
      
    	//against all segements on c2
    	if ( line_intersection(c1[c11],c1[c12],c2[c21],c2[c22]) ) return true;
      }

      //including the connection between the first and the last
      if ( line_intersection(c1[c11],c1[c12],c2.back(),c2.front()) ) return true;
    }

    return false;
  }
				
  
  float FlashlightMerge::d_to_vtx(const Contour_t& ctor,
				  const Point2D& vtx) {
    float d=1e8;
    for(const auto& pt : ctor) {
      auto dd = sqrt((pt.x - vtx.x)*(pt.x - vtx.x) +
		     (pt.y - vtx.y)*(pt.y - vtx.y));
      if (dd < d)
	d = dd;
    }
    return d;
  }
  
  bool FlashlightMerge::line_intersection(const ::cv::Point& p1,
					  const ::cv::Point& p2,
					  const ::cv::Point& p3,
					  const ::cv::Point& p4) {

    //explicity allocate floats
    float p0_x = p1.x;
    float p0_y = p1.y;
      
    float p1_x = p2.x;
    float p1_y = p2.y;

    float p2_x = p3.x;
    float p2_y = p3.y;

    float p3_x = p4.x;
    float p3_y = p4.y;
		
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;
    s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;
    s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) return true;
    
    return false;
    
  }

}
#endif
