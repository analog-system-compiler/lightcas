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

int CMathExpression::Parse( CParser& IC )
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
  unsigned precedence;
  const char* char_pos;
  char c;
  bool var_found;
  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();

  if ( IC.IsStopChar() )
  {
    return false;
  }

  var_found = ParseAtom( IC );
  char_pos = IC.GetPos();

  precedence = var_found ? priority : 0;

  while ( precedence < st.GetSize() )
  {
    const char* sp = st[precedence]->m_Syntax;
    bool check_var = ( TryMatchExp( sp ) != '\0' );
    if ( check_var == var_found )
    {
      while ( *sp && IC.TryMatchSymbol( sp ) )
      {
        c = TryMatchExp( sp );
        if ( c )
        {
          if ( !GetLevel( IC, ( c < 'a' ) ? 0 : precedence + 1 ) )
          {
            return false;
          }
        }
      }
    }

    if ( *sp == '\0' )
    {
      Push( st[precedence]->m_Equation.GetLastOperator() );
      var_found = true;
      char_pos = IC.GetPos();
      precedence = priority;

#if ( DEBUG_LEVEL >= 3 )
      CDisplay ds;
      ds.Clear();
      Display( ds );
      TRACE( ds.GetBufferPtr() );
#endif
    }
    else
    {
      IC.SetPos( char_pos );
      precedence++;
    }
  }

  return var_found || ( precedence != st.GetSize() );
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

  if ( IC.TryFind( '(' ) )
  {
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
    if ( !e->IsFunct()  )
    {
      ASSERT( !e->IsFunct() );
      f->SetParameterNb( i );
      e->SetFunct();
#if ( DEBUG_LEVEL >= 0 )
      CDisplay ds;
      ds += "SetParamNb   : ";
      e->Display( ds );
      ds += ':';
      ds += CString( e->GetFunction()->GetParameterNb() );
      TRACE( ds.GetBufferPtr() );
#endif
    }
    if ( f->GetParameterNb() != i )
    {
      //ASSERT( false );
      return false;
    }
  }
  Push( e );
  return true;
}

char CMathExpression::TryMatchExp( const char*& sp )
{
  char c = *sp++;
  if ( c == '(' ) //Parenthesis not used in parsing stage
  {
    c = *sp++;
    if ( *sp == ')' ) //Parenthesis not used in parsing stage
    {
      sp++;
    }
  }

  if ( !CParser::IsWord( c ) )
  {
    c = '\0';
    sp--;
  }
  return c;
}



