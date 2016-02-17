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
#pragma link C++ class larcv::ImageClusterFactory+;
#pragma link C++ class larcv::ImageClusterViewer+;

#pragma link C++ class larcv::SBCluster+;

#pragma link C++ class larcv::TrackShower+;

#pragma link C++ class larcv::ClusterParams+;
#pragma link C++ class larcv::PCABox+;
#pragma link C++ class larcv::PCAPath+;
#pragma link C++ class larcv::PCASegmentation+;

#pragma link C++ class larcv::SatelliteMerge+;

//ADD_NEW_CLASS ... do not change this line
#endif


