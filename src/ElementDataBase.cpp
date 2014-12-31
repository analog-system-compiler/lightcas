/*******************************************************************************/
/*  Copyright (C) 2014 The LightCAS project                                    */
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

CElementDataBase::CElementDataBase( const CString& name,  CElementDataBase* parent, CEvaluator* eval, bool bInitialize )
{
  m_Parent = parent;
  m_Name = name;
  m_Evaluator = eval;
  if( bInitialize )
  {
    Initialize();
  }
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
    AddReservedElements();
    AddEvalFunctionTable( CRules::m_FunctionProperties, CRules::m_FunctionPropertiesSize );
    AddSyntaxSymbolTable( CRules::m_FunctionSymbol      );
    AddOperandTable     ( CRules::m_Functions           );
    AddAlgebraRuleTable ( CRules::m_AlgebraRulePolynom  );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleAcrossFunct);
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleMisc     );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleDerivals );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleSystems  );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleLogic    );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleVectors  );
    AddAlgebraRuleTable ( CRules::m_AlgebraRuleTaylorSeries );
  }
  else
  {
    m_Evaluator = m_Parent->GetEvaluator();
  }
}

void CElementDataBase::AddReservedElements()
{
  const double PI = 3.14159265358979323846264338327950288;
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
  GetElement( "ERROR_SIZE"  );//->SetOperandNb( 0 );
  SetConstValue( GetElement( "PI" ), CValue( PI ) );
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
  ASSERT( ElementToRef( m_ElementRefArray[ OP_ERROR_SIZE  ] ) == OP_ERROR_SIZE  );
}

void CElementDataBase::AddOperandTable( const char* operand_table )
{
  CParser IC( operand_table );
  CElement* e;
  unsigned operand_nb;

  while( !IC.IsStopChar() )
  {
    operand_nb = 0;
    IC.GetWord();
    e = GetElement( IC );
    IC.Find( '(' );
    while( !IC.TryFind( ')' ) )
    {
      IC.GetWord();
      operand_nb++;
      IC.TryFind( ',' );
    }
    e->SetOperandNb( operand_nb );
  }
}


void  CElementDataBase::AddSyntaxSymbolTable( const char* symbol_table )
{
  const char* pos1;
  unsigned cnt;
  CSymbolSyntaxStruct* sss;
  CElementDataBase db( "tmp2", this );
  CMathExpression equ( &db );
  CMathExpression dummy_equ( &db );

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

    CMathExpression::ConvertToRule( equ , dummy_equ ); //TODO remove dummy_equ

    sss->m_Equation.Copy( equ );
    m_SymbolSyntaxArray.Append( sss );

#ifdef _DEBUG
    CDisplay ds;
    ds += "Adding symbol syntax #";
    ds += CString( ( int )( m_SymbolSyntaxArray.GetSize() - 1 ) );
    ds += ": ";
    ds += sss->m_Syntax;
    ds += " => ";
    sss->m_Equation.Display( ds );
    TRACE( ds.GetBufferPtr() );
#endif
  }

}

void CElementDataBase::AddAlgebraRuleTable( const char* rules_table )
{
  CElementDataBase db(  "tmp1", this  );
  CMathExpression src( &db );
  CParser IC(  rules_table );
  do
  {
    src.GetFromString( IC );
  }
  while( IC.TryFind( ';' ) );
}

void CElementDataBase::AddEvalFunctionTable( const SProperties* property_table, unsigned size )
{
  for( unsigned i = 0; i < size; i++ )
  {
    const SProperties& prop = property_table[i];
    AddEvalFunction( CString( prop.m_FunctionName ), prop.m_ParameterNb, prop.m_FunctionCall );
  }
}

void CElementDataBase::AddEvalFunction( const CString& name, unsigned parameter_nb, CEvaluatorFunct funct )
{
  CElement* e;
  e = GetElement( name );
  e->SetOperandNb( parameter_nb );
  if( funct )
  {
    e->SetNumeric();
    GetEvaluator()->SetFunction( e->ToRef(), funct );
  }
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
  }
}

CElement* CElementDataBase::ParseElement( CParser& IC )
{
  CElement* e = NULL;
  CValue v;

  if( IC.GetChar() != '-' )
  {
    const char* pos = v.GetFromString( IC.GetPos() );
    if( pos != IC.GetPos() )
    {
      IC.SetPos( pos );
      e = GetElement( v );
    }
    else if( IC.IsWord() )
    {
      IC.GetWord();
      e = GetElement( IC );
    }
  }
  return e;
}

CElement* CElementDataBase::GetElement()
{
  unsigned n;
  CElement*	e;
  CString	s;

  n = m_ElementRefArray.GetSize();
  s = "_";
  s += CString( ( int )n );
  e = CreateElement( s, GetSize() );
  e->SetTemporary();
  return e;
}

CElement* CElementDataBase::GetElement( const CValue& v )
{
  CElement*	e;
  CDisplay ds;
  v.Display( ds );

  unsigned old_size = m_ElementRefArray.GetSize();
  if( m_Parent )
  {
    e = m_Parent->GetElement( ds );
  }
  else
  {
    e = GetElement( ds );
  }
  if( m_ElementRefArray.GetSize() != old_size )
  {
    e->SetConst();
    SetValue( e, v );
  }

  return e ;
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
  SetValue( e, CValue( 0 ) );
  return e;
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
    //if( !e->IsConst() )
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

#ifdef _DEBUG
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

void CElementDataBase::SetValue( const CElement* e, const CValue& v )
{
  CEvaluator* eval = GetEvaluator();
  ASSERT( eval );
  eval->SetElementValue( e->ToRef(), v );
  eval->SetFunction( e->ToRef(), NULL );
}

void CElementDataBase::SetConstValue( CElement* e, const CValue& v )
{
  SetValue( e, v );
  e->SetConst();
}


