#ifndef __PCASEGMENTATIONCOMBINE_CXX__
#define __PCASEGMENTATIONCOMBINE_CXX__

#include "PCASegmentationCombine.h"

namespace larcv{

  void PCASegmentationCombine::_Configure_(const ::fcllite::PSet &pset)
  {

    _min_trunk_length = pset.get<int>    ("MinTrunkLength");
    _trunk_deviation  = pset.get<double> ("TrunkDeviation");
    _closeness        = pset.get<double> ("Closeness");


    _segments_x = pset.get<int> ("NSegmentsX");
    _segments_y = pset.get<int> ("NSegmentsY");
    
    _nhits_cut  = pset.get<int> ("NHitsCut");
      
    _outtree = new TTree("PCA","PCA");
    
    _outtree->Branch("_eval1",&_eval1,"eval1/D");
    _outtree->Branch("_eval2",&_eval2,"eval2/D");

    _outtree->Branch("_area",&_area,"area/D");
    _outtree->Branch("_perimeter",&_perimeter,"perimeter/D");

    _outtree->Branch("_trunk_length",&_trunk_length,"trunk_length/D");
    _outtree->Branch("_trunk_cov",&_trunk_cov,"trunk_cov/D");

    
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

    for(unsigned i = 0; i < clusters.size(); ++i) {

      auto& cluster    =  clusters[i];
      auto& cntr_pt    = _cntr_pt;
      auto& eigen_vecs = _eigen_vecs;
      auto& eigen_val  = _eigen_val;
      auto& line       = _line;      
      
      auto& trunk_index= _trunk_index; //index trunk end point (pair)
      auto& pearsons_r = _pearsons_r;  //linearity

      //Lets get subimage that only includes this cluster
      //and the points in immediate vicinity
      ::cv::Rect rect = ::cv::boundingRect(cluster);
      ::cv::Mat subMat(img, rect);

      
      std::vector<::cv::Rect> rec;
      std::vector<std::vector<double> > llines;
      std::vector<int> ccharge;
      std::vector<std::pair<double,double> > mean_loc;
      std::vector<double> covar_loc;
      std::map<int,std::vector<double> > neighbors;
      std::vector<double> slopes;
      
      int ijvec[nsegments_x+2][nsegments_y+2];
      for(int i = 0; i < nsegments_x+2; ++i)
	for(int j = 0; j < nsegments_y+2; ++j)
	  ijvec[i][j] = -1;
      
      int ij = 0;
      for(unsigned i = 1; i <= nsegments_x; ++i) {
	for(unsigned j = 1; j <= nsegments_y; ++j) {

	  ijvec[1][1] = -1;
	  
	  auto dx = rect.width  / nsegments_x;
	  auto dy = rect.height / nsegments_y;

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
	  
	  if( ! pca_line(subMat,inside_locations,r,line) ) // only scary part is contour may be "tangled?"
	    continue;
	  
	  
	  mean_loc.push_back ( get_mean_loc  (r,inside_locations) );
	  covar_loc.push_back( get_roi_cov   (inside_locations) );
	  ccharge.push_back  ( get_charge_sum(subMat,inside_locations) );
	  rec.emplace_back(r);	  
	  llines.emplace_back(line);
	  slopes.push_back( ( (double) line[3] - (double) line[1] ) / ( (double) line[2] - (double) line[0] ) );
	  ijvec[i][j] = ij; ++ij;
	}
      }
      
      //We will try to connect adjacent boxes
      
      for(unsigned i = 1; i <= nsegments_x; ++i) {
	for(unsigned j = 1; j <= nsegments_y; ++j) {

	  if ( ijvec[i][j] < 0 )
	    continue;


	  // 8 cases to check neighbors... disgusting
	  if ( ijvec[i-1][j] >= 0  )
	    neighbors[ ijvec[i][j] ].push_back(ijvec[i-1][j]);

	  if ( ijvec[i-1][j-1] >= 0  )
	    neighbors[ ijvec[i][j-1] ].push_back(ijvec[i-1][j-1]);

	  if ( ijvec[i][j-1] >= 0  )
	    neighbors[ ijvec[i][j] ].push_back(ijvec[i][j-1]);

	  if ( ijvec[i+1][j] >= 0  )
	    neighbors[ ijvec[i][j] ].push_back(ijvec[i+1][j+1]);

	  if ( ijvec[i+1][j+1] >= 0  )
	    neighbors[ ijvec[i][j] ].push_back(ijvec[i+1][j+1]);

	  if ( ijvec[i][j+1] >= 0  )
	    neighbors[ ijvec[i][j] ].push_back(ijvec[i][j+1]);

	  if ( ijvec[i+1][j-1] >= 0  )
	    neighbors[ ijvec[i][j] ].push_back(ijvec[i+1][j-1]);

	  if ( ijvec[i-1][j+1] >= 0  )
	    neighbors[ ijvec[i][j] ].push_back(ijvec[i-1][j+1]);
	}
      }
      

      double angle_cut = 15; //degrees
      std::map<int,std::vector<int> > combined;
      angle_cut *= 3.14159/180.0;

      std::map<int,bool> used; for(int i = 0; i < ij; ++i) { used[i] = false; }

      // loop over the squares and get neighbors
      for (int i = 0; i < ij; ++i) {

	if ( used[i] ) continue;
		 
	const auto& line1 = llines.at(i);
	  
	for(const auto& n : neighbors[i]) { // std::vector of neighbors

	  if ( used[n] ) continue;
	  
	  const auto& line2 = llines.at(n);

	  auto cos_angle = compute_angle(line1,line2);
	  
	  if ( std::acos(cos_angle) <= angle_cut ) { //angle between PCA lines //use acos since it returns smallest angle
	    combined[i].append(n);
	    used[i] = true; used[n] = true;
	  }
	  
	}
      }
      

      
      //weighted mean...
      double mean_slope = 0.0;
      double mean_x = 0.0;
      double mean_y = 0.0;
      double sum_charge = 0.0;
      for(int i = 0; i < llines.size(); ++i) {
	auto& li = llines.at(i);
	
	auto slope = ( (double) li[3] - (double) li[1] )/((double) li[2] - (double) li[0]);

	sum_charge += ccharge[i];
	mean_slope += slope*ccharge[i];

	mean_x += mean_loc[i].first *ccharge[i];
	mean_y += mean_loc[i].second*ccharge[i];
      }


      mean_slope /= sum_charge;
      mean_x     /= sum_charge;
      mean_y     /= sum_charge;

      mean_x -= rect.x;
      mean_y -= rect.y;
      
      std::vector<double> meanline; meanline.resize(4);

      meanline[0] = rect.x;
      meanline[1] = mean_y + ( (0 - mean_x) / 1.0 ) * mean_slope + rect.y;
      meanline[2] = rect.x + rect.width;
      meanline[3] = mean_y + ( (rect.width - mean_x) / 1.0) * mean_slope + rect.y;

      
      mean_x += rect.x;
      mean_y += rect.y;


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
      line[0] = 0;
      line[1] = cntr_pt.y + ( (0 - cntr_pt.x) / eigen_vecs[0].x ) * eigen_vecs[0].y;
      line[2] = rect.width;
      line[3] = cntr_pt.y + ( (rect.width - cntr_pt.x) / eigen_vecs[0].x) * eigen_vecs[0].y;
      
      std::map<int,double> odist; // ordered left to right closest distance to line
      std::map<int,int>    opts;  // ordered points (hit x, hit y)
      std::map<int,std::pair<int,int> >    cpts;  // closest points (ordered on line)
	
      int nhits = 0;
      
      //find how far away the points are to the line
      std::vector<std::pair<int,int> > hits;
      std::vector<int> charge;
      for(const auto& loc : locations) {

	//is this point in the contour, if not continue
	if ( ::cv::pointPolygonTest(cluster_s, loc,false) < 0 )
	  continue;
	
	charge.push_back( (int) subMat.at<uchar>(loc.y,loc.x) );
	
	auto dist     = distance_to_line     (line,loc.x,loc.y);
	auto close_pt = closest_point_on_line(line,loc.x,loc.y);
	
	odist[close_pt.first] = dist;    // ordered set of distance by wire number
	opts [loc.x]          = loc.y;   // ordered set of mapped X->Y values
	cpts [close_pt.first] = std::make_pair(loc.x,loc.y);
	hits.emplace_back(loc.x + rect.x, loc.y + rect.y);
	++nhits;
       
      }
      
      // you must have atleast 10 points to proceed or trunk is useless
      if ( nhits < 10 ) // j == number of points in cluster
	continue;
      
      
      //lets march through the line left to right
      //put the points in ddd for python opts holds ordered hits
      std::vector<int> opts_x; opts_x.reserve(odist.size());
      std::vector<int> opts_y; opts_y.reserve(odist.size());

      std::vector<double> dists; dists.reserve(nhits); // dists ordered as vector
      
      for(auto& pt: cpts) {
	dists.push_back ( odist[pt.first]  );
	opts_x.push_back( pt.second.first  );
	opts_y.push_back( pt.second.second );
      }
      
      //find the trunk with this info 
      
      //start left to right
      trunk_index = {0,0};
      
      int k  = 0;	
      int j  = 0;
      int y  = 0;
      
      for(j = 0 ; j < dists.size(); ++j) 
	if ( dists[j] > _trunk_deviation )
	  break;
      
      for(k = dists.size() - 1; k >= 0; --k) 
	if ( dists[k] > _trunk_deviation )
	  break;

      if ( j >= dists.size() ) j = dists.size() - 1;
      if (     k <=  0       ) k = 0;
      
      k = (dists.size() - 1 - k);
      
      if ( j < k ) {
	trunk_index = { dists.size() - 1 - k, dists.size() - 1}; // trunk is on the right
	std::cout << "\t==> Trunk is on the right...\n";
      }
      else if ( j == k ) {
	std::cout << "\t==> NO trunk is found...\n";
	trunk_index = { 0 , 0 }; // no trunk on either side
	//ignore this cluster...
	continue;
      }
      else  {
	trunk_index = { 0 , j }; // trunk is on the left
	std::cout << "\t==> Trunk is on the left\n";
      }
      
      // Should we reject bad trunks? This probably involves some fit?
      // the showering region needs enough hits with enough sparsity
      // to be actual shower like
      
      auto co = cov  (opts_x,opts_y,trunk_index.first,trunk_index.second);
      auto sx = stdev(opts_x,trunk_index.first,trunk_index.second);
      auto sy = stdev(opts_y,trunk_index.first,trunk_index.second);

      if ( trunk_index.first != trunk_index.second) // trunk exists
	pearsons_r =  co / ( sx * sy );
      else
	pearsons_r = 0;

      _trunk_cov    = std::abs(pearsons_r);
      _trunk_length = std::sqrt( std::pow(opts_x.at( trunk_index.second ) - opts_x.at( trunk_index.first ),2.0) +
				 std::pow(opts_y.at( trunk_index.second ) - opts_y.at( trunk_index.first ),2.0) );
      
      auto normal = std::sqrt( eigen_val[0]*eigen_val[0] + eigen_val[1]*eigen_val[1] ) ;
      
      _eval1 = eigen_val[0] / normal;
      _eval2 = eigen_val[1] / normal;
      
      _area      = (double) ::cv::contourArea(cluster);
      _perimeter = (double) ::cv::arcLength  (cluster,1);

      auto shower_len   = std::sqrt(rect.width*rect.width + rect.height*rect.height);
      auto eigen_normal = std::sqrt(std::pow(eigen_vecs[0].x,2) + std::pow(eigen_vecs[0].y,2));

      
      int start_idx = trunk_index.first == 0 ? trunk_index.first : trunk_index.second;

      
      std::vector<double> lline = {line[0] + rect.x,line[1] + rect.y,line[2]+rect.x,line[3]+rect.y};
      _cparms_v.emplace_back(i,
			     _trunk_length,
			     _trunk_cov,
			     shower_len,
			     _eval1,
			     _eval2,
			     _area,
			     _perimeter,
			     opts_x.at(start_idx) + rect.x,
			     opts_y.at(start_idx) + rect.y,
			     eigen_vecs[0].x / eigen_normal,
			     eigen_vecs[0].y / eigen_normal,
			     nhits,
			     hits,
			     lline,
			     charge,
			     rec,
			     llines,
			     meanline,
			     mean_x,
			     mean_y,
			     covar_loc,
			     combined);
      
      
      _outtree->Fill();
      
    }
    
    for (int i = 0; i < _cparms_v.size(); ++i) {
      for (int j = 0; j < _cparms_v.size(); ++j) {
	
	if ( i == j ) continue;
	
	auto& cparm1 = _cparms_v[i];
	auto& cparm2 = _cparms_v[j];

	if (cparm1.trunk_cov_ == 0) continue;
	if (cparm2.trunk_cov_ == 0) continue;
	
	
	cparm1.compare(cparm2);
	
	
      }
    }
    
    
    //just return the clusters
    return clusters;


  }



  double PCASegmentationCombine::stdev( std::vector<int>& data,
					size_t start, size_t end )
  {

    if ( (end - start) <=0 ) return 0.0;
    
    double result = 0.0;
    auto    avg   = mean(data,start,end);
    
    for(int i = start; i <= end; ++i)
      result += (data.at(i) - avg)*(data.at(i) - avg);
    
    return std::sqrt(result/((double)(end - start)));
  }

  
  double PCASegmentationCombine::cov ( std::vector<int>& data1,
				       std::vector<int>& data2,
				       size_t start, size_t end )
  {
    
    if ( start == end ) return 0.0;
    
    double result = 0.0;
    auto   mean1  = mean(data1,start,end);
    auto   mean2  = mean(data2,start,end);
    
    for(int i = start; i <= end; ++i)
      result += (data1.at(i) - mean1)*(data2.at(i) - mean2);
    
    return result/((double)(end - start));
  }
  
  double PCASegmentationCombine::mean( std::vector<int>& data,
				       size_t start, size_t end )
  {
    if ( start == end ) return 0.0;
    double result = 0.0;
    
    for(int i = start; i <= end; ++i)
      result += data.at(i);
    
    return result / ((double)( end - start ));
  }  

  void PCASegmentationCombine::clear_vars() {

    _eigen_vecs.clear();
    _eigen_val.clear();

    _cparms_v.clear();
  }

  double PCASegmentationCombine::distance_to_line(std::array<double,4>& line,int lx,int ly) {

    //real time collision detection page 128
    //closest point on line
    auto ax = line[0];
    auto ay = line[1];
    auto bx = line[2];
    auto by = line[3];
	
    auto abx = bx-ax;
    auto aby = by-ay;
	
    auto t = ( (lx-ax)*abx + (ly-ay)*aby ) / (abx*abx + aby*aby);
    auto dx = ax + t * abx;
    auto dy = ay + t * aby;
    auto dist = (dx-lx)*(dx-lx) + (dy-ly)*(dy-ly); //squared distance
    return dist;
  }
  
  std::pair<double,double> PCASegmentationCombine::closest_point_on_line(std::array<double,4>& line,int lx,int ly) {
    
    //real time collision detection page 128
    //closest point on line
    auto ax = line[0];
    auto ay = line[1];
    auto bx = line[2];
    auto by = line[3];
	
    auto abx = bx-ax;
    auto aby = by-ay;
	
    auto t = ( (lx-ax)*abx + (ly-ay)*aby ) / (abx*abx + aby*aby);
    auto dx = ax + t * abx;
    auto dy = ay + t * aby;

    return std::make_pair(dx,dy);
  }

  bool PCASegmentationCombine::pca_line(const ::cv::Mat& subimg,
					Contour_t cluster_s,
					const ::cv::Rect& rect,
					std::vector<double>& line) {
    
    
    //shift it down, yes I want explicit copy of the contour...

    //put this back in if using ACTUAL CONTOUR ROI
    //for(auto &pt : cluster_s) { pt.x -= rect.x; pt.y -= rect.y; }

    ::cv::Mat ctor_pts(cluster_s.size(), 2, CV_64FC1);
	

    for (unsigned i = 0; i < ctor_pts.rows; ++i) {
      ctor_pts.at<double>(i, 0) = cluster_s[i].x;
      ctor_pts.at<double>(i, 1) = cluster_s[i].y;
    }
      
    ::cv::PCA pca_ana(ctor_pts, ::cv::Mat(), CV_PCA_DATA_AS_ROW,0);
      
    //Center point
    // ::cv::Point
    auto cntr_pt = Point2D( pca_ana.mean.at<double>(0,0),
			    pca_ana.mean.at<double>(0,1) );
    

    //Principle directions (vec) and relative lengths (vals)
    std::vector<Point2D> eigen_vecs;
    std::vector<double>  eigen_val;
    
    eigen_vecs.resize(2);
    eigen_val.resize (2);
      
    for (unsigned i = 0; i < 2; ++i) {
      eigen_vecs[i] = Point2D(pca_ana.eigenvectors.at<double>(i, 0),
			      pca_ana.eigenvectors.at<double>(i, 1));
      eigen_val[i]  = pca_ana.eigenvalues.at<double>(0, i);
    }


    line[0] = 0 + rect.x;
    line[1] = cntr_pt.y + ( (0 - cntr_pt.x) / eigen_vecs[0].x ) * eigen_vecs[0].y + rect.y;
    line[2] = rect.width + rect.x;
    line[3] = cntr_pt.y + ( (rect.width - cntr_pt.x) / eigen_vecs[0].x) * eigen_vecs[0].y + rect.y;

    return true;
  }

  std::pair<double,double> PCASegmentationCombine::get_mean_loc(const ::cv::Rect& rect,
								const Contour_t& pts ) {
    
    double mean_x = 0.0;
    double mean_y = 0.0;
    
    for(const auto& pt : pts) { mean_x += pt.x;  mean_y += pt.y; }
    
    return { mean_x / ( (double) pts.size() ) + rect.x,mean_y / ( (double) pts.size() ) + rect.y};
  }
  
  int PCASegmentationCombine::get_charge_sum(const ::cv::Mat& subImg, const Contour_t& pts ) {
    int charge_sum = 0;
    for(const auto& pt : pts ) charge_sum += (int) subImg.at<uchar>(pt.y,pt.x);
    return charge_sum;
  }

  double PCASegmentationCombine::get_roi_cov(const Contour_t & pts) {

    std::vector<int> x_; x_.resize(pts.size()); 
    std::vector<int> y_; y_.resize(pts.size());
    for(unsigned i = 0; i < pts.size(); ++i)
      { x_[i] = pts[i].x; y_[i] = pts[i].y; }
    
    double pearsons_r;
    auto co = cov  (x_,y_,0,pts.size() - 1);
    auto sx = stdev(x_,   0,pts.size() - 1);
    auto sy = stdev(y_,   0,pts.size() - 1);

    if ( sx != 0 && sy != 0) // trunk exists
      pearsons_r =  co / ( sx * sy );
    else
      pearsons_r = 0;
    
    return pearsons_r;
  }

  double PCASegmentationCombine::compute_angle(const std::vector<double>& line1,
					       const std::vector<double>& line2) {

    auto ax = line1[2] - line1[0];
    auto ay = line1[3] - line1[1];
    auto sq = std::sqrt(ax*ax + ay*ay);

    ax /= sq;
    ay /= sq;

    auto bx = line2[2] - line2[0];
    auto by = line2[3] - line2[1];
    sq = std::sqrt(bx*bx + by*by);

    bx /= sq;
    by /= sq;

    return ax*bx + ay*by;
    
  }
}

#endif
