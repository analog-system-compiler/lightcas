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

CPosArray CMathExpression::m_PosArray;

//#define OPTIMIZE_PROCESSING

void CMathExpression::OptimizeTree()
{
  pos_t pos;
  unsigned pos_index;
  CAlgebraRule* rule;

  if ( ExecuteCommand() )
  {
    pos_index = m_PosArray.GetSize();
    rule = RuleSearch( pos, pos_index );
    if ( rule )
    {
#ifdef _TEST
      rule->m_AccessNb++;
#endif
#if( DEBUG_LEVEL >= 1 )
      CDisplay ds;
      ds += "OptimizeTree rule";
      //        ds += CString( rule->m_LineNo );
      ds += " [";
      e->Display( ds );
      ds += "] ";
      rule->Display( i, ds );
      ds += " : ";
      Display( ds );
      ds += " => ";
#endif
      if ( rule->m_bHasRule )
      {
#if 1
        ApplyRule( pos, pos_index, rule->m_DstEquation );
#else
        unsigned pos2 = m_StackSize;
        ApplyRule( *this, pos_array, &rule->m_DstEquation );
        memcpy( m_StackArray + pos, m_StackArray + pos2, ( m_StackSize - pos2 ) * sizeof( OP_CODE ) );
        SetSize( pos + m_StackSize - pos2 );
#endif
      }

#if( DEBUG_LEVEL >= 1 )
      Display( ds );
      TRACE( ds.GetBufferPtr() );
#endif
    }
    m_PosArray.SetSize( pos_index );
  }
}

CAlgebraRule* CMathExpression::RuleSearch( pos_t& pos, unsigned pos_index )
{
  unsigned i;
  unsigned n;

  OP_CODE op = GetLastOperator();
  const CElement* e = RefToElement( op );
  ASSERT( e );
  const CFunction* funct = e->GetFunction();

  if( funct )
  {
    n = funct->m_AlgebraRuleArray.GetSize();
    for( i = 0; i < n; i++ )
    {
      CAlgebraRule* rule = funct->m_AlgebraRuleArray[ i ];
      const CMathExpression* equ = &( rule->m_SrcEquation );

      pos = Match( GetSize(), *equ, pos_index );
      if( pos != GetSize() )
      {
        return rule;
      }
    }
  }
  return NULL;
}

bool CMathExpression::Match( const CMathExpression& equ ) const
{
  return Match( GetSize(), equ, m_PosArray.GetSize() ) != GetSize();
}

pos_t CMathExpression::Match( pos_t pos3, const CMathExpression& equ, unsigned pos_index ) const
{
  unsigned i, n;
  OP_CODE op1, op2, op3;
  bool match = true;
  pos_t pos1 = equ.GetSize();
  pos_t pos2 = pos3;

  //Clear position table
  n = pos_index + CElementDataBase::MAX_EXP;
  m_PosArray.SetSize( n );
  for ( i = pos_index; i < n; i++ )
  {
    m_PosArray[i] = 0;
  }

  //Try to match pattern
  while( match && pos1 )
  {
    op1 = equ.Pop( pos1 );

    if( ( op1 >= CElementDataBase::OP_CONST ) && ( op1 <= CElementDataBase::OP_FUNCT2 ) )
    {
      op3 = equ.Pop( pos1 );
      ASSERT( CElementDataBase::IsReservedOP( op3 ) );
      match = RegisterBranch( pos_index,  op3,  pos2 );
      if( match )
      {
        op2 = Pop( pos2 );
        match = false;
        if ( !CElementDataBase::IsReservedOP( op2 ) )
        {
          CElement* e = RefToElement( op2 );
          int n = ( int )e->GetFunction()->GetParameterNb();
          if ( n == 0 )
          {
            if( e->IsConst() )
            {
              match = ( op1 == CElementDataBase::OP_CONST );
            }
            else
            {
              match = ( op1 == CElementDataBase::OP_ELEM );
            }
          }
          else
          {
            match = ( ( op1 - CElementDataBase::OP_FUNCT0 ) == n );
          }
        }
      }
    }
    else if( CElementDataBase::IsReservedOP( op1 ) ) //a,b or c
    {
      match = RegisterBranch( pos_index,  op1,  pos2 );
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

#ifdef OPTIMIZE_PROCESSING
void CMathExpression::ApplyRule( pos_t pos4, pos_t const pos_array[], const CMathExpression& rule_equ, bool optimize )
{
  OP_CODE op3;
  unsigned pos, pos3;
  unsigned j;

#if ( DEBUG_LEVEL >= 2 )
  CDisplay ds;
  ds += "Apply rule " ;
  rule_equ.Display( ds );
  ds += " : ";
  equ.Display( ds );
  ds += " => " ;
#endif

  ASSERT( m_StackSize > pos4 );
  pos3 = GetSize();
  ASSERT( pos_array[0] <= pos3 );
  ASSERT( pos_array[1] <= pos3 );
  ASSERT( pos_array[2] <= pos3 );
  ASSERT( pos_array[3] <= pos3 );

  for( pos = 0; pos < rule_equ.GetSize(); pos++ )
  {

    op3 = rule_equ.Get( pos );

    if( IsReserved( op3 ) )
    {
      j = ReservedParameterIndex( op3 );
      ASSERT( j < CElementDataBase::MAX_EXP );
      unsigned pos2 = pos_array[ j ];
      ASSERT( pos2 <= GetSize() );
      unsigned pos1 = pos2;
      NextBranch( pos1 );
      unsigned pos3 = GetSize();
      InnerCopy( pos3, pos1, pos2 - pos1 );
    }
    else
    {
      ASSERT( RefToElement( op3 ) != NULL );
      Push( op3 );
      if( optimize )
      {
        OptimizeTree();  // optimization is only made on the top branch
      }
    }
  }

  unsigned pos2 = GetSize();
  InnerCopy( pos4, pos3, pos2 - pos3 );

#if ( DEBUG_LEVEL >= 2 )
  Display( ds );
  TRACE( ds.GetBufferPtr() );
#endif

}
#else
void CMathExpression::ApplyRule( pos_t pos2, unsigned pos_index, const CMathExpression& rule_equ, bool optimize )
{
  CMathExpression equ( m_ElementDB );
  OP_CODE op3;
  pos_t pos;
  unsigned j;

#if( DEBUG_LEVEL >= 2 )
  CDisplay ds;
  ds += "Apply rule ";
  rule_equ.Display( ds );
  ds += " : ";
  equ.Display( ds );
  ds += " => ";
#endif

  equ.SetSize( GetSize() );
  equ.Clear();

  unsigned n = rule_equ.GetSize();
  for ( pos = 0; pos < n; pos++ )
  {

    op3 = rule_equ.Get( pos );

    if ( CElementDataBase::IsReservedOP( op3 ) )
    {
      j = ReservedParameterIndex( op3 );
      pos_t pos2 = m_PosArray[pos_index + j];
      ASSERT( pos2 <= GetSize() );
      unsigned pos3 = pos + 1;
      if ( pos3 < n ) // if followed by FUNCT1 or FUNCT2
      {
        op3 = rule_equ.Get( pos3 );
        if ( CElementDataBase::IsFunctionOP( op3 ) )
        {
          pos = pos3;
          op3 = Get( m_PosArray[pos_index + j] - 1 );
          goto L1;
        }
      }
      equ.PushBranch( *this, pos2 );  // WARNING pos2 is modified
    }
    else
    {
L1:
      ASSERT( RefToElement( op3 ) != NULL );
      equ.Push( op3 );
      if ( optimize )
      {
        equ.OptimizeTree();  // optimization is only made on the top branch
      }
    }
  }

  SetSize( pos2 );
  Push( equ );

#if( DEBUG_LEVEL >= 2 )
  Display( ds );
  TRACE( ds.GetBufferPtr() );
#endif

}
#endif

bool CMathExpression::ExecuteCommand()
{
  OP_CODE op3 = Pop( m_StackSize );

#if( DEBUG_LEVEL >= 1 )
  CDisplay ds;
  if ( ( op3 == CElementDataBase::OP_SET ) || ( op3 == CElementDataBase::OP_GET ) || ( op3 == CElementDataBase::OP_RANK ) || ( op3 == CElementDataBase::OP_EVAL ) )
  {
    ds += "Execute Command ";
    RefToElement( op3 )->Display( ds );
    ds += " : ";
    Display( ds );
    ds += " => ";

  }
#endif

  if( op3 == CElementDataBase::OP_SET )
  {
    CMathExpression equ_dst( m_ElementDB );
    CMathExpression equ_src( m_ElementDB );
    equ_dst.PushBranch( *this, m_StackSize );
    pos_t pos = m_StackSize;
    equ_src.PushBranch( *this, pos );
    OP_CODE op1 = equ_src.GetLastOperator();
    CElement* e = RefToElement( op1 );
    e->AddFunction( equ_src, equ_dst );
    return false;
  }

  if ( op3 == CElementDataBase::OP_GET )
  {
    return true;
  }

  if ( op3 == CElementDataBase::OP_RANK )
  {
    OP_CODE op1 = Pop( m_StackSize );
    OP_CODE op2 = Pop( m_StackSize );
    Push( op1 > op2 ? op1 : op2 );
    return false;
  }

  if ( op3 == CElementDataBase::OP_EVAL )
  {
    CEvaluator* eval = m_ElementDB->GetEvaluator();
    if ( eval )
    {
      pos_t pos = m_StackSize;
      NextBranch( pos );
      eval->Evaluate( m_StackSize - pos, m_StackArray + pos );
      m_StackSize = pos;
      PushEvalElement( *eval );
    }
    return false;
  }

  m_StackSize++;

#if( DEBUG_LEVEL >= 1 )
  if ( ( op3 == CElementDataBase::OP_SET ) || ( op3 == CElementDataBase::OP_GET ) || ( op3 == CElementDataBase::OP_RANK ) || ( op3 == CElementDataBase::OP_EVAL ) )
  {
    Display( ds );
    TRACE( ds.GetBufferPtr() );
  }
#endif
  return true;
}

bool CMathExpression::CompareBranchElement( pos_t pos1, pos_t pos2 ) const
{
  OP_CODE op3, op4;
  const CElement* e;

  op4 = Pop( pos2 );
  e = RefToElement( op4 );

  if ( e && e->IsVoid() )
  {
    op3 = Pop( pos1 );
    return ( op3 == op4 );
  }

  return false;
}

bool CMathExpression::CompareBranch( pos_t pos1, pos_t pos2 ) const
{
  pos_t pos11, pos22;

  if ( CompareBranchElement( pos1, pos2 ) )
  {
    return true;
  }

  pos11 = pos1;
  pos22 = pos2;
  NextBranch( pos11 );
  NextBranch( pos22 );

  return ( ( pos1 - pos11 ) == ( pos2 - pos22 ) ) && !memcmp( &m_StackArray[pos11], &m_StackArray[pos22], ( pos2 - pos22 ) * sizeof( OP_CODE ) );

}

bool CMathExpression::RegisterBranch( unsigned pos_index, OP_CODE op1, pos_t pos2 ) const
{
  unsigned i, j;
  bool match;

  j = ReservedParameterIndex( op1 );
  ASSERT( j < CElementDataBase::MAX_EXP );
  i = pos_index + j;

  if( m_PosArray[ i ] == 0 )
  {
    m_PosArray[ i ] = pos2;
    ASSERT( pos2 != 0 );
    match = true;
  }
  else
  {
    match = CompareBranch( pos2, m_PosArray[ i ] );
  }

  return match;
}

void CMathExpression::PushEvalElement( CEvaluator& eval )
{
  bool minus;
  CString s;
  minus = eval.Display( s );
  CElement* e = m_ElementDB->GetElement( s );
  eval.SetElementValue( e->ToRef() );
  e->SetConst();
  Push( e );
  if ( minus )
  {
    Push( CElementDataBase::OP_NEG );
  }
}