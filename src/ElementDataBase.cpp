/*******************************************************************************/
/*  Copyright (C) 2014 The CASLight project                                      */
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
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "Debug.h"
#include "Element.h"
#include "Function.h"
#include "ElementDataBase.h"

CElementArray      CElementDataBase::m_ElementRefArray;
CSymbolSyntaxArray CElementDataBase::m_SymbolSyntaxArray;

CElementDataBase::CElementDataBase( CElementDataBase* parent, const CString& name, bool bInitialize )
{
  m_Parent = parent;
  m_Name = name;
  m_Evaluator = NULL;
  if(bInitialize) Initialize();
}

CElementDataBase::~CElementDataBase()
{
  Clear();
}

void CElementDataBase::Initialize()
{

  Clear();
  if( m_Parent == NULL )
  {
    CreateEvaluator();
    AddReservedElements();
    AddOperandTable     ( CRules::m_VoidFunctions,   0  );
    AddOperandTable     ( CRules::m_UnaryFunctions,  1  );
    AddOperandTable     ( CRules::m_BinaryFunctions, 2  );
    AddSyntaxSymbolTable( CRules::m_FunctionSymbol      );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleString   );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleDerivals );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleSystems  );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleLogic    );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleVectors  );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleTaylorSeries );
    AddEvalFunctionTable( CRules::m_FunctionProperties, CRules::m_FunctionPropertiesSize );
  }
  else
  {
    m_Evaluator = m_Parent->GetEvaluator();
  }
}

void CElementDataBase::CreateEvaluator()
{
 m_Evaluator = new CEvaluator();
}

void CElementDataBase::AddReservedElements()
{
  GetElement( CValue( 0. ) );
  GetElement()->SetPseudoName(  "a"  );
  GetElement()->SetPseudoName(  "b"  );
  GetElement()->SetPseudoName(  "c"  );
  GetElement()->SetPseudoName(  "d"  );
  GetElement()->SetPseudoName(  "e"  );
  GetElement()->SetPseudoName(  "f"  );
  GetElement()->SetPseudoName(  "g"  );
  GetElement()->SetPseudoName(  "h"  );
  GetElement( "CONCAT" )->SetOperandNb( 2 );
  GetElement( "SET"    )->SetOperandNb( 2 );
  GetElement( "NONE"   );//->SetOperandNb( 0 );
  GetElement( "CONST"  )->SetOperandNb( 1 );
  GetElement( "ELEM"   )->SetOperandNb( 1 );
  GetElement( "NEG"    )->SetOperandNb( 1 );
  GetElement( "j"      );//->SetOperandNb( 0 );
  GetElement( "RANK"   )->SetOperandNb( 2 );
  GetElement( "SUBST"  )->SetOperandNb( 2 );
  //GetElement( "STACK_SIZE_ERROR" );
  // GetElement( "NEW" );

  ASSERT( ElementToRef( m_ElementRefArray[ OP_ZERO   ] ) == OP_ZERO   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_EXP1   ] ) == OP_EXP1   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_EXP2   ] ) == OP_EXP2   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_EXP3   ] ) == OP_EXP3   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_EXP4   ] ) == OP_EXP4   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_EXP5   ] ) == OP_EXP5   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_EXP6   ] ) == OP_EXP6   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_EXP7   ] ) == OP_EXP7   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_EXP8   ] ) == OP_EXP8   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_CONCAT ] ) == OP_CONCAT );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_SET    ] ) == OP_SET    );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_NONE   ] ) == OP_NONE   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_CONST  ] ) == OP_CONST  );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_ELEM   ] ) == OP_ELEM   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_NEG    ] ) == OP_NEG    );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_CPLX   ] ) == OP_CPLX   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_RANK   ] ) == OP_RANK   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_SUBST  ] ) == OP_SUBST  );
  //ASSERT( ElementToRef( m_ElementRefArray[17] ) == OP_ERROR_SIZE   );
  //  ASSERT( ElementToRef( m_ElementRefArray[16] ) == OP_NEW    );
}

void CElementDataBase::AddOperandTable( const char* operand_table, unsigned operand_nb )
{
  CParser IC( operand_table );
  CElement* e;

  while( !IC.IsStopChar() )
  {
    IC.GetWord();
    e = GetElement( IC );
    IC.Find( ';' );
    e->SetOperandNb( operand_nb );

  }
}

void  CElementDataBase::AddSyntaxSymbolTable( const char* symbol_table )
{
  const char* pos1;
  unsigned cnt;
  CSymbolSyntaxStruct* sss;
  CElementDataBase db( this, "tmp2" );
  CEquation equ( &db );
  CEquation dummy_equ( &db );
  CSymbolSyntaxArray tmp_syntax_array;

  CParser IC( symbol_table );

  while( IC.GetChar() )
  {
    sss = new CSymbolSyntaxStruct();
    pos1 = IC.GetPos();
    cnt = strchr( pos1, ' ' ) - pos1;
    ASSERT( cnt < sizeof( sss->m_Syntax ) );
    strncpy( sss->m_Syntax, pos1, cnt );
    sss->m_Syntax[cnt] = '\0';
    IC.SetPos( pos1 + cnt );
    IC.Find( '=' );
    IC.Find( '>' );

    equ.GetFromTextRPN( IC );

    CEquation::ConvertToRule( equ , dummy_equ ); //TODO remove dummy_equ

    sss->m_Equation.Copy( equ );
    tmp_syntax_array.Append( sss );

#ifdef _DEBUG
    CDisplay ds;
    ds += "Adding symbol syntax: ";
    ds += sss->m_Syntax;
    ds += " => ";
    sss->m_Equation.Display( ds );
    TRACE( ds.GetBufferPtr() );
#endif
  }

  m_SymbolSyntaxArray.Copy( tmp_syntax_array );

}

void CElementDataBase::AddAlgebraRuleTable( const char* rules_table )
{
  CElementDataBase db(  this, "tmp1"  );
  CEquation src( &db );
  CParser IC(  rules_table );
  do src.GetFromText( IC );
  while( IC.TryFind( ';' ) );
}

void CElementDataBase::AddEvalFunctionTable( const SProperties* property_table, unsigned size )
{
  for( unsigned i = 0; i < size; i++ )
  {
    AddEvalFunction( CString( property_table[i].m_FunctionName ), property_table[i].m_FunctionCall );
  }
}

void CElementDataBase::AddEvalFunction( CString& name, CEvaluatorFunct funct )
{
  unsigned pos;
  CElement* e;
  e = SearchElement( name, pos );
  e->SetNumeric();
  GetEvaluator()->SetFunction( e->ToRef(), funct );
}

OP_CODE CElementDataBase::ElementToRef( const CElement* e )
{
  return e->ToRef();
}

void CElementDataBase::Clear()
{
  DeleteAll();
  if( m_Parent == NULL )
  {
    m_ElementRefArray.RemoveAll();
    m_SymbolSyntaxArray.DeleteAll();
    if( m_Evaluator )
    {
      delete m_Evaluator;
      m_Evaluator=NULL;
    }
  }
}

CElement* CElementDataBase::GetElement()
{
  unsigned n;
  CElement*	e;
  CString		s;

  n = GetSize();
  s = "_";
  s += CString( ( int )n );
  e = new CElement( s );
  e->SetTemporary();
  Register( e, n );
  return e;
}

CElement* CElementDataBase::GetElement( const CValue& v )
{
  unsigned pos;
  CElement*	e;

  if( m_Parent )
  {
    e = m_Parent->GetElement( v );
  }
  else
  {
    e = SearchElement( v, pos );
    if( e == NULL )
    {
      CDisplay ds;
      v.Display( ds );
      e = CreateElement( ds, pos );
      e->SetConst();
      GetEvaluator()->SetElementValue( e->ToRef(), v );
    }
  }
  return( e );
}

CElement* CElementDataBase::GetElement( const CString& string )
{
  unsigned pos;
  CElement*	e;

  e = SearchElement( string, pos );
  if( e == NULL )
  {
    e = CreateElement( string, pos );
  }
  return e;
}

CElement* CElementDataBase::CreateElement( const CString& string, unsigned pos )
{
  CElement* e;
  e = new CElement( string );
  Register( e, pos );
  GetEvaluator()->SetElementValue( e->ToRef(), CValue( 0. ) );
  GetEvaluator()->SetFunction( e->ToRef(), NULL );
  return e;
}

CElement* CElementDataBase::SearchElement( const CValue& v1, unsigned& pos ) const
{
  unsigned	i, n;
  CElement*	e;

  n = GetSize();

  for( i = 0; i < n; i++ )
  {
    e = GetAt( i );

    if( e->IsConst() )
    {
      const CValue& v2 = GetEvaluator()->GetElementValue( e->ToRef() );

      if(  v1 == v2  )
      {
        return e ;
      }

      if( v1 < v2 )
      {
        break;
      }
    }
    else
    {
      break;
    }
  }
  pos = i;

  return NULL;
}

CElement* CElementDataBase::SearchElement( const CString& string, unsigned& pos ) const
{
  unsigned	i, n;
  int compare;
  CElement*	e;

  if( m_Parent )
  {
    e = m_Parent->SearchElement( string, pos );

    if( e )
    {
      return e;
    }
  }

  n = GetSize();

  for( i = 0; i < n; i++ )
  {

    e = GetAt( i );
    if( !e->IsConst() )
    {
      compare = string.Compare( e->GetName() );

      if( compare == 0 )
      {
        return e;
      }

      if( compare < 0 )
      {
        break;
      }
    }
  }

  pos = i;
  return NULL;
}

unsigned CElementDataBase::Register( CElement* e, unsigned index )
{
  unsigned n;

  InsertAt( index, e );

  n = m_ElementRefArray.GetSize();
  e->SetRef( n );
  e->SetGlobal( m_Parent == NULL );
  m_ElementRefArray.Append( e );

#if _DEBUG
  CDisplay ds;
  ds += "Register #";
  ds += CString( e->ToRef() );
  ds += " : ";
  ds += m_Name;
  ds +=  '/' ;
  e->Display( ds );
  TRACE( ds.GetBufferPtr() );
#endif

  return  n;
}



