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

#include "Equation.h"

const CValue& CEquation::Evaluate( CEvaluator* val ) const
{
  return Evaluate( 0, val );
}

const CValue& CEquation::Evaluate( unsigned pos_init, CEvaluator* val ) const
{
  unsigned pos;
  OP_CODE op;
  unsigned size = m_StackSize;

  val->AllocateStack( size );

  for( pos = pos_init; pos < m_StackSize; pos ++ )
  {
    op = Get( pos );
    val->Evaluate( op );
  }

  return val->GetValue();
}
