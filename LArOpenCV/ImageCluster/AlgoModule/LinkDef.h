//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class std::pair<float,float>+;
#pragma link C++ class std::vector<std::pair<float, float> >+;
#pragma link C++ class std::vector<std::vector<std::pair<float, float> > >+;
#pragma link C++ class std::vector<std::vector<TVector3> >+;

#pragma link C++ class larocv::SuperClusterMaker+;
#pragma link C++ class larocv::TrackVertexSeeds+;
#pragma link C++ class larocv::TrackVertexEstimate+;
#pragma link C++ class larocv::TrackParticleCluster+;
#pragma link C++ class larocv::SuperClusterer+;
#pragma link C++ class larocv::LinearTrackSeeds+;
#pragma link C++ class larocv::ShowerVertexEstimate+;
#pragma link C++ class larocv::ShowerParticleCluster+;
#pragma link C++ class larocv::ShowerOnTrackEnd+;
#pragma link C++ class larocv::ShowerVertexMerger+;
#pragma link C++ class larocv::TrackVertexAnalysis+;
#pragma link C++ class larocv::ShowerVertexAnalysis+;
#pragma link C++ class larocv::CombineVertex+;
#pragma link C++ class larocv::VerteSeedResize+;
#pragma link C++ class larocv::ShowerPlaneSeeds+;
#pragma link C++ class larocv::ShowerVertexScan+;
#pragma link C++ class larocv::ShapeAnalysis+;
#pragma link C++ class larocv::EmptyAlgoModule+;
#pragma link C++ class larocv::TrumpExample+;
#pragma link C++ class larocv::AngleAnalysis+;
#pragma link C++ class larocv::dQdsAnalysis+;
#pragma link C++ class larocv::MatchOverlap+;
#pragma link C++ class larocv::MatchAnalysis+;
#pragma link C++ class larocv::EmptyAnalysis+;
#pragma link C++ class larocv::VertexCheater+;

//ADD_NEW_CLASS ... do not change this line
#endif




