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
#include "Equation.h"
#include "Element.h"
#include "Function.h"
#define DEBUG_OPTIMIZE

bool CEquation::OptimizeConst()
{
  unsigned pos;
  bool bOk = false;
  CElement* e3, *e1, *e2;

#ifdef DEBUG_OPTIMIZE
  CDisplay ds;
  ds += "OptimizeConst :" ;
  Display( ds );
  ds += " => ";
#endif

  pos = m_StackSize;
  if( !pos )
  {
    return true;
  }

  e3 = RefToElement( Pop( pos ) );

  if( e3->IsNumeric() && !e3->IsVoid() )
  {
    e1 = RefToElement( Pop( pos ) );
    if( e1->IsConst() )
    {
      if( e3->IsBinary()  )
      {
        e2 = RefToElement( Pop( pos ) );
        if( e2->IsConst() )
        {
          bOk = true;
        }
      }
      else if( e3->IsUnary()  )
      {
        bOk = true; //( e3->ToRef() == CElementDataBase::OP_NEG ) ? false : true;
      }
    }
  }


  if( bOk )
  {
    const CValue& v = Evaluate( pos, GetElementDB()->GetEvaluator() );
    m_StackSize = pos;
    Push( v );

#ifdef DEBUG_OPTIMIZE
    Display( ds );
    TRACE( ds.GetBufferPtr() );
#endif
  }

  return bOk;
}

void CEquation::OptimizeTree() //( OP_CODE op, const CEquation* equR )
{
  CEquation equ( m_ElementDB );
  OptimizeTree( equ );
}

void CEquation::OptimizeTree( CEquation& equ ) //( OP_CODE op, const CEquation* equR )
{
  if( !OptimizeConst() )
  {
    OptimizeTree2( equ );
  }
}

void CEquation::OptimizeTree2( CEquation& equ2 ) //( OP_CODE op, const CEquation* equR )
{

  unsigned pos;
  unsigned i;
  unsigned n;
  bool match;
  unsigned pos_array[ CElementDataBase::MAX_EXP ];
  OP_CODE elem_array[ CElementDataBase::MAX_EXP ];

  OP_CODE op = GetLastOperator();
  CElement *e = RefToElement( op );
  CFunction* funct = e->GetFunction();

  if( funct )
  {
    n   = funct->m_AlgebraRuleArray.GetSize();
    for( i = 0; i < n; i++ )
    {

      CAlgebraRule* rule = funct->m_AlgebraRuleArray[ i ];
      CEquation* equ = &( rule->m_SrcEquation );

      pos = Match( equ, elem_array, pos_array );
      match = ( pos != m_StackSize );

      if( match )
      {

#ifdef DEBUG_OPTIMIZE
        CDisplay ds;
        ds += "OptimizeTree rule #" ;
        //        ds += CString( rule->m_LineNo );
        e->Display(ds);
        ds += " : ";
        rule->m_SrcEquation.Display( ds );
        ds += " => ";
        rule->m_DstEquation.Display( ds );
        ds += " : ";
        Display( ds );
        ds += " => ";
#endif

        if( rule->m_bHasRule )
        {
          //CEquation equ2( m_ElementDB );
          equ2.ApplyRule( *this, elem_array, pos_array, &rule->m_DstEquation );
          m_StackSize = pos;
          Push( equ2 );
          /*if( m_StackSize >= MAX_STACK_SIZE )
          {
            Init( RefToElement( CElementDataBase::OP_ERROR_SIZE ) );
          }*/
        }

#ifdef DEBUG_OPTIMIZE
        Display( ds );
        TRACE( ds.GetBufferPtr() );
#endif
        return;
      }
    }
  }
}

bool CEquation::Match( const CEquation& equ ) const
{
  unsigned pos_array[ CElementDataBase::MAX_EXP ];
  OP_CODE elem_array[ CElementDataBase::MAX_EXP ];
  return Match( &equ, elem_array, pos_array ) != m_StackSize;
}

unsigned CEquation::Match( const CEquation* equ, OP_CODE elem_array[], unsigned pos_array[] ) const
{
  OP_CODE op1, op2, op3;
  bool match = true;
  CElement* e;
  unsigned pos1 = equ->GetSize();
  unsigned pos2 = GetSize();

  op1 = equ->Pop( pos1 ); // to trash
  op2 = Pop( pos2 ); // to trash
  ASSERT( op1 == op2 );

  InitParameterLUT( elem_array );

  while( match && pos1 )
  {
    op1 = equ->Pop( pos1 );

    if( op1 == CElementDataBase::OP_CONST || op1 == CElementDataBase::OP_ELEM /*|| op1 == CElementDataBase::OP_UNARY || op1 == CElementDataBase::OP_BINARY*/ )
    {
      op3 = op1;
      op1 = equ->Pop( pos1 );
      ASSERT( IsReserved( op1 ) );

      match = MatchBranch(  elem_array,  pos_array,  op1,  pos2 );
      if( match )
      {

        op2 = Pop( pos2 );
        if( IsReserved( op2 ) )
        {
          match = false;
        }
        else
        {
          e = RefToElement( op2 );

          if( op3 == CElementDataBase::OP_ELEM  )
          {
            match = e->IsVar();
          }
          else
          {
            match = e->IsConst();
          }
        }
      }
    }
    else if( IsReserved( op1 ) ) //a,b or c
    {

      match = MatchBranch(  elem_array,  pos_array,  op1,  pos2 );
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

  return match ? pos2 : m_StackSize;
}

void CEquation::ApplyRule( const CEquation& equ, OP_CODE const elem_array[], unsigned const pos_array[], const CEquation* rule_equ, bool optimize )
{
  OP_CODE op3;
  unsigned pos = 0;
  unsigned j;
  CElement* e;
  CEquation equ2( m_ElementDB );
  //TODO : suppress elem_array and access pos_array by pos_array[ op3 - CElementDataBase::OP_EXP1 ]
  //TODO remove Clear() so that we don't need PushEquation( equ )

#if 0//def _DEBUG
  CDisplay ds;
  rule_equ->Display( ds );
  equ.Display( ds );
#endif

  Clear();

  while( pos < rule_equ->GetSize() ) // for( pos=0; pos<rule_equ->GetSize();  pos += sizeof( CElementDataBase::OP_CODE ) )
  {

    op3 = rule_equ->Get( pos );
    pos ++;

    if( IsReserved( op3 ) )
    {
      j = MatchParameter( elem_array, op3 );
      ASSERT( j < CElementDataBase::MAX_EXP );
      ASSERT( pos_array[ j ] != 0 );
      unsigned pos2 = pos_array[ j ];
      PushBranch( equ, pos2 ); // WARNING pos2 is modified
      ASSERT( m_StackSize );
    }
    else {
        bool bOk= ExecuteInternalCommand( op3, equ2 );   
        if( !bOk ) 
            bOk=ExecuteExternalCommand( op3, equ2 );
        if( !bOk ) 
            Push( op3 );
    }

    //TODO: put the code below just after PushOperator( op3 )
    if( optimize )
    {
      op3 = GetLastOperator();
      e = RefToElement( op3 );
      if( !e->IsVoid() )
      {
        OptimizeTree( equ2 ); // optimization is only made on the top branch
      }
      else if( e && e->IsFunct() )
      {
        OptimizeTree( equ2 ); // optimization is only made on the top branch
      }
    }
#if 0 //def _DEBUG
    ds.Clear();
    Display( ds );
    TRACE( ds.GetBufferPtr() );
#endif
  }
}

bool CEquation::ExecuteInternalCommand( OP_CODE op3, CEquation& equ )
{
  CElement* e;
  bool bOK=false;
  if( op3 == CElementDataBase::OP_SET )
  {
    equ.CopyBranch( *this, m_StackSize );
    op3 = GetLastOperator();
    e = RefToElement( op3 );
    e->AddFunction( *this, equ );
    Clear();
    Push( equ );
    bOK=true;
  }
  else if ( op3 == CElementDataBase::OP_RANK )
  {
    OP_CODE op1 = Pop( m_StackSize );
    OP_CODE op2 = Pop( m_StackSize );
    Push( op1 > op2 ? op1 : op2 );
    bOK=true;
  }
  else if ( op3 == CElementDataBase::OP_SUBST )
  {
    OP_CODE opd = Pop( m_StackSize );
    OP_CODE op1 = Pop( m_StackSize );
    ASSERT( op1 == CElementDataBase::OP_CONCAT );
    OP_CODE ops = Pop( m_StackSize );
    equ.CopyBranch( *this, m_StackSize );
    equ.Replace( ops, opd );
    Push( equ );
    bOK=true;
  } 
  return bOK;
}

OP_CODE CEquation::GetLastOperator() const
{
  // check redundancy with ReduceToElement()
  OP_CODE op1;

  if( IsNull() )
  {
    op1 = CElementDataBase::OP_ZERO;
  }
  else
  {
    op1 = Get( m_StackSize - 1 );
  }

  return op1;
}

bool CEquation::CompareBranch( unsigned pos1, unsigned pos2 ) const
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

bool CEquation::MatchBranch( OP_CODE elem_array[], unsigned pos_array[], OP_CODE op1, unsigned pos2 ) const
{
  unsigned j;
  bool match;

  j = MatchParameter( elem_array, op1 );
  if( elem_array[ j ] == CElementDataBase::OP_NONE )
  {
    elem_array[ j ] = op1;
    pos_array[ j ] = pos2;
    ASSERT( pos2 != 0 );
    match = true;
  }
  else
  {
    match = CompareBranch( pos2, pos_array[ j ] );
  }
  ASSERT( j < CElementDataBase::MAX_EXP );
  return match;
}
