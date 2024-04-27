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

#pragma once

#include "MathExpression.h"

enum CAnalysisMode
{
  AC_ANALYSIS,
  TRANS_ANALYSIS,
  DC_ANALYSIS
};

class CMathExpressionEx : public CMathExpression
{
  // friend class CEquSystem;
private:
  OP_CODE m_op_hier;
  OP_CODE m_op_getv;

public:
  bool ToPython(CDisplay &ds, CAnalysisMode mode, const char * circuit_name);
  pos_t DisplayBranch(CDisplay &ds, pos_t pos, unsigned priority = 0) const override;
  pos_t DisplaySymbol(CDisplay &ds, pos_t pos, unsigned priority = 0) const override;

public:
  CMathExpressionEx(CElementDataBase *edb = NULL) : CMathExpression(edb), m_op_hier(CElementDataBase::OP_NONE), m_op_getv(CElementDataBase::OP_NONE) {};
};