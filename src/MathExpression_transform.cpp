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

#include "Element.h"
#include "MathExpression.h"

void CMathExpression::ConvertToRule( CMathExpression& src, CMathExpression& dst )
{
  OP_CODE op;
  CElement* e;
  unsigned i = 0;
  unsigned pos;

  for( pos = 0; pos < src.GetSize();  pos++ )
  {
    op = src.Get( pos );
    e = RefToElement( op );
    if( e->IsVar() && ( op >= CElementDataBase::GetSecureLimit() ) )
    {
      src.Replace( op, ( OP_CODE )( CElementDataBase::OP_EXP1 + i ) );
      dst.Replace( op, ( OP_CODE )( CElementDataBase::OP_EXP1 + i ) );
      i++;
    }
  }
}

void CMathExpression::Replace( OP_CODE op1, OP_CODE op2, unsigned pos )
{
  while( pos < m_StackSize )
  {
    if( Get( pos ) == op1  )
    {
      Set( pos, op2 );
    }
    pos++;
  }
}