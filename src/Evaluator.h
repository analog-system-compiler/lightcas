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

#pragma once

#include "Value.h"
#include "LCVector.h"

class CElement;
class CEvaluator;

typedef void ( *CEvaluatorFunct )( CEvaluator& );
typedef CVector< class CValue >    CValueArray;
typedef CVector< double >          CDoubleArray;
typedef CVector< CEvaluatorFunct > CFunctArray;

typedef unsigned short OP_CODE;

class CEvaluator
{

protected:
  CDoubleArray    m_ValueStack;
  unsigned        m_ValPos;
  CValue          m_Value;
  CValueArray     m_ElementValueArray;
  CFunctArray     m_FunctionArray;

public:
  void GetMantAndExp( double v1, double v2, int& m1, int& m2, int& n );
  void DeclareElement( unsigned index );
  void SetElementValue( unsigned index );
  void SetElementValue( unsigned index, const CValue& val );
  void SetFunction( unsigned index, const CEvaluatorFunct funct );
  virtual const CValue& GetElementValue( unsigned index ) const;
  void Evaluate( unsigned size, const OP_CODE* p );

  virtual const char* GetValueFromString( const char* pos );
  virtual bool Display( CString& s );

  double  Pop()             { return m_ValueStack[ --m_ValPos ]; }
  void    Push( double v )  { m_ValueStack[ m_ValPos++ ] = v; }
  void    ClearValue()      { m_Value.SetValue( 0. ); }
  const   CValue& GetValue();

protected:
  virtual void AllocateStack( unsigned size );

public:
  static void Rand( CEvaluator& );
  static void Sin( CEvaluator& );
  static void Cos( CEvaluator& );
  static void Tan( CEvaluator& );
  static void Asin( CEvaluator& );
  static void Acos( CEvaluator& );
  static void Atan( CEvaluator& );
  static void SinH( CEvaluator& );
  static void CosH( CEvaluator& );
  static void TanH( CEvaluator& );
  static void AsinH( CEvaluator& );
  static void AcosH( CEvaluator& );
  static void AtanH( CEvaluator& );
  static void Exp( CEvaluator& );
  static void Ln( CEvaluator& );
  static void Log( CEvaluator& );
  static void Sqrt( CEvaluator& );
  static void Floor( CEvaluator& );
  static void Ceil( CEvaluator& );
  static void Abs( CEvaluator& );
  static void Sqr( CEvaluator& );
  static void Neg( CEvaluator& );
  static void Inv( CEvaluator& );
  static void Id( CEvaluator& );
  static void Bool( CEvaluator& );
  static void LNot( CEvaluator& );
  static void Fact( CEvaluator& );
  static void Not( CEvaluator& );
  static void Add( CEvaluator& );
  static void Sub( CEvaluator& );
  static void Mul( CEvaluator& );
  static void Div( CEvaluator& );
  static void Lower( CEvaluator& );
  static void LowerOrEqual( CEvaluator& );
  static void Greater( CEvaluator& );
  static void GreaterOrEqual( CEvaluator& );
  static void Equal( CEvaluator& );
  static void NotEqual( CEvaluator& );
  static void Min( CEvaluator& );
  static void Max( CEvaluator& );
  static void LAnd( CEvaluator& );
  static void LOr( CEvaluator& );
  static void Pow( CEvaluator& );
  static void Par( CEvaluator& );
  static void Mod( CEvaluator& );
  static void And( CEvaluator& );
  static void Or( CEvaluator& );
  static void Xor( CEvaluator& );
  static void ShiftRight( CEvaluator& );
  static void ShiftLeft( CEvaluator& );
  static void If( CEvaluator& );

public:
  CEvaluator( void );
  virtual ~CEvaluator( void );
};
