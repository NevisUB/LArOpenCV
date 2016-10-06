/**
 * \file AlgoDataManager.h
 *
 * \ingroup Base
 * 
 * \brief Class def header for a class AlgoDataManager
 *
 * @author kazuhiro
 */

/** \addtogroup Base

    @{*/
#ifndef ALGODATAMANAGER_H
#define ALGODATAMANAGER_H

#include "Core/laropencv_base.h"
#include "AlgoDataBase.h"
#include <TTree.h>
#include <vector>

namespace larocv {

  /**
     \class AlgoDataManager
     @brief Manager for an algorithm specific data representation
  */
  class AlgoDataManager : public laropencv_base {

  public:

    /// Default constructor
    AlgoDataManager();
    
    /// Default destructor
    ~AlgoDataManager(){}

    /// Registration of algorithm specific data
    void Register(AlgoDataBase* data);

    /// Registration of algorithm data storage ttree (optional)
    void Register(TTree* tree);

    /// Algorithm ID getter from name (slow search)
    AlgorithmID_t ID(const std::string& name);

    /// Data access method
    const AlgoDataBase* Data(AlgorithmID_t id);

  private:

    bool   _tree_attached;
    TTree* _tree;
    std::vector<AlgoDataBase*> _data_v;
    std::vector<std::string> _name_v;
  };
}

#endif
/** @} */ // end of doxygen group 

