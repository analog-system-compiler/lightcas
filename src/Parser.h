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

#include "LCString.h"
#include "LCVector.h"

struct SContext
{
  unsigned    m_LineNb;
  const char* m_Text;
  const char* m_Pos;
};

typedef CVector< SContext >  CParserContextArray;

class CParser
{

protected:

  static const char   m_CharTab[];
  unsigned            m_LineNb;
  CString             m_FileName;
  const char*         m_Pos;
  CString             m_Buffer;
  CParserContextArray m_ContextArray;

public:
  static const char m_StringDelimiter = '"';
  static const char m_SymbolMacro     = '#';
  static const char m_OperatorExclude = '\\';
  static const char m_OperatorAlpha   = '$';

protected:
  bool  SkipLineComment();
  bool  SkipBlockComment();
  void  SkipSpace();

public:
  bool  Find( char c );
  bool  TryFind( char c );
  void  Init( const char* pText );
  bool  TryMatchSymbol( const char*& symbol_str );
  bool  LoadFile( const CString& name );
  bool  CloseFile();

  bool  ProcessMacro();
  bool  GetQuote();
  const CString& GetWord();
  const CString& GetBuffer()                       { return m_Buffer;  }
  char  GetChar()                                  { return( *m_Pos ); }
  void  CopyBuffer( const char s[], unsigned len ) { m_Buffer.Copy( s, len ); }
  void  CopyBuffer( const CString& s )             { m_Buffer = s;            }
  unsigned  GetLineNb() const                      { return m_LineNb;         }

  static bool IsWord( char c )         { return ( m_CharTab[( int )c] & 1 ) != 0; }
  static bool IsStopChar( char c )     { return ( m_CharTab[( int )c] & 2 ) != 0; }
  static bool IsDigit( char c )        { return ( m_CharTab[( int )c] & 4 ) != 0; }

  bool  IsWord()                       { return IsWord( GetChar() );     }
  bool  IsStopChar()                   { SkipSpace(); return IsStopChar( GetChar() ); }
  bool  IsDigit()                      { return IsDigit( GetChar() );    }
  bool  IsChar( char c )               { return GetChar() == c;          }

  const char* GetPos() const           { return m_Pos;          }
  void  SetPos( const char* pText )    { m_Pos = pText;         }
  void  Next()                         { if( *m_Pos )  m_Pos++; }

  CParser();
  CParser( const char* pText );
  virtual ~CParser();

};
