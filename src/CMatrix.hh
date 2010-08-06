#ifndef __CMATRIX_HH__
#define __CMATRIX_HH__

#include <iostream>
#include <ostream>
#include <vector>
using std::cerr;
using std::ostream;
using std::vector;

template <typename DataType>
class CMatrix {

private:
  vector<vector<DataType> > _Data;
  unsigned long _Size1, _Size2;
  
  inline friend ostream& operator<<(ostream &os, const CMatrix<DataType>&m){
    os << "[" << m._Size1 << "," << m._Size2 << "]" << endl;
    for ( unsigned long i=0; i<m._Size1; ++i) {
      for (unsigned long j=0; j<m._Size2; ++j) 
	os << m._Data[i][j] << ", ";
      os << endl;
    }

    return os;
  }


public: 
  inline CMatrix(unsigned long size1, unsigned long size2, DataType default_value) {
    _Size1 = size1; 
    _Size2 = size2;

    vector<DataType> tmp (size2, default_value);
    _Data.resize(size1, tmp);
  }

  inline vector<DataType> &operator[](unsigned long row) {
    return _Data[row];
  }

  inline const vector<DataType> &operator[](unsigned long row) const {
    return _Data[row];
  }


  inline unsigned long size1() {return _Size1;}
  inline unsigned long size2() {return _Size2;}

  inline CMatrix<DataType> &operator+(const CMatrix<DataType> & m) {
    if ( _Size1 != m._Size1 || _Size2 != m._Size2 ) {
      cerr << "Matirx size mismatch: " << _Size1 << "<->" << m._Size1 << ", " << _Size2 << "<->" << m._Size2 << endl;
      exit(1);
    }
    for ( unsigned long i=0; i<_Size1; ++i) 
      for (unsigned long j=0; j<_Size2; ++j) 
	_Data[i][j] += m._Data[i][j];
      
    return *this;
  }
};



#endif
