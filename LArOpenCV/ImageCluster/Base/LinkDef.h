//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace larocv::data;
#pragma link C++ enum larocv::ParticleType_t+;
#pragma link C++ enum larocv::AlgorithmType_t+;
#pragma link C++ enum larocv::ImageSetID_t+;
#pragma link C++ enum larocv::data::AlgoDataCategory_t+;
#pragma link C++ enum larocv::data::AlgoDataType_t+;

#pragma link C++ class larocv::ImageAnaBase+;
#pragma link C++ class larocv::ImageClusterBase+;
#pragma link C++ class larocv::AlgoFactory+;
#pragma link C++ class larocv::ImageClusterManager+;

#pragma link C++ class std::vector<geo2d::Vector<int> >+;
#pragma link C++ class std::vector<std::vector<geo2d::Vector<int> > >+;

#pragma link C++ namespace larocv::data+;
#pragma link C++ class larocv::data::AlgoDataBase+;
#pragma link C++ class larocv::data::AlgoDataEmpty+;
#pragma link C++ class larocv::data::AlgoDataManager+;
#pragma link C++ class larocv::data::AlgoDataAssManager+;
//#pragma link C++ class larocv::data::AlgoDataElementBase+;
#pragma link C++ class larocv::data::AlgoDataArrayElementBase+;
#pragma link C++ class larocv::data::AlgoDataArrayBase+;


//ADD_NEW_CLASS ... do not change this line
#endif
