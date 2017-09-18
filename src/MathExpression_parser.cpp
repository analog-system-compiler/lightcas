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

#include <cctype>
#include "Element.h"
#include "MathExpression.h"

unsigned CMathExpression::Parse( CParser& IC )
{
  Clear();
  return GetLevel( IC );
}

bool CMathExpression::ParseMacro( CParser& IC )
{
  CString s = IC.GetWord();
  if ( s == "sym" )
  {
    if ( !m_ElementDB->AssociateSymbol( IC ) )
    {
      return false;
    }
  }
  else if ( s == "inc" )
  {
    if ( IC.GetQuote() )
    {
      CParser IC2;
      if ( IC2.LoadFile( IC.GetBuffer() ) )
      {
        Parse( IC2 );
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  return true;
}

bool CMathExpression::ParseAtom( CParser& IC )
{
  if ( IC.TryFind( '(' ) )
  {
    GetLevel( IC );
    if ( !IC.Find( ')' ) )
    {
      return false;
    }
  }
  else if ( IC.IsDigit() )
  {
    CEvaluator* eval = m_ElementDB->GetEvaluator();
    const char* pos = IC.GetPos();
    pos = eval->GetValueFromString( pos );
    IC.SetPos( pos );
    PushEvalElement( *eval );
  }
  else if ( IC.IsWord() )
  {
    if ( !ParseElement( IC ) )
    {
      return false;
    }
  }
  else
  {
    return false;
  }
  return true;
}

int CMathExpression::GetLevel( CParser& IC )
{
  int i = 0;
  while ( !IC.IsStopChar() )
  {
    if ( IC.TryFind( CParser::m_SymbolMacro ) )
    {
      if( !ParseMacro( IC ) )
      {
        return -1; // IC.Error(CParserException::ID_ERROR_FILE_NOT_FOUND);
      }
    }
    else
    {
      if( !GetLevel( IC, 0 ) )
      {
        return -2; //IC.Error( CParserException::ID_ERROR_OPERATOR_EXPECTED );
      }
      i++;
    }
  }
  return i;
}

bool CMathExpression::GetLevel( CParser& IC, unsigned priority )
{
  bool symbol_first = true;

  while( !IC.IsStopChar() )
  {
    if ( !SearchOperator( IC, symbol_first ? 0 : priority, symbol_first ) )
    {
      if ( symbol_first )
      {
        if( !ParseAtom( IC ) )
        {
          return false;
        }
      }
      else
      {
        break;
      } //exit level
    }
    symbol_first = false;
  }
  return true;
}

bool CMathExpression::SearchOperator( CParser& IC, unsigned priority, bool symbol_first )
{
  unsigned i, k, n;
  pos_t pos_array[CElementDataBase::MAX_EXP] = { 0 };
  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();
  const char* init_pos = IC.GetPos();

  n = GetSize();

  for( i = priority; i < st.GetSize(); i++ )
  {
    const char* sp = st[i]->m_Syntax;
    k = MatchOperator( IC, sp, pos_array, i + 1, symbol_first );
    if ( k != 0 )
    {
      const CMathExpression& rule_equ = st[i]->m_Equation;
      ASSERT( rule_equ.GetSize() );

      CMathExpression equ( m_ElementDB ); //save current stack
      pos_t pos = n;
      if ( !symbol_first ) //FIXME
      {
        NextBranch( pos );  //remove first parameter
      }
      ApplyRule( pos, pos_array, rule_equ, false );
#if ( DEBUG_LEVEL >= 3 )
      CDisplay ds;
      Display( ds );
      TRACE( ds.GetBufferPtr() );
#endif
      return true;
    }
    IC.SetPos( init_pos );
    SetSize( n );
  }

  return false;
}

unsigned CMathExpression::MatchOperator( CParser& IC, const char* sp, pos_t pos_array[], unsigned precedence, bool symbol_first )
{
  unsigned k = 0;
  char c;

  c = *sp;
  if ( c == '(' || c == ')' ) //Parenthesis not used in parsing stage
  {
    sp++;
    c = *sp;
  }

  if ( CParser::IsWord( c ) == symbol_first )
  {
    return 0;
  }

  if ( CParser::IsWord( c ) )
  {
    StoreStackPointer( c, pos_array );
    sp++;
    k++;
  }

  // try to match prefix operator
  while( ( c = *sp ) )
  {
    if ( c == '(' || c == ')' ) //Parenthesis not used in parsing stage
    {
      sp++;
      c = *sp;
    }

    if ( CParser::IsWord( c ) )
    {
      if ( c < 'a' )
      {
        precedence = 0;
      }
      GetLevel( IC, precedence ); //Push parameters
    }

    if ( CParser::IsWord( c ) )
    {
      StoreStackPointer( c, pos_array );
      sp++;
      k++;
    }
    else if( !IC.TryMatchSymbol( sp ) )
    {
      return 0;
    }
  }

  return k;
}

bool CMathExpression::ParseElement( CParser& IC )
{
  CElement* e;
  CFunction* f;
  int i = 0;

  ASSERT( IC.IsWord() );
  CEvaluator* eval = m_ElementDB->GetEvaluator();
  eval->ClearValue();
  e = m_ElementDB->GetElement( IC.GetWord() );

  if( !e )
  {
    return false;
  }

  if ( IC.IsOpenParenthesis() )
  {
    IC.Next();
    i = GetLevel( IC );
    if ( i < 0 )
    {
      return false;
    }
    if ( !IC.Find( ')' ) )
    {
      return false;
    }

    f = e->GetFunction();
    if ( !e->IsFunct() /*element_creation*/ /*&& (i > 0)*/ )
    {
      ASSERT( !e->IsFunct() );
      //f = e->GetFunction();
      f->SetParameterNb( i );
      e->SetFunct();
#if ( DEBUG_LEVEL >= 1 )
      CDisplay ds;
      ds += "SetParamNb   : ";
      e->Display( ds );
      ds += ':';
      ds += CString( e->GetFunction()->GetParameterNb() );
      TRACE( ds.GetBufferPtr() );
#endif
    }
    ASSERT( f->GetParameterNb() == i );
  }

  Push( e );

  return true;
}

void CMathExpression::StoreStackPointer( char c, pos_t pos_array[] )
{
  unsigned elem_id;
  elem_id = toupper( c ) - 'A';
  ASSERT( elem_id < CElementDataBase::MAX_EXP );
  pos_array[elem_id]  = m_StackSize;
}
