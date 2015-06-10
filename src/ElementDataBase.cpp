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
#include <cctype>
#include "Debug.h"
#include "Element.h"
#include "Function.h"
#include "ElementDataBase.h"

CElementArray      CElementDataBase::m_ElementRefArray;
CSymbolSyntaxArray CElementDataBase::m_SymbolSyntaxArray;
unsigned           CElementDataBase::m_SecureLimit;

#ifdef EMBED_RULES
extern const char Rules[] asm("_binary____src_Rules_txt_start");
#endif

const SProperties CElementDataBase::m_FunctionProperties[] =
{
    { "ADD",    2, &CEvaluator::Add           },
    { "SUB",    2, &CEvaluator::Sub           },
    { "OR",     2, &CEvaluator::Or            },
    { "LOR",    2, &CEvaluator::LOr           },
    { "XOR",    2, &CEvaluator::Xor           },
    { "MUL",    2, &CEvaluator::Mul           },
    { "DIV",    2, &CEvaluator::Div           },
    { "AND",    2, &CEvaluator::And           },
    { "LAND",   2, &CEvaluator::LAnd          },
    { "EQ",     2, &CEvaluator::Equal         },
    { "NEQ",    2, &CEvaluator::NotEqual      },
    { "LTE",    2, &CEvaluator::LowerOrEqual  },
    { "SHL",    2, &CEvaluator::ShiftLeft     },
    { "LT",     2, &CEvaluator::Lower         },
    { "GTE",    2, &CEvaluator::GreaterOrEqual},
    { "SHR",    2, &CEvaluator::ShiftRight    },
    { "GT",     2, &CEvaluator::Greater       },
    { "MIN",    2, &CEvaluator::Min           },
    { "MAX",    2, &CEvaluator::Max           },
    { "MOD",    2, &CEvaluator::Mod           },
    { "POW",    2, &CEvaluator::Pow           },
    { "NEG",    1, &CEvaluator::Neg           },
    { "INV",    1, &CEvaluator::Inv           },
    { "NOT",    1, &CEvaluator::Not           },
    { "ID",     1, &CEvaluator::Id            },
    { "BOOL",   1, &CEvaluator::Bool          },
    { "LNOT",   1, &CEvaluator::LNot          },
    { "SIN",    1, &CEvaluator::Sin           },
    { "COS",    1, &CEvaluator::Cos           },
    { "TAN",    1, &CEvaluator::Tan           },
    { "ASIN",   1, &CEvaluator::Asin          },
    { "ACOS",   1, &CEvaluator::Acos          },
    { "ATAN",   1, &CEvaluator::Atan          },
    { "SINH",   1, &CEvaluator::SinH          },
    { "COSH",   1, &CEvaluator::CosH          },
    { "TANH",   1, &CEvaluator::TanH          },
    { "ASINH",  1, &CEvaluator::AsinH         },
    { "ACOSH",  1, &CEvaluator::AcosH         },
    { "ATANH",  1, &CEvaluator::AtanH         },
    { "EXP",    1, &CEvaluator::Exp           },
    { "LOG",    1, &CEvaluator::Ln            },
    { "LOG10",  1, &CEvaluator::Log           },
    { "SQRT",   1, &CEvaluator::Sqrt          },
    { "SQR",    1, &CEvaluator::Sqr           },
    { "FACT",   1, &CEvaluator::Fact          },
    { "ABS",    1, &CEvaluator::Abs           },
    { "FLOOR",  1, &CEvaluator::Floor         },
    { "CEIL",   1, &CEvaluator::Ceil          },
    { "RAND",   0, &CEvaluator::Rand          },
    { "IF2",    2, &CEvaluator::If            },
    { "IF",     2, &CEvaluator::If            },
    { "CONCAT", 2, &CEvaluator::Concat        },
    { "TED",    2, NULL                       },

};

const unsigned CElementDataBase::m_FunctionPropertiesSize = sizeof( CElementDataBase::m_FunctionProperties ) / sizeof( SProperties );

CElementDataBase::CElementDataBase( const CString& name,  CElementDataBase* parent, CEvaluator* eval, bool bInitialize )
{
  m_Parent = parent;
  m_Name = name;
  m_Evaluator = eval;
  m_SecureLimit = 0;
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
    CParser IC;
    AddReservedElements();
    AddEvalFunctionTable( m_FunctionProperties, m_FunctionPropertiesSize );
    SetSecureLimit( GetSize() );
#ifdef EMBED_RULES
    IC.SetPos( Rules );
    AddAlgebraRuleTable ( IC );
#else
    if( IC.LoadFile( CString( "Rules.txt" ) ) )
    {
      AddAlgebraRuleTable ( IC );
      IC.CloseFile();
    }
    else
    {
      IC.Append( "rules description file not found." );
      IC.Error( CParserException::ID_ERROR_FILE_NOT_FOUND );
    }
#endif
    CleanTempElements();
  }
  else
  {
    m_Evaluator = m_Parent->GetEvaluator();
  }
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
  GetElement( "GET"    )->SetOperandNb( 1 );
  GetElement( "NONE"   );//->SetOperandNb( 0 );
  GetElement( "CONST"  )->SetOperandNb( 1 );
  GetElement( "ELEM"   )->SetOperandNb( 1 );
  GetElement( "NEG"    )->SetOperandNb( 1 );
  GetElement( "j"      );//->SetOperandNb( 0 );
  GetElement( "RANK"   )->SetOperandNb( 2 );
  GetElement( "SUBST"  )->SetOperandNb( 3 );
  GetElement( "SYST"   )->SetOperandNb( 2 );
  GetElement( "ERROR"  )->SetOperandNb( 1 );
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
  ASSERT( ElementToRef( m_ElementRefArray[ OP_GET    ] ) == OP_GET    );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_NONE   ] ) == OP_NONE   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_CONST  ] ) == OP_CONST  );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_ELEM   ] ) == OP_ELEM   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_NEG    ] ) == OP_NEG    );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_CPLX   ] ) == OP_CPLX   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_RANK   ] ) == OP_RANK   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_SUBST  ] ) == OP_SUBST  );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_SYST   ] ) == OP_SYST   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_ERROR  ] ) == OP_ERROR  );
}

void CElementDataBase::AssociateSymbol( CParser& IC, const CMathExpression& dst_equ )
{
  char c;
  unsigned i;
  CSymbolSyntaxStruct* sss;
  CString s;
  CMathExpression src_equ( this );

  sss = new CSymbolSyntaxStruct();
  i = 0;
  IC.Next();
  c = IC.GetChar();
  while( c && c != CParser::m_SymbolDelimiter && ( i < ( sizeof( sss->m_Syntax ) - 1 ) ) )
  {
    if( CParser::IsWord( c ) )
    {
      s.Clear();
      s.Append( tolower( c ) );
      CElement* e = GetElement( s );
      if( e )
      {
        src_equ.Push( e );
      }
      else
      {
        ASSERT( false );
      }
    }

    sss->m_Syntax[ i++ ] = c;
    IC.Next();
    c = IC.GetChar();
  }

  IC.Next();
  sss->m_Syntax[ i++ ] = '\0';

  sss->m_Equation.Copy( dst_equ );
  CMathExpression::ConvertToRule( src_equ, sss->m_Equation );
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

void CElementDataBase::AddAlgebraRuleTable( CParser& IC )
{
  CMathExpression src( this );

  src.GetFromString("SET(  ACTIVATE( SYST( a SET( b c ) ) )  SYST( ACTIVATE( a ) SET( b c )  )  )");
  src.OptimizeTree();

  src.GetFromString(IC);
  src.Push( GetElement("ACTIVATE") );

#ifdef _DEBUG
  CDisplay ds;
  src.Display(ds);
#endif

  src.OptimizeTree(); // optimize 'if' if any.    
}

void CElementDataBase::CleanTempElements()
{
  CElement* e;
  for( unsigned i = 0; i < GetSize(); i++ )
  {
    e = GetAt( i );
    if( ( e->ToRef() > CElementDataBase::GetSecureLimit() ) && !e->IsFunct() && !e->IsConst() )
    {
      delete e;
      RemoveAt( i );
      i--;
    }
  }
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
    const char* pos = IC.GetPos();
    v = GetEvaluator()->GetValueFromString( &pos );
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


