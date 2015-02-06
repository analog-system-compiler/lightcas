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
  CElement *e;
  unsigned i;
  unsigned pos;
  OP_CODE  elem_array[CElementDataBase::MAX_EXP];
  unsigned index = 0;

  for( pos = 0; pos < src.GetSize();  pos++ )
  {
    op = src.Get( pos );
    e=RefToElement( op );
    if( e->IsVar() && ( op >= CElementDataBase::GetSecureLimit() )/*&& !RefToElement( op )->IsLocked()*/ )
    {
      for( i = 0; ( i < index ) && ( i < CElementDataBase::MAX_EXP ); i++ )
      {
        if( elem_array[i] == op )
        {
          break;
        }
      }
      ASSERT( i < CElementDataBase::MAX_EXP );
      if( i == index )
      {
        elem_array[index++] = op;
      }
      src.Replace( op, ( OP_CODE )( CElementDataBase::OP_EXP1 + i ) );
      dst.Replace( op, ( OP_CODE )( CElementDataBase::OP_EXP1 + i ) );
    }
  }
}

void CMathExpression::Replace( OP_CODE op1, OP_CODE op2 )
{
  unsigned	pos;

  for( pos = 0; pos < m_StackSize; pos++ )
  {
    if( Get( pos ) == op1  )
    {
      Set( pos, op2 );
    }
  }
}