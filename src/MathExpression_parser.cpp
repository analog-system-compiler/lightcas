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

void CMathExpression::GetFromString( CParser& IC )
{
  Clear();
  Parse( IC );
}

unsigned CMathExpression::Parse( CParser& IC )
{
  unsigned i = 0;
  CMathExpression equ( m_ElementDB );

  while( !IC.IsStopChar() )
  {
    equ.GetLevel( IC, 0 );
    Push( equ );
    i++;
  }
  return i;
}

unsigned CMathExpression::ParseParenthesis( CParser& IC )
{
  unsigned i = 0;
  if( IC.TryFind( '(' ) )
  {
    i = Parse( IC );
    IC.Find( ')' );
  }
  return i;
}


void CMathExpression::Display( CDisplay& ds ) const
{
  unsigned pos = m_StackSize;;

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

unsigned CMathExpression::DisplayBranch( unsigned pos , unsigned priority, CDisplay& ds ) const
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
      ds += '(' ;
      ds2.Copy( ds );
      for( i = 0; i < n; i++ )
      {
        ds.Clear();
        pos = DisplayBranch( pos, 0, ds );
        if( i != 0 )
        {
          ds += ' ' ;
        }
        ds3.Prepend( ds );
      }
      ds.Copy( ds2 );
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
    const char* sp = ss->m_Syntax;
    unsigned pos1 = Match( pos, *equ, pos_array );
    if( pos1 != pos )
    {
      if( i < precedence )
      {
        ds += '(' ;
      }
      DisplaySymbolString( sp,  pos_array,  i,  ds );
      if( i < precedence )
      {
        ds += ')' ;
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

  while ( ( c = *sp++ ) )
  {
    if( c == '(' )
    {
      sp++;
      while( *sp != ')' )
      {
        sp++;
      }
      sp++;
    }
    else if( CParser::IsWord( c ) )
    {
      if( c < 'a' )
      {
        precedence = 0;
      }
      i = tolower( c ) - 'a';
      DisplayBranch( pos_array[i], precedence, ds );
    }
    else
    {
      ds += c;
    }
  }
}

void CMathExpression::GetLevel( CParser& IC, unsigned priority )
{
  Clear();

  if( !IC.IsStopChar() )
  {
    if( ParseParenthesis( IC ) == 0 )
    {
      if( !ParseElement( IC ) )
      {
        if( !SearchOperator( IC, 0, true ) )
        {
          IC.Error( CParserException::ID_ERROR_OPERATOR_EXPECTED );
        }
      }
    }
  }

  while( !IC.IsStopChar() )
  {
    if( !SearchOperator( IC, priority, false ) )
    {
      break;
    }
  }
}

bool CMathExpression::SearchOperator( CParser& IC, unsigned priority, bool symbol_first )
{
  unsigned i;
  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();

  for( i = priority; i < st.GetSize(); i++ )
  {
    const char* sp = st[i]->m_Syntax;
    const CMathExpression& equ =  st[i]->m_Equation;

    if( CParser::IsWord( sp[0] ) != symbol_first )
    {
      if( MatchOperator( IC, sp, equ, i + 1  ) )
      {
        return true;
      }
    }
  }

  return false;
}

bool CMathExpression::MatchOperator( CParser& IC, const char* sp, const CMathExpression& rule_equ, unsigned precedence )
{
  char c;
  unsigned pos_array[ CElementDataBase::MAX_EXP ];
  CMathExpression equ( m_ElementDB );
  InitPositionTable( pos_array );

  c = *sp;
  if( CParser::IsWord( c ) )
  {
    StoreStackPointer( c, pos_array );
    sp++;
  }

  // try to match prefix operator
  while( ( c = *sp ) )
  {
    if( CParser::IsWord( c )  )
    {
      if( c < 'a' )
      {
        precedence = 0;
      }
      equ.GetLevel( IC, precedence );
      Push( equ );
      StoreStackPointer( c, pos_array );
      sp++;

    }
    else if( !IC.TryMatchSymbol( sp ) )
    {
      return false;
    }
  }

  if( *sp  ) //check that all characters have matched
  {
    return false;
  }

  if( rule_equ.GetSize() )
  {
    equ.Copy( *this );
    Clear();
    ApplyRule( equ, pos_array, &rule_equ, false );
  }

#if 0 //def _DEBUG
  CDisplay ds;
  Display( ds );
  TRACE( ds.GetBufferPtr() );
#endif

  return true;
}

bool CMathExpression::ParseElement( CParser& IC )
{
  CElement* e;
  CFunction* f;
  bool element_creation;
  unsigned i, n;
  const char* pos;

  pos = IC.GetPos();
  n = m_ElementDB->GetSize();
  e = m_ElementDB->ParseElement( IC );
  element_creation = ( m_ElementDB->GetSize() - n ) != 0; //put there !!!
  i = ParseParenthesis( IC );

  if( e )
  {
    f = e->GetFunction();
    if ( element_creation && ( i > 0 ) )
    {
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

    //Check if symbol definition for this operator
    if( IC.GetChar() == CParser::m_SymbolDelimiter )
    {
      m_ElementDB->AssociateSymbol( IC, *this );
      //Clear(); //clear equation to avoid being executed after being parsed.
      Init( RefToElement( CElementDataBase::OP_NONE ) );
    }
  }

  return pos != IC.GetPos();
}

void CMathExpression::StoreStackPointer( char c, unsigned pos_array[] )
{
  unsigned elem_id;
  elem_id = toupper( c ) - 'A';
  ASSERT( elem_id < CElementDataBase::MAX_EXP );
  pos_array[elem_id]  = m_StackSize;

}
