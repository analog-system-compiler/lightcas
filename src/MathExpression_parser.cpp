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

#include "Element.h"
#include "MathExpression.h"

void CMathExpression::GetFromString( CParser& IC )
{
  Clear();
  do
  {
    GetLevel( IC, 0 );
  }
  while( IC.TryFind( ';' ) );
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
        ds += " ; ";
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
  pos2 = DisplaySymbol(  pos, priority, ds );

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
          ds += ',' ;
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

  OP_CODE op = Get( pos - 1 );

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
      break;
    }
  }

  return pos;
}

unsigned  CMathExpression::DisplaySymbolString(  const CSymbolSyntaxStruct& st, unsigned pos, unsigned precedence, CDisplay& ds ) const
{
  char c;
  unsigned k, j;

  unsigned pos2, reserved_pos[8];

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

  const char* sp = st.m_Syntax;

  while ( c = *sp++ )
  {
    if( CParser::IsWord( c ) )
    {
      if( c < 'a' )
      {
        precedence = 0;
      }
      DisplayBranch( reserved_pos[--j], precedence, ds );
    }
    else
    {
      ds += c;
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
  while( c = *sp )
  {
    if( CParser::IsWord( c ) )
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

  if( rule_equ.GetSize() )
  {
    equ.Copy( *this );
    Clear();
    ApplyRule( equ, pos_array, &rule_equ, false );

  }

#if 0//def _DEBUG
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

  i = 0;
  if( IC.TryFind( '(' ) )
  {
    while( IC.GetChar() && !IC.TryFind( ')' ) )
    {
      GetLevel( IC, 0 );
      IC.TryFind( ',' );
      i++;
    }
  }

  if( e )
  {
    f = e->GetFunction();
    if ( element_creation && ( i > 0 ) )
    {
      f->SetParameterNb( i );
      e->SetFunct();
    }
    else if( ( f->GetParameterNb() == 2 ) && ( GetLastOperator() == CElementDataBase::OP_CONCAT ) )
    {
      m_StackSize--;
      ASSERT( f->GetParameterNb() == i + 1 );
    }
    else
    {
      ASSERT( f->GetParameterNb() == i );
    }
    Push( e );
  }

  return pos != IC.GetPos();
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

void CMathExpression::StoreStackPointer( char c, unsigned pos_array[] )
{
  unsigned elem_id;
  elem_id = toupper(c) - 'A';
  ASSERT( elem_id < CElementDataBase::MAX_EXP );
  pos_array[elem_id]  = m_StackSize;

}