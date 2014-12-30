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

#include "String.h"

char CString::m_Buffer[ 256 ];
char const CString::m_NullString[ 1 ] = "";

void CString::SetLength( unsigned len )
{
  m_Length = len;
  m_Data = ( char* )realloc( m_Data, ( m_Length + 1 ) * sizeof( char ) );
  m_Data[ len ] = '\0';
}

void CString::Append( const char* s, unsigned i )
{
  ASSERT( s != NULL );

  SetLength( m_Length + i );

  if( i == 1 )	// char
    m_Data[ m_Length - 1 ] = *s;

  else	// string;
    memcpy( &m_Data[ m_Length - i ], s, i * sizeof( char ) );

  m_Data[ m_Length ] = '\0';

}
#if 0
int CString::Compare( const char* s1, const char* s2 )
{
  int bcomp;

  if( s1 != NULL )
  {
    if( s2 != NULL )
      bcomp = strcmp( s1, s2 );

    else // s2 == NULL
      bcomp = ( *s1 == '\0' ) ? 0 : 1;
  }

  else  // s1 == NULL
  {
    if( s2 != NULL )
      bcomp = ( *s2 == '\0' ) ? 0 : -1;

    else // s2 == NULL
      bcomp = 0;
  }

  return bcomp;
}
#endif
#if !OPT24 || defined( __GNUG__ )
const char* CString::Convert( unsigned i, unsigned base )
{
  unsigned j;
  unsigned k;
  unsigned val;

  j = sizeof( m_Buffer ) - 1;
  m_Buffer[ j ] = '\0';

  do
  {

    k = i / base;
    val = i - k * base;
    i = k;

    if( val < 10 )
      val += '0';

    else
      val += 'A' - 10;

    j--;
    m_Buffer[ j ] = val;

  }
  while( i && j );

  return &m_Buffer[ j ];

}
#endif
