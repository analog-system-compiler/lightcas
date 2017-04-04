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

void CMathExpression::Display( CDisplay& ds ) const
{
  unsigned pos = m_StackSize;

  if( pos == 0 )
  {
    ds += '0';
  }
  else
  {
    while( pos )
    {
      pos = DisplayBranch( pos, 0, ds );
      if( pos )
      {
        ds += ' ';
      }
    }
  }
}

unsigned CMathExpression::DisplayBranch( unsigned pos, unsigned priority, CDisplay& ds ) const
{
  unsigned i, n;
  CElement* e;
  unsigned pos2;
  CDisplay ds2, ds3;

  ASSERT( pos );
  pos2 = DisplaySymbol( pos, priority, ds );

  if( pos == pos2 ) //no symbol displayed
  {
    OP_CODE op = Pop( pos );
    e = RefToElement( op );
    e->Display( ds );
    n = e->GetFunction()->GetParameterNb();
    if( n || e->IsNumeric() ) // For Rand(), n=0
    {
      for( i = 0; i < n; i++ )
      {
        ds2.Clear();
        pos = DisplayBranch( pos, 0, ds2 );
        if( i != 0 )
        {
          ds2 += ' ' ;
        }
        ds3.Prepend( ds2 );
      }
      ds += '(';
      ds += ds3 ;
      ds += ')' ;
    }
  }
  else
  {
    pos = pos2;
  }

  return pos;
}

unsigned  CMathExpression::DisplaySymbol(  unsigned pos, unsigned precedence, CDisplay& ds ) const
{
  unsigned pos_array[ CElementDataBase::MAX_EXP ];

  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();
  for( unsigned i = 0; i < st.GetSize(); i++ )
  {
    const CSymbolSyntaxStruct* ss = st[i];
    const CMathExpression* equ =  &ss->m_Equation;
    unsigned pos1 = Match( pos, *equ, pos_array );
    if( pos1 != pos )
    {
      //ASSERT( i != 25 );
      const char* sp = ss->m_Syntax;
      if( i < precedence )
      {
        ds += '(' ;
        DisplaySymbolString( sp, pos_array, i, ds );
        ds += ')';
      }
      else
      {
        DisplaySymbolString( sp, pos_array, i, ds );
      }
      pos = pos1;
      break;
    }
  }

  return pos;
}

void  CMathExpression::DisplaySymbolString(  const char* sp, unsigned pos_array[], unsigned precedence, CDisplay& ds ) const
{
  char c;
  unsigned i;
  unsigned precedence2;

  while ( ( c = *sp++ ) )
  {
    if ( c == '(' )
    {
      precedence++;
    }
    else if ( c == ')' )
    {
      if ( precedence ) { precedence--; }
    }
    else if( c == CParser::m_OperatorExclude )
    {
      sp++; //Exclude operator
    }
    else if( CParser::IsWord( c ) )
    {
      precedence2 = precedence;
      if( c < 'a' )
      {
        precedence2 = 0;
      }
      i = tolower( c ) - 'a';
      DisplayBranch( pos_array[i], precedence2, ds );
    }
    else
    {
      ds += c;
    }
  }
}

void CMathExpression::ParseMacro( CParser& IC )
{
  CString s = IC.GetWord();
  if ( s == "sym" )
  {
    m_ElementDB->AssociateSymbol( IC );
  }
  else if ( s == "inc" )
  {
    CParser IC2;
    if ( IC2.LoadFile( IC.GetQuote() ) )
    {
      Parse( IC2 );
    }
    else
    {
      IC.Error( CParserException::ID_ERROR_FILE_NOT_FOUND );
    }

  }
}

void CMathExpression::ParseAtom( CParser& IC )
{
  if ( IC.TryFind( '(' ) )
  {
    GetLevel( IC );
    IC.Find( ')' );
  }
  else if ( IC.IsDigit() )
  {
    CEvaluator* eval = m_ElementDB->GetEvaluator();
    const char* pos = IC.GetPos();
    pos = eval->GetValueFromString( pos );
    IC.SetPos( pos );
    PushEvalElement();
  }
  else if ( IC.IsWord() )
  {
    ParseElement( IC );
  }
  else
  {
    IC.Error( CParserException::ID_ERROR_OPERATOR_EXPECTED );
  }
}

unsigned CMathExpression::GetLevel( CParser& IC )
{
  unsigned i = 0;
  while ( !IC.IsStopChar() )
  {
    if ( IC.TryFind( CParser::m_SymbolMacro ) )
    {
      ParseMacro( IC );
    }
    else
    {
      GetLevel( IC, 0 );
      i++;
    }
  }
  return i;
}

void CMathExpression::GetLevel( CParser& IC, unsigned priority )
{
  bool symbol_first = true;

  while( !IC.IsStopChar() )
  {
    if ( !SearchOperator( IC, symbol_first ? 0 : priority, symbol_first ) )
    {
      if ( symbol_first )
      {
        ParseAtom( IC );
      }
      else
      {
        break;
      } //exit level
    }
    symbol_first = false;
  }
}

bool CMathExpression::SearchOperator( CParser& IC, unsigned priority, bool symbol_first )
{
  unsigned i, k, n;
  unsigned pos_array[CElementDataBase::MAX_EXP];
  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();
  const char* init_pos = IC.GetPos();

  InitPositionTable( pos_array );
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
      unsigned pos = n;
      if ( !symbol_first ) //FIXME
      {
        NextBranch( pos );  //remove first parameter
      }
      ApplyRuleWrapper( pos, pos_array, rule_equ, false );
#if 0 //def _DEBUG
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

unsigned CMathExpression::MatchOperator( CParser& IC, const char* sp, unsigned pos_array[], unsigned precedence, bool symbol_first )
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
  c = *sp;
  while( c )
  {
    if ( c == '(' || c == ')' ) //Parenthesis not used in parsing stage
    {
      sp++;
      c = *sp;
    }
    if( CParser::IsWord( c )  )
    {
      if( c < 'a' )
      {
        precedence = 0;
      }
      GetLevel( IC, precedence ); //Push parameters
      StoreStackPointer( c, pos_array );
      sp++;
      k++;
    }
    else if( !IC.TryMatchSymbol( sp ) )
    {
      return 0;
    }
    c = *sp;
  }

  return k;
}

bool CMathExpression::ParseElement( CParser& IC )
{
  CElement* e;
  CFunction* f;
  unsigned i = 0;

  ASSERT( IC.IsWord() );
  CEvaluator* eval = m_ElementDB->GetEvaluator();
  eval->ClearValue();
  e = m_ElementDB->GetElement( IC.GetWord() );

  if( e )
  {
    if ( IC.TryFind( '(' ) )
    {
      i = GetLevel( IC );
      IC.Find( ')' );
    }

    f = e->GetFunction();
    if ( !e->IsFunct() /*element_creation*/ && ( i > 0 ) )
    {
      ASSERT( !e->IsFunct() );
      //f = e->GetFunction();
      f->SetParameterNb( i );
      e->SetFunct();
#ifdef _DEBUG
      CDisplay ds;
      ds += "SetParamNb   : ";
      e->Display( ds );
      ds +=  ':' ;
      ds += CString( e->GetFunction()->GetParameterNb() );
      TRACE( ds.GetBufferPtr() );
#endif
    }
    else if( ( f->GetParameterNb() == 2 ) && ( GetLastOperator() == CElementDataBase::OP_CONCAT ) )
    {
      m_StackSize--;
      ASSERT( e->IsNumeric() ); // for numeric function, it is allowed to use comma without concatenation op.
      ASSERT( f->GetParameterNb() == i + 1 );
    }
    else
    {
      ASSERT( f->GetParameterNb() == i );
    }
    Push( e );
  }
  return ( e != NULL );
}

void CMathExpression::StoreStackPointer( char c, unsigned pos_array[] )
{
  unsigned elem_id;
  elem_id = toupper( c ) - 'A';
  ASSERT( elem_id < CElementDataBase::MAX_EXP );
  pos_array[elem_id]  = m_StackSize;
}
