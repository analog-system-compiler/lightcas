/*******************************************************************************/
/*  Copyright (C) 2006 The SIMCAS project                                      */
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

#include "Element.h"
#include "MathExpression.h"


void CMathExpression::GetFromString( CParser& IC )
{
  Clear();
  GetLevel( IC, 0 );
}

void CMathExpression::Display( CDisplay& ds ) const
{
  unsigned pos= m_StackSize;;

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
        ds += " ; ";
      }
    }
  }
}
unsigned CMathExpression::DisplayBranch( unsigned pos , unsigned priority, CDisplay& ds ) const
{
  unsigned i;
  CElement* e;
  unsigned pos2;
  CDisplay ds2, ds3;

  ASSERT( pos );
  pos2 = DisplaySymbol(  pos, priority, ds );

  if( pos == pos2 ) //no symbol displayed
  {
    OP_CODE op = Pop( pos );
    e = RefToElement( op );
    e->Display( ds );

    if( e->IsFunct() )
    {
      ds += '(' ;

      for( i = 0; i < e->GetFunction()->GetParameterNb(); i++ )
      {
        ds2.Clear();       
        pos = DisplayBranch( pos, 0, ds2 );
        if( i != 0 )
        {
            ds2 += ',' ;
        }
        ds3.Prepend( ds2 );
      }
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

  OP_CODE op = Get( pos-1 );
  
  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();
  for( unsigned i = 0; i < st.GetSize(); i++ )
  {
    const CSymbolSyntaxStruct* ss = st[i];
    const CMathExpression* equ =  &ss->m_Equation;
    if( equ->GetLastOperator() == op )
    {
      if( i < precedence )
      {
        ds += '(' ;
      }
      pos = DisplaySymbolString( *ss,  pos,  i,  ds );
      if( i < precedence )
      {
        ds += ')' ;
      }
    }
  }

  return pos;
}

unsigned  CMathExpression::DisplaySymbolString(  const CSymbolSyntaxStruct& st, unsigned pos, unsigned i, CDisplay& ds ) const
{
  char c;
  unsigned k, j;

  unsigned pos2, reserved_pos[8];
  //CDisplay ds2, ds3;

  const CMathExpression& equ =  st.m_Equation;
  j = 0;

  pos2 = equ.GetSize();
  while( pos2 )
  {
    OP_CODE op = equ.Pop( pos2 );
    if( IsReserved( op ) ) 
    {
      ASSERT( j < sizeof( reserved_pos ) / sizeof( unsigned ) );
      reserved_pos[j++] = pos;
      NextBranch( pos );
    }
    else
    {
      Pop( pos );
    }
  }
  const CString& s = st.m_Syntax;

  for( k = 0; k < s.GetLength(); k++ )
  {
    c = s[k];

    if( c >= 'a' && c <= 'h' )
    {
      DisplayBranch( reserved_pos[--j], i, ds );
    }
    else if ( c >= 'A' && c <= 'H' )
    {
      DisplayBranch( reserved_pos[--j], 0, ds );
    }
    else
    {
      ds += c ;
    }

  }

  return pos;
}

void CMathExpression::GetLevel( CParser& IC, unsigned priority )
{
  if( !IC.IsStopChar() )
  {
    if( !ParseElement( IC ) )
    {
      if( !SearchOperator( IC, 0, true ) )
      {
        IC.Error( CParserException::ID_ERROR_OPERATOR_EXPECTED );
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
}

bool CMathExpression::SearchOperator( CParser& IC, unsigned priority, bool symbol_first )
{
  unsigned i;
  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();

  for( i = priority; i < st.GetSize(); i++ )
  {
    const CString& s = st[i]->m_Syntax;
    const CMathExpression& equ =  st[i]->m_Equation;

    if( MatchOperator( IC, s, equ, i + 1, symbol_first  ) )
    {
      return true;
    }
  }

  return false;
}

bool CMathExpression::MatchOperator( CParser& IC, const CString& s, const CMathExpression& rule_equ, unsigned priority, bool symbol_first )
{
  const char* sp = s.GetBufferPtr();
  unsigned pos_array[ CElementDataBase::MAX_EXP ];

  InitPositionTable( pos_array );

  if( symbol_first )
  {
    if( CParser::IsWord( *sp ) )
    {
      return false;
    }
  }
  else
  {
    if( !CParser::IsWord( *sp ) )
    {
      return false;
    }

    sp = ParseExpression( sp, pos_array, priority, IC, false );

  }
  // try to match prefix operator

  while( *sp )
  {

    if( !CParser::IsWord( *sp ) )
    {
      if( !IC.TryMatchSymbol( sp ) )
      {
        break;
      }
    }

    if( *sp == '\0' )
    {
      break;  // case [a]
    }

    sp = ParseExpression( sp, pos_array, priority, IC, true );

  }

  if( *sp == '\0' )
  {

    if( rule_equ.GetSize() )
    {
      CMathExpression equ( m_ElementDB );
      equ.Copy( *this );
      ApplyRule( equ, pos_array, &rule_equ, false );

    }
#if 0//_DEBUG
    CDisplay ds;
    Display( ds );
    TRACE( ds.GetBufferPtr() );
#endif
    return true;
  }
  return false;
}

bool CMathExpression::ParseElement( CParser& IC )
{
  CElement* e;
  CFunction* f;
  bool element_creation;
  unsigned n;

  n = m_ElementDB->GetSize();

  e = m_ElementDB->ParseElement( IC );
  if( e )
  {
    element_creation = ( m_ElementDB->GetSize() - n ) != 0;
    if( element_creation )
    {
      if( IC.TryFind( '(' ) )
      {
        e->SetFunct();
        if( IC.TryFind( ')' ) )
        {
          e->GetFunction()->SetParameterNb( 0 );
        }
        else
        {
          e->GetFunction()->SetParameterNb( 1 );
          GetLevel( IC, 0 );
          IC.Find( ')' );
        }
      }
    }
    else
    {
      f = e->GetFunction();
      if( f )
      {
        if( f->GetParameterNb() == 0 )
        {
          if( IC.TryFind( '(' ) )
          {
            IC.Find( ')' );
          }
        }
        else
        {
          IC.Find( '(' );
          GetLevel( IC, 0 );
          IC.Find( ')' );

          //specific case for 2-operands function
          if( ( f->GetParameterNb() == 2 ) && ( GetLastOperator() == CElementDataBase::OP_CONCAT ) )
          {
            m_StackSize--;
          }
        }
      }
    }

    Push( e );
  }
  else if( IC.TryFind( '(' ) )
  {
    GetLevel( IC, 0 );
    IC.Find( ')' );
  }
  else
  {
    return false;
  }

  return true;
}

void CMathExpression::GetFromTextRPN( CParser& IC )
{
  // see GetAtom
  CElement* e;
  Clear();
  while( !IC.TryFind( ';' ) )
  {
    e = m_ElementDB->ParseElement( IC );
    Push( e );
  }
}

const char* CMathExpression::ParseExpression( const char* sp, unsigned pos_array[], unsigned precedence, CParser& IC, bool allow_recursion )
{
  char elem_id;

  if( *sp >= 'a' )
  {
    elem_id = *sp - 'a';
  }
  else
  {
    elem_id = *sp - 'A';
    precedence = 0;
  }

  sp++;

  if( allow_recursion )
  {
    CMathExpression equ( m_ElementDB );
    equ.GetLevel( IC, precedence );
    Push( equ );
  }

  ASSERT( elem_id < CElementDataBase::MAX_EXP );
  pos_array[elem_id]  = m_StackSize;

  return sp;
}