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

CValue CEvaluator::GetValueFromString( const char **pos ) const
{
    CValue v;
    *pos = v.GetFromString( *pos );
    return v;
}

const CValue& CEvaluator::Evaluate( unsigned size, const OP_CODE* p )
{
  unsigned pos;

  AllocateStack( size );

  for( pos = 0; pos < size; pos ++ )
  {
    Evaluate( *p++ );
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

void CEvaluator::Evaluate( unsigned index )
{
  CEvaluatorFunct funct_ptr = NULL;

  if( index < m_FunctionArray.GetSize() )
  {
    funct_ptr = m_FunctionArray.GetAt( index );
  }

  if( funct_ptr )
  {
    ( this->*funct_ptr ) ();
  }
  else
  {
    const CValue& v=GetElementValue( index );
    Push( v.GetValue() );
  }
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
void CEvaluator::Rand( void )
{
  Push( ( double )rand() / ( double )RAND_MAX ) ;
}

/***** Unitary ****/
void CEvaluator::Sin()
{
  double v = Pop();
  Push( sin(  v  ) );
}

void CEvaluator::Cos()
{
  double v = Pop();
  Push( cos(  v  ) );
}
void CEvaluator::Tan()
{
  double v = Pop();
  Push( tan(  v  ) );
}
void CEvaluator::Asin()
{
  double v = Pop();
  Push( asin(  v  ) );
}
void CEvaluator::Acos()
{
  double v = Pop();
  Push( acos(  v  ) );
}
void CEvaluator::Atan()
{
  double v = Pop();
  Push( atan(  v  ) );
}
void CEvaluator::SinH()
{
  double v = Pop();
  Push( sinh(  v  ) );
}
void CEvaluator::CosH()
{
  double v = Pop();
  Push( cosh(  v  ) );
}
void CEvaluator::TanH()
{
  double v = Pop();
  Push( tanh(  v  ) );
}
void CEvaluator::AsinH()
{
  double v = Pop();
  Push( log( v + sqrt( v * v + 1. ) ) );
}
void CEvaluator::AcosH()
{
  double v = Pop();
  Push( log( v + sqrt( ( v + 1. ) * ( v - 1. ) ) ) );
}
void CEvaluator::AtanH()
{
  double v = Pop();
  Push( 0.5 * log( ( 1 + v ) / ( 1 - v ) ) );
}
void CEvaluator::Exp()
{
  double v = Pop();
  Push( exp(  v  ) );
}
void CEvaluator::Ln()
{
  double v = Pop();
  Push( log(  v  ) );
}
void CEvaluator::Log()
{
  double v = Pop();
  Push( log10(  v  ) );
}
void CEvaluator::Sqrt()
{
  double v = Pop();
  Push( sqrt(  v  ) );
}
void CEvaluator::Floor()
{
  double v = Pop();
  Push( floor(  v  ) );
}
void CEvaluator::Ceil()
{
  double v = Pop();
  Push( ceil(  v  ) );
}
void CEvaluator::Abs()
{
  double v = Pop();
  Push( fabs(  v  ) );
}

void CEvaluator::Sqr()
{
  double v = Pop();
  Push( v * v );
}
void CEvaluator::Neg()
{
  double v = Pop();
  Push( -v );
}
void CEvaluator::Inv()
{
  double v = Pop();
  Push( ( double )1. / v );
}
void CEvaluator::Id()
{
  double v = Pop();
  Push( v );
}
void CEvaluator::Bool()
{
  double v = Pop();
  Push( ( double )( v == 0. ) );
}
void CEvaluator::LNot()
{
  double v = Pop();
  Push( ( double )( v != 0. ) );
}
void CEvaluator::Fact()
{
  double v = Pop();
  int a = 1;
  int b = ( int ) v ;

  for( int i = 1; i <= b ; i++ )
  {
    a *= i;
  }

  Push( ( double )a );
}

void CEvaluator::Not()
{
  // int n, m;
  //  double x;
  double v = Pop();
  /*x = frexp( v, &n ) * ( double )( 1 << 30 );
  m = ~( int )x;
  Push(ldexp( ( double )m, n - 30 );*/
  Push( -v - 1 );
}

void CEvaluator::Square()
{
  double v = Pop();
  double	v2;

  v2 = floor(  v  );
  v = v - v2;

  if( v < 0.5 )
  {
    v = ( double )0.5;
  }
  else
  {
    v = ( double ) - 0.5;
  }
  Push( v );
}

/******* Binary *******/
void CEvaluator::Add()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( v1 + v2 );
}

void CEvaluator::Sub()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( v1 - v2 );
}

void CEvaluator::Mul()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( v1 * v2 );
}

void CEvaluator::Div()
{
  double v2 = Pop();
  double v1 = Pop();
  /*ASSERT(v2!=0.);*/
  Push( v1 / v2 );
}

void CEvaluator::Lower()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( double )( v1 < v2 ) );
}

void CEvaluator::LowerOrEqual()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( double )( v1 <= v2 ) );
}

void CEvaluator::Greater()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( double )( v1 > v2 ) );
}

void CEvaluator::GreaterOrEqual()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( double )( v1 >= v2 ) );
}

void CEvaluator::Equal()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( double )( v1 == v2 ) );
}

void CEvaluator::NotEqual()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( double )( v1 != v2 ) );
}

void CEvaluator::Min()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( v1 < v2 ) ? v1 : v2 );
}

void CEvaluator::Max()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( v1 > v2 ) ? v1 : v2 );
}

void CEvaluator::LAnd()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( ( v1 != 0. ) && ( v2 != 0. ) ) );
}

void CEvaluator::LOr()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( ( v1 != 0. ) || ( v2 != 0. ) ) );
}

void CEvaluator::Pow()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( pow(  v1  ,  v2  ) );
}

void CEvaluator::Par()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( ( v1 * v2 ) / ( v1 + v2 ) );
}

void CEvaluator::Mod()
{
  double v2 = Pop();
  double v1 = Pop();
  Push( fmod(  v1 ,  v2   ) );
}

void CEvaluator::And()
{
  int m1, m2, n;
  double v2 = Pop();
  double v1 = Pop();
  GetMantAndExp( v1, v2, m1, m2, n );
  m1 = m1 & m2;
  Push( ldexp( ( double )m1, n ) );

}

void CEvaluator::Or()
{
  int m1, m2, n;
  double v2 = Pop();
  double v1 = Pop();
  GetMantAndExp( v1, v2, m1, m2, n );
  m1 = m1 | m2;
  Push( ldexp( ( double )m1, n ) );

}

void CEvaluator::Xor()
{
  int m1, m2, n;
  double v2 = Pop();
  double v1 = Pop();
  GetMantAndExp( v1, v2, m1, m2, n );
  m1 = m1 ^ m2;
  Push( ldexp( ( double )m1, n ) );

}

void CEvaluator::ShiftRight()
{
  int		n;
  double	x;
  double v2 = Pop();
  double v1 = Pop();
  x = frexp(  v1 , &n );
  n -= ( int ) v2 ;
  Push( ldexp( x, n ) );
}

void CEvaluator::ShiftLeft()
{
  int		n;
  double	x;
  double v2 = Pop();
  double v1 = Pop();
  x = frexp(  v1 , &n );
  n += ( int ) v2;
  Push( ldexp( x, n ) );
}

void CEvaluator::If()
{
  double v1 = Pop();
  double v2 = Pop();
  double v3 = Pop();
  if( v3 != 0 )
  {
    Push( v2 );
  }
  else
  {
    Push( v1 );
  }
}



