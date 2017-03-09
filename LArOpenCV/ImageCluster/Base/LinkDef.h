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

#pragma link C++ class larocv::AlgoFactory+;

#pragma link C++ class larocv::ImageClusterManager+;
#pragma link C++ class larocv::ImageClusterViewer+;

#pragma link C++ class larocv::Cluster2D+;

#pragma link C++ namespace larocv::data+;
#pragma link C++ class larocv::data::AlgoDataBase+;
#pragma link C++ class larocv::data::AlgoDataEmpty+;
#pragma link C++ class larocv::data::AlgoDataManager+;
#pragma link C++ class larocv::data::AlgoDataAssManager+;
#pragma link C++ class larocv::data::AlgoDataElementBase+;
#pragma link C++ class larocv::data::AlgoDataArrayElementBase+;
#pragma link C++ class larocv::data::AlgoDataArrayBase+;


//ADD_NEW_CLASS ... do not change this line
#endif
