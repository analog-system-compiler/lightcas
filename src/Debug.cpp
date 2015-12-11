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
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "Debug.h"

#ifdef _DEBUG

int (*PUTS)( const char *) = puts;
static FILE* debug_file = NULL;

void TRACE( const char* format, ... )
{
  unsigned i;
  static char buffer[ 8192 ];
  va_list args;
  va_start ( args, format );
  vsnprintf( buffer, sizeof(buffer), format, args );
  i=strlen(buffer);
  if( (i+1) < sizeof( buffer ) )
  {
    buffer[i] = '\n';
    buffer[i+1] = '\0';
  }
  va_end ( args );

  if( !debug_file )
  {
    debug_file = fopen( "debug.log", "w+" );
  }

  if( debug_file )
  {
    fwrite( buffer, strlen( buffer ), sizeof( char ), debug_file );
  }

#if defined( _WIN32 )
  _CrtDbgReport( _CRT_WARN, NULL, 0, NULL, buffer );
#else
  puts( buffer );
#endif
}

#endif
