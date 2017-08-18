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

#include <cstring>
#include <cctype>
#include "Debug.h"
#include "Parser.h"
#include "Display.h"
#define eof(x)  ((x)=='\0')
#define eol(x)  ((x)=='\n')

const char CParser::m_CharTab[] =
{
  /*                              \n       \r                                                      */
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  /*   !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ? */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 4, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0,

  /*@  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _ */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 0, 1,

  /*   a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }       */
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2, 0, 0
};

CParser::CParser()
{
  m_LineNb = 1;
  m_Text = NULL;
}

CParser::CParser( const char* pText )
{
  m_LineNb = 1;
  m_Text = NULL;
  SetPos( pText );
}

CParser::~CParser()
{
  CloseFile();
}

const CString& CParser::GetWord()
{
  int i;

  SkipSpaceNL();
  i = 0;

  while( IsWord( m_Pos[i] ) )
  {
    i++;
  }

  m_Buffer.Copy( m_Pos, i );
  m_Pos += i;

  return m_Buffer;
}

void CParser::Find( char c )
{

  if( GetChar() != c )
  {

    if( eof( *m_Pos ) )
    {
      m_Buffer = "end of file";
    }
    else if( eol( *m_Pos ) )
    {
      m_Buffer = "end of line";
    }
    else
    {
      m_Buffer = "character \'";
      m_Buffer += *m_Pos;
      m_Buffer += '\'';
    }

    m_Buffer += " was found instead of \'";
    m_Buffer += c;
    m_Buffer += '\'';
    Error( CParserException::ID_ERROR_SYNTAX );

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
#ifdef _DEBUG
    CDisplay ds;
    ds.Append( "Processing line " );
    ds.Append( m_LineNb );
    TRACE( ds.GetBufferPtr() );
#endif
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
  if( IsChar( c ) )
  {
    Next();
    return true;
  }
  return false;
}

void CParser::SkipComment()
{
  char c;

  if( m_Pos[0] == '/' )
  {
    c = m_Pos[1];
    if( c == '/' || c == '*' )
    {
      m_Pos += 2;
      while( !eof( *m_Pos ) )
      {
        if( eol( *m_Pos ) )
        {
          m_LineNb++;
          if( c == '/' )
          {
            m_Pos++;
            SkipSpace();
            return;
          }
        }
        else if( c == '*' && ( m_Pos[0] == '*' ) && ( m_Pos[1] == '/' ) )
        {
          m_Pos += 2;
          SkipSpace();
          return;
        }
        m_Pos++;
      }
    }
  }
}

bool CParser::TryMatchSymbol( const char*& symbol_str )
{
  const char* s1 = symbol_str;
  const char* s2 = m_Pos;

  while( *s1 && !( IsWord( *s1 ) || ( *s1 == '(' ) ) )
  {
    if( *s1 == CParser::m_OperatorAlpha )
    {
      s1++; //Next char is considered as word
    }

    if ( *s2 == *s1 )
    {
      s1++;
      s2++;
    }
    else if( *s1 == CParser::m_OperatorExclude )
    {
      s1++; //'\'
      if( *s2 == *s1 ) //Check for forbidden characters
      {
        return false;
      }
      s1++;
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

const CString& CParser::GetQuote()
{
  m_Buffer.Clear();

  if ( TryFind( '"' ) )
  {
    const char* pos = strchr( m_Pos, '"' );
    if ( pos )
    {
      m_Buffer.Copy( m_Pos, pos - m_Pos );
      m_Pos = pos + 1;
    }
    else
    {
      Error( CParserException::ID_ERROR_SYNTAX );
    }
  }
  else
  {
    GetWord();
  }

  return m_Buffer;
}

void CParser::Error( unsigned id ) const
{
  CParserException ex;
  ex.SetErrorID( id );
  ex.SetLineNb( m_LineNb );
  if( !m_Buffer.IsEmpty() )
  {
    ex.SetErrorString( m_Buffer );
  }

#if _DEBUG
  CDisplay ds;
  ds += "Error found line ";
  ds += CString( ( int )ex.GetLineNb() );
  ds += " : ";
  if( !m_Buffer.IsEmpty() )
  {
    ds += m_Buffer;
  }
  TRACE( ds.GetBufferPtr() );
#endif

  throw( ex );
}

bool CParser::LoadFile( const CString& name )
{
  int   size;
  FILE* file;

  m_FileName = name;
  file = fopen( name.GetBufferPtr(), "r" );

  if( file )
  {
    fseek( file, 0, SEEK_END );
    size = ftell( file );
    fseek( file, 0, SEEK_SET );
    m_Text = new char[ size + 1 ];
    size = fread( ( void* )m_Text, sizeof( char ), size, file );
    fclose( file );
    m_Text[ size ] = '\0';
    SetPos( m_Text );
    return true;
  }

  return false;
}

void CParser::CloseFile()
{
  if( m_Text )
  {
    delete m_Text;
    m_Text = NULL;
  }
}