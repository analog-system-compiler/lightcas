

/*
 * Copyright (C) 2006-2025 The LightCAS project                        
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

#include <string>
#include <algorithm>
#include <cstdio>

#ifndef __GNUC__
#define snprintf _snprintf_s
#endif

class CString: public std::string
{

#if (DEBUG_LEVEL == 0)
protected:
#else
public:
#endif
  void  Append( const char* s, unsigned i )        { append (s, i); }

public:
  const char* GetBufferPtr() const                 { return c_str(); }
  unsigned GetLength() const                       { return length(); }
  void Copy( const CString& s )                    { assign( s ); }
  void Copy( const char* pText, unsigned length )  { assign( pText, length ); }
  void Clear()                                     { clear(); }
  void Prepend( const CString& s )                 { insert( 0, s ); }
  void Prepend( char c )                           { insert( 0, 1, c ); }
  virtual CString& Append( const CString& s )      { append( s ); return *this; }
  virtual void Append( char c )                    { push_back( c ); }
  void SetLength( size_t n )                       { resize( n ); }
  int  Compare(  const CString& s ) const          { return compare( s ); }
  size_t Find( char c ) const                      { return find( c ); }
  bool IsEmpty() const                             { return empty(); }
  char At( unsigned n ) const                      { return at( n ); }
  void ToLower()                                   { std::for_each( begin(), end(), ::tolower ); }
  void Set( double v )                             { char buffer[32]; snprintf( buffer, sizeof( buffer ), "%.12g", v ); Copy( buffer ); }
#ifdef __GNUC__
  void Set( unsigned i, unsigned base = 10 ) { char buffer[32]; snprintf( buffer, sizeof( buffer ), ( base == 16 ? "%x" : ( base == 10 ? "%d" : "%o" ) ), i ); Copy( buffer ); }
#else
  void Set( unsigned i, unsigned base = 10 )       { char buffer[32]; _itoa_s( i, buffer, base ); Copy( buffer ); }
#endif
  operator const char* () const                    { return c_str();  }
  //Constructors
  CString(): std::string()                              {}
  CString( const char* s ): std::string( s )            {}
  CString( const std::string& s ): std::string( s )     {} //implicit  
  explicit CString( int i, unsigned base = 10 ) : std::string() { Set( i, base ); }

};

