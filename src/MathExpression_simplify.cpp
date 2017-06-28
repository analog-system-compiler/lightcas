/*******************************************************************************/
/*  Copyright (C) 2014 The CASLight project                                      */
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

#include <cstring>
#include "Debug.h"
#include "LCVector.h"
#include "MathExpression.h"
#include "Element.h"
#include "Function.h"

//#define DEBUG_OPTIMIZE

bool CMathExpression::OptimizeConst()
{
  unsigned pos, i, n;
  CElement* e3;

  pos = m_StackSize;
  if( pos == 0 )
  {
    return true;
  }

  e3 = RefToElement( Pop( pos ) );

  if( e3->IsNumeric() && !e3->IsVoid() )
  {
    n = e3->GetFunction()->GetParameterNb();
    for( i = 0; i < n; i++ )
    {
      if( !RefToElement( Pop( pos ) )->IsConst() )
      {
        return false;
      }
    }

    CEvaluator* eval = m_ElementDB->GetEvaluator();
    if ( eval )
    {

#ifdef DEBUG_OPTIMIZE
      CDisplay ds;
      ds += "OptimizeConst :" ;
      Display( ds );
      ds += " => ";
#endif

      eval->Evaluate( m_StackSize - pos, m_StackArray + pos );
      m_StackSize = pos;
      PushEvalElement();

#ifdef DEBUG_OPTIMIZE
      Display( ds );
      TRACE( ds.GetBufferPtr() );
#endif

    }

    return true; // even if no evaluator

  }

  return false;
}

void CMathExpression::OptimizeTree()
{
  OptimizeTree3();
}

void CMathExpression::OptimizeTree3()
{

  if( !ExecuteCommand() )
  {
    if( !OptimizeTree2( /*bImpure*/ ) )
    {
      OptimizeConst();
    }
  }
}

bool CMathExpression::OptimizeTree2( /*bool& bImpure*/ )
{
  unsigned pos;
  unsigned i;
  unsigned n;
  bool match;
  unsigned pos_array[ CElementDataBase::MAX_EXP ];

  OP_CODE op = GetLastOperator();
  CElement* e = RefToElement( op );
  CFunction* funct = e->GetFunction();

  if( funct )
  {
    n = funct->m_AlgebraRuleArray.GetSize();
    for( i = 0; i < n; i++ )
    {
      CAlgebraRule* rule = funct->m_AlgebraRuleArray[ i ];
      CMathExpression* equ = &( rule->m_SrcEquation );

      pos = Match( GetSize(), *equ, pos_array );
      match = ( pos != GetSize() );

      if( match )
      {
#ifdef _DEBUG
        rule->m_AccessNb++;
#endif
#ifdef DEBUG_OPTIMIZE
        CDisplay ds;
        ds += "OptimizeTree rule" ;
        //        ds += CString( rule->m_LineNo );
        ds += " [";
        e->Display( ds );
        ds += "] ";
        rule->Display( i, ds );
        ds += " : ";
        Display( ds );
        ds += " => ";
#endif
        if( rule->m_bHasRule )
        {
#if 1
          ApplyRuleWrapper( pos, pos_array, rule->m_DstEquation );
#else
          unsigned pos2 = m_StackSize;
          ApplyRule( *this, pos_array, &rule->m_DstEquation );
          memcpy( m_StackArray + pos, m_StackArray + pos2, ( m_StackSize - pos2 ) * sizeof( OP_CODE ) );
          SetSize( pos + m_StackSize - pos2 );
#endif
          if( m_StackSize >= MAX_STACK_SIZE )
          {
            Init( CElementDataBase::OP_STACKERR );
            ASSERT( false );
          }
        }

#ifdef DEBUG_OPTIMIZE
        Display( ds );
        TRACE( ds.GetBufferPtr() );
#endif
        return true;
      }
    }
  }
  return false;
}

bool CMathExpression::Match( const CMathExpression& equ ) const
{
  unsigned pos_array[ CElementDataBase::MAX_EXP ];
  return Match( GetSize(), equ, pos_array ) != GetSize();
}

unsigned CMathExpression::Match( unsigned pos3, const CMathExpression& equ, unsigned pos_array[] ) const
{
  OP_CODE op1, op2, op3;
  bool match = true;
  unsigned pos1 = equ.GetSize();
  unsigned pos2 = pos3;

  InitPositionTable( pos_array );

  while( match && pos1 )
  {
    op1 = equ.Pop( pos1 );

    if( op1 == CElementDataBase::OP_CONST || op1 == CElementDataBase::OP_ELEM )
    {
      op3 = op1;
      op1 = equ.Pop( pos1 );
      ASSERT( IsReserved( op1 ) );

      match = MatchBranch(  pos_array,  op1,  pos2 );
      if( match )
      {
        op2 = Pop( pos2 );
        if( IsReserved( op2 ) )
        {
          match = false;
        }
        else if( op3 == CElementDataBase::OP_ELEM  )
        {
          match = RefToElement( op2 )->IsVoid() && !RefToElement( op2 )->IsConst();
        }
        else if( op3 == CElementDataBase::OP_CONST )
        {
          match = RefToElement( op2 )->IsConst();
        }
      }
    }
    else if( IsReserved( op1 ) ) //a,b or c
    {
      match = MatchBranch(  pos_array,  op1,  pos2 );
      if( match )
      {
        NextBranch( pos2 );
      }
    }
    else
    {
      op2 = Pop( pos2 );
      match = ( op2 == op1 );
    }
  }

  return match ? pos2 : pos3;
}

void CMathExpression::ApplyRuleWrapper( unsigned pos, unsigned const pos_array[], const CMathExpression& rule_equ, bool optimize )
{
#if 1
  //CMathExpression* equ2 = m_ElementDB->GetMathExprBuffer();
#else
  CMathExpression equ3( m_ElementDB );
  CMathExpression* equ2 = &equ3;
#endif
  ASSERT( m_StackSize > pos );
  unsigned pos3 = GetSize();
  ASSERT( pos_array[0] <= pos3 );
  ASSERT( pos_array[1] <= pos3 );
  ASSERT( pos_array[2] <= pos3 );
  ASSERT( pos_array[3] <= pos3 );
  ApplyRule( *this, pos_array, rule_equ, optimize );
  unsigned pos2 = GetSize();
  //Append( &m_StackArray[pos3], pos2 - pos3 );
  //memcpy(&m_StackArray[old_size], src, size * sizeof(OP_CODE));
  SetSize( pos + pos2 - pos3 );
  memmove( &m_StackArray[pos], &m_StackArray[pos3], ( pos2 - pos3 ) * sizeof( OP_CODE ) );
}

void CMathExpression::ApplyRule( const CMathExpression& equ, unsigned const pos_array[], const CMathExpression& rule_equ, bool optimize )
{
  OP_CODE op3;
  unsigned pos;
  unsigned j;

#ifdef DEBUG_OPTIMIZE
  CDisplay ds;
  ds += "Apply rule " ;
  rule_equ.Display( ds );
  ds += " : ";
  equ.Display( ds );
  ds += " => " ;
#endif

  for( pos = 0; pos < rule_equ.GetSize(); pos++ )
  {

    op3 = rule_equ.Get( pos );

    if( IsReserved( op3 ) )
    {
      j = ReservedParameterIndex( op3 );
      ASSERT( j < CElementDataBase::MAX_EXP );
      unsigned pos2 = pos_array[ j ];
      ASSERT( pos2 <= equ.m_StackSize );
      //PushBranch( equ, pos2 ); // WARNING pos2 is modified
      unsigned pos1 = pos2;
      equ.NextBranch( pos1 );
      unsigned pos3 = GetSize();
      SetSize( pos3 + pos2 - pos1 ); //force realoc when &equ == this;
      memmove( &m_StackArray[pos3], &m_StackArray[pos1], ( pos2 - pos1 ) * sizeof( OP_CODE ) );
    }
    else
    {
      ASSERT( RefToElement( op3 ) != NULL );
      Push( op3 );
      //ASSERT( op3 != m_ElementDB->GetElement( "SIN" )->ToRef() );
      if( optimize )
      {
        OptimizeTree3();  // optimization is only made on the top branch
      }
    }
    //ASSERT( GetLastOperator() != m_ElementDB->GetElement( "SIN" )->ToRef() );
  }

#ifdef DEBUG_OPTIMIZE
  Display( ds );
  TRACE( ds.GetBufferPtr() );
#endif

}

bool CMathExpression::ExecuteCommand()
{
  CElement* e;
  bool bOK = false;

  OP_CODE op3 = Pop( m_StackSize );

#ifdef DEBUG_OPTIMIZE
  CDisplay ds;
  if ( ( op3 == CElementDataBase::OP_SET ) || ( op3 == CElementDataBase::OP_GET ) || ( op3 == CElementDataBase::OP_RANK ) || ( op3 == CElementDataBase::OP_SUBST ) )
  {
    ds += "Execute Command ";
    Display( ds );
    ds += " => ";
  }
#endif

  switch( op3 )
  {
  case CElementDataBase::OP_SET:
  {
    CMathExpression equ_dst( m_ElementDB );
    CMathExpression equ_src( m_ElementDB );

    equ_dst.PushBranch( *this, m_StackSize );
    unsigned pos = m_StackSize;
    equ_src.PushBranch( *this, pos );
    OP_CODE op1 = equ_src.GetLastOperator();
    e = RefToElement( op1 );
    e->AddFunction( equ_src, equ_dst );
    //Copy( *this );
    bOK = true;
  }
  break;

  case CElementDataBase::OP_GET:
  {
    bOK = false; //force a call to OptimizeTree2
  }
  break;

  case CElementDataBase::OP_RANK:
  {
    OP_CODE op1 = Pop( m_StackSize );
    OP_CODE op2 = Pop( m_StackSize );
    Push( op1 > op2 ? op1 : op2 );
    bOK = true;
  }
  break;

  case CElementDataBase::OP_SUBST:
  {
    OP_CODE opd = Pop( m_StackSize );
    OP_CODE ops = Pop( m_StackSize );
    unsigned pos = m_StackSize;
    NextBranch( pos );
    Replace( ops, opd, pos );
    bOK = false; //force a call to OptimizeTree2
  }
  break;

  /* case CElementDataBase::OP_BASE:
   {
     CEvaluator* eval = m_ElementDB->GetEvaluator();
     OP_CODE op1 = Pop( m_StackSize );
     OP_CODE op2 = Pop( m_StackSize );
     CElement* e = RefToElement( op2 );
     CValue v2;
     v2.GetFromString( e->GetName().GetBufferPtr(), ( unsigned )eval->GetElementValue( op1 ).GetValue() );
     Push( m_ElementDB->GetElement( v2 ) );
   }
   break;*/

  default:
    m_StackSize++;

  }

#ifdef DEBUG_OPTIMIZE
  if ( ( op3 == CElementDataBase::OP_SET ) || ( op3 == CElementDataBase::OP_GET ) || ( op3 == CElementDataBase::OP_RANK ) || ( op3 == CElementDataBase::OP_SUBST ) )
  {
    Display( ds );
    TRACE( ds.GetBufferPtr() );
  }
#endif

  return bOK;
}

OP_CODE CMathExpression::GetLastOperator() const
{
  OP_CODE op1;

  if( IsEmpty() )
  {
    op1 = CElementDataBase::OP_ZERO;
  }
  else
  {
    op1 = Get( m_StackSize - 1 );
  }

  return op1;
}

bool CMathExpression::CompareBranch( unsigned pos1, unsigned pos2 ) const
{
  OP_CODE op3, op4;
  unsigned pos11, pos22;
  CElement* e;

  op4 = Pop( pos2 );
  e = RefToElement( op4 );

  if( e && e->IsVar() )
  {
    op3 = Pop( pos1 );
    return ( op3 == op4 );
  }

  pos2++;
  pos11 = pos1;
  pos22 = pos2;
  NextBranch( pos11 );
  NextBranch( pos22 );

  return ( ( pos1 - pos11 ) == ( pos2 - pos22 ) ) && !memcmp( &m_StackArray[pos11], &m_StackArray[pos22], ( pos2 - pos22 ) * sizeof( OP_CODE ) );

}

bool CMathExpression::MatchBranch( unsigned pos_array[], OP_CODE op1, unsigned pos2 ) const
{
  unsigned j;
  bool match;

  j = ReservedParameterIndex( op1 );
  ASSERT( j < CElementDataBase::MAX_EXP );

  if( pos_array[ j ] == 0 )
  {
    pos_array[ j ] = pos2;
    ASSERT( pos2 != 0 );
    match = true;
  }
  else
  {
    match = CompareBranch( pos2, pos_array[ j ] );
  }

  return match;
}

void CMathExpression::PushEvalElement()
{
  bool minus;
  CString s;
  CEvaluator* eval = m_ElementDB->GetEvaluator();
  minus = eval->Display( s );
  CElement* e = m_ElementDB->GetElement( s );
  eval->SetElementValue( e->ToRef() );
  e->SetConst();
  Push( e );
  if ( minus )
  {
    Push( CElementDataBase::OP_NEG );
  }
}