//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class larcv::Contour_t+;
#pragma link C++ class larcv::ContourArray_t+;
#pragma link C++ class larcv::ImageClusterBase+;
#pragma link C++ class larcv::ImageClusterManager+;
#pragma link C++ class larcv::ClusterAlgoFactory+;
#pragma link C++ class larcv::ImageClusterViewer+;

#pragma link C++ class larcv::Cluster2D+;
#pragma link C++ class larcv::ClusterParams+;

//#pragma link C++ class larcv::EmptyImageCluster+;
//#pragma link C++ class larcv::ToyImageCluster+;
//#pragma link C++ class larcv::SmoothBinaryCluster+;
#pragma link C++ class larcv::SBCluster+;
//#pragma link C++ class larcv::TCluster+;
// #pragma link C++ class larcv::CDTrackShower+;
// #pragma link C++ class larcv::HoughTrackShower+;
// #pragma link C++ class larcv::HoughPTrackShower+;
#pragma link C++ class larcv::SatelliteMerge+;
#pragma link C++ class larcv::TrackShower+;

#pragma link C++ class larcv::PCABox+;
#pragma link C++ class larcv::PCAPath+;
#pragma link C++ class larcv::PCASegmentation+;

//ADD_NEW_CLASS ... do not change this line
#endif







