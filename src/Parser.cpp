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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0,

  /*@  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _ */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,

  /*   a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }       */
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
};

CParser::CParser()
{
  m_LineNb = 1;
  m_Pos = "";
}

CParser::CParser( const char* pText )
{
  m_LineNb = 1;
  SetPos( pText );
}

CParser::~CParser()
{
  for( unsigned i = 0; i < m_ContextArray.GetSize(); i++ )
  {
    delete m_ContextArray[i].m_Text;
  }
}

const CString& CParser::GetWord()
{
  const char* pos;
  SkipSpace();
  pos = m_Pos;
  while ( IsWord( *pos ) )
  {
    pos++;
  }
  m_Buffer.Copy( m_Pos, pos - m_Pos );
  m_Pos = pos;
  return m_Buffer;
}

bool CParser::Find( char c )
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
    return false;
  }
  m_Pos++;
  return true;
}

void CParser::Init( const char* pText )
{
  SetPos( pText );
  m_LineNb = 1;
}

bool CParser::TryFind( char c )
{
  if ( IsChar( c ) )
  {
    Next();
    return true;
  }
  return false;
}

void CParser::SkipSpace()
{
  char c;

  while ( true )
  {
    c = m_Pos[0];
    if ( eol( c ) )
    {
      m_LineNb++;
      m_Pos++;
#if( DEBUG_LEVEL >= 1 )
      CDisplay ds;
      ds.Append( "Processing line " );
      ds.Append( CString( m_LineNb ) );
      TRACE( ds.GetBufferPtr() );
#endif
    }
    else if ( isspace( c ) ) //\n is considered as space
    {
      m_Pos++;
    }
    else if ( c == '/' )
    {
      c = m_Pos[1];
      if ( c == '/' )
      {
        if ( !SkipLineComment() )
        {
          return;
        }
      }
      else if ( c == '*' )
      {
        if ( !SkipBlockComment() )
        {
          return;
        }
      }
      else
      {
        return;
      }
    }
    else if ( c == CParser::m_SymbolMacro )
    {
      if( !ProcessMacro() )
      {
        return;
      }
    }
    else if ( eof( c ) )
    {
      if( !CloseFile() )
      {
        return;
      }
    }
    else
    {
      return ;
    }
  }
}

bool CParser::SkipLineComment()
{
  m_Pos += 2;
  while( !eof( *m_Pos ) )
  {
    if( eol( *m_Pos ) )
    {
      return true;
    }
    m_Pos++;
  }
  return false;
}

bool CParser::SkipBlockComment()
{
  m_Pos += 2;
  while ( !eof( *m_Pos ) )
  {
    if ( ( m_Pos[0] == '*' ) && ( m_Pos[1] == '/' ) )
    {
      m_Pos += 2;
      return true;
    }
    if ( eol( *m_Pos ) )
    {
      m_LineNb++;
    }
    m_Pos++;
  }
  return false;
}

bool CParser::TryMatchSymbol( const char*& symbol_str )
{
  const char* s1 = symbol_str;
  const char* s2 = m_Pos;

  if ( IsWord( *s1 ) )
  {
    return false;
  }

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

bool CParser::GetQuote()
{
  m_Buffer.Clear();

  SkipSpace();
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
      return false; // Error(CParserException::ID_ERROR_SYNTAX);
    }
  }
  else
  {
    return false;//GetWord();
  }

  return true;//return m_Buffer;
}

bool CParser::LoadFile( const CString& name )
{
  int   size;
  FILE* file;
  SContext context;
  char* text;

  m_FileName = name;
  file = fopen( name.GetBufferPtr(), "r" );

  if( file )
  {
    TRACE( "Opening file %s", name.GetBufferPtr() );
    fseek( file, 0, SEEK_END );
    size = ftell( file );
    fseek( file, 0, SEEK_SET );
    text = new char[ size + 1 ];
    size = fread( ( void* )text, sizeof( char ), size, file );
    fclose( file );
    text[ size ] = '\0';
    context.m_LineNb = GetLineNb();
    context.m_Text = text;
    context.m_Pos  = GetPos();
    m_ContextArray.Push( context );
    SetPos( text );
    return true;
  }
  TRACE( "Failed to open file %s", name.GetBufferPtr() );
  return false;
}

bool CParser::CloseFile()
{
  if( m_ContextArray.GetSize() )
  {
    const SContext& context = m_ContextArray.Pop();
    delete context.m_Text;
    m_Pos    = context.m_Pos;
    m_LineNb = context.m_LineNb;
    return true;
  }
  return false;
}

bool CParser::ProcessMacro()
{
  const char* pos = m_Pos;
  m_Pos++;
  const CString& s = GetWord();
  if ( s == "inc" )
  {
    if ( GetQuote() )
    {
      LoadFile( GetBuffer() );
      return true;
    }
  }
  else
  {
    m_Pos = pos;
  }
  return false;
}