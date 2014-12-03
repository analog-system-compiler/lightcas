/*******************************************************************************/
/*  Copyright (C) 2014 The LightCAS project                                      */
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
#include <ctype.h>
#include <sstream>
#include "Debug.h"
#include "Parser.h"
#include "Display.h"
#define eof(x)	((x)=='\0')
#define eol(x)	((x)=='\n')

const char CParser::m_CharTab[] =
{
  /*                              \n       \r                                                      */
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  /*   !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ? */
  0, 0, 1, 1, 0, 1, 0, 0, 0, 2, 4, 4, 4, 4, 4, 4, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 4, 4, 4, 4, 4, 4,

  /*@  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _ */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2, 4, 1,

  /*   a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }       */
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 4, 2, 0, 0
};

CParser::CParser()
{
  m_LineNb = 1;
}

CParser::CParser( const char* pText )
{
  m_LineNb = 1;
  SetPos( pText );
}

CParser::~CParser()
{
}

const CString& CParser::GetWord()
{
  int	i;

  SkipSpaceNL();
  i = 0;

  while( IsWord( m_Pos[i] ) )
  {
    i++;
  }

  Copy( m_Pos, i );
  m_Pos += i;

  return *this;
}

void CParser::Find( char c )
{
  std::stringstream str_stream;

  if( GetChar() != c )
  {

    if( eof( *m_Pos ) )
    {
      str_stream << "end of file was found instead of \'" << c << '\'';
    }

    else if( eol( *m_Pos ) )
    {
      str_stream << "end of line was found instead of \'" << c << '\'';
    }

    else
    {
      str_stream << "character \'" << *m_Pos << "\' was found instead of \'" << c << '\'';
    }

    CString error_str( str_stream.str() );
    Error( CParserException::ID_SYNTAX_ERROR, &error_str );

  }

  m_Pos++;

}

void CParser::SkipSpaceNL()
{
  SkipSpace();

  while( eol( *m_Pos ) )
  {

    m_LineNb++;
    m_Pos++;
    SkipSpace();

  }
}

void CParser::SkipSpace()
{
  while( !eof( *m_Pos ) && !eol( *m_Pos ) && isspace( *m_Pos ) )
  {
    m_Pos++;
  }
  SkipComment();
}

char CParser::GetChar()
{
  SkipSpaceNL();
  return( *m_Pos );
}

void CParser::Init( const char* pText )
{
  SetPos( pText );
  m_LineNb = 1;
}

bool CParser::TryFind( char c )
{

  if( GetChar() == c )
  {

    Next();
    return true;
  }

  return false;

}

void CParser::SkipComment()
{
  if( m_Pos[0] == '/' )
  {

    if( m_Pos[1] == '/' )
    {

      m_Pos += 2;

      while( !eof( *m_Pos ) )
      {

        if( eol( *m_Pos ) )
        {

          m_Pos++;
          m_LineNb++;
          SkipSpace();
          return;
        }

        else
        {
          m_Pos++;
        }
      }
    }

    else if( m_Pos[1] == '*' )
    {

      m_Pos += 2;

      while( !eof( *m_Pos ) )
      {

        if( eol( *m_Pos ) )
        {

          m_Pos++;
          m_LineNb++;
        }

        else if( ( m_Pos[0] == '*' ) && ( m_Pos[1] == '/' ) )
        {

          m_Pos += 2;
          SkipSpace();
          return;
        }

        else
        {
          m_Pos++;
        }
      }
    }
  }
}

bool CParser::TryMatchSymbol( const char *& symbol_str )
{
  unsigned i = 0;

  const char* s1 = symbol_str;
  const char* s2 = m_Pos;

  while( *s1 && !IsWord( *s1 ) )
  {
    if( *s1 == '\\' )
    {
      s1++;
    }

    if ( *s2 == *s1 )
    {
      i++;
      s1++;
      s2++;
    }
    else
    {
      return false;
    }
  }

  symbol_str = s1;
  m_Pos = s2;
  return true;
}

void CParser::Error( unsigned id, const CString* str ) const
{
  CParserException ex;
  ex.SetErrorID( id );
  ex.SetLineNb( m_LineNb );
  if( str )
  {
    ex.SetErrorString( *str );
  }

#if _DEBUG
  char buffer[32];
  sprintf( buffer, "Error found line %d", ex.GetLineNb() );
  TRACE( buffer );
  if( str )
  {
    TRACE( str->c_str() );
  }
#endif

  throw( ex );
}
