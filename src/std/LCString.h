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

#pragma once

#include <string>
#include <algorithm>
#include <cstdio>

#ifdef _WIN32
#define snprintf _snprintf_s
#endif

class CString: public std::string
{
  public:
    const char * GetBufferPtr() const                { return c_str(); }
    unsigned GetLength() const                       { return length(); }
    void Copy( const CString& s )                    { assign( s ); }
    void Copy( const char * pText, unsigned length ) { assign(pText,length); }
    void Clear()                                     { clear(); }
    void Prepend( const CString& s )                 { insert( 0, s ); }
    void Append( const CString& s )                  { append( s ); }
    void SetLength( size_t n )                       { resize( n ); }
    int  Compare(  const CString& s ) const          { return compare(s); }
    size_t Search( char c ) const                    { return find(c); }    
    //operator const char * () const                   { return GetBufferPtr();  }
    bool IsEmpty() const                             { return empty(); }
    char At( unsigned n ) const                      { return at(n); }
    void ToLower()                                   { std::for_each(begin(),end(),::tolower); }
    void Set( int i, unsigned base=10 ) 
#ifdef _WIN32
        { char buffer[32];  _itoa_s( i, buffer, base ); Copy(buffer); }
#else
        { char buffer[32];  snprintf( buffer, sizeof(buffer), (base==16?"%x":(base==10?"%d":"%o")), i ); Copy(buffer); }
#endif
    
    //Constructors
    CString(const char*s):std::string(s)             {}
    CString(const std::string &s):std::string(s)     {} //implicit
    CString():std::string()                          {}
    CString(int i, unsigned base=10)    { Set(i,base); }

};

