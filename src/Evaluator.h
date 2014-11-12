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

typedef void (CEvaluator::*CEvaluatorFunct)();
typedef CVector< class CValue > CValueArray;
typedef CVector< double > CDoubleArray;
typedef CVector< CEvaluatorFunct > CFunctArray;

class CEvaluator
{

  protected:
    CDoubleArray    m_ValueStack;
    unsigned   m_ValPos;
    CValue     m_Value;
    CValueArray m_ElementValueArray;
    CFunctArray m_FunctionArray;

  public:
    virtual void AllocateStack( unsigned size );
    void Evaluate( unsigned index );
    void GetMantAndExp( double v1, double v2, int& m1, int& m2, int& n );    

    void SetElementValue( unsigned index, const CValue& v );
    void SetFunction( unsigned index, const CEvaluatorFunct funct );
    const CValue& GetElementValue( unsigned index ) const;    
    const CValue& GetValue();    

  protected:

  public:
    void VectorStart();
    void VectorNext();
    void At();
    void PI();
    void Rand();
    void Sin();
    void Cos();
    void Tan();
    void Asin();
    void Acos();
    void Atan();
    void SinH();
    void CosH();
    void TanH();
    void AsinH();
    void AcosH();
    void AtanH();
    void Exp();
    void Ln();
    void Log();
    void Sqrt();
    void Floor();
    void Ceil();
    void Abs();
    void Sqr();
    void Neg();
    void Inv();
    void Id();
    void Bool();
    void LNot();
    void Fact();
    void Not();
    void Square();
    void Add();
    void Sub();
    void Mul();
    void Div();
    void Lower();
    void LowerOrEqual();
    void Greater();
    void GreaterOrEqual();
    void Equal();
    void NotEqual();
    void Min();
    void Max();
    void LAnd();
    void LOr();
    void Pow();
    void Par();
    void Mod();
    void And();
    void Or();
    void Xor();
    void ShiftRight();
    void ShiftLeft();

  public:
    CEvaluator( void );
    ~CEvaluator( void );
};
