//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class larocv::Contour_t+;
#pragma link C++ class larocv::ContourArray_t+;
#pragma link C++ class larocv::ImageClusterBase+;
#pragma link C++ class larocv::ClusterAlgoBase+;
#pragma link C++ class larocv::MatchAlgoBase+;
#pragma link C++ class larocv::ReClusterAlgoBase+;

#pragma link C++ class larocv::ClusterAlgoFactory+;
#pragma link C++ class larocv::MatchAlgoFactory+;
#pragma link C++ class larocv::ReClusterAlgoFactory+;

#pragma link C++ class larocv::ImageClusterManager+;
#pragma link C++ class larocv::ImageClusterViewer+;

#pragma link C++ class larocv::Cluster2D+;

#pragma link C++ class larocv::BaseUtil+;

//ADD_NEW_CLASS ... do not change this line
#endif



