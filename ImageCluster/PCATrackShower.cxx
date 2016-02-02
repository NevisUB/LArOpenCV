#ifndef __PCATRACKSHOWER_CXX__
#define __PCATRACKSHOWER_CXX__

#include "PCATrackShower.h"

namespace larcv{

  void PCATrackShower::_Configure_(const ::fcllite::PSet &pset)
  {

    _min_trunk_length = pset.get<int>    ("MinTrunkLength");
    _trunk_deviation  = pset.get<double> ("TrunkDeviation");
    
    _outtree = new TTree("PCA","PCA");
    
    _outtree->Branch("_eval1",&_eval1,"eval1/D");
    _outtree->Branch("_eval2",&_eval2,"eval2/D");

    _outtree->Branch("_dists_v" , "std::vector<double>" , &_dists_v);
    
    _outtree->Branch("_area",&_area,"area/D");
    _outtree->Branch("_perimeter",&_perimeter,"perimeter/D");

    _outtree->Branch("_trunk_length",&_trunk_length,"trunk_length/D");
    _outtree->Branch("_trunk_cov",&_trunk_cov,"trunk_cov/D");
  }

  ContourArray_t PCATrackShower::_Process_(const larcv::ContourArray_t& clusters,
					   const ::cv::Mat& img,
					   larcv::ImageMeta& meta)
  {

    
    //http://docs.opencv.org/master/d3/d8d/classcv_1_1PCA.html
    
    //cluster == contour
    ContourArray_t ctor_v;    


    //Most of these are diagnistics...
    _subMat_v.clear();
    _dists_v.clear();
    
    _cntr_pt_v.clear();
    _cntr_pt_v.resize(clusters.size());
    
    _eigen_vecs_v.clear();
    _eigen_vecs_v.resize(clusters.size());
    
    _eigen_val_v.clear();
    _eigen_val_v.resize(clusters.size());
    
    _line_v.clear();
    _line_v.resize(clusters.size());
    
    _dlines_v.clear();
    _dlines_v.resize(clusters.size());
    
    _ddists_v.clear();
    _ddists_v.resize(clusters.size());
    
    _ddd_v.clear();
    _ddd_v.resize(clusters.size());
    
    _trunk_index_v.clear();
    _trunk_index_v.resize(clusters.size());

    _pearsons_r_v.clear();
    _pearsons_r_v.resize(clusters.size());

    _trunk_len_v.clear();
    _trunk_len_v.resize(clusters.size());
    
    for(unsigned i = 0; i < clusters.size(); ++i) {

      auto& cluster    = clusters[i];
      auto& cntr_pt    = _cntr_pt_v[i];
      auto& eigen_vecs = _eigen_vecs_v[i];
      auto& eigen_val  = _eigen_val_v[i];
      auto& line       = _line_v[i];
      
      auto& dlines     = _dlines_v[i];
      auto& ddists     = _ddists_v[i];
      auto& ddd        = _ddd_v[i];
      auto& trunk_index= _trunk_index_v[i];
      auto& pearsons_r = _pearsons_r_v[i];
      auto& trunk_len  = _trunk_len_v[i];
      
      //Lets get subimage that only includes this cluster
      //and the points in immediate vicinity
      ::cv::Rect rect = ::cv::boundingRect(cluster);
      ::cv::Mat subMat(img, rect);
      
      //subMat holds pixels in the bounding rectangle
      //around the given contour. We can make a line from principle eigenvector
      //and compute the distance to this line of the hits may be weighted by
      //charge at some point?

      //first lets make sure the image is what i freaking think it is (view in python)
      _subMat_v.emplace_back(subMat);
      //ok it works
      
      //make a copy cluster_s = shifted clusters to the bounding box
      auto cluster_s = cluster;

      //shift it down
      for(auto &pt : cluster_s) { pt.x -= rect.x; pt.y -= rect.y; }
      ctor_v.emplace_back(cluster_s);
      //just checked it works fine

      
      // PCA ana requies MAT object w/ data sitting in rows, 2 columns
      // 1 for each ``feature" or coordinate
      ::cv::Mat ctor_pts(cluster_s.size(), 2, CV_64FC1);
      
      for (unsigned i = 0; i < ctor_pts.rows; ++i)
	{
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
      eigen_val.resize(2);
      
      for (unsigned i = 0; i < 2; ++i) {
	eigen_vecs[i] = Point2D(pca_ana.eigenvectors.at<double>(i, 0),
				pca_ana.eigenvectors.at<double>(i, 1));
	eigen_val[i]  = pca_ana.eigenvalues.at<double>(0, i);
      }


      //Get the nonzero locations of the pixels in this subimage
      //these are the "hits"
      std::vector<::cv::Point> locations;
      ::cv::findNonZero(subMat, locations);
      
      //take principle eigenvector (the first one?)
      //and get the line that best represents the shower
      //this line passes through center point and is principle eigenvec
      line[0] = 0;
      line[1] = cntr_pt.y + ( (0 - cntr_pt.x) / eigen_vecs[0].x ) * eigen_vecs[0].y;
      line[2] = rect.width;
      line[3] = cntr_pt.y + ( (rect.width - cntr_pt.x) / eigen_vecs[0].x) * eigen_vecs[0].y;

      //diagnostics
      dlines.reserve(locations.size());
      ddists.reserve(locations.size());
      _dists_v.clear();
      ddd.clear();
      
      std::map<int,double> ordered_dist;
      std::map<int,int>    ordered_pts;

      int j = 0;

      //lets try to find how far away the points are to the line
      for(const auto& loc : locations) {
	
	//is this point in the contour, if not continue
	if ( ::cv::pointPolygonTest(cluster_s, loc,false) < 0 )
	  continue;
	  
	//real time collision detection page 128
	//closest point on line
	auto ax = line[0];
	auto ay = line[1];
	auto bx = line[2];
	auto by = line[3];
	
	auto abx = bx-ax;
	auto aby = by-ay;

	auto& lx = loc.x;
	auto& ly = loc.y;
	
	auto t = ( (lx-ax)*abx + (ly-ay)*aby ) / (abx*abx + aby*aby);
	auto dx = ax + t * abx;
	auto dy = ay + t * aby;
	auto dist = (dx-lx)*(dx-lx) + (dy-ly)*(dy-ly) ; //squared dis
	
	dlines.push_back( { (double)lx, (double)ly, dx, dy } );
	ddists.push_back( dist );
	_dists_v.push_back(ddists[j]);
	
	ordered_dist[lx] = dist;
	ordered_pts [lx] = ly;
	++j;
	
      }

      // you must have atleast 10 points to proceed...
      
      if ( dlines.size() < 10 )
	continue;
	
      
      //lets march through the line left to right
      //put the points in ddd for python
      std::vector<int>  opts_x; opts_x.reserve(ordered_dist.size());
      std::vector<int>  opts_y; opts_y.reserve(ordered_dist.size());
      
      for(auto& pt: ordered_dist) {
	ddd.emplace_back(   pt.first,pt.second  );
	opts_x.push_back(      pt.first         );
	opts_y.push_back( ordered_pts[pt.first] );
      }
      
      //find the trunk with this info (hopefully wont have to
      //implement truncated mean again... )
      
      //start left to right
      trunk_index = {0,0};
      
      j = 0;
      int k  = 0;	
      
      for(j = 0 ; j < ddd.size(); ++j) 
	if ( ddd[j].second > _trunk_deviation )
	  break;

      for(k = ddd.size() - 1; k >= 0; --k) 
	if ( ddd[k].second > _trunk_deviation )
	  break;

      if ( j >= ddd.size() ) j = ddd.size() - 1;
      if (     k <=  0     ) k = 0;

      // std::cout << "\t==> j = " << j << " ddd.size() " << ddd.size() << "\n";
      
      k = (ddd.size() - 1 - k);
      
      if ( j < k )
	trunk_index = { ddd.size() - 1 - k, ddd.size() - 1 }; // trunk is on the right
      else if ( j == k )
	trunk_index = { 0 , 0 }; // no trunk
      else 
	trunk_index = { 0 , j }; // trunk is on the left
      
      // Should we reject bad trunks? This probably involves some fit?
      // the showering region needs enough hits with enough sparsity
      // to be actual shower like
      
      auto co = cov  (opts_x,opts_y,trunk_index.first,trunk_index.second);
      auto sx = stdev(opts_x,trunk_index.first,trunk_index.second);
      auto sy = stdev(opts_y,trunk_index.first,trunk_index.second);

      if ( trunk_index.first != trunk_index.second)
	pearsons_r =  co / ( sx * sy );
      else
	pearsons_r = 0;

      // std::cout << "\t==> co : " << co << " sx : "
      // 		<< sx << " sy : " << sy << " pearsons_r : "
      // 		<< pearsons_r << "\n";

      _trunk_cov    = pearsons_r;
      _trunk_length = std::sqrt( std::pow(opts_x.at( trunk_index.second ) - opts_x.at( trunk_index.first ),2.0) +
				 std::pow(opts_y.at( trunk_index.second ) - opts_y.at( trunk_index.first ) ,2.0) );
      trunk_len = _trunk_length;
	
      _eval1 = eigen_val[0];
      _eval2 = eigen_val[1];
      
      _area      = (double) ::cv::contourArea(cluster);
      _perimeter = (double) ::cv::arcLength(cluster,1);
      
      _outtree->Fill();
      
    }
    


    return ctor_v;


  }



  double PCATrackShower::stdev( std::vector<int>& data,
				size_t start, size_t end )
  {
    if (start < 0)          { std::cout << "stdev: start < 0"; std::exception();         }
    if (end >= data.size()) { std::cout << "stdev: end > data.size()"; std::exception(); }
    
    double result = 0.0;
    auto    avg   = mean(data,start,end);
    
    for(int i = start; i <= end; ++i)
      result += (data[i] - avg)*(data[i] - avg);
    
    return std::sqrt(result/((double)(end - start)));
  }


  double PCATrackShower::cov ( std::vector<int>& data1,
			       std::vector<int>& data2,
			       size_t start, size_t end )
  {

    if( data1.size() != data2.size() ) { std::cout << "cov : data not the same"; std::exception();}
    if (start < 0)          { std::cout << "cov: start < 0"; std::exception();         }
    if (end >= data1.size()) { std::cout << "cov: end > data1.size()"; std::exception(); }
    if (end >= data2.size()) { std::cout << "cov: end > data1.size()"; std::exception(); }
    
    double result = 0.0;
    auto   mean1  = mean(data1,start,end);
    auto   mean2  = mean(data2,start,end);
    
    for(int i = start; i <= end; ++i)
      result += (data1[i] - mean1)*(data2[i] - mean2);
    
    return result/((double)(end - start));
  }
  
  double PCATrackShower::mean( std::vector<int>& data,
			       size_t start, size_t end )
  {
    double result = 0.0;
    if (start < 0)          { std::cout << "mean: start < 0"; std::exception();         }
    if (end >= data.size()) { std::cout << "mean :end > data.size()"; std::exception(); }
    
    for(int i = start; i <= end; ++i)
      result += data[i];
    
    return result / ((double)( end - start ));
  }  

}
#endif
