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

#include "MathExpression.h"

class CAlgebraRule
{
public:
  unsigned  m_LineNo;
  bool      m_bHasRule;
#ifdef _TEST
  unsigned m_AccessNb;
#endif

  CMathExpression m_SrcEquation;
  CMathExpression m_DstEquation;

  CAlgebraRule( const CMathExpression& src, const CMathExpression& dst, unsigned line_no )
  {
#ifdef _TEST
    m_AccessNb = 0;
#endif
    m_LineNo = line_no;
    m_bHasRule = ( dst.GetLastOperator() != CElementDataBase::OP_NONE ); //no rule
    m_SrcEquation.Copy( src );
    m_DstEquation.Copy( dst );
    CMathExpression::ConvertToRule( m_SrcEquation, m_DstEquation );
  }

#if defined( _TEST ) || defined( _DEBUG )
  void Display( unsigned i, CDisplay& ds )
  {
    ds += "[";
    /*CElementDataBase::RefToElement( m_SrcEquation.GetLastOperator() )->Display(ds);
    ds += "][";*/
    ds += CString( ( int )i );
    ds += "] ";
    m_SrcEquation.Display( ds );
    ds += " => ";
    m_DstEquation.Display( ds );
  }
#endif
};

typedef CVector< CAlgebraRule* > CAlgebraRuleArray;

class CFunction
{

public:
  unsigned m_ParameterNb;
  CAlgebraRuleArray m_AlgebraRuleArray;

private:
  void AddOrReplaceRule( CAlgebraRule* rule2 );

public:
  void Clear();
  void AddAlgebraRule( const CMathExpression& src, const CMathExpression& dst, unsigned line_no );
  void SetParameterNb( unsigned parameter_nb )          { m_ParameterNb = parameter_nb; }
  unsigned GetParameterNb() const                       { return m_ParameterNb; }
  const CAlgebraRuleArray& GetAlgebraRulesArray() const { return m_AlgebraRuleArray; }

  CFunction();
  ~CFunction();
};
