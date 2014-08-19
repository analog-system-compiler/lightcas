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
#include "Vector.h"
#include "Equation.h"
#include "Element.h"
#include "Function.h"

CEquation::CEquation( CElementDataBase* db )
{
  Initialize( db );
}

// Copy Constructor
CEquation::CEquation( const CEquation& equ )
{
  Initialize( equ.m_ElementDB );
  Copy( equ );
}

CEquation::~CEquation()
{
  if( m_StackArray )
  {
    free( m_StackArray );
  }
}

void CEquation::Initialize( CElementDataBase* db )
{
  m_StackArray = NULL;
  m_StackSize = 0;
  m_OldStackSize = 0;
  m_AllocSize = 0;
  m_ElementDB  = db;

}

void CEquation::PushEquation( const CEquation& equ )
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

void CEquation::Append( const OP_CODE* src, unsigned size )
{
  if( size != 0 )
  {
    SetSize( m_StackSize + size );
    memcpy( &m_StackArray[m_OldStackSize], src, size * sizeof( OP_CODE ) );
  }
}

void CEquation::SetSize( unsigned i )
{

  m_OldStackSize = m_StackSize;
  m_StackSize = i;

  if( i > m_AllocSize )
  {
    m_AllocSize = i + 16;
    m_StackArray = ( OP_CODE* )realloc( m_StackArray, m_AllocSize * sizeof( OP_CODE ) );
  }
}

bool  CEquation::Compare( const CEquation& equ ) const
{
  return ( equ.m_StackSize == m_StackSize ) && !memcmp( equ.m_StackArray, m_StackArray, m_StackSize* sizeof( OP_CODE ) );
}

void CEquation::Copy( const CEquation& equ )
{
  if( m_ElementDB == NULL )
  {
    m_ElementDB = equ.m_ElementDB;
  }

  ASSERT( equ.m_ElementDB );
  Clear();

  if( !equ.IsNull() ) // TODO : to remove
  {
    PushEquation( equ );
  }
}

void CEquation::PushBranch( const CEquation& equ, unsigned& pos )
{
  unsigned pos1 = pos;
  equ.NextBranch( pos );
  Append( &equ.m_StackArray[pos] , pos1 - pos );
}

void CEquation::NextBranch( unsigned& pos ) const
{
  unsigned i = 1;
  OP_CODE op;
  CElement* e;

  while( pos && i )
  {

    op = Pop( pos );
    e = RefToElement( op );
    if( e && e->IsFunct()  )
    {
      i += e->GetFunction()->GetParameterNb();
    }
    i--;
  }
}

void CEquation::PushElement( CValue& v )
{
  if( v.IsNegative() )
  {
    v.Negate();
    CElement* e =  m_ElementDB->GetElement( v );
    PushElement( e );
    Push( CElementDataBase::OP_NEG );
  }

  else
  {
    CElement* e = m_ElementDB->GetElement( v );
    PushElement( e );
  }
}

void CEquation::InitParameterLUT( OP_CODE elem_array[] )
{
  unsigned i;
  for( i = 0; i < CElementDataBase::MAX_EXP; i++ )
  {
    elem_array[ i ] = CElementDataBase::OP_NONE;
  }
}

unsigned CEquation::MatchParameter( const OP_CODE elem_array[], OP_CODE op )
{
  unsigned i;
  OP_CODE op1;
  for( i = 0; i < CElementDataBase::MAX_EXP; i++ )
  {
    op1 = elem_array[i];
    if( ( op1 == op ) || ( op1 == CElementDataBase::OP_NONE ) )
    {
      break;
    }
  }
  return i;
}

#ifdef FUNCT_EXT

void CEquation::BinaryOperation( OP_CODE op, const CEquation& equ )
{
  ASSERT( RefToElement( op )->IsBinary() );

  AddZero();
  PushEquation( equ );
  Push( op );
  OptimizeTree();
  RemoveZero();

}

void CEquation::BinaryOperation( OP_CODE op, const CElement* e1 )
{

  ASSERT( e1 );
  ASSERT( RefToElement( op )->IsBinary() );

  AddZero();
  PushElement( e1 );
  Push( op );
  OptimizeTree();
  RemoveZero();

}

void CEquation::UnaryOperation( OP_CODE op )
{
  ASSERT( RefToElement( op )->IsUnary() );
  AddZero();
  Push( op );
  OptimizeTree();
  RemoveZero();
}

void CEquation::VoidOperation( OP_CODE op )
{
  Push( op );
  RemoveZero();
}

void CEquation::AddZero()
{
  if( IsNull() )
  {
    Push( CElementDataBase::OP_ZERO );
  }
}

void CEquation::RemoveZero()
{
  if( GetLastOperator() == CElementDataBase::OP_ZERO && ( m_StackSize == 1 ) )
  {
    Clear();
  }
}

#endif