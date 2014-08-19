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

#pragma once
#include "Equation.h"

class CAlgebraRule
{
public:
	unsigned  m_LineNo;
	bool      m_bHasRule;
	CEquation m_SrcEquation;
	CEquation m_DstEquation;

    CAlgebraRule(const CEquation& src, const CEquation& dst, unsigned line_no )
	{
		m_LineNo = 0;
        m_bHasRule = ( dst.GetLastOperator() != CElementDataBase::OP_NONE ); //no rule
        m_SrcEquation.Copy( src );
        m_DstEquation.Copy( dst );
        CEquation::ConvertToRule( m_SrcEquation, m_DstEquation );
        m_LineNo = line_no;
	}
};

typedef CVector< CAlgebraRule* > CAlgebraRuleArray;

class CFunction
{

public:
    unsigned m_ParameterNb;
  CAlgebraRuleArray m_AlgebraRuleArray;

private:
    void AddOrReplaceRule( CAlgebraRule*rule2);

public:
  
  void Clear();
  void AddAlgebraRule( const CEquation & src, const CEquation & dst, unsigned line_no );
  unsigned GetParameterNb() const { return m_ParameterNb; }
  void SetParameterNb( unsigned parameter_nb ) { m_ParameterNb=parameter_nb; }

  CFunction();
  ~CFunction();
};
