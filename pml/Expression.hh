#ifndef __EXPRESSION_HH__
#define __EXPRESSION_HH__

#include "Mfunc.hh"
#include "location.hh"

enum tDirection {INPUT, OUTPUT, INOUT, NONPORT};


// ------------------------------
//   Base class
// ------------------------------
class CSymbol;

class CExpression
{
public: 
  inline CExpression() {};

public:
  virtual bool         IsConst()  = 0;
  virtual ulonglong    Width()  = 0;
  virtual ulonglong    Value()  = 0;
  virtual CExpression* ValueExp() =0;
  virtual CExpression* Reduce()  =0;
  virtual void         Print(ostream& os=cout)  =0;
  virtual inline void  GetSymbol(set<CSymbol*> *) {}
  virtual bool         HasParam() =0;
  virtual bool         Update(tDirection direction) =0;
  virtual void         AddRoccure(yy::location loc) =0;
  virtual void         AddLoccure(yy::location loc) =0;

public:
  inline string ItoS(ulonglong num, int width=-1, int base=2)
  {
    assert (base == 2 || base == 10 || base == 16);

    if ( base == 2 ) {
      string str;
      int rmd;

      do {
	rmd = num % 2;
	if ( rmd ) str = "1" + str;
	else str = "0" + str;
      }  while ( num = num / 2 ) ;
  
      if ( width < 0 || str.length() == width )  {
	return str;
      }
      else if ( str.length() > width ) {
	str = str.substr(str.length() - width);
	return str;
      }
      else if ( str.length() < width ) {
	string pad;
	for ( int i = 0; i<width-str.length(); ++i) {
	  pad += "0";
	}
	str = pad + str;
	return str;
      }
    }
    else {
      stringstream sstr;
      sstr << setbase(base)
	   << num;
      return sstr.str();
    }
  }

  inline ulonglong StoI(const string &str, int base=10)
  {
    assert(base == 2 || base == 10 || base == 16);

    string s = str;
    string::iterator iter1 = s.begin(); 

    while ( iter1 != s.end() ) {
      for ( string::iterator iter = s.begin(); 
	    iter != s.end(); iter++) {
	if (*iter == '_' ) {
	  s.erase(iter);
	  iter1 = s.begin();
	  break;
	}
	else {
	  ++iter1;
	}
      }
    }
    return (ulonglong) strtoll(s.c_str(), NULL, base);
  }
};


// ------------------------------
//   CConstant
// ------------------------------
class CConstant : public CExpression
{
protected:
  ulonglong _width;
  ulonglong _value;

public:
  inline CConstant() {}
  inline CConstant(ulonglong width, ulonglong value) : _width (width), _value (value) {};
  
public:
  inline bool IsConst() {return true;}

  virtual inline ulonglong Width() {return _width;}
  virtual inline ulonglong Value() {return _value;}
  virtual inline CConstant* ValueExp() {return this;}


  virtual void Print(ostream& os=cout) =0;
  virtual CExpression* Reduce()  = 0;

  inline virtual bool HasParam() {return false;}

  inline virtual bool Update(tDirection direction) {return true;}
  inline virtual void AddLoccure(yy::location loc) {}
  inline virtual void AddRoccure(yy::location loc) {}

public:
  inline string BinStr(int width=-1) {
    return ItoS(_value, width, 2);
  }

  inline ulonglong CalcWidth(ulonglong val) {
    double logval =  log(val)/log(2);
    ulonglong i_logval = (ulonglong) logval;

    if ( val == 0 ) {
      return 1;
    }
    else {
      return i_logval + 1;
    }
  }
};

class CNumber : public CConstant
{
public: 
  inline CNumber(const string &str) {_value = StoI(str); _width = 32; }
  inline CNumber(const string &str, int base) {_value = StoI(str, base); _width=32; }
  inline CNumber(ulonglong value): CConstant(32, value) {}
  inline CNumber(ulonglong width, ulonglong value) : CConstant(width, value) {};
    

public:
  inline void Print(ostream&os=cout) {os << _value;}
  inline CNumber* Reduce()  {return this;}

};


class CBasedNum : public CConstant
{
private: 
  string _str;

public:
  inline CBasedNum(const string &str, int base) : _str (str) {
    size_t q_pos = str.find_first_of("'");
    _width = StoI(str.substr(0, q_pos), 10);

    size_t xz_pos = str.find_first_of("xXzZ");
    ulonglong _value_num;
    if ( xz_pos == string::npos ) {
      _value = StoI(str.substr(q_pos+2), base);
      string bin_str = BinStr();
      if (bin_str.length() > _width) {
	ostringstream sstr;
	sstr << _str << " needs " << bin_str.length() << " bits, " << _width << " is not enough.";
	throw sstr.str();
      }
    }
    else {
      _value = 0;
    }
  }
  
  inline void Print(ostream&os=cout) {os << _str;}
  inline CBasedNum* Reduce() {return this;}
};



// ------------------------------
//   const number
// ------------------------------
extern CNumber* CONST_NUM_0;



// ------------------------------
//   CSymbol
// ------------------------------
enum tType {REG, WIRE, LOGIC, INT, INTEGER};
class CCodeBlock;
class CSymbol
{
public:
  string name;
  CExpression* msb;
  tDirection direction;
  tType type;
  bool io_fixed;
  bool width_fixed;
  bool is_2D;
  CExpression* length_msb;
  bool is_const;
  CExpression* value;
  vector<CCodeBlock*> driver;
  vector<yy::location> loccur, roccur;
  CSymbol *reference;
  
  
public:
  inline CSymbol(const string &name_) : 
    name (name_), msb (CONST_NUM_0), direction (NONPORT), 
    type (LOGIC), io_fixed (false), width_fixed (false),
    is_2D (false), length_msb (NULL),
    is_const (false), value (NULL),  reference (NULL) {}
  
  inline CSymbol(const string &name_, CExpression* msb_) :
    name (name_), msb (msb_), direction (NONPORT), type (LOGIC),
    io_fixed (false), width_fixed (false), 
    is_2D (false), length_msb (NULL),
    is_const (false), value (NULL),  reference (NULL)  {}    
    
  inline CSymbol(const string &name_, CExpression* msb_, tType type_) :
    name (name_), msb (msb_), direction (NONPORT), type (type_),
    io_fixed (false), width_fixed (false), 
    is_2D (false), length_msb (NULL),
    is_const (false), value (NULL),  reference (NULL)  {}    


  inline CSymbol(const string &name_, CExpression* msb_, CExpression* length_msb_, tType type_) :
    name (name_), msb (msb_), direction (NONPORT), type (type_),
    io_fixed (false), width_fixed (false), 
    is_2D (true), length_msb (length_msb_),
    is_const (false), value (NULL),  reference (NULL)  {}    


  inline CSymbol(const string &name_, CExpression* msb_, tDirection direction_) :
    name (name_), msb (msb_), direction (direction_), type (LOGIC),
    io_fixed (false), width_fixed (false), 
    is_2D (false), length_msb (NULL),
    is_const (false), value (NULL),  reference (NULL)  {}    

  inline CSymbol(const string &name_, CExpression* msb_, tDirection direction_, CSymbol* reference_) :
    name (name_), msb (msb_), direction (direction_), type (LOGIC),
    io_fixed (false), width_fixed (false), 
    is_2D (false), length_msb (NULL),
    is_const (false), value (NULL),  reference (reference_)  {}    



  inline void PrintPort(ostream &os=cout) {
    if ( direction == NONPORT ) {
      return ;
    }
    else {
      switch (direction) {
      case INPUT:   os << "input "; break;
      case OUTPUT:  os << "output "; break;
      case INOUT:   os << "inout ";  break;
      }
    
      PrintWidth(os, msb);
      os << name << ";" << endl;
    }
  }

  inline void PrintDeclare(ostream &os=cout) {
    if ( is_const ) os << "const "; 
    switch (type) {
    case REG:     os << "reg "; PrintWidth(os, msb); break;
    case WIRE:    os << "wire "; PrintWidth(os, msb); break;
    case LOGIC:   os << "logic "; PrintWidth(os, msb); break;
    case INT:     os << "int "; break;
    case INTEGER: os << "integer "; break; 
    }
    
    os << name;

    if (is_2D ) PrintWidth(os, length_msb);

    if (is_const ) {
      os << " = ";
      value->Print(os);
    }
    os << ";" << endl;
  }


  inline bool Update(tDirection direction_) {
    if ( io_fixed || is_const ) {
      return false;
    }
    else {
      if ( direction == INOUT ) {
      }
      else if ( direction != NONPORT && 
		direction != direction_ ) {
	direction = NONPORT;
	io_fixed = true;
      }
      else {
	direction = direction_;
      }
      return true;
    }
  }

  inline bool Update(CExpression* msb_) {
    if (width_fixed || is_const || is_2D ) {
       if ( msb_->Value() <= msb->Value() ) {
	  return true;
       }
       else {
	  return false;
       }
    }
    else {
      if ( (msb_->Value() == msb->Value() && msb_->HasParam()) || 
	   (msb_->Value() > msb->Value()) ) {
	msb = msb_;
      }
      return true;
    }
  }

  inline void PrintOccurence(ostream&os=cout) {
     os << "R-occur of " << name << endl;
     for (vector<yy::location>::iterator iter = roccur.begin();
	  iter != roccur.end(); ++iter) {
	os << *iter << endl;
     }

     os << "L-occur of " << name <<endl;
     for (vector<yy::location>::iterator iter = loccur.begin();
	  iter != loccur.end(); ++iter) {
	os << *iter << endl;
     }
  }


private:
  inline void PrintWidth(ostream &os, CExpression* msb_) {
    if (msb_->HasParam()) {
      os << "[";
      msb_->Print(os);
      os << ":0] ";
    }
    else if ( msb_->Value() != 0 ) {
      os << "[";
      msb_->Reduce()->Print(os);
      os << ":0] ";
    }
    else if ( msb_ != CONST_NUM_0 ) {
       os << "[0:0] ";
    }       
  }
    
};


// ------------------------------
//   CNet
// ------------------------------
class CNet : public CExpression
{
public:
  inline CNet() {}

  virtual inline bool IsConst() = 0;
  virtual inline ulonglong Width()  = 0;
  virtual inline ulonglong Value()  = 0;
  virtual inline CExpression* ValueExp() = 0;
  virtual inline CExpression* Reduce() =0;
  virtual inline void Print(ostream&os=cout) = 0;

  virtual inline bool Update(tDirection direction) {};
  virtual inline bool Update(CExpression *msb) {};
  virtual inline void GetSymbol(set<CSymbol*> *) {};
  
  virtual inline void AddLoccure(yy::location loc) {};
  virtual inline void AddRoccure(yy::location loc) {};

  virtual bool HasParam() =0;

};

class CVariable : public CNet 
{
private:
  bool _is_2D;
  CSymbol *_symbol;
  CExpression *_addr;
  CExpression *_msb, *_lsb;

public:
  inline CVariable( CSymbol *symbol) : 
    _is_2D (false), _addr (NULL), _symbol (symbol), _msb (NULL), _lsb (NULL) {};

  inline CVariable( CSymbol *symbol, CExpression *msb) : 
    _is_2D (false), _addr (NULL), _symbol (symbol), _msb (msb), _lsb (NULL) {};

  inline CVariable( CSymbol *symbol, CExpression *msb, CExpression *lsb) : 
    _is_2D (false), _addr (NULL), _symbol (symbol), _msb (msb), _lsb (lsb) {};

  inline CVariable( bool is_2D, CSymbol *symb, CExpression *addr, CExpression *msb) : 
    _is_2D (is_2D), _addr (addr), _symbol (symb), _msb (msb), _lsb (NULL)  {}

  inline CVariable( bool is_2D, CSymbol *symb, CExpression *addr, CExpression *msb, CExpression *lsb) : 
    _is_2D (is_2D), _addr (addr), _symbol (symb), _msb (msb), _lsb (lsb)  {}

  inline bool IsConst() {return _symbol->is_const;}
  inline ulonglong Width() {
     if ( _is_2D ) {
	if ( _msb && _lsb ) {
	   return _msb->Value() - _lsb->Value() + 1;
	}
	else {
	   return 1;
	}
     }
     else {
	if ( _symbol->is_2D || !_msb ) {
	   return _symbol->msb->Value() + 1;
	}
	else {
	   if ( _msb && _lsb ) 
	      return _msb->Value() - _lsb->Value() + 1;
	   else
	      return 1;
	}
     }
  }

  inline ulonglong Value() {
    if ( IsConst() ) {
      return _symbol->value->Value();
    }
    else {
      cerr << "**MHDL Internal Error:Unable to get value from a variable:";
      Print(cerr);
      exit(1);
    }
  }
  
  inline CExpression* ValueExp() {
    if ( _symbol->is_const ) {
      return _symbol->value->ValueExp();
    }
    else {
      return this;
    }
  }


  inline CExpression* Reduce() {
    if ( IsConst() ) 
      return _symbol->value->Reduce();
    else 
      return NULL;
  }

  inline void Print(ostream& os=cout) {
    os << _symbol->name;

    if ( _is_2D ) {
       os << "[";
       _addr->Print(os);
       os << "]";
       os << "[";
       _msb->Print(os);
       if ( _lsb ) {
	  os << ":";
	  _lsb->Print(os);
       }
       os << "]";
    }
    else {
       if (_msb) {
	  os << "[";
	  _msb->Print(os);

	  if ( _lsb ) {
	     os << ":"; 
	     _lsb->Print(os);
	     os << "]";
	  }
	  else {
	     os << "]";
	  }
       }
    }
  }

  inline bool HasParam() {return false;}

  inline bool Update(tDirection direction) { return _symbol->Update(direction);}
  inline bool Update(CExpression *msb) {return _symbol->Update(msb);}

  inline CSymbol* Symb() {return _symbol;}
  inline CExpression* Msb() {return _msb;}
  inline CExpression* Lsb() {return _lsb;}
  inline void GetSymbol(set<CSymbol*> *st) {
     st->insert(_symbol);
     if (_msb) _msb->GetSymbol( st );
     if (_lsb )_lsb->GetSymbol( st );
  }

  inline void AddLoccure(yy::location loc) {_symbol->loccur.push_back(loc);}
  inline void AddRoccure(yy::location loc) {_symbol->roccur.push_back(loc);}

};


class CParameter : public CNet
{
private:
  string _name;
  CExpression* _value;
  CExpression* _override;

public: 
  inline CParameter(const string &name, CExpression* value) : 
    _name (name), _value (value), _override (NULL) {}

public: 
  inline bool IsConst() {return true;}

  inline ulonglong Width() {
    if ( _override ) 
      return _override->Width();
    else 
      return _value->Width();
  }

  inline ulonglong Value() {
    if ( _override ) 
      return _override->Value();
    else 
      return _value->Value();
  }

  inline CExpression* ValueExp() {
    if ( _override ) {
      return _override;
    }
    else {
      return _value->ValueExp();
    }
  }

  inline CExpression* Reduce()  {
    if ( _override ) {
      return new CNumber (_override->Width(), _override->Value());
    }
    else {
      return new CNumber (_value->Width(), _value->Value());
    }
  }

  inline void Print(ostream&os=cout) {os << _name;}
  inline bool Update(tDirection direction) {return true;}
  inline bool Update(CExpression*msb) {return true;}

  inline string Name() {return _name;}

  inline void SetValue(CExpression* override) {_override = override;}
  
  inline bool HasParam() {return true;}

};


// ------------------------------
//   CTrinaryExp
// ------------------------------
class CTrinaryExp : public CExpression
{
private:
  CExpression* _cond;
  CExpression* _t_opt;
  CExpression* _f_opt;
  
public:
  inline CTrinaryExp(CExpression* cond, CExpression* t_opt, CExpression* f_opt) : 
    _cond (cond), _t_opt (t_opt), _f_opt (f_opt) {};


  inline bool IsConst() {
    return _cond->IsConst() && _t_opt->IsConst() && _f_opt->IsConst();
  }

  inline ulonglong Width() {
    return Max(_t_opt->Width(), _f_opt->Width());
  }

  inline ulonglong Value() {
    return _cond->Value() ? _t_opt->Value() : _f_opt->Value();
  }

  inline CTrinaryExp* ValueExp() {
    return new CTrinaryExp ( _cond->ValueExp(), _t_opt->ValueExp(), _f_opt->ValueExp());
  }

  inline  CNumber* Reduce() {
    if ( IsConst() ) {
      ulonglong val;
      if ( _cond->Value() ) 
	val = _t_opt->Value();
      else
	val = _f_opt->Value();
      return new CNumber (Width(), val);
    }
    else {
      return NULL;
    }
  }

  inline void Print(ostream& os=cout) {
    _cond->Print(os);  os << " ? ";
    _t_opt->Print(os); os << " : ";
    _f_opt->Print(os); 
  }

  inline void GetSymbol(set<CSymbol*> *st) {
    _cond->GetSymbol(st);
    _t_opt->GetSymbol(st);
    _f_opt->GetSymbol(st);
  }

  inline bool HasParam() {
    return
      _cond->HasParam() || 
      _t_opt->HasParam() || 
      _f_opt->HasParam();
  }

  inline virtual bool Update(tDirection direction) {
    bool flag = true;
    flag = _cond->Update(direction) && flag;
    flag = _t_opt->Update(direction) && flag;
    flag = _f_opt->Update(direction) && flag;

    return flag;
  }

  inline virtual void AddRoccure(yy::location loc) {
    _cond->AddRoccure(loc);
    _t_opt->AddRoccure(loc);
    _f_opt->AddRoccure(loc);
  }

  inline virtual void AddLoccure(yy::location loc) {
    _cond->AddLoccure(loc);
    _t_opt->AddLoccure(loc);
    _f_opt->AddLoccure(loc);
  }

};


// ------------------------------
//    CFuncCallExp
// ------------------------------
class CFuncCallExp : public CExpression
{
   private:
      string _func_name;
      vector<CExpression*> *_args;

   public:
      inline CFuncCallExp (const string &func_name, vector<CExpression*> *args) :
	 _func_name (func_name), _args (args) {}

   public:
      inline bool         IsConst() {return false; }
      inline ulonglong    Width() {return 1;}

      inline ulonglong    Value() {
	 cerr << "**Internal Error:"<< __FILE__ << ":" << __LINE__ << ":Try to call Value() from CFuncCallExp!"; 
	 exit(1);
      }

      inline CFuncCallExp* ValueExp() {return new CFuncCallExp(_func_name, _args);}
      inline CExpression*  Reduce() {
	 cerr << "**Internal Error:"<< __FILE__ << ":" << __LINE__ << ":Try to call Reduce() from CFuncCallExp!"; 
	 exit(1);
      }

      inline void Print(ostream& os=cout) {
	 os << _func_name << "(";
	 for ( vector<CExpression*>::iterator iter = _args->begin(); 
	       iter != _args->end(); ++iter) {
	    (*iter)->Print(os);
	    if ( iter != _args->end() -1 ) os << ", ";
	 }
	 os<<")";
      }

      inline void GetSymbol(set<CSymbol*> *st) {
	 for ( vector<CExpression*>::iterator iter = _args->begin(); 
	       iter != _args->end(); ++iter) {
	    (*iter)->GetSymbol(st);
	 }
      }

      inline bool HasParam() {
	 for ( vector<CExpression*>::iterator iter = _args->begin(); 
	       iter != _args->end(); ++iter) {
	    if ( (*iter)->HasParam() ) return true;
	 }
	 return false;
      }

  inline virtual bool Update(tDirection direction) {
    bool flag = true;
    for (vector<CExpression*>::iterator iter = _args->begin();
	 iter != _args->end(); ++iter) {
      flag = (*iter)->Update(direction) && flag;
    }
    return flag;
  }

  inline virtual void AddRoccure(yy::location loc) {
    for (vector<CExpression*>::iterator iter = _args->begin();
	 iter != _args->end(); ++iter) {
      (*iter)->AddRoccure(loc);
    }
  }

  inline virtual void AddLoccure(yy::location loc) {
    cerr << "**Internal Error:" << __FILE__ << ":" << __LINE__
	 << ":try to set arguments of function call to LHS variable." << endl;
    exit(1);
  }
};

// ------------------------------
//   CParenthExp
// ------------------------------
class CParenthExp : public CExpression
{
private:
  CExpression* _exp;
  
public:
  inline CParenthExp(CExpression* exp) : _exp (exp) {};
  
public:
  inline bool         IsConst() {return _exp->IsConst(); }
  inline ulonglong    Width() {return _exp->Width();}
  inline ulonglong    Value() {return _exp->Value();}
  inline CParenthExp* ValueExp() {return new CParenthExp (_exp->ValueExp());}
  inline CExpression* Reduce() {return IsConst() ? _exp->Reduce() : NULL;}
  inline void         Print(ostream& os=cout) {os << "(";_exp->Print(os);os<<")";}
  inline void         GetSymbol(set<CSymbol*> *st) {_exp->GetSymbol(st);}
  inline bool         HasParam() {return _exp->HasParam();}

  inline virtual bool Update(tDirection direction) {return _exp->Update(direction);}
  inline virtual void AddRoccure(yy::location loc) {_exp->AddRoccure(loc);}
  inline virtual void AddLoccure(yy::location loc) {_exp->AddLoccure(loc);}      
};


// ------------------------------
//   CConcatenation
// ------------------------------
class CConcatenation : public CExpression
{
private:
  vector<CExpression*> *_exp_list;
  
public: 
  inline CConcatenation(vector<CExpression*>* exp_list) : 
    _exp_list (exp_list) {};
  
public:
  virtual inline bool IsConst() {
    for ( vector<CExpression*>::iterator iter = _exp_list->begin(); 
	  iter != _exp_list->end(); ++iter) {
      if ( (*iter)->IsConst() ) continue;
      else return false;
    }
    return true;
  }

  virtual inline ulonglong Width() {
    ulonglong width = 0;
    for ( vector<CExpression*>::iterator iter = _exp_list->begin(); 
	  iter != _exp_list->end(); ++iter) {
      width += (*iter)->Width();
    }
    return width;
  }

  virtual inline ulonglong Value() {
    string str;
    for (vector<CExpression*>::iterator iter = _exp_list->begin(); 
	 iter != _exp_list->end(); ++iter) {
      str = str + ItoS((*iter)->Value(), (*iter)->Width());
    }
    return StoI(str, 2);
  }
    
  virtual inline CConcatenation* ValueExp() {
    vector<CExpression*> *val_exp_list = new vector<CExpression*>;
    for ( vector<CExpression*>::iterator iter = _exp_list->begin(); 
	  iter != _exp_list->end(); ++iter) {
      val_exp_list->push_back( (*iter)->ValueExp() );
    }
    return new CConcatenation (val_exp_list);
  }

  virtual inline CNumber* Reduce() {
    if ( IsConst() ) {
      return new CNumber (Width(), Value());
    } 
    else {
      return NULL;
    }
  }

  virtual inline void Print(ostream& os=cout) {
    os << "{";
    for (vector<CExpression*>::iterator iter = _exp_list->begin(); 
	 iter != _exp_list->end(); ++iter) {
      (*iter)->Print(os);
      if ( iter != _exp_list->end()-1 ) os << ", ";
    }
    os << "}";
  }
  
  inline void GetSymbol(set<CSymbol*> *st) {
    for (vector<CExpression*>::iterator iter = _exp_list->begin(); 
	 iter != _exp_list->end(); ++iter) {
      (*iter)->GetSymbol(st);
    }
  }

  inline bool HasParam() {
    bool flag = false;
    for (vector<CExpression*>::iterator iter = _exp_list->begin(); 
	 iter != _exp_list->end(); ++iter) {
      flag = flag || (*iter)->HasParam();

      if (flag) return flag;
    }
    return flag;
  }

  inline virtual bool Update(tDirection direction) {
    bool flag = true;
    for (vector<CExpression*>::iterator iter = _exp_list->begin();
	 iter != _exp_list->end(); ++iter) {
      flag = (*iter)->Update(direction) && flag;
    }
    return flag;
  }

  inline virtual void AddRoccure(yy::location loc) {
    for (vector<CExpression*>::iterator iter = _exp_list->begin();
	 iter != _exp_list->end(); ++iter) {
      (*iter)->AddRoccure(loc);
    }
  }
    
  inline virtual void AddLoccure(yy::location loc) {
    for (vector<CExpression*>::iterator iter = _exp_list->begin();
	 iter != _exp_list->end(); ++iter) {
      (*iter)->AddLoccure(loc);
    }
  }

  inline vector<CExpression*> *List() {return _exp_list;}
};

class CDupConcat : public CExpression
{
private: 
  CExpression* _times;
  CConcatenation* _exp_concat;

public: 
  inline CDupConcat(CExpression* times, CConcatenation* exp_concat) :
    _times (times), _exp_concat (exp_concat) {};
  
public:
  inline bool IsConst() {
    return _times->IsConst() && _exp_concat->IsConst();
  }

  inline ulonglong Width() {
    if ( _times->IsConst() ) {
      return _exp_concat->Width() * _times->Value();
    }
    else {
      return _exp_concat->Width() * (Power(2, _times->Width())-1);
    }
  }

  inline ulonglong Value() {
    if ( _times->IsConst() ) {
      CNumber *num = new CNumber(_exp_concat->Width(), _exp_concat->Value());
      string str = num->BinStr(num->Width());
      for ( ulonglong i=1; i<_times->Value(); ++i) {
	str = str + str;
      }
      CNumber *val = new CNumber(str, 2);
      return val->Value();

      delete num;
      delete val;
    }
    else {
      cerr << "**Error:" << __FILE__ << ":" << __LINE__ << ":" 
	   << "Try to get value from non-constant duplicated concatenation.";
      Print(cerr);
      exit(1);
    }
  }

  inline CDupConcat* ValueExp() {
    return new CDupConcat ( _times->ValueExp(), _exp_concat->ValueExp() );
  }

  inline CNumber* Reduce() {
    if ( IsConst() ) {
      return new CNumber (Width(), Value());
    }
    else {
      return NULL;
    }
  }

  inline void Print(ostream& os=cout) {
    os << "{";_times->Print(os);
    _exp_concat->Print(os);
    os << "}";
  }

  inline void GetSymbol(set<CSymbol*> *st) {
    _times->GetSymbol(st);
    _exp_concat->GetSymbol(st);
  }
  
  inline bool HasParam() {
    return
      _times->HasParam() || 
      _exp_concat->HasParam();
  }

  inline virtual bool Update(tDirection direction) {
    bool flag = true;
    flag = _times->Update(direction) && flag;
    flag = _exp_concat->Update(direction) && flag;

    return flag;
  }

  inline virtual void AddRoccure(yy::location loc) {
    _times->AddRoccure(loc);
    _exp_concat->AddRoccure(loc);
  }

  inline virtual void AddLoccure(yy::location loc) {
    cerr << "**Internal Error:" << __FILE__ << ":" << __LINE__ 
	 << ":try to set DupConcat expression to LHS." << endl;
    exit(1);
  }

};
  
class CUnaryExp : public CExpression
{
protected: 
  string _operator;
  CExpression* _exp;
  
public:
  inline CUnaryExp(const string &opt, CExpression *exp) : _operator (opt), _exp (exp) {}
  virtual inline bool IsConst() { return _exp->IsConst();}

  virtual inline void Print(ostream&os=cout) {
    os << _operator;
    _exp->Print(os);
  }

  virtual inline ulonglong Width() {};
  virtual inline ulonglong Value() {};

  virtual inline CExpression* ValueExp() =0;

  virtual inline CNumber* Reduce() {
    if ( this->IsConst() ) {
      return new CNumber(Width(), Value());
    }
    else {
      return NULL;
    }
  }

  virtual inline void GetSymbol(set<CSymbol*> *st) {
    _exp->GetSymbol(st);
  }

  virtual inline bool HasParam() {
    return _exp->HasParam();
  }

  virtual inline bool Update(tDirection direction) {
    return _exp->Update(direction);
  }

  virtual inline void AddRoccure(yy::location loc) {
    _exp->AddRoccure(loc);
  }

  virtual inline void AddLoccure(yy::location loc) {
    cerr << "**Internal Error:" << __FILE__ << ":" << __LINE__
	 << ":try to set unary expression to LHS." << endl;
  }

};

class CCondExpNOT : public CUnaryExp
{
public:
  inline CCondExpNOT(CExpression* exp) : CUnaryExp("!", exp) {}

  inline ulonglong Width() {return 1;}
  inline ulonglong Value() {return !_exp->Value();}
  inline CCondExpNOT* ValueExp() {return new CCondExpNOT (_exp->ValueExp());}
};

class CUnaryExpNOT : public CUnaryExp
{
public:
  inline CUnaryExpNOT(CExpression* exp) : CUnaryExp("~", exp) {}

  inline ulonglong Width() {return _exp->Width();}
  inline ulonglong Value() {
    ulonglong val = _exp->Value();
    string str = ItoS(val, _exp->Width());
    for (int i =0; i<str.length(); ++i){
      if ( str[i] == '1' ) 
	str[i] = '0';
      else 
	str[i] = '1';
    }
    return StoI(str, 2);
  }

  inline CUnaryExpNOT* ValueExp() {return new CUnaryExpNOT (_exp->ValueExp());}

};
  

class CUnaryExpAND : public CUnaryExp
{
public : 
  inline CUnaryExpAND(CExpression* exp) : CUnaryExp("&", exp) {}
  
  inline ulonglong Width() {return 1;}
  inline ulonglong Value() {
    string str = ItoS(_exp->Value(), _exp->Width());
    size_t pos = str.find_first_of("0");
    if ( pos == string::npos ) 
      return 1;
    else 
      return 0;
  }
  inline CUnaryExpAND* ValueExp() {return new CUnaryExpAND (_exp->ValueExp());}
};

class CUnaryExpOR : public CUnaryExp
{
public : 
  inline CUnaryExpOR(CExpression* exp) : CUnaryExp("&", exp) {}
  
  inline ulonglong Width() {return 1;}
  inline ulonglong Value() {
    string str = ItoS(_exp->Value(), _exp->Width());
    size_t pos = str.find_first_of("1");
    if ( pos == string::npos ) 
      return 0;
    else 
      return 1;
  }
  inline CUnaryExpOR* ValueExp() {return new CUnaryExpOR (_exp->ValueExp());}
};

class CUnaryExpXOR : public CUnaryExp
{
public : 
  inline CUnaryExpXOR(CExpression* exp) : CUnaryExp("&", exp) {}
  
  inline ulonglong Width() {return 1;}
  inline ulonglong Value() {
    string str = ItoS(_exp->Value(), _exp->Width());
    ulonglong result = 0;
    for (int i=0; i<str.length(); ++i) {
      if (str[i] == '1') 
	result = !result;
    }
    return result;
  }

  inline CUnaryExpXOR* ValueExp() {return new CUnaryExpXOR (_exp->ValueExp());}
};


// ------------------------------
//   CBinaryExp
// ------------------------------
class CBinaryExp : public CExpression
{
protected:
  string _operator;
  CExpression* _exp_a;
  CExpression* _exp_b;

public:
  inline CBinaryExp(const string &opt, CExpression* exp_a, CExpression* exp_b) :
    _operator (opt), _exp_a (exp_a), _exp_b (exp_b) {}


  virtual inline bool IsConst() {return _exp_a->IsConst() && _exp_b->IsConst();}
  virtual inline ulonglong Width() {return Max(_exp_a->Width(), _exp_b->Width());}
  virtual inline void Print(ostream&os=cout) {
    _exp_a->Print(os);
    os << " " << _operator << " ";
    _exp_b->Print(os);
  }
  virtual inline CNumber* Reduce() {
    if ( IsConst() ) {
      return new CNumber (Width(), Value());
    }
    else {
      return NULL;
    }
  }

  virtual ulonglong Value() =0;
  virtual CExpression* ValueExp() =0;

  inline ulonglong CalcMsk(ulonglong width) {
    ulonglong msk = 0;
    for (ulonglong i = 0; i<width; ++i) {
      msk = msk << 1;
      msk += 1;
    }
    return msk;
  }

  virtual inline void GetSymbol(set<CSymbol*> *st) {
    _exp_a->GetSymbol(st);
    _exp_b->GetSymbol(st);
  }

  virtual inline bool HasParam() {
    return 
      _exp_a->HasParam() || 
      _exp_b->HasParam();
  }
  
  virtual inline bool Update(tDirection direction) {
    bool flag = true;
    
    flag = _exp_a->Update(direction) && flag;
    flag = _exp_b->Update(direction) && flag;

    return flag;
  }

  virtual inline void AddRoccure(yy::location loc) {
    _exp_a->AddRoccure(loc);
    _exp_b->AddRoccure(loc);
  }

  virtual inline void AddLoccure(yy::location loc) {
    cerr << "**Internal Error:" << __FILE__ << ":" << __LINE__
	 << ":try to set binary expression to LHS." << endl;
  }
};


class CBinExpAND : public CBinaryExp
{
public :
  inline CBinExpAND(CExpression* exp_a, CExpression* exp_b) : 
    CBinaryExp("&", exp_a, exp_b) {}
  
  inline ulonglong Value() {
    ulonglong val_a = _exp_a->Value();
    ulonglong val_b = _exp_b->Value();
    
    return val_a & val_b;
  }

  inline CBinExpAND* ValueExp() { return new CBinExpAND (_exp_a->ValueExp(), _exp_b->ValueExp());}

};

class CBinExpOR : public CBinaryExp
{
public:
  inline CBinExpOR(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryExp("|", exp_a, exp_b) {}

  inline ulonglong Value() {
    ulonglong val_a = _exp_a->Value();
    ulonglong val_b = _exp_b->Value();
    
    return val_a | val_b;
  }
  inline CBinExpOR* ValueExp() { return new CBinExpOR (_exp_a->ValueExp(), _exp_b->ValueExp());}
};

class CBinExpXOR : public CBinaryExp
{
public:
  inline CBinExpXOR(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryExp("^", exp_a, exp_b) {}

  inline ulonglong Value() {
    ulonglong val_a = _exp_a->Value();
    ulonglong val_b = _exp_b->Value();
    
    return val_a ^ val_b;
  }
  inline CBinExpXOR* ValueExp() { return new CBinExpXOR (_exp_a->ValueExp(), _exp_b->ValueExp());}
};


class CBinExpADD : public CBinaryExp
{
public:
  inline CBinExpADD(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryExp("+", exp_a, exp_b) {}

  inline ulonglong Value() {
    ulonglong val_a = _exp_a->Value();
    ulonglong val_b = _exp_b->Value();
    
    ulonglong val = val_a + val_b;
    
    return val & CalcMsk(Width());
  }
  inline CBinExpADD* ValueExp() { return new CBinExpADD (_exp_a->ValueExp(), _exp_b->ValueExp());}
};


class CBinExpSUB : public CBinaryExp
{
public:
  inline CBinExpSUB(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryExp("-", exp_a, exp_b) {}

  inline ulonglong Value() {
    ulonglong val_a = _exp_a->Value();
    ulonglong val_b = _exp_b->Value();
    ulonglong val = val_a - val_b;
    
    return val & CalcMsk(Width());
  }
  inline CBinExpSUB* ValueExp() { return new CBinExpSUB (_exp_a->ValueExp(), _exp_b->ValueExp());}
};


class CBinExpMUL : public CBinaryExp
{
public:
  inline CBinExpMUL(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryExp("*", exp_a, exp_b) {}

  inline ulonglong Width() {return _exp_a->Width() + _exp_b->Width();}
  inline ulonglong Value() {
    ulonglong val_a = _exp_a->Value();
    ulonglong val_b = _exp_b->Value();
    ulonglong val = val_a * val_b;
    
    return val & CalcMsk(Width());
  }
  inline CBinExpMUL* ValueExp() { return new CBinExpMUL (_exp_a->ValueExp(), _exp_b->ValueExp());}
};

class CBinExpDIV : public CBinaryExp
{
public:
  inline CBinExpDIV(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryExp("/", exp_a, exp_b) {}

  inline ulonglong Value() {
    ulonglong val_a = _exp_a->Value();
    ulonglong val_b = _exp_b->Value();
    ulonglong val = val_a / val_b;
    
    return val;
  }
  inline CBinExpDIV* ValueExp() { return new CBinExpDIV (_exp_a->ValueExp(), _exp_b->ValueExp());}
};


class CBinExpMOD : public CBinaryExp
{
public:
  inline CBinExpMOD(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryExp("%", exp_a, exp_b) {}

  inline ulonglong Value() {
    ulonglong val_a = _exp_a->Value();
    ulonglong val_b = _exp_b->Value();
    ulonglong val = val_a % val_b;
    
    return val;
  }
  inline CBinExpMOD* ValueExp() { return new CBinExpMOD (_exp_a->ValueExp(), _exp_b->ValueExp());}
};

class CBinExpRSHFT : public CBinaryExp
{
public:
  inline CBinExpRSHFT(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryExp(">>", exp_a, exp_b) {}

  inline ulonglong Value() {
    ulonglong val_a = _exp_a->Value();
    ulonglong val_b = _exp_b->Value();
    ulonglong val = val_a >> val_b;
    
    return val;
  }
  inline CBinExpRSHFT* ValueExp() { return new CBinExpRSHFT (_exp_a->ValueExp(), _exp_b->ValueExp());}
};


class CBinExpLSHFT : public CBinaryExp
{
public:
  inline CBinExpLSHFT(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryExp("<<", exp_a, exp_b) {}

  inline ulonglong Value() {
    ulonglong val_a = _exp_a->Value();
    ulonglong val_b = _exp_b->Value();
    ulonglong val = val_a << val_b;
    
    return val;
  }
  inline CBinExpLSHFT* ValueExp() { return new CBinExpLSHFT (_exp_a->ValueExp(), _exp_b->ValueExp());}
};


// ------------------------------
//   CBinaryCondExp
// ------------------------------
class CBinaryCondExp : public CBinaryExp 
{
public: 
  inline CBinaryCondExp(const string &opt, CExpression *exp_a, CExpression *exp_b) : 
    CBinaryExp(opt, exp_a, exp_b) {}

  inline ulonglong Width() {return 1;}
  inline CNumber* Reduce() {
    if ( IsConst() ) {
      return new CNumber (1, Value());
    }
    else {
      return NULL;
    }
  }

  virtual ulonglong Value() =0;
  virtual CExpression* ValueExp() =0;

};

#define BIN_COND_EXP_DECLARE(name, str, operator)			\
  class name : public CBinaryCondExp					\
  {									\
  public:								\
    inline name(CExpression *exp_a, CExpression *exp_b) :		\
      CBinaryCondExp(str, exp_a, exp_b) {}				\
									\
    inline ulonglong Value() {return _exp_a->Value() operator _exp_b->Value() ? 1 : 0;} \
    inline name* ValueExp() {return new name (_exp_a->ValueExp(), _exp_b->ValueExp());} \
  }
  
BIN_COND_EXP_DECLARE(CCondExpAND, "&&", &&);

// class CCondExpAND : public CBinaryCondExp
// {
// public:
//   inline CCondExpAND(CExpression *exp_a, CExpression *exp_b) : 
//     CBinaryCondExp("&&", exp_a, exp_b) {}

//   inline ulonglong Value() {return _exp_a->Value() && _exp_b->Value() ? 1 : 0;}
//   inline CExpression* ValueExp() {return new CCondExpAND (_exp_a->ValueExp(), _exp_b->ValueExp());}
// };

class CCondExpOR : public CBinaryCondExp
{
public:
  inline CCondExpOR(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryCondExp("||", exp_a, exp_b) {}

  inline ulonglong Value() {return _exp_a->Value() || _exp_b->Value() ? 1 : 0;}
  inline CExpression* ValueExp() {return new CCondExpOR (_exp_a->ValueExp(), _exp_b->ValueExp());}
};


class CCondExpGT : public CBinaryCondExp
{
public:
  inline CCondExpGT(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryCondExp(">", exp_a, exp_b) {}

  inline ulonglong Value() {return _exp_a->Value() > _exp_b->Value() ? 1 : 0;}
  inline CExpression* ValueExp() {return new CCondExpGT (_exp_a->ValueExp(), _exp_b->ValueExp());}
};

class CCondExpLT : public CBinaryCondExp
{
public:
  inline CCondExpLT(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryCondExp("<", exp_a, exp_b) {}

  inline ulonglong Value() {return _exp_a->Value() < _exp_b->Value() ? 1 : 0;}
  inline CExpression* ValueExp() {return new CCondExpLT (_exp_a->ValueExp(), _exp_b->ValueExp());}
};

class CCondExpGE : public CBinaryCondExp
{
public:
  inline CCondExpGE(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryCondExp(">=", exp_a, exp_b) {}

  inline ulonglong Value() {return _exp_a->Value() >= _exp_b->Value() ? 1 : 0;}
  inline CExpression* ValueExp() {return new CCondExpGE (_exp_a->ValueExp(), _exp_b->ValueExp());}
};

class CCondExpLE : public CBinaryCondExp
{
public:
  inline CCondExpLE(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryCondExp("<=", exp_a, exp_b) {}

  inline ulonglong Value() {return _exp_a->Value() <= _exp_b->Value() ? 1 : 0;}
  inline CExpression* ValueExp() {return new CCondExpLE (_exp_a->ValueExp(), _exp_b->ValueExp());}
};

class CCondExpNE : public CBinaryCondExp
{
public:
  inline CCondExpNE(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryCondExp("!=", exp_a, exp_b) {}

  inline ulonglong Value() {return _exp_a->Value() != _exp_b->Value() ? 1 : 0;}
  inline CExpression* ValueExp() {return new CCondExpNE (_exp_a->ValueExp(), _exp_b->ValueExp());}  
};

class CCondExpEQ : public CBinaryCondExp
{
public:
  inline CCondExpEQ(CExpression *exp_a, CExpression *exp_b) : 
    CBinaryCondExp("==", exp_a, exp_b) {}

  inline ulonglong Value() {return _exp_a->Value() == _exp_b->Value() ? 1 : 0;}
  inline CExpression* ValueExp() {return new CCondExpEQ (_exp_a->ValueExp(), _exp_b->ValueExp());}
};


#endif
