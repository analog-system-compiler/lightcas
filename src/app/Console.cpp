/*******************************************************************************/
/*  Copyright (C) 2014 The LightCAS project                                     */
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
#include <iostream>
#include <string>
#include "Display.h"
#include "MathExpression.h"
#include "Element.h"
#include "ElementDataBase.h"

class CDisplayEx : public CDisplay
{
  public:
    unsigned m_DisplayBase;

#ifndef _WIN32
#define COLOR_CYAN "\e[35m"
#define COLOR_BLUE "\e[36m"
#define COLOR_OFF  "\e[0m"
    void Add( const CString& str )
    {
      *this += CString( COLOR_CYAN ) + str + CString( COLOR_OFF );
    }
    void AddValue( const CString& str )
    {
      *this += CString( COLOR_BLUE ) + ConValue( str ) + CString( COLOR_OFF );
    }
#else
    void AddValue( const CString& str )
    {
      *this += ConValue( str );
    }
#endif
    CString ConValue( const CString& str )
    {
      CString s;
      if( str == "1.#INF" )
      {
        s = "INF";
      }      
      else
      {
        double v = strtod( str.GetBufferPtr(), NULL );
        if( isdigit( str[0] ) && ( v == floor( v ) ) ) // if integer
        {
          if ( m_DisplayBase == 2 )
          {
            s = '#';
          }
          else if ( m_DisplayBase == 16 )
          {
            s = '$';
          }
          s += CString( ( int )v, m_DisplayBase );
        }
        else // else display as is
        {
          s = str;
        }
      }
      return s;
    }

    CDisplayEx(): CDisplay()
    {
      m_DisplayBase = 10;
    }
};

class CValueEx : public CValue
{
  public:
    const char* GetFromString( const char* s1 )
    {
      char* s2;
      if( *s1 == '#' )
      {
        m_Value = strtol( s1 + 1, &s2, 2 );
      }
      else if( *s1 == '$' )
      {
        m_Value = strtol( s1 + 1, &s2, 16 );
      }
      else
      {
        m_Value = strtod( s1, &s2 );
      }
      return s2;
    }
};

class CEvaluatorEx : public CEvaluator
{
  public:
    CValue GetValueFromString( const char** pos ) const
    {
      CValueEx v;
      *pos = v.GetFromString( *pos );
      return v;
    }
};

int main()
{

  setlocale( LC_NUMERIC, "C" );
  std::cout << "*************************************\n";
  std::cout << "*** LighCAS Console ("__DATE__") ***\n";
  std::cout << "*************************************\n";

  CDisplayEx ds;
  CEvaluatorEx eval;
  CElementDataBase db_root( "Root", NULL, &eval );
  CElementDataBase db( "User", &db_root );

#ifdef _DEBUG
  db.Test();
  db.Initialize();
#endif

  CMathExpression equ( &db );
  CParser IC;
  CElement* simplify = db_root.GetElement( "SIMPLIFY" );
  CElement* ans      = db.GetElement( "ans" );
  OP_CODE simplify_op = simplify->ToRef();
  std::string expression_str;

  while( true )
  {
    std::cout << '>';
    std::getline ( std::cin, expression_str );
    if( expression_str == "exit" )
    {
      std::cout << "Exiting." << "\n";
      exit( 0 );
    }
    else if ( expression_str == "hex" )
    {
      ds.m_DisplayBase = 16;
      std::cout << "Switching to hexadecimal mode." << "\n";
    }
    else if ( expression_str == "bin" )
    {
      ds.m_DisplayBase = 2;
      std::cout << "Switching to binary mode." << "\n";
    }
    else if ( expression_str == "dec" )
    {
      ds.m_DisplayBase = 10;
      std::cout << "Switching to decimal mode." << "\n";
    }
    else
    {
      IC.SetPos( expression_str.c_str() );

      try
      {
        equ.GetFromString( IC );
        equ.UnaryOperation( simplify_op );
        ans->SetEquation( equ );
        ds.Clear();
        equ.Display( ds );
        std::cout << ds << "\n";
      }
      catch( CParserException e )
      {
        std::cerr << "ERROR: " << e.GetErrorString() << " at line " << e.GetLineNb() << "\n";
      }
    }
  }

  return 0;
}
