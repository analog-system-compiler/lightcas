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

#include "Evaluator.h"
#include "Element.h"
#include <cstdlib>
#include <cmath>

CEvaluator::CEvaluator(  )
{
  m_ValueStack = NULL;
  m_ReferenceStack = NULL;
  m_StackSize = 0;
  m_ValPos = 0;
}

CEvaluator::~CEvaluator(  )
{
  if( m_ValueStack )
  {
    free ( m_ValueStack );
  }
  if( m_ReferenceStack )
  {
    free ( m_ReferenceStack );
  }
}

void CEvaluator::AllocateStack( unsigned size )
{
  m_ValPos = 0;
  if( size >= m_StackSize )
  {
    m_StackSize		 = size;
    m_ValueStack	 = ( double* )     realloc( m_ValueStack,     size * sizeof( double ) );
    m_ReferenceStack = (  CElement** ) realloc( m_ReferenceStack, size * sizeof( CElement ) );
  }
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

void CEvaluator::Evaluate( CElement* e )
{
  m_ReferenceStack[ m_ValPos ] = e;
  void ( CEvaluator::*funct_ptr )() = e->GetEvalFunction();
  if( funct_ptr )
  {
    ( this->*funct_ptr ) ();
  }
  else
  {
    m_ValueStack[ m_ValPos++ ] = e->GetValue().GetValue();
  }
}

CValue& CEvaluator::GetValue()
{

  if( m_ValPos == 0 )
  {
    m_Value = 0.;
  }
  else
  {
    m_Value = m_ValueStack[ --m_ValPos ];
  }

  return m_Value;
}

/***** void ****/

void CEvaluator::Rand( void )
{
  m_ValueStack[ m_ValPos++ ] =  ( double )rand() / ( double )RAND_MAX ;
}

/***** Unitary ****/
void CEvaluator::Sin()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  sin(  v  );
}

void CEvaluator::Cos()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  cos(  v  );
}
void CEvaluator::Tan()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  tan(  v  );
}
void CEvaluator::Asin()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  asin(  v  );
}
void CEvaluator::Acos()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  acos(  v  );
}
void CEvaluator::Atan()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  atan(  v  );
}
void CEvaluator::SinH()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  sinh(  v  );
}
void CEvaluator::CosH()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  cosh(  v  );
}
void CEvaluator::TanH()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  tanh(  v  );
}
void CEvaluator::AsinH()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  log( v + sqrt( v * v + 1. ) );
}
void CEvaluator::AcosH()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  log( v + sqrt( ( v + 1. ) * ( v - 1. ) ) );
}
void CEvaluator::AtanH()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  0.5 * log( ( 1 + v ) / ( 1 - v ) );
}
void CEvaluator::Exp()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  exp(  v  );
}
void CEvaluator::Ln()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  log(  v  );
}
void CEvaluator::Log()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  log10(  v  );
}
void CEvaluator::Sqrt()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  sqrt(  v  );
}
void CEvaluator::Floor()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  floor(  v  );
}
void CEvaluator::Ceil()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  ceil(  v  );
}
void CEvaluator::Abs()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  fabs(  v  );
}

void CEvaluator::Sqr()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  v * v;
}
void CEvaluator::Neg()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  -v;
}
void CEvaluator::Inv()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  ( double )1. / v;
}
void CEvaluator::Id()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  v;
}
void CEvaluator::Bool()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  ( double )( v == 0. );
}
void CEvaluator::LNot()
{
  double v = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] =  ( double )( v != 0. );
}


void CEvaluator::Fact()
{
  double v = m_ValueStack[ --m_ValPos ];
  int a = 1;
  int b = ( int ) v ;

  for( int i = 1; i <= b ; i++ )
  {
    a *= i;
  }

  m_ValueStack[ m_ValPos++ ] =  ( double )a;
}

void CEvaluator::Not()
{
 // int n, m;
//  double x;
  double v = m_ValueStack[ --m_ValPos ];
  /*x = frexp( v, &n ) * ( double )( 1 << 30 );
  m = ~( int )x;
  m_ValueStack[ m_ValPos++ ] = ldexp( ( double )m, n - 30 );*/
  m_ValueStack[ m_ValPos++ ] = -v-1;
}

void CEvaluator::Square()
{
  double v = m_ValueStack[ --m_ValPos ];
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
  m_ValueStack[ m_ValPos++ ] = v;
}

/******* Binary *******/
void CEvaluator::Add()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = v1 + v2;
}

void CEvaluator::Sub()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = v1 - v2;
}

void CEvaluator::Mul()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = v1 * v2;
}

void CEvaluator::Div()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  /*ASSERT(v2!=0.);*/
  m_ValueStack[ m_ValPos++ ] = v1 / v2;
}

void CEvaluator::Lower()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( double )( v1 < v2 );
}

void CEvaluator::LowerOrEqual()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( double )( v1 <= v2 );
}

void CEvaluator::Greater()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( double )( v1 > v2 );
}

void CEvaluator::GreaterOrEqual()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( double )( v1 >= v2 );
}

void CEvaluator::Equal()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( double )( v1 == v2 );
}

void CEvaluator::NotEqual()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( double )( v1 != v2 );
}

void CEvaluator::Min()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( v1 < v2 ) ? v1 : v2;
}

void CEvaluator::Max()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( v1 > v2 ) ? v1 : v2;
}

void CEvaluator::LAnd()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( ( v1 != 0. ) && ( v2 != 0. ) );
}

void CEvaluator::LOr()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( ( v1 != 0. ) || ( v2 != 0. ) );
}

void CEvaluator::Pow()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = pow(  v1  ,  v2  );
}

void CEvaluator::Par()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = ( v1 * v2 ) / ( v1 + v2 );
}

void CEvaluator::Mod()
{
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  m_ValueStack[ m_ValPos++ ] = fmod(  v1 ,  v2   );
}

void CEvaluator::And()
{
  int m1, m2, n;
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  GetMantAndExp( v1, v2, m1, m2, n );
  m1 = m1 & m2;
  m_ValueStack[ m_ValPos++ ] = ldexp( ( double )m1, n );

}

void CEvaluator::Or()
{
  int m1, m2, n;
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  GetMantAndExp( v1, v2, m1, m2, n );
  m1 = m1 | m2;
  m_ValueStack[ m_ValPos++ ] = ldexp( ( double )m1, n );

}

void CEvaluator::Xor()
{
  int m1, m2, n;
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  GetMantAndExp( v1, v2, m1, m2, n );
  m1 = m1 ^ m2;
  m_ValueStack[ m_ValPos++ ] = ldexp( ( double )m1, n );

}

void CEvaluator::ShiftRight()
{
  int		n;
  double	x;
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  x = frexp(  v1 , &n );
  n -= ( int ) v2 ;
  m_ValueStack[ m_ValPos++ ] = ldexp( x, n );
}

void CEvaluator::ShiftLeft()
{
  int		n;
  double	x;
  double v2 = m_ValueStack[ --m_ValPos ];
  double v1 = m_ValueStack[ --m_ValPos ];
  x = frexp(  v1 , &n );
  n += ( int ) v2;
  m_ValueStack[ m_ValPos++ ] = ldexp( x, n );
}




