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
#include "Equation.h"


void CEquation::GetFromText( CParser& IC )
{
  Clear();
  //do
  {
    GetLevel( IC, 0 );
  }
  //while( IC.TryFind( ';' ) );

  /* if( IC.GetChar() != '\0' )
   {
     IC.Error( CParserException::ID_SYNTAX_ERROR );
   }*/
}

void CEquation::Display( CDisplay& ds ) const
{
  unsigned pos;

  if( IsNull() )
  {
    ds += '0';
  }

  else
  {

    /* CString s1;
     s1 = ds;
     ds.Clear();*/

    pos = m_StackSize;
    while( pos )
    {

      pos = DisplayBranch( pos, 0, ds );
      if( pos )
      {
        ds += " ; ";
      }
    }

    //ds.Prepend( s1 );
    ASSERT( pos == 0 );
  }
}
unsigned CEquation::DisplayBranch( unsigned pos , unsigned priority, CDisplay& ds ) const
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
        if( i < ( e->GetFunction()->GetParameterNb() - 1 ) )
        {
          ds2 += ',' ;
        }
        pos = DisplayBranch( pos, 0, ds2 );
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

unsigned  CEquation::DisplaySymbol(  unsigned pos, unsigned priority, CDisplay& ds ) const
{
  char c;
  unsigned i, k;
  CDisplay ds2, ds3;
  OP_CODE op = Pop( pos );

  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();
  for( i = 0; i < st.GetSize(); i++ )
  {

    const CEquation& equ =  st[i]->m_Equation;
    if( equ.GetLastOperator() == op )
    {
      break;
    }
  }

  if( i == st.GetSize() ) // not found
  {
    pos ++;
    return pos;
  }

  const CString& s = st[i]->m_Syntax;

  for( k = s.GetLength(); k > 0; k-- )
  {
    ds2.Clear();
    c = s[k - 1];

    if( c >= 'a' && c <= 'h' )
    {
      pos = DisplayBranch( pos, i, ds2 );
    }
    else if ( c >= 'A' && c <= 'H' )
    {
      pos = DisplayBranch( pos, 0, ds2 );
    }
    else
    {
      ds2 += c ;
    }

    ds3.Prepend( ds2 );
  }

  if( i < priority )
  {
    ds += '(' ;
    ds += ds3 ;
    ds += ')' ;
  }
  else
  {
    ds += ds3 ;
  }

  return pos;
}

void CEquation::GetLevel( CParser& IC, unsigned priority )
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

/*
bool CEquation::GetAtom( CParser& IC )
{
  CValue v;
  const char* pos = v.GetFromString( IC.GetPos() );
  if( pos != IC.GetPos() )
  {
    IC.SetPos( pos );
    Push( v );
  }
  else if( IC.IsWord() )
  {
    ParseElement( IC );
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
}*/
bool CEquation::SearchOperator( CParser& IC, unsigned priority, bool symbol_first )
{
  unsigned i;
  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();

  for( i = priority; i < st.GetSize(); i++ )
  {
    const CString& s = st[i]->m_Syntax;
    const CEquation& equ =  st[i]->m_Equation;

    if( MatchOperator( IC, s, equ, i + 1, symbol_first  ) )
    {
      return true;
    }
  }

  return false;
}

bool CEquation::MatchOperator( CParser& IC, const CString& s, const CEquation& rule_equ, unsigned priority, bool symbol_first )
{
  unsigned index;
  const char* sp = s.GetBufferPtr();
  unsigned pos_array[ CElementDataBase::MAX_EXP ];
  OP_CODE elem_array[ CElementDataBase::MAX_EXP ];

  InitParameterLUT( elem_array );

  index = 0;
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

    sp = ParseExpression( sp, elem_array, pos_array, index++, priority, IC, false );

  }
  // try to match prefix operator

  while( *sp )
  {

    if( !CParser::IsWord( *sp ) )
    {
      if( !IC.TryMatchSymbol( sp ) ) break;
      /*j = CParser::TryMatchSymbol( sp, pos );
      if( j == 0 )
      {
        break;
      }
      sp += j;
      IC.SetPos( pos + j );*/
    }

    if( *sp == '\0' )
    {
      break;  // case [a]
    }

    sp = ParseExpression( sp, elem_array, pos_array, index++, priority, IC, true );

  }

  if( *sp == '\0' )
  {

    if( rule_equ.GetSize() )
    {
      CEquation equ( m_ElementDB );
      equ.Copy( *this );
      ApplyRule( equ, elem_array, pos_array, &rule_equ, false );

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

bool CEquation::ParseElement( CParser& IC )
{
  CElement* e;
  CFunction* f;
  bool element_creation;
  unsigned n;

  //Symbol match failed, try to use default syntax
  //ASSERT( IC.IsWord() );

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

void CEquation::GetFromTextRPN( CParser& IC )
{
  // see GetAtom
  CElement* e;
  Clear();
  while( !IC.TryFind( ';' ) )
  {
    e=m_ElementDB->ParseElement( IC );
    Push( e );
  }
}

const char* CEquation::ParseExpression( const char* sp, OP_CODE* elem_array, unsigned* pos_array, unsigned index, unsigned priority, CParser& IC, bool allow_recursion )
{
  char elem_id;

  if( *sp >= 'a' )
  {
    elem_id = *sp - 'a';
  }
  else
  {
    elem_id = *sp - 'A';
    priority = 0;
  }

  sp++;

  if( allow_recursion )
  {
    CEquation equ( m_ElementDB );
    equ.GetLevel( IC, priority );
    Push( equ );
  }

  elem_array[index] = CElementDataBase::OP_EXP1 + elem_id;
  pos_array[index]  = m_StackSize;

  return sp;
}