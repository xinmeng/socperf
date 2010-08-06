#include "CReport.hh"


CReport& 
CReport::operator<< ( ostream& (*pf) ( ostream& ) ) {
  for ( unsigned int i=0; i<_Dst.size(); i++ ) {
    pf( *(_Dst[i]) );
  }
  return *this;
}

CWarning WARNING;
CError   ERROR;
CLog     LOG;
