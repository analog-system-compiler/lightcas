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
#include <cstring>
#include "Debug.h"
#include "LCVector.h"
#include "MathExpression.h"
#include "Element.h"
#include "Function.h"

CMathExpression::CMathExpression( CElementDataBase* db )
{
  m_StackArray   = NULL;
  m_ElementDB    = db;
  m_AllocSize    = 0;
  m_StackSize    = 0;
}

CMathExpression::~CMathExpression()
{
  if( m_StackArray )
  {
    free( m_StackArray );
  }
}

void CMathExpression::Initialize( CElementDataBase* db )
{
  Clear();
  m_ElementDB  = db;
}

void CMathExpression::Push( const CMathExpression& equ )
{
  if( equ.IsNull() )
  {
    Push( CElementDataBase::OP_ZERO );
  }
  else
  {
    Append( equ.m_StackArray, equ.m_StackSize );
  }
}

void CMathExpression::Append( const OP_CODE* src, unsigned size )
{
  if( size != 0 )
  {
    unsigned old_size = m_StackSize;
    SetSize( m_StackSize + size );
    memcpy( &m_StackArray[old_size], src, size * sizeof( OP_CODE ) );
  }
}

void CMathExpression::SetSize( unsigned i )
{
  m_StackSize = i;
  if( i > m_AllocSize )
  {
    m_AllocSize = i + 16;
    m_StackArray = ( OP_CODE* )realloc( m_StackArray, m_AllocSize * sizeof( OP_CODE ) );
  }
}

bool  CMathExpression::Compare( const CMathExpression& equ ) const
{
  return ( equ.m_StackSize == m_StackSize ) && !memcmp( equ.m_StackArray, m_StackArray, m_StackSize * sizeof( OP_CODE ) );
}

void CMathExpression::Copy( const CMathExpression& equ )
{
  if( m_ElementDB == NULL )
  {
    m_ElementDB = equ.m_ElementDB;
  }

  ASSERT( equ.m_ElementDB );
  Clear();

  if( !equ.IsNull() )
  {
    Push( equ );
  }
}

void CMathExpression::PushBranch( const CMathExpression& equ, unsigned& pos )
{
  unsigned pos1 = pos;
  ASSERT( pos );
  equ.NextBranch( pos );
  Append( &equ.m_StackArray[pos] , pos1 - pos );
}

void CMathExpression::NextBranch( unsigned& pos ) const
{
  unsigned i = 1;
  OP_CODE op;
  CElement* e;
  
  ASSERT(pos);

  while( pos && i )
  {
    op = Pop( pos );
    e = RefToElement( op );
    i += e->GetFunction()->GetParameterNb();
    i--;
  }
}

void CMathExpression::Push( const CValue& v )
{
  if( v.IsNegative() )
  {
    CValue v1 = v;
    v1.Negate();
    CElement* e =  m_ElementDB->GetElement( v1 );
    Push( e );
    Push( CElementDataBase::OP_NEG );
  }
  else
  {
    CElement* e = m_ElementDB->GetElement( v );
    Push( e );
  }
}

void CMathExpression::InitPositionTable( unsigned pos_array[] )
{
    memset( pos_array, 0, CElementDataBase::MAX_EXP*sizeof(unsigned) ) ;
}

void CMathExpression::BinaryOperation( OP_CODE op, const CMathExpression& equ )
{
  ASSERT( RefToElement( op )->IsBinary() );

  AddZero();
  Push( equ );
  Push( op );
  OptimizeTree();
  RemoveZero();
}
/*
void CMathExpression::BinaryOperation( OP_CODE op, const CElement* e1 )
{
  ASSERT( e1 );
  ASSERT( RefToElement( op )->IsBinary() );

  AddZero();
  Push( e1 );
  Push( op );
  OptimizeTree();
  RemoveZero();
}*/

void CMathExpression::UnaryOperation( OP_CODE op )
{
  ASSERT( RefToElement( op )->IsUnary() );
  AddZero();
  Push( op );
  OptimizeTree();
  RemoveZero();
}

void CMathExpression::VoidOperation( OP_CODE op )
{
  Push( op );
  RemoveZero();
}

void CMathExpression::AddZero()
{
  if( IsNull() )
  {
    Push( CElementDataBase::OP_ZERO );
  }
}

void CMathExpression::RemoveZero()
{
  if( GetLastOperator() == CElementDataBase::OP_ZERO && ( m_StackSize == 1 ) )
  {
    Clear();
  }
}

const CValue& CMathExpression::Evaluate() const
{
    return m_ElementDB->GetEvaluator()->Evaluate( m_StackSize, m_StackArray );
}