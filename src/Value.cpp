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

#include <cstdio>
#include "Value.h"

void CValue::Display( CString& ds ) const
{
  char buffer[32];
  snprintf( buffer, sizeof( buffer ), "%.12g", m_Value );
  ds.Append( buffer );
}

const char* CValue::GetFromString( const char* s1 )
{
  char* s2;
  if( s1[0] == '0' && ( s1[1] == 'b' ) || ( s1[1] == 'B' ) )
  {
    m_Value = strtol( s1 + 2, &s2, 2 );
  }
  else
  {
    m_Value = strtod( s1, &s2 );
  }
  return s2;
}