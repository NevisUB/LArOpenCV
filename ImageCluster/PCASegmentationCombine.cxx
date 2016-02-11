//by vic
#ifndef __PCASEGMENTATIONCOMBINE_CXX__
#define __PCASEGMENTATIONCOMBINE_CXX__

#include "PCASegmentationCombine.h"
#include "PCAUtilities.h"

namespace larcv{

  void PCASegmentationCombine::_Configure_(const ::fcllite::PSet &pset)
  {

    _min_trunk_length = pset.get<int>    ("MinTrunkLength");
    _trunk_deviation  = pset.get<double> ("TrunkDeviation");
    _closeness        = pset.get<double> ("Closeness");


    _segments_x = pset.get<int> ("NSegmentsX");
    _segments_y = pset.get<int> ("NSegmentsY");
    
    _nhits_cut  = pset.get<int>   ("NHitsCut");
    _angle_cut  = pset.get<double>("AngleCut");

    _cov_breakup= pset.get<double>("CovSubDivide");
    
    _outtree = new TTree("PCA","PCA");
    
    _outtree->Branch("_eval1",&_eval1,"eval1/D");
    _outtree->Branch("_eval2",&_eval2,"eval2/D");

    _outtree->Branch("_area",&_area,"area/D");
    _outtree->Branch("_perimeter",&_perimeter,"perimeter/D");

    // _outtree->Branch("_trunk_length",&_trunk_length,"trunk_length/D");
    // _outtree->Branch("_trunk_cov",&_trunk_cov,"trunk_cov/D");

  }
  
  ContourArray_t PCASegmentationCombine::_Process_(const larcv::ContourArray_t& clusters,
						   const ::cv::Mat& img,
						   larcv::ImageMeta& meta)
  {

    //http://docs.opencv.org/master/d3/d8d/classcv_1_1PCA.html
    
    //cluster == contour
    ContourArray_t ctor_v;    
    clear_vars();
    _cparms_v.reserve(clusters.size());
    //All of these are diagnostics for the python view program

    for(unsigned u = 0; u < clusters.size(); ++u) {

      auto& cluster    =  clusters[u];
      auto& cntr_pt    = _cntr_pt;
      auto& eigen_vecs = _eigen_vecs;
      auto& eigen_val  = _eigen_val;
      auto& lline      = _line;      
      
      auto& pearsons_r = _pearsons_r;  //linearity

      //Lets get subimage that only includes this cluster
      //and the points in immediate vicinity
      ::cv::Rect rect = ::cv::boundingRect(cluster);
      ::cv::Mat subMat(img, rect);

      std::vector<std::pair<double,double> > mean_loc;
      std::vector<PCABox> boxes;
      std::map<int,std::vector<int> > neighbors;
      
      // we need a way for a box to tell what his neighbors are
      // we can do this based on ij indexing. Give me location on
      // the grid of the box (i,j) i will tell you who your neighbors
      // are in the std::vector of boxes
      //SCRAP THIS 

      double angle_cut = _angle_cut; //degrees
      angle_cut *= 3.14159/180.0; //to radians


      auto dx = rect.width  /  _segments_x;
      auto dy = rect.height /  _segments_y;

      for(unsigned i = 0; i < _segments_x; ++i) {
	for(unsigned j = 0; j < _segments_y; ++j) {


	  auto x  = i * dx;
	  auto y  = j * dy;
	  
	  ::cv::Rect r(x + rect.x,y + rect.y,dx,dy);
	  ::cv::Mat subMat(img, r);
	  std::vector<double> line; line.resize(4);
	  
	  Contour_t locations;
	  Contour_t inside_locations;
	  
	  ::cv::findNonZero(subMat, locations);

	  if ( locations.size() == 0 ) 
	    continue;
	  
	  for(auto& loc : locations) {
	    //must be "in" the contour...
	    loc.x += r.x;
	    loc.y += r.y;
	    
	    if ( ::cv::pointPolygonTest(cluster, loc ,false) < 0 )
	      continue;
	    
	    loc.x -= r.x;
	    loc.y -= r.y;
	    
	    inside_locations.emplace_back(loc);
	  }
	  
	  if ( inside_locations.size() == 0 ) 
	    continue;
	  
	  if ( inside_locations.size() <= _nhits_cut )
	    continue;
	  
	  Point2D e_vec;
	  Point2D e_center;
	  
	  if( ! pca_line(inside_locations,
			 r,line,e_vec,e_center) ) // only scary part is contour may be "tangled" ?
	    continue;
	  
	  auto cov = get_roi_cov(inside_locations);
	  PCABox box(e_vec,e_center,cov,line,inside_locations,r);
	  box.SetAngleCut(angle_cut);
	  check_linearity(box,angle_cut,0.75,boxes,2);
	  
	 
	}
      }

      
      
      if ( boxes.size() == 0 )
	continue;
      

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
      
      std::map<int,std::vector<int> > combined; //combined boxes            
      std::map<int,bool> used; for(int i = 0; i < boxes.size(); ++i) { used[i] = false; }
      
      if ( boxes.size() > 1 ) {
	
	for (int i = 0; i < boxes.size(); ++i) {
	  
	  if ( used[i] ) continue;
	
	  const auto& box = boxes.at(i);
	  used[i] = true;

	  connect(box,boxes,used,neighbors,combined,i,i);
	}
	
	std::cout << "Finished connecting..." << "\n";
	std::cout << "\n\n";
	for(const auto& k : combined) {
	  std::cout << "\t key: " << k.first << " : {";
	  for(const auto& kk : k.second) {
	    std::cout << kk << ",";
	  }
	  std::cout << "}\n";
	}
	
      }      
      
      
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
      if ( nhits < 10 ) // j == number of points in cluster
	continue;
      
      //find the trunk with this info 
      
      auto normal = std::sqrt( eigen_val[0]*eigen_val[0] + eigen_val[1]*eigen_val[1] ) ;
      
      _eval1 = eigen_val[0] / normal;
      _eval2 = eigen_val[1] / normal;
      
      _area      = (double) ::cv::contourArea(cluster);
      _perimeter = (double) ::cv::arcLength  (cluster,1);
      
      auto eigen_normal = std::sqrt(std::pow(eigen_vecs[0].x,2) + std::pow(eigen_vecs[0].y,2));
      
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
			     combined);
      
      
      _outtree->Fill();
      
    }
    
    // for (int i = 0; i < _cparms_v.size(); ++i) {
    //   for (int j = 0; j < _cparms_v.size(); ++j) {
	
    // 	if ( i == j ) continue;
	
    // 	auto& cparm1 = _cparms_v[i];
    // 	auto& cparm2 = _cparms_v[j];

    // 	cparm1.compare(cparm2);
	
	
    //   }
    // }
    
    
    //just return the clusters
    return clusters;


  }




  void PCASegmentationCombine::clear_vars() {

    _eigen_vecs.clear();
    _eigen_val.clear();

    _cparms_v.clear();

    _line.resize(4);
    _line.clear();
  }



  
  void PCASegmentationCombine::connect(const PCABox& box,                  //incoming line to compare too
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
      if ( ! box.compatible(box2) )
	continue;
      
      combined[i].push_back(n);
      used[n] = true;
      
      connect(box2,boxes,used,neighbors,combined,i,n);
    }
    
    return;
      
  }

  //check_linearity(box,angle_cut,0.75,boxes);
  void PCASegmentationCombine::check_linearity(PCABox& box, double angle_cut, double cov_cut, std::vector<PCABox>& boxes,int ndivisions) {

    if ( ! ndivisions )
      { box.SetAngleCut(angle_cut); boxes.emplace_back(box); return; }

    --ndivisions;
	
    if ( std::abs(box.cov_) < cov_cut ) {

      box.SubDivide(4);

      if ( box.subdivided_ ) {

	for(auto& b: box.subboxes_) {

	  if ( b.empty_ )
	    continue;

	  check_linearity(b,angle_cut,cov_cut,boxes,ndivisions);

	  // b.SetAngleCut(angle_cut);
	  // boxes.emplace_back(b);
	  
	}
	
      }
      else
	{ box.SetAngleCut(angle_cut); boxes.emplace_back(box); }
      
    }
    else
      { box.SetAngleCut(angle_cut); boxes.emplace_back(box); }
  
  
  }
    
	  

}

#endif
