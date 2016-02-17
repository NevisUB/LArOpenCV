//by vic
#ifndef __PCASEGMENTATION_CXX__
#define __PCASEGMENTATION_CXX__

#include "PCASegmentation.h"
#include "PCAUtilities.h"
#include "PCAPath.h"

namespace larcv{

  void PCASegmentation::_Configure_(const ::fcllite::PSet &pset)
  {

    //Deprecated
    _min_trunk_length = pset.get<int>    ("MinTrunkLength");
    _trunk_deviation  = pset.get<double> ("TrunkDeviation");
    _closeness        = pset.get<double> ("Closeness");

    //How many pixes for initial subdivision
    _segments_x = pset.get<int> ("NSegmentsX");
    _segments_y = pset.get<int> ("NSegmentsY");
    
    
    //various arbys cold cuts
    _nhits_cut     = pset.get<int>   ("NHitsCut"   );
    _sub_nhits_cut = pset.get<int>   ("NSubHitsCut");
    _angle_cut     = pset.get<double>("AngleCut"   );
    _cov_cut       = pset.get<double>("CovSubDivide");
    _n_divisions   = pset.get<int>   ("NDivisions" );
    _n_clustersize = pset.get<int>   ("MinClusterSize");
    
    //do you want to be able to connect by crossing empty boxes?
    _allow_cross_empties = pset.get<bool> ("AllowEmptyCross");
   
    //output tree
    _outtree = new TTree("PCA","PCA");
    
    _outtree->Branch("_eval1",&_eval1,"eval1/D");
    _outtree->Branch("_eval2",&_eval2,"eval2/D");

    _outtree->Branch("_area",&_area,"area/D");
    _outtree->Branch("_perimeter",&_perimeter,"perimeter/D");


  }
  
  ContourArray_t PCASegmentation::_Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  {
    
    //http://docs.opencv.org/master/d3/d8d/classcv_1_1PCA.html
    
    //cluster == contour
    ContourArray_t ctor_v;    
    clear_vars();
    _cparms_v.reserve(clusters.size());

    double angle_cut = _angle_cut; //degrees
    angle_cut *= 3.14159/180.0;    //to radians

    for(unsigned u = 0; u < clusters.size(); ++u) {
      
      auto& cluster    =  clusters[u];
      auto& cntr_pt    = _cntr_pt;
      auto& eigen_vecs = _eigen_vecs;
      auto& eigen_val  = _eigen_val;
      auto& lline      = _line;      
      
      auto& pearsons_r = _pearsons_r;  //linearity
      
      //Lets get subimage that only includes this cluster
      //and the points in immediate vicinity (ROI)
      ::cv::Rect rect = ::cv::boundingRect(cluster);
      ::cv::Mat subMat(img, rect);

      std::vector<PCABox> boxes;
      std::map<int,std::vector<int> > neighbors;

      auto dx = rect.width  /  _segments_x;
      auto dy = rect.height /  _segments_y;

      for(unsigned i = 0; i < _segments_x; ++i) {
	for(unsigned j = 0; j < _segments_y; ++j) {
	  
	  auto x  = i * dx;
	  auto y  = j * dy;
	  
	  ::cv::Rect r(x + rect.x,y + rect.y,dx,dy);
	  ::cv::Mat subM(img, r);

	  std::vector<double> line; line.resize(4);
	  
	  Contour_t locations;
	  Contour_t inside_locations;
	  
	  ::cv::findNonZero(subM, locations);

	  if ( locations.size() == 0 ) 
	    continue;
	  
	  for(auto& loc : locations) {

	    //hit must be "in" the contour
	    loc.x += r.x;
	    loc.y += r.y;
	    
	    if ( ::cv::pointPolygonTest(cluster, loc ,false) < 0 )
	      continue;

	    loc.x -= r.x;
	    loc.y -= r.y;
	    
	    inside_locations.emplace_back(loc);
	  }
	  
	  if ( inside_locations.size() == 0 ) 
	    { boxes.emplace_back(r); continue; }
	  
	  if ( inside_locations.size() < _nhits_cut )
	    { boxes.emplace_back(r); continue; }
	  
	  Point2D e_vec,e_center;
	  
	  // fille line, e_vec, e_center
	  pca_line(inside_locations,r,line,e_vec,e_center);
	  
	  //New box object
	  PCABox box(e_vec,e_center,line,inside_locations,
		     r,r,angle_cut,_cov_cut,_sub_nhits_cut);
	  
	  //do subdivision recusively if the box has low linearity...
	  check_linearity(box,boxes,_n_divisions);
	  
	}
      }


      
      
      for(unsigned b1 = 0; b1 < boxes.size(); ++b1) {
	
	auto& box1 = boxes[b1];
	box1.expand(2,2);
	
	for(unsigned b2 = 0; b2 < boxes.size(); ++b2) {
	  
	  if ( b1 == b2 ) continue;
	  
	  auto& box2 = boxes[b2];
	  
	  if ( box1.touching(box2) ) 
	    neighbors[b1].push_back(b2);
	}
	
      }
      
      
      // kazu suggests we are allowed to jump across empty boxes
      // this will become recursive I hope, lets decide yes or no to jump neighbors
      if ( _allow_cross_empties )
	cross_empty_neighbors(boxes,neighbors);
      
      int empty = 0;
      std::map<int,std::vector<int> > combined; //combined boxes            
      std::map<int,bool> used;
      for(int i = 0; i < boxes.size(); ++i) { used[i] = false; if (boxes[i].empty_) { used[i] = true; ++empty; } }
      
      if ( boxes.size() == empty ) // all boxes are empty
	continue;

      if ( (boxes.size() - empty) > 1 ) { // better be at least two non-empty boxes
	
	for (int i = 0; i < boxes.size(); ++i) {
	  
	  if ( used[i] ) continue;
	  
	  const auto& box = boxes.at(i);

	  used[i] = true;
	  
	  connect(box,boxes,used,neighbors,combined,i,i);
	}
	
	// std::cout << "Finished connecting..." << "\n";
	// std::cout << "\n\n";
	// for(const auto& k : combined) {
	//   std::cout << "\t key: " << k.first << " : {";
	//   for(const auto& kk : k.second) {
	//     std::cout << kk << ",";
	//   }
	//   std::cout << "}\n";
	// }
	
      }     

      
      //At this point we have some idea about "connectedness". 
      

      //lets set the charge sum first
      //per box
      
      for( auto& box : boxes ) {

	if ( box.empty_ ) continue;
	
	for ( const auto &pt : box.pts_ )  
	  
	  box.charge_.push_back( (int) img.at<uchar>(pt.y + box.parent_roi_.y,
						     pt.x + box.parent_roi_.x) ); 
	
	
      }

    

      //return the index in combined that we choose as the shower axis
      //what condition do we set?
      if ( combined.size() == 0 ) continue;


      //decide the shower axis via some combination of everything
      //you see in PCAPath.cxx
      auto paths = decide_axis(boxes,combined);
      
      
      ///////////////////////////////////////////////
      ////////////////OLD CODE///////////////////////
      ///////////////////////////////////////////////
      
      //subMat holds pixels in the bounding rectangle
      //around the given contour. We can make a line from principle eigenvector
      //and compute the distance to this line of the hits may be weighted by
      //charge at some point?
      
      //make a copy cluster_s = shifted clusters to the bounding box
      auto cluster_s = cluster;
      
      //shift it down
      for(auto &pt : cluster_s) { pt.x -= rect.x; pt.y -= rect.y; }
      ctor_v.emplace_back(cluster_s);
      
      // PCA ana requies MAT object w/ data sitting in rows, 2 columns
      // 1 for each ``feature" or coordinate
      ::cv::Mat ctor_pts(cluster_s.size(), 2, CV_64FC1);
      
      for (unsigned i = 0; i < ctor_pts.rows; ++i) {
	ctor_pts.at<double>(i, 0) = cluster_s[i].x;
	ctor_pts.at<double>(i, 1) = cluster_s[i].y;
      }
      
      //Perform PCA analysis
      ::cv::PCA pca_ana(ctor_pts, ::cv::Mat(), CV_PCA_DATA_AS_ROW,0); // maxComponents = 0 (retain all)
      
      //Center point
      // ::cv::Point
      cntr_pt = Point2D( pca_ana.mean.at<double>(0,0),
			 pca_ana.mean.at<double>(0,1) );
      

      //Principle directions (vec) and relative lengths (vals)
      eigen_vecs.resize(2);
      eigen_val.resize (2);
      
      for (unsigned i = 0; i < 2; ++i) {
	eigen_vecs[i] = Point2D(pca_ana.eigenvectors.at<double>(i, 0),
				pca_ana.eigenvectors.at<double>(i, 1));
	eigen_val[i]  = pca_ana.eigenvalues.at<double>(0, i);
      }
      
      
      //Get the nonzero locations of the pixels in this subimage
      //these are the "hits"
      std::vector<::cv::Point> locations;
      ::cv::findNonZero(subMat, locations);
      
      //take principle eigenvector (the first one)
      //and get the line that best represents the shower
      //this line passes through center point and is principle eigenvec
      lline[0] = 0;
      lline[1] = cntr_pt.y + ( (0 - cntr_pt.x) / eigen_vecs[0].x ) * eigen_vecs[0].y;
      lline[2] = rect.width;
      lline[3] = cntr_pt.y + ( (rect.width - cntr_pt.x) / eigen_vecs[0].x) * eigen_vecs[0].y;
      
      int nhits = 0;
      
      //find how far away the points are to the line
      std::vector<std::pair<int,int> > hits;
      std::vector<int> charge;
      
      for(const auto& loc : locations) {
	
	//is this point in the contour, if not continue
	if ( ::cv::pointPolygonTest(cluster_s, loc,false) < 0 )
	  continue;
	
	charge.push_back( (int) subMat.at<uchar>(loc.y,loc.x) );
	hits.emplace_back(loc.x + rect.x, loc.y + rect.y);
	++nhits;
       
      }
      
      // you must have atleast 10 points to proceed or trunk is useless
      if ( nhits < _n_clustersize ) // j == number of points in cluster
	continue;
      
      //find the trunk with this info 
      auto normal = std::sqrt( eigen_val[0]*eigen_val[0] + eigen_val[1]*eigen_val[1] ) ;
      
      _eval1 = eigen_val[0] / normal;
      _eval2 = eigen_val[1] / normal;
      
      _area      = (double) ::cv::contourArea(cluster);
      _perimeter = (double) ::cv::arcLength  (cluster,1);
      
      auto eigen_normal = std::sqrt(std::pow(eigen_vecs[0].x,2) + std::pow(eigen_vecs[0].y,2));


      // this is the only way you can get shit into python at this point
      _cparms_v.emplace_back(u,
			     _eval1,
			     _eval2,
			     _area,
			     _perimeter,
			     eigen_vecs[0].x / eigen_normal,
			     eigen_vecs[0].y / eigen_normal,
			     nhits,
			     hits,
			     lline,
			     charge,
			     boxes,
			     combined,
			     paths
			     );
      
      
      _outtree->Fill();
    }
    

    //just return the clusters you didn't do anything...
    return clusters;

  }

  
  void PCASegmentation::clear_vars() {
    
    _eigen_vecs.clear();
    _eigen_val.clear();

    _cparms_v.clear();

    _line.resize(4);
    _line.clear();
  }


  
  void PCASegmentation::connect(const PCABox& box,                  //incoming line to compare too
				       const std::vector<PCABox>& boxes,   //reference to all the lines
				       std::map<int,bool>& used,           //used lines
				       const std::map<int,std::vector<int> >& neighbors,
				       std::map<int,std::vector<int> >& combined,
				       int i,int k) {
    

    if ( neighbors.count(k) <= 0 ) 
      return;
    
    for(const auto& n : neighbors.at(k)) { // std::vector of neighbors
      
      if ( used[n] ) continue;

      const auto& box2 = boxes.at(n);
      
      //Here you make the decision if you should connected the boxes or not
      if ( !compatible(box,box2) )
	continue;
      
      combined[i].push_back(n);
      used[n] = true;
      
      connect(box2,boxes,used,neighbors,combined,i,n);
    }
    
    return;
      
  }

  void PCASegmentation::check_linearity(PCABox& box, std::vector<PCABox>& boxes,int ndivisions) {

    //don't try divide anymore, just keep it
    if ( ! ndivisions )
      { boxes.emplace_back(box); return; }
    
    --ndivisions;
    
    if ( std::abs(box.cov_) < box.cov_cut_ ) {
      
      auto subboxes = sub_divide(box,4);
      
      if ( box.subdivided_ ) {

	for(auto& b: subboxes) {

	  if ( b.empty_ ) {  boxes.emplace_back(b); continue; }

	  check_linearity(b,boxes,ndivisions);
	  
	}
      } else {
	//nothing happened when I tried to subdivide, it's a box with
	//bad linearity, lets just call this box "empty?"
	// box.empty_ = true;
	boxes.emplace_back(box);
      }
    } else { 
      // pass linearity test, this is a "good" box
      boxes.emplace_back(box);
    }
  
  }



  void PCASegmentation::cross_empty_neighbors(const std::vector<PCABox>& boxes,
					      std::map<int,std::vector<int> >& neighbors) {
    

    for(unsigned b = 0; b < boxes.size(); ++b) {
      auto& box = boxes[b];
      
      //i'm empty, move on
      if ( box.empty_ ) continue;
	
      //no neighbors anyways, move on
      if ( neighbors.count(b) <= 0 ) continue;

      //my neighbors
      auto& neighbor = neighbors[b];

      for(auto& n : neighbor) {
	  
	// no neighbors, move on
	if ( neighbors.count(n) <= 0 ) continue;
	  
	// this box NOT empty, i DONT want it's neighbors
	if ( ! boxes.at(n).empty_ ) continue;
	  
	// this box is empty, loop over its neighbors
	for ( const auto& en : neighbors[n] ) {

	  // I don't wany myself, move on
	  if ( en == b ) continue;
	    
	  // has no neighbors, move on
	  if ( neighbors.count(en) <= 0 ) continue;
	    
	  // its empty, i DONT want your neighbors ( depth == 1 for now)
	  if ( boxes.at(en).empty_ ) continue;
	    
	  //I have you already, move on
	  if( std::find(neighbor.begin(), neighbor.end(), en) != neighbor.end())
	    continue;

	  //I don't have you as a neighbor, give me that
	  neighbor.push_back(en);

	}//end empty neighbors
	  
      }//end my neighbors
	
    }// end ridiculous logic
  }
}

#endif
