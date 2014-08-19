/*******************************************************************************/
/*  Copyright (C) 2014 The CASLight project                                     */
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
#include <string>
#include <cstdio>

class CString: public std::string
{
  public:
    const char * GetBufferPtr() const                { return c_str(); }
    unsigned GetLength() const                       { return length(); }
    void Copy( const CString& s )                    { assign( s ); }
    void Copy( const char * pText, unsigned length ) { assign(pText,length); }
    void Clear()                                     { clear(); }
    void Prepend( const CString& s )                 { insert( 0, s ); }
    int Compare(  const CString& s ) const           { return compare(s); }
    //Constructors
    CString(const char*s):std::string(s)             {}
    //CString(const CString &s):std::string(s)         {} //implicit
    CString(const std::string &s):std::string(s)         {} //implicit
    CString():std::string()                          {}
#ifdef _WIN32
    CString(unsigned i, unsigned base=10){ char buffer[32];  _itoa_s( i, buffer, base ); assign(buffer); }
#else
    CString(unsigned i, unsigned base=10){ char buffer[32];  snprintf( buffer, 32, (base==16?"%x":(base==10?"%d":"%o")), i ); }
#endif
};

