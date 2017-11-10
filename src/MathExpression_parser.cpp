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

#define NEW_ALGO

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

#ifndef NEW_ALGO
bool CMathExpression::GetLevel( CParser& IC, unsigned priority )
{
  pos_t pos_array[CElementDataBase::MAX_EXP];
  bool symbol_first = true;

  if ( IC.IsStopChar() )
  {
    return false;
  }

  while ( !IC.IsStopChar() )
  {
    pos_t pos = GetSize();
    CMathExpression* rule_equ = SearchOperator( IC, pos_array, symbol_first ? 0 : priority, symbol_first );
    if ( rule_equ )
    {
      CMathExpression equ( m_ElementDB ); //save current stack
      if ( !symbol_first ) //FIXME
      {
        NextBranch( pos ); //remove first parameter
      }
      ASSERT( rule_equ->GetSize() );
      ApplyRule( pos, pos_array, *rule_equ );
#if ( DEBUG_LEVEL >= 3 )
      CDisplay ds;
      Display( ds );
      TRACE( ds.GetBufferPtr() );
#endif
    }
    else
    {
      if ( symbol_first )
      {
        if ( !ParseAtom( IC ) )
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

CMathExpression* CMathExpression::SearchOperator( CParser& IC, pos_t pos_array[CElementDataBase::MAX_EXP], unsigned priority, bool symbol_first )
{
  unsigned i;
  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();

  const char* init_pos = IC.GetPos();
  unsigned n = GetSize();

  for( i = priority; i < st.GetSize(); i++ )
  {
    const char* sp = st[i]->m_Syntax;
    if ( MatchOperator( IC, sp, pos_array, i + 1, symbol_first ) )
    {
      return &( st[i]->m_Equation );
    }
    IC.SetPos( init_pos );
    SetSize( n );
  }

  return NULL;
}

bool CMathExpression::MatchOperator( CParser& IC, const char* sp, pos_t pos_array[CElementDataBase::MAX_EXP], unsigned precedence, bool symbol_first )
{
  char c;

  c = *sp;
  if ( c == '(' || c == ')' ) //Parenthesis not used in parsing stage
  {
    sp++;
    c = *sp;
  }

  if ( CParser::IsWord( c ) == symbol_first )
  {
    return false;
  }

  if ( CParser::IsWord( c ) )
  {
    StoreStackPointer( c, pos_array );
    sp++;
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

      if ( !GetLevel( IC, precedence ) )  //Push parameters
      {
        return false;
      }
    }

    if ( CParser::IsWord( c ) )
    {
      StoreStackPointer( c, pos_array );
      sp++;
    }
    else if( !IC.TryMatchSymbol( sp ) )
    {
      return false;
    }
  }

  return true;
}
#endif

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

void CMathExpression::StoreStackPointer( char c, pos_t pos_array[CElementDataBase::MAX_EXP] )
{
  unsigned elem_id;
  elem_id = toupper( c ) - 'A';
  ASSERT( elem_id < CElementDataBase::MAX_EXP );
  pos_array[elem_id]  = m_StackSize;
}

#if 0
priority = 0;
while( !IC.IsStopChar() )
{
  if ( op = SearchPrefix( IC ) )
  {
  }
  else if ( ParseAtom( IC ) )
  {
    op = SearchPostFix( IC );
    if ( priority( op ) < priority )
    {
      Push( op );
      Push( stack );
      priority = priority( op );
    }
    else
    {
      stack.Push( op );
    }
  }
  else
  {
    break;
  }
}

CharPtrArray char_ptr_array;
n = GetSize();
for ( i = priority; i < st.GetSize(); i++ )
{
  const char* sp = st[i]->m_Syntax;
  while ( *sp )
  {
    if ( CParser::IsWord( *sp ) )
    {
      if ( IC.ParseWord() )
      {
        char_ptr_array.Push( IC.GetPos() );
        if ( IC.TryFind( '(' ) )
        {
          while ( IC.GetChar() != ')' )
          {
            IC.Next();
          }
        }
      }
      else
      {
        break;
      }
    }
    else if( !IC.TryMatchSymbol( sp ) )
    {
      break;
    }
  }
  sp++;
}
if ( !*sp ) //finded
{
  Push( st[i]->m_OpCode );
  break;
}
}

/**************/
double ParseAtom( char*& expr )
{
  // Read the number from string
  char* end_ptr;
  double res = strtod( expr, &end_ptr );
  // Advance the pointer and return the result
  expr = end_ptr;
  return res;
}

// Parse multiplication and division
double ParseFactors( char*& expr )
{
  double num1 = ParseAtom( expr );
  for ( ;; )
  {
    // Save the operation
    char op = *expr;
    if ( op != '/' && op != '*' )
    {
      return num1;
    }
    expr++;
    double num2 = ParseAtom( expr );
    // Perform the saved operation
    if ( op == '/' )
    {
      num1 /= num2;
    }
    else
    {
      num1 *= num2;
    }
  }
}

// Parse addition and subtraction
double ParseSummands( char*& expr )
{
  double num1 = ParseFactors( expr );
  for ( ;; )
  {
    char op = *expr;
    if ( op != '-' && op != '+' )
    {
      return num1;
    }
    expr++;
    double num2 = ParseFactors( expr );
    if ( op == '-' )
    {
      num1 -= num2;
    }
    else
    {
      num1 += num2;
    }
  }
  /********************/

}
#endif
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

bool CMathExpression::GetLevel( CParser& IC, unsigned priority )
{
  unsigned precedence, next_precedence;
  const char*  char_pos;
  char c;
  //pos_t pos_array[CElementDataBase::MAX_EXP];

  if ( IC.IsStopChar() )
  {
    return false;
  }

// ASSERT( IC.GetChar() != '~' );
  bool var_found = ParseAtom( IC );
  //Check for other symbols
  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();

  if ( !var_found ) { precedence = 0; } //symbol first
  else { precedence = priority; }
  while ( precedence < st.GetSize() )
  {
    const char* sp = st[precedence]->m_Syntax;

    char_pos = IC.GetPos();
    unsigned n = GetSize();

    bool check_var = ( TryMatchExp( sp ) != '\0' );
    if ( check_var == var_found )
    {
      while ( *sp != '\0' )
      {
        if ( IC.TryMatchSymbol( sp ) )
        {
          c = TryMatchExp( sp );
          if ( c )
          {
            if ( c < 'a' )
            {
              next_precedence = 0;
            }
            else
            {
              next_precedence = precedence + 1;
            }

            if ( !GetLevel( IC, next_precedence ) )
            {
              return false;
            }
            //StoreStackPointer( c, pos_array );
          }
        }
        else
        {
          ASSERT( GetSize() == n );
          SetSize( n );
          IC.SetPos( char_pos );
          break;
        }
      }

      /*else
      {
        precedence++;
      }*/
    }
    if ( *sp == '\0' )
    {
      //ApplyRule( GetSize(), pos_array, st[precedence]->m_Equation );
      Push( st[precedence]->m_Equation.GetLastOperator() );
#if ( DEBUG_LEVEL >= 3 )
      CDisplay ds;
      ds.Clear();
      Display( ds );
      TRACE( ds.GetBufferPtr() );
#endif
      var_found = true;
      //precedence = priority;
      //break;
    }
    if( *sp == '\0' )
    {
      precedence = priority;
    }
    else
    {
      precedence++;
    }
  }

  return var_found || ( precedence != st.GetSize() );
}

