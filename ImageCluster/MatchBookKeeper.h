/**
 * \file MatchBookKeeper.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class MatchBookKeeper
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef IMAGECLUSTER_MATCHBOOKKEEPER_H
#define IMAGECLUSTER_MATCHBOOKKEEPER_H

#include <iostream>
#include <vector>
#include <map>

namespace larcv {
  /**
     \class MatchBookKeeper
     User defined class MatchBookKeeper ... these comments are used to generate
     doxygen documentation!
  */
  class MatchBookKeeper {
    
  public:
    
    /// Default constructor
    MatchBookKeeper();
    
    /// Default destructor
    virtual ~MatchBookKeeper(){};
  
    /// Reset method
    void Reset();

    /// Method to register matched clusters
    void Match(const std::vector<unsigned int>& matched_indexes,
	       const float& score);

    /// Method to get result
    std::vector<std::vector<unsigned int> > GetResult() const;

    /// Method to pass result
    void PassResult(std::vector<std::vector<unsigned int> >& result) const;

  protected:

    std::multimap<float,std::vector<unsigned int> > _register;

  };
}
#endif
/** @} */ // end of doxygen group 

