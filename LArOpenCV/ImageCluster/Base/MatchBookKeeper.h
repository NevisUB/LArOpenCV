#ifndef IMAGECLUSTER_MATCHBOOKKEEPER_H
#define IMAGECLUSTER_MATCHBOOKKEEPER_H

#include <iostream>
#include <vector>
#include <map>

namespace larocv {

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
    std::vector<std::vector<unsigned int> > GetResultAndScore(std::vector<float>& score_v) const; 

    /// Method to pass result
    void PassResult(std::vector<std::vector<unsigned int> >& result) const;
    void PassResultAndScore(std::vector<std::vector<unsigned int> >& result, std::vector<float>& score_v) const;


  protected:

    std::multimap<float,std::vector<unsigned int> > _register;

  };
}
#endif
/** @} */ // end of doxygen group 

