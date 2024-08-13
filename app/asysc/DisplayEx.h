/*
 * Copyright (C) 2006-2024 The LightCAS project                        
 *                                                                    
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or   
 * any later version.                                                  
 *                                                                    
 * This program is distributed in the hope that it will be useful,     
 * but WITHOUT ANY WARRANTY; without even the implied warranty of      
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       
 * GNU General Public License for more details.                        
 *                                                                    
 * You should have received a copy of the GNU General Public License   
 * along with this program; If not, see <https://www.gnu.org/licenses/>
 */

#pragma once

#include <iostream>
#include "Display.h"

#ifdef __GNUC__
#include <unistd.h>
#define ISATTY(fd) isatty(fd)
#else
#include <io.h>
#define ISATTY(fd) _isatty(fd)
#endif

#define COLOR_CYAN "\033[1;35m"
#define COLOR_BLUE "\033[1;34m"
#define COLOR_OFF  "\033[m"

class CDisplayEx : public CDisplay
{
public:
  bool m_IsATTY;
  unsigned m_DisplayBase;

  CString& Append( const CString& str ) override
  {
    if ( m_IsATTY )
    {
      if ( isdigit( str[0] ) )
      {
        *this += CString( COLOR_BLUE ) + ValueConvert( str ) + CString( COLOR_OFF );
      }
      else
      {
        *this += CString( COLOR_CYAN ) + str + CString( COLOR_OFF );
      }
    }
    else
    {
      if ( isdigit( str[0] ) )
      {
        *this += ValueConvert( str );
      }
      else
      {
        *this +=  str;
      }
    }
    return *this;
  }

  CString ValueConvert( const CString& str )
  {
    CString s;
    if( str == "1.#INF" )
    {
      s = "INF";
    }
    else
    {
      double v = strtod( str.GetBufferPtr(), NULL );
      if( isdigit( str[0] ) && ( v == floor( v ) ) ) // if integer
      {
        if ( m_DisplayBase == 2 )
        {
          s = "0b";
        }
        else if ( m_DisplayBase == 16 )
        {
          s = "0x";
        }
        s += CString( ( int )v, m_DisplayBase );
      }
      else // else display as is
      {
        s = str;
      }
    }
    return s;
  }

  CDisplayEx( bool isatty ): CDisplay()
  {
    m_DisplayBase = 10;
    m_IsATTY = isatty;
  }
};