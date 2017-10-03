/*******************************************************************************/
/*  Copyright (C) 2014 The LightCAS project                                      */
/*                                                                             */
/*  This program is free software; you can redistribute it and/or modify       */
/*  it under the terms of the GNU General Public License as published by       */
/*  the Free Software Foundation; either version 2 of the License, or          */
/*  (at your option) any later version.                                        */
/*                                                                             */
/*  This program is distributed in the hope that it will be useful,            */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of             */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              */
/*  GNU General Public License for more details.                               */
/*                                                                             */
/*  You should have received a copy of the GNU General Public License          */
/*  along with this program; if not, write to the Free Software                */
/*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */
/*******************************************************************************/

#pragma once

#include "LCString.h"
#include "Parser.h"
#include "Evaluator.h"
#include "LCVector.h"

class CMathExpression;
class CElement;
struct CSymbolSyntaxStruct;

typedef CVector< class CElement* >             CElementArray;
typedef CVector< struct CSymbolSyntaxStruct* > CSymbolSyntaxArray;

struct SProperties
{
  const char*     m_FunctionName;
  unsigned        m_ParameterNb;
  CEvaluatorFunct m_FunctionCall;
};

class CElementDataBase : public CElementArray
{

protected:

  CString                   m_Name;
  CElementDataBase*         m_Parent;
  CEvaluator*               m_Evaluator;
  unsigned                  m_SearchStart;
  bool                      m_Error;
  static unsigned           m_SecureLimit;
  static CElementArray      m_ElementRefArray;
  static CSymbolSyntaxArray m_SymbolSyntaxArray;
  static const SProperties  m_FunctionProperties[];
  static const unsigned     m_FunctionPropertiesSize;

public :

  enum PRIVATE_OPCODE
  {
    OP_EXP1,
    OP_EXP2,
    OP_EXP3,
    OP_EXP4,
    OP_EXP5,
    OP_EXP6,
    OP_EXP7,
    OP_EXP8,
    OP_ZERO,
    OP_SET,
    OP_GET,
    OP_NONE,
    OP_CONST,
    OP_ELEM,
    OP_FUNCT0,
    OP_FUNCT1,
    OP_FUNCT2,
    OP_NEG,
    OP_RANK,
    OP_EVAL,
    OP_END_RESERVED
  };

  enum { MAX_EXP = OP_EXP8 - OP_EXP1 + 1 };

protected:

  unsigned      Register( CElement* e, unsigned index );
  CElement*     CreateElement( const CString& string, unsigned pos );
  virtual void  AddReservedElements();
  virtual void  AddReservedFunctions();
  virtual void  AddAlgebraRuleTable( CParser& IC );
  virtual void  AddEvalFunctionTable( const SProperties* property_table, unsigned size );
  virtual void  AddEvalFunction( const CString& name, unsigned parameter_nb, CEvaluatorFunct funct );
  void          InitAlgebraRuleTable();
  void          CleanTempElements();
  static void   SetSecureLimit( unsigned i ) { m_SecureLimit = i; }

public:

  void            Clear();
  bool            IsOK()                           { return !m_Error; }
  const CString&  GetName() const                  { return m_Name; }
  void            SetName( const CString& name )   { m_Name = name; }
  bool            Initialize();
  CEvaluator*     GetEvaluator() const             { return m_Evaluator; }
  void            SetEvaluator( CEvaluator* eval ) { m_Evaluator = eval; }
  CElement*       ParseElement( CParser& IC );
  CElement*       GetElement();
  CElement*       GetElement( const CString& );
  CElement*       SearchElement( const CString&, unsigned& pos ) const;
  bool            AssociateSymbol( CParser& IC );

  static OP_CODE    ElementToRef( const CElement* e );
  static CElement*  RefToElement( OP_CODE op )         { return m_ElementRefArray[ ( unsigned )op ]; }
  static void       UnRef( OP_CODE op )                { m_ElementRefArray[ ( unsigned )op ] = NULL; }
  static const CSymbolSyntaxArray& GetSymbolTable()    { return m_SymbolSyntaxArray; }
  static unsigned   GetSecureLimit()                   { return m_SecureLimit;       }
  static bool IsReservedOP( OP_CODE op )               { return ( op >= OP_EXP1   ) && ( op < ( OP_EXP1 + MAX_EXP ) ); }
  static bool IsFunctionOP( OP_CODE op )               { return ( op >= OP_CONST ) && ( op <= OP_FUNCT2 ); }
  CElementDataBase* GetParent() const                  { return m_Parent;            }


#ifdef _TEST
  void Printf( const char* format, ... );
  void Test();
  void Check( const char* s1, const char* s2 );
  void CheckEval( const char* s1, const CValue& v1 );
  void CheckSyntaxError( const char* s1 );
  void DisplayStats();
#endif

  CElementDataBase( const CElementDataBase& db ); // avoid simple copy
  CElementDataBase( const CString& name = CString(), CElementDataBase* db = NULL, CEvaluator* eval = NULL, bool bInitialize = true );
  virtual ~CElementDataBase();

};
