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
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include "Display.h"
#include "Value.h"

void CValue::Display( CDisplay& ds ) const
{
  char buffer[32];
  snprintf( buffer, 32, "%.12g", m_Value );
  ds.AddValue( buffer );
}

const char* CValue::GetFromString( const char* s1 )
{
  char* s2;
  m_Value = strtod( s1, &s2 );
  return s2;
}