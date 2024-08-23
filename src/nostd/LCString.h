

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

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "../Debug.h"

#ifndef __GNUC__
#define snprintf _snprintf_s
#endif

class CString
{

private:
  char*      m_Data;
  unsigned   m_Length;
  static const char m_NullString[1];

#if (DEBUG_LEVEL == 0)
protected:
#else
public:
#endif
  void  Append( const char* s, unsigned i );

public:

  void  SetLength( unsigned len );
  static int Compare( const char* s1, const char* s2 );
  void  Set( unsigned i, unsigned base );

  void Set( double v )                     { char buffer[32]; snprintf( buffer, sizeof( buffer ), "%.12g", v ); Copy( buffer ); }
  bool     IsEmpty() const                 { return( m_Length == 0 );  }
  unsigned GetLength() const               { return m_Length;  }
  unsigned Find( char c ) const            { const char* s = strchr( m_Data, c ); return s ? s - m_Data : -1;  }

  int Compare( const CString& s ) const    { return Compare( m_Data, s.m_Data );  }

  char  At( unsigned i ) const             { if( i < m_Length ) return m_Data[ i ]; else return '\0';  }
  void  Clear()                            { free( m_Data ); m_Data = NULL; m_Length = 0; }

  virtual CString& Append( const CString& s )  { Append( s.m_Data, s.m_Length ); return *this; }
  virtual CString& Append( char c )            { Append( &c, 1 ); return *this; }

  void  Prepend( const CString& s )        { CString s1( *this ); m_Length = 0; CString::Append( s ); CString::Append( s1 );  }
  void  Prepend( char c )                  { CString s1( *this ); m_Length = 0; CString::Append( c ); CString::Append( s1 );  }

  void  Copy( const CString& s )           { m_Length = 0; Append( s, s.m_Length );  }
  void  Copy( const char* s )              { m_Length = 0; if( s ) Append( s, strlen( s ) );  }
  void  Copy( const char* s, unsigned i )  { m_Length = 0; if( s ) Append( s, i );  }


  const char* GetBufferPtr() const         { return ( m_Length == 0 ) ? m_NullString : m_Data;  }
  operator const char* () const            { return GetBufferPtr();  }

  CString operator + ( const CString& s ) const  { CString s1( *this ); s1.CString::Append( s ); return s1; }
  CString operator + ( const char* s )    const  { CString s1( *this ); s1.CString::Append( s ); return s1; }
  CString operator + ( char c )           const  { CString s1( *this ); s1.CString::Append( c ); return s1; }

  void  operator += ( const CString& s )   { CString::Append( s ); }
  void  operator += ( const char* s )      { CString::Append( s ); }
  void  operator += ( char c )             { CString::Append( c ); }
  void  operator = ( const CString& s )    { Copy( s );   }
  void  operator = ( const char* s )       { Copy( s );   }
  void  operator = ( char c )              { m_Length = 0; CString::Append( c );  }

  char  operator[] (  unsigned pos )       { ASSERT( pos < m_Length ); ASSERT( m_Data ); return m_Data[pos]; }

  bool  operator == ( const char* s )    const  { return( Compare( s )        == 0 ); }
  bool  operator == ( const CString& s ) const  { return( Compare( s.m_Data ) == 0 ); }
  bool  operator != ( const char* s )    const  { return( Compare( s )        != 0 ); }
  bool  operator != ( const CString& s ) const  { return( Compare( s.m_Data ) != 0 ); }

  CString()                                { m_Data = NULL; m_Length = 0;  }
  CString( const char* s )                 { m_Data = NULL; Copy( s );  }
  CString( const CString& s )              { m_Data = NULL; Copy( s );  }
  CString( int i, unsigned base = 10 )     { m_Data = NULL; m_Length=0; Set( i, base ); }
  virtual ~CString()                       { Clear();  }

};
