#ifndef __CBITE_HH__
#define __CBITE_HH__

#include "CFacet.hh"

struct CBite
{

  CFacet *Facet;
  uint64 Size;
  bool   Last;

  inline CBite(CFacet *facet,uint64 size, bool last) : Facet(facet), Size(size), Last (last) {}

};


#endif
