#ifndef ALGODATABASE_CXX
#define ALGODATABASE_CXX

#include "AlgoDataBase.h"

namespace larocv {

  const std::string& AlgoDataBase::Name() const
  { return _name; }
  
  AlgorithmID_t AlgoDataBase::ID() const
  { return _id;   }

}

#endif
