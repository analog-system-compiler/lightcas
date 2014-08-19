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

#include "String.h"

class CParserException
{


  public:
    enum EXCEPTION_ID
    {
      ID_OK,
      ID_SYNTAX_ERROR,
      ID_ERROR_OPERATOR_EXPECTED
    };

  private:
    EXCEPTION_ID  m_ErrorID;
    unsigned m_LineNb;
    CString m_ErrorStr;

  public:
    void SetErrorID( EXCEPTION_ID id )
    {
      m_ErrorID = id;
    }
    void SetLineNb( unsigned n )
    {
      m_LineNb = n;
    }
    void SetErrorString( const CString& s )
    {
      m_ErrorStr = s;
    }

    unsigned GetLineNb() const
    {
      return m_LineNb;
    }
    const CString& GetErrorString() const
    {
      return m_ErrorStr;
    }
    EXCEPTION_ID GetErrorID() const
    {
      return m_ErrorID;
    }

    CParserException()
    {
      m_ErrorID = ID_OK;
      m_LineNb = 0;
    }
};


class CParser : public CString
{

  private:

    const char*			m_Pos;
    static	const char	m_CharTab[];
    int		m_LineNb;

  private:
    void	SkipComment();
    void	SkipSpaceNL();
    void	SkipSpace();

  public:

    void	Error( CParserException::EXCEPTION_ID id, const CString* str = NULL ) const;

    void	Find( char c );
    bool	TryFind( char c );

    void	Init( const char* pText );

    static unsigned TryMatchSymbol( const char* s1, const char* s2 );

    const   CString& GetWord();
    char    GetChar();

    bool	IsWord()
    {
      return IsWord( GetChar() );
    }
    bool	IsStopChar()
    {
      return IsStopChar( GetChar() );
    }
    bool    IsChar( char c )
    {
      return GetChar() == c;
    }

    /* position */
    const char* GetPos()	const
    {
      return m_Pos;
    }
    void	SetPos( const char* pText )
    {
      m_Pos = pText;
    }
    void	Next()
    {
      if( *m_Pos )
      {
        m_Pos++;
      }
    }

    static	bool	IsWord( char c )
    {
      return ( bool )( ( m_CharTab[( int )c] & 1 ) != 0 );
    }
    static	bool	IsStopChar( char c )
    {
      return ( bool )( ( m_CharTab[( int )c] & 2 ) != 0 );
    }

    CParser();
    CParser( const char* pText );
    virtual ~CParser();

};

