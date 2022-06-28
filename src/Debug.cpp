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

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "Debug.h"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#endif

int( *PUTS )( const char* ) = puts;

#ifdef _DEBUG

static FILE* debug_file = NULL;

void TRACE( const char* format, ... )
{
  va_list args;

  if( !debug_file )
  {
    debug_file = fopen( "debug.log", "w+" );
  }

  if( debug_file )
  {
    va_start ( args, format );
    vfprintf( debug_file, format, args );
    va_end ( args );
    fputc( '\n', debug_file );
  }

  static char buffer[ 8192 ];
  va_start ( args, format );
  vsnprintf( buffer, sizeof( buffer ), format, args );
  va_end ( args );
  buffer[sizeof( buffer )-2] = '\n';
  buffer[sizeof( buffer )-1] = '\0';
  
#ifdef _WIN32
  const unsigned CRT_DEBUG_BUFFER_MAX = 256;
  buffer[CRT_DEBUG_BUFFER_MAX - 6] = '.';
  buffer[CRT_DEBUG_BUFFER_MAX - 5] = '.';
  buffer[CRT_DEBUG_BUFFER_MAX - 4] = '.';
  buffer[CRT_DEBUG_BUFFER_MAX - 3] = '\r';
  buffer[CRT_DEBUG_BUFFER_MAX - 2] = '\n';
  buffer[CRT_DEBUG_BUFFER_MAX - 1] = '\0';
  OutputDebugStringA( buffer );
#else
  PUTS( buffer );
#endif

}
#endif
