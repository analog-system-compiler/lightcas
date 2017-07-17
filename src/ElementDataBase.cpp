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
extern const char Rules[] asm( "_binary___objs_Rules_txt_start" );
#endif

const SProperties CElementDataBase::m_BuiltInProperties[] =
{
  { "CONCAT",           2, NULL },
  { "SET",              2, NULL },
  { "GET",              1, NULL },
  { "NONE",             0, NULL },
  { "STACK_SIZE_ERROR", 0, NULL },
  { "CONST",            1, NULL },
  { "ELEM",             1, NULL },
  { "NEG",              1, NULL },
  { "j",                0, NULL },
  { "RANK",             2, NULL },
  { "SUBST",            3, NULL },
  { "SYST",             2, NULL },
  { "ERROR",            1, NULL },
  { "TED",              2, NULL },
  { "EVAL",             1, NULL },
};

const SProperties CElementDataBase::m_FunctionProperties[] =
{
  { "_add",    2, &CEvaluator::Add           },
  { "_sub",    2, &CEvaluator::Sub           },
  { "_or",     2, &CEvaluator::Or            },
  { "_lor",    2, &CEvaluator::LOr           },
  { "_xor",    2, &CEvaluator::Xor           },
  { "_mul",    2, &CEvaluator::Mul           },
  { "_div",    2, &CEvaluator::Div           },
  { "_and",    2, &CEvaluator::And           },
  { "_land",   2, &CEvaluator::LAnd          },
  { "_eq",     2, &CEvaluator::Equal         },
  { "_neq",    2, &CEvaluator::NotEqual      },
  { "_lte",    2, &CEvaluator::LowerOrEqual  },
  { "_shl",    2, &CEvaluator::ShiftLeft     },
  { "_lt",     2, &CEvaluator::Lower         },
  { "_gte",    2, &CEvaluator::GreaterOrEqual},
  { "_shr",    2, &CEvaluator::ShiftRight    },
  { "_gt",     2, &CEvaluator::Greater       },
  { "_min",    2, &CEvaluator::Min           },
  { "_max",    2, &CEvaluator::Max           },
  { "_mod",    2, &CEvaluator::Mod           },
  { "_pow",    2, &CEvaluator::Pow           },
  { "_neg",    1, &CEvaluator::Neg           },
  { "_inv",    1, &CEvaluator::Inv           },
  { "_not",    1, &CEvaluator::Not           },
  { "_id",     1, &CEvaluator::Id            },
  { "_bool",   1, &CEvaluator::Bool          },
  { "_lnot",   1, &CEvaluator::LNot          },
  { "_sin",    1, &CEvaluator::Sin           },
  { "_cos",    1, &CEvaluator::Cos           },
  { "_tan",    1, &CEvaluator::Tan           },
  { "_asin",   1, &CEvaluator::Asin          },
  { "_acos",   1, &CEvaluator::Acos          },
  { "_atan",   1, &CEvaluator::Atan          },
  { "_sinh",   1, &CEvaluator::SinH          },
  { "_cosh",   1, &CEvaluator::CosH          },
  { "_tanh",   1, &CEvaluator::TanH          },
  { "_asinh",  1, &CEvaluator::AsinH         },
  { "_acosh",  1, &CEvaluator::AcosH         },
  { "_atanh",  1, &CEvaluator::AtanH         },
  { "_exp",    1, &CEvaluator::Exp           },
  { "_log",    1, &CEvaluator::Ln            },
  { "_log10",  1, &CEvaluator::Log           },
  { "_sqrt",   1, &CEvaluator::Sqrt          },
  { "_sqr",    1, &CEvaluator::Sqr           },
  { "_fact",   1, &CEvaluator::Fact          },
  { "_abs",    1, &CEvaluator::Abs           },
  { "_floor",  1, &CEvaluator::Floor         },
  { "_ceil",   1, &CEvaluator::Ceil          },
  { "_rand",   0, &CEvaluator::Rand          },
  { "_if2",    2, &CEvaluator::If            },
  { "_if",     2, &CEvaluator::If            }
};

const unsigned CElementDataBase::m_FunctionPropertiesSize = sizeof( CElementDataBase::m_FunctionProperties ) / sizeof( SProperties );
const unsigned CElementDataBase::m_BuiltInPropertiesSize  = sizeof( CElementDataBase::m_BuiltInProperties )  / sizeof( SProperties );

CElementDataBase::CElementDataBase( const CString& name,  CElementDataBase* parent, CEvaluator* eval, bool bInitialize )
{
  m_Parent = parent;
  if( parent )
  {
    m_Name = parent->GetName() + '.' + name;
  }
  else
  {
    m_Name = name;
  }
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
    AddReservedFunctions();
    SetSecureLimit( GetSize() );
    InitAlgebraRuleTable();
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
      IC.CopyBuffer( "rules description file not found." );
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
  unsigned i;
  const char pseudo_name[MAX_EXP][2] = { "a", "b", "c", "d", "e", "f", "g", "h" };

  GetElement( "0" )->SetConst();
  for(  i = 0; i < MAX_EXP; i++ )
  {
    GetElement()->SetPseudoName( pseudo_name[i] );
  }
  AddEvalFunctionTable( m_BuiltInProperties, m_BuiltInPropertiesSize );
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
  ASSERT( ElementToRef( m_ElementRefArray[OP_STACKERR] ) == OP_STACKERR );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_CONST  ] ) == OP_CONST  );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_ELEM   ] ) == OP_ELEM   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_NEG    ] ) == OP_NEG    );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_CPLX   ] ) == OP_CPLX   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_RANK   ] ) == OP_RANK   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_SUBST  ] ) == OP_SUBST  );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_SYST   ] ) == OP_SYST   );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_ERROR  ] ) == OP_ERROR  );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_TED    ] ) == OP_TED    );
  ASSERT( ElementToRef( m_ElementRefArray[ OP_EVAL   ] ) == OP_EVAL   );
  ASSERT( GetSize() == OP_END_RESERVED );
}

void CElementDataBase::AddReservedFunctions()
{
  AddEvalFunctionTable( m_FunctionProperties, m_FunctionPropertiesSize );
}

void CElementDataBase::AssociateSymbol( CParser& IC )
{
  unsigned char c;
  unsigned i;
  CSymbolSyntaxStruct* sss;
  CString s;
  CMathExpression src_equ( this );
  CMathExpression dst_equ( this );
  dst_equ.GetLevel( IC, 0 );

  sss = new CSymbolSyntaxStruct();
  const char* sp = IC.GetQuote().GetBufferPtr();
  i = 0;
  c = ( unsigned char ) * sp++;
  while( c && !isspace( c ) && ( i < sizeof( sss->m_Syntax ) - 1 ) )
  {
    if( CParser::IsWord( c ) )
    {
      s.Clear();
      s.Append( tolower( c ) );
      CElement* e = GetElement( s );
      ASSERT( e );
      if( e )
      {
        src_equ.Push( e );
      }
    }

    sss->m_Syntax[i++] = c;
    c = *sp++;
  }
  sss->m_Syntax[i] = '\0';
  sss->m_Equation.Copy( dst_equ );
  CMathExpression::ConvertToRule( src_equ, sss->m_Equation );
  m_SymbolSyntaxArray.Append( sss );

#ifdef _DEBUG
  CDisplay ds;
  ds += "Adding symbol syntax [";
  ds += CString( ( int )( m_SymbolSyntaxArray.GetSize() - 1 ) );
  ds += "] ";
  ds += sss->m_Syntax;
  ds += " => ";
  sss->m_Equation.Display( ds );
  TRACE( ds.GetBufferPtr() );
#endif
}

void CElementDataBase::InitAlgebraRuleTable()
{
  CMathExpression src( this );
  src.GetFromString( "SET( EXECUTE( SYST( a b ) )  SYST( EXECUTE( a ) EXECUTE( b ) )  )" );
  src.OptimizeTree();
#if OPTIMIZATION_LEVEL < 2
  src.GetFromString( "SET( EXECUTE( SET( a b ) )  SET( a b ) )" );
  src.OptimizeTree();
  src.GetFromString( "SET( EXECUTE( SIMPLIFY( a ) )  SIMPLIFY( a ) )" );
  src.OptimizeTree();
#endif
  src.GetFromString( "SET( EXECUTE( a )  a )" );
  src.OptimizeTree();
}

void CElementDataBase::AddAlgebraRuleTable( CParser& IC )
{
  CMathExpression src( this );
  src.GetFromString( IC );
  src.Push( GetElement( "EXECUTE" ) );
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

  if( IC.IsWord() )
  {
    e = GetElement( IC.GetWord() );
  }

  return e;
}

CElement* CElementDataBase::GetElement()
{
  unsigned n;
  CElement* e;
  CString s;

  n = m_ElementRefArray.GetSize();
  s = "_";
  s += CString( ( int )n );
  e = CreateElement( s, GetSize() );
  e->SetTemporary();
  return e;
}

CElement* CElementDataBase::GetElement( const CString& string )
{
  unsigned pos;
  CElement* e;

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
  m_Evaluator->DeclareElement( e->ToRef() );
  return e;
}

CElement* CElementDataBase::SearchElement( const CString& string, unsigned& pos ) const
{
  unsigned  i, n;
  int compare;
  CElement* e;

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
