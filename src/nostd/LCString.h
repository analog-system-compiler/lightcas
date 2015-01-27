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

#include <cstring>
#include <cstdlib>
#include "../Debug.h"

#ifdef _WIN32
#define snprintf _snprintf_s
#endif

/*
#if defined(__GNUG__)
#define STRCASECMP( a , b ) strcasecmp( a , b )
#else
#define STRCASECMP( a , b ) _stricmp( a , b )
#endif
*/
//typedef	CVector< class CString* > CStringArray;

class CString
{

private:
  char*	            m_Data;
  unsigned			m_Length;
//  static char		m_Buffer[ 256 ];
  static const char	m_NullString[ 1 ];

public:

  /* Main functions */
  void	SetLength( unsigned len );
  void	Append( const char* s, unsigned i );


  void 	Set( unsigned i, unsigned base );

  //static bool		CaseCompare( const char* s1, const char* s2 )  {  ASSERT(s1);ASSERT(s2);    return !STRCASECMP( s1, s2 );  }
  static int		Compare( const char* s1, const char* s2 ) ; //     {  ASSERT(s1);ASSERT(s2);	  return !strcmp( s1,s2 );  }
  /* In-Line functions */
  bool	   IsEmpty() const    {    return( m_Length == 0 );  }
  unsigned GetLength() const  {    return m_Length;  }
  unsigned Search( char c ) const   {    const char* s = strchr( m_Data, c );    return s ? s - m_Data : -1;  }

  int		Compare( const CString& s ) const {    return Compare( m_Data, s.m_Data );  }
  //bool		Compare( const char* s1 )    {    return Compare( m_Data, s1 );  }

  char	At( unsigned i ) const  {    if( i < m_Length ) return m_Data[ i ];    else return '\0';  }
  void	Clear()                 {    free( m_Data );    m_Data = NULL;    m_Length = 0;  }

  void	Append( const CString& s )  {    Append( s.m_Data, s.m_Length );  }
 // void	Append( const char* s )     {    if( s ) Append( s, strlen( s ) );  }
  void	Append( char c )            {    Append( &c, 1 );  }

  void	Prepend( const CString& s )        {    CString s1( *this );    m_Length = 0;    Append( s );    Append( s1 );  }
  //void	Prepend( const char* s )           {    CString s1( *this );    m_Length = 0;    Append( s );    Append( s1 );  }
  void	Prepend( char c )                  {    CString s1( *this );    m_Length = 0;    Append( c );    Append( s1 );  }

  void	Copy( const CString& s )           {    m_Length = 0;    Append( s, s.m_Length );  }
  void	Copy( const char* s )              {    m_Length = 0;    if( s ) Append( s, strlen( s ) );  }
  void	Copy( const char* s, unsigned i )  {    m_Length = 0;    if( s ) Append( s, i );  }

  /*CString&	Right( unsigned i )
  {
    if( m_Length > i )
    {
      memcpy( m_Data, &m_Data[ m_Length - i ], i );
      m_Length = i + 1;
      m_Data[ i ] = '\0';
    }

    return *this;
  }*/

  /*void	Bin( unsigned i )
  {
    Append( Convert( i, 2 )  );
  }  
  void	Hex( unsigned i )
  {
    Append( Convert( i, 16 ) );
  }
  void	Scalar( double r )
  {
    sprintf( m_Buffer, "%.12g", r );
    Append( m_Buffer );
  }*/

 // void	Dec( unsigned i )  {      Append( Set( i, 10 ) );  }
  // implicit conversion to C string
  const char* GetBufferPtr() const   {    return ( m_Length == 0 ) ? m_NullString : m_Data;  }
  operator const char * () const     {    return GetBufferPtr();  }

  //CString	operator + ( CString s )        const  {    CString s1( *this );    s1.Append( s );    return s1;  }
  CString	operator + ( const CString& s ) const  {    CString s1( *this );    s1.Append( s );    return s1;  }
  CString	operator + ( const char* s )	const  {    CString s1( *this );    s1.Append( s );    return s1;  }
  CString	operator + ( char c ) 			const  {    CString s1( *this );    s1.Append( c );    return s1;  }
//  CString	operator + ( double r )			const  {    CString s1( *this );    s1.Scalar( r );    return s1;  }
//  CString	operator + ( int i )			const  {    CString s1( *this );    s1.Dec( i );    return s1;  }
    //CString	operator + ( unsigned i )		const  {    CString s1( *this );    s1.Dec( i );    return s1;  }
  //CString	operator << ( const CString& s ) const  {    return operator + ( s );  }  
  //CString	operator << ( const char* s )	 const  {    return operator + ( s );  }
    //CString	operator << ( char c ) 			 const  {    return operator + ( c );  }
  //CString	operator << ( double r )		 const  {    return operator + ( r );  }
  //CString	operator << ( int i )			 const  {    return operator + ( i );  }
  //CString	operator << ( unsigned i )		 const  {    return operator + ( i );  }

  //void	operator += ( CString s )         {    Append( s );  }
  void	operator += ( const CString& s )  {    Append( s );  }
  void	operator += ( const char* s )     {    Append( s );  }
   void	operator += ( char c )            {    Append( c );  }
//  void	operator += ( double r )          {    Scalar( r );  }
  //void	operator += ( int i )             {    Dec( i );  }
  //void	operator = ( CString s )          {    Copy( s );  }
  void	operator = ( const CString& s )   {    Copy( s );  }
  void	operator = ( const char* s )      {    Copy( s );  }
  void	operator = ( char c )             {    m_Length = 0;    Append( c );  }
  //void	operator = ( double r )           {    m_Length = 0;    Scalar( r );  }
  //void	operator = ( int i )              {    m_Length = 0;    Dec( i );  }

  char	operator[] (  unsigned pos )  {    ASSERT( pos < m_Length );    ASSERT( m_Data );    return m_Data[pos];  }

  bool	operator == ( const char* s )	 const  { return( Compare( s )        == 0 );  }
  bool	operator == ( const CString& s ) const  { return( Compare( s.m_Data ) == 0 );  }
  bool	operator != ( const char* s )	 const  { return( Compare( s )        != 0 );  }
  bool	operator != ( const CString& s ) const  { return( Compare( s.m_Data ) != 0 );  }

  CString()                             {    m_Data = NULL;    m_Length = 0;  }
  //CString( const char* s, unsigned i )  {    m_Data = NULL;    m_Length = 0;    Append( s, i );  }
  CString( const char* s )              { m_Data = NULL; Copy( s );  }
  CString( const CString& s )           { m_Data = NULL; Copy( s );  }
  CString(int i, unsigned base=10)      { m_Data = NULL; Set(i,base); }
  virtual ~CString()                    { Clear();  }

};
