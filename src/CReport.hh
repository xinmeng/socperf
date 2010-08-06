#ifndef __REPORT_HH__
#define __REPORT_HH__

#include <vector>
#include <iostream>
#include <string>
#include "systemc.h"
using std::vector;
using std::ostream;
using std::string;
using std::cout;
using std::cerr;

class CReport 
{
protected:
  vector<ostream*> _Dst;
  string _LeftDilimt, _RightDilimt;

  inline CReport ( ) {};

public:
  inline CReport ( ostream *dst) : 
    _LeftDilimt ( "" ), _RightDilimt ( "" ) { _Dst.push_back( dst ); };

  inline CReport ( const string & leftdilimit, const string &rightdilimt, ostream *dst) : 
    _LeftDilimt ( leftdilimit ), _RightDilimt ( rightdilimt ) { _Dst.push_back( dst ); };

  inline void AddDst( ostream *dst) {_Dst.push_back( dst );}

  template<class T> inline CReport& operator<< ( const T &msg ) {
    for ( unsigned int i=0; i<_Dst.size(); i++ ) {
      if ( _Dst[i] == &cout || _Dst[i] == &cerr ) {
	*(_Dst[i]) << _LeftDilimt << msg << _RightDilimt;
      }
      else {
	*(_Dst[i]) << msg;
      }
    }
    return *this;
  }

  CReport& operator<< ( ostream& (*pf) ( ostream& ) ) ;


};


class CWarning : public CReport 
{
public:
  inline CWarning ( ) : 
    CReport ( "\033[00;35m", "\033[00m", &cout )  {}

  inline CWarning ( ostream *dst ) : 
    CReport ( "\033[00;35m", "\033[00m", dst )  {}

};


class CError : public CReport
{
public:
  inline CError( ) : 
    CReport ( "\033[00;31m", "\033[00m", &cout ) {}

  inline CError( ostream *dst ) : 
    CReport ( "\033[00;31m", "\033[00m", dst ) {}
  
};
  
class CLog : public CReport 
{
public: 
  inline CLog ( ) :
    CReport ( ) {}

  inline CLog ( ostream *dst ) : 
    CReport ( dst )  {}

};


extern CWarning WARNING;
extern CError   ERROR;
extern CLog     LOG;


#endif
