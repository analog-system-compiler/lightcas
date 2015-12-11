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

#include "Evaluator.h"
#include <cstdlib>
#include <cmath>

CEvaluator::CEvaluator(  )
{
  m_ValPos = 0;
}

CEvaluator::~CEvaluator(  )
{
}

CValue CEvaluator::GetValueFromString( const char** pos ) const
{
  CValue v;
  *pos = v.GetFromString( *pos );
  return v;
}

const CValue& CEvaluator::Evaluate( unsigned size, const OP_CODE* p )
{
  unsigned pos, index;
  CEvaluatorFunct funct_ptr;

  AllocateStack( size );

  for( pos = 0; pos < size; pos ++ )
  {
    index = *p++;
    funct_ptr = NULL;

    if( index < m_FunctionArray.GetSize() )
    {
        funct_ptr = m_FunctionArray.GetAt( index );
    }

    if( funct_ptr )
    {
        ( *funct_ptr ) ( *this );
    }
    else
    {
        const CValue& v = GetElementValue( index );
        Push( v.GetValue() );
    }
  }

  return GetValue();
}

void CEvaluator::AllocateStack( unsigned size )
{
  m_ValPos = 0;
  m_ValueStack.SetSize( size );
}

void CEvaluator::SetElementValue( unsigned index, const CValue& v )
{
  m_ElementValueArray.CheckSize( index );
  m_ElementValueArray.SetAt( index, v );
}

const CValue& CEvaluator::GetElementValue( unsigned index ) const
{
  ASSERT( index < m_ElementValueArray.GetSize() );
  return m_ElementValueArray.GetAt( index );
}

void CEvaluator::SetFunction( unsigned index, const CEvaluatorFunct funct )
{
  m_FunctionArray.CheckSize( index );
  m_FunctionArray.SetAt( index, funct );
}

const CValue& CEvaluator::GetValue()
{

  if( m_ValPos == 0 )
  {
    m_Value = 0.;
  }
  else
  {
    m_Value = Pop();
  }

  ASSERT(m_ValPos==0);
  return m_Value;
}

void CEvaluator::GetMantAndExp( double v1, double v2, int& m1, int& m2, int& n )
{
  int n1, n2;
  double x1, x2;

  x1 = frexp(  v1 , &n1 );
  x2 = frexp(  v2 , &n2 );

  if( n2 > n1 )
  {
    x2 *= ( double )( 1 << 30 );
    x1 *= ( double )( 1 << ( 30 - ( n2 - n1 ) ) );
    n = n2 - 30;
  }

  else
  {
    x1 *= ( double )( 1 << 30 );
    x2 *= ( double )( 1 << ( 30 - ( n1 - n2 ) ) );
    n = n1 - 30;
  }

  m1 = ( int )x1;
  m2 = ( int )x2;

}

/***** void ****/
void CEvaluator::Rand( CEvaluator& eval )
{
  eval.Push( ( double )rand() / ( double )RAND_MAX ) ;
}

/***** Unitary ****/
void CEvaluator::Sin( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( sin(  v  ) );
}

void CEvaluator::Cos( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( cos(  v  ) );
}
void CEvaluator::Tan( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( tan(  v  ) );
}
void CEvaluator::Asin( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( asin(  v  ) );
}
void CEvaluator::Acos( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( acos(  v  ) );
}
void CEvaluator::Atan( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( atan(  v  ) );
}
void CEvaluator::SinH( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( sinh(  v  ) );
}
void CEvaluator::CosH( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( cosh(  v  ) );
}
void CEvaluator::TanH( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( tanh(  v  ) );
}
void CEvaluator::AsinH( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( log( v + sqrt( v * v + 1. ) ) );
}
void CEvaluator::AcosH( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( log( v + sqrt( ( v + 1. ) * ( v - 1. ) ) ) );
}
void CEvaluator::AtanH( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( 0.5 * log( ( 1 + v ) / ( 1 - v ) ) );
}
void CEvaluator::Exp( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( exp(  v  ) );
}
void CEvaluator::Ln( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( log(  v  ) );
}
void CEvaluator::Log( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( log10(  v  ) );
}
void CEvaluator::Sqrt( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( sqrt(  v  ) );
}
void CEvaluator::Floor( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( floor(  v  ) );
}
void CEvaluator::Ceil( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( ceil(  v  ) );
}
void CEvaluator::Abs( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( fabs(  v  ) );
}

void CEvaluator::Sqr( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( v * v );
}
void CEvaluator::Neg( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( -v );
}
void CEvaluator::Inv( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( 1. / v );
}
void CEvaluator::Id( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( v );
}
void CEvaluator::Bool( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( v ? 1. : 0. );
}
void CEvaluator::LNot( CEvaluator& eval )
{
  double v = eval.Pop();
  eval.Push( v ? 0. : 1. );
}
void CEvaluator::Fact( CEvaluator& eval )
{
  double v = eval.Pop();
  int a = 1;
  int b = ( int ) v ;

  for( int i = 1; i <= b ; i++ )
  {
    a *= i;
  }

  eval.Push( ( double )a );
}

void CEvaluator::Not( CEvaluator& eval )
{
  // int n, m;
  //  double x;
  double v = eval.Pop();
  /*x = frexp( v, &n ) * ( double )( 1 << 30 );
  m = ~( int )x;
  eval.Push(ldexp( ( double )m, n - 30 );*/
  eval.Push( -v - 1 );
}

/******* Binary *******/
void CEvaluator::Add( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( v1 + v2 );
}

void CEvaluator::Sub( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( v1 - v2 );
}

void CEvaluator::Mul( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( v1 * v2 );
}

void CEvaluator::Div( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  /*ASSERT(v2!=0.);*/
  eval.Push( v1 / v2 );
}

void CEvaluator::Lower( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( double )( v1 < v2 ) );
}

void CEvaluator::LowerOrEqual( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( double )( v1 <= v2 ) );
}

void CEvaluator::Greater( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( double )( v1 > v2 ) );
}

void CEvaluator::GreaterOrEqual( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( double )( v1 >= v2 ) );
}

void CEvaluator::Equal( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( double )( v1 == v2 ) );
}

void CEvaluator::NotEqual( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( double )( v1 != v2 ) );
}

void CEvaluator::Min( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( v1 < v2 ) ? v1 : v2 );
}

void CEvaluator::Max( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( v1 > v2 ) ? v1 : v2 );
}

void CEvaluator::LAnd( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( ( v1 != 0. ) && ( v2 != 0. ) ) );
}

void CEvaluator::LOr( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( ( v1 != 0. ) || ( v2 != 0. ) ) );
}

void CEvaluator::Pow( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( pow(  v1  ,  v2  ) );
}

void CEvaluator::Par( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( ( v1 * v2 ) / ( v1 + v2 ) );
}

void CEvaluator::Mod( CEvaluator& eval )
{
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.Push( fmod(  v1 ,  v2   ) );
}

void CEvaluator::And( CEvaluator& eval )
{
  int m1, m2, n;
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.GetMantAndExp( v1, v2, m1, m2, n );
  m1 = m1 & m2;
  eval.Push( ldexp( ( double )m1, n ) );
}

void CEvaluator::Or( CEvaluator& eval )
{
  int m1, m2, n;
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.GetMantAndExp( v1, v2, m1, m2, n );
  m1 = m1 | m2;
  eval.Push( ldexp( ( double )m1, n ) );
}

void CEvaluator::Xor( CEvaluator& eval )
{
  int m1, m2, n;
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  eval.GetMantAndExp( v1, v2, m1, m2, n );
  m1 = m1 ^ m2;
  eval.Push( ldexp( ( double )m1, n ) );
}

void CEvaluator::ShiftRight( CEvaluator& eval )
{
  int		n;
  double	x;
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  x = frexp(  v1 , &n );
  n -= ( int ) v2 ;
  eval.Push( ldexp( x, n ) );
}

void CEvaluator::ShiftLeft( CEvaluator& eval )
{
  int		n;
  double	x;
  double v2 = eval.Pop();
  double v1 = eval.Pop();
  x = frexp(  v1 , &n );
  n += ( int ) v2;
  eval.Push( ldexp( x, n ) );
}

void CEvaluator::If( CEvaluator& eval )
{
  double v1 = eval.Pop(); eval.Pop(); //CONCAT
  double v2 = eval.Pop(); 
  double v3 = eval.Pop();
  if( v1 != 0 )
  {
    eval.Push( v3 );
  }
  else
  {
    eval.Push( v2 );
  }
}



