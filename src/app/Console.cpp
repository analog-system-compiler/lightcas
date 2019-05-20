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

#include <cstdio>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <io.h>
#define ISATTY(fd) _isatty(fd)
#else
#include <unistd.h>
#define ISATTY(fd) isatty(fd)
#endif

#include "Display.h"
#include "MathExpression.h"
#include "Element.h"
#include "ElementDataBase.h"
#define COLOR_CYAN "\x1B[35m"
#define COLOR_BLUE "\x1B[36m"
#define COLOR_OFF  "\x1B[0m"

void Help();

class CDisplayEx : public CDisplay
{
public:
  bool m_IsATTY;
  unsigned m_DisplayBase;

  void Append( const CString& str )
  {
    if ( m_IsATTY )
    {
      if ( isdigit( str[0] ) )
      {
        *this += CString( COLOR_BLUE ) + ValueConvert( str ) + CString( COLOR_OFF );
      }
      else
      {
        *this += CString( COLOR_CYAN ) + str + CString( COLOR_OFF );
      }
    }
    else
    {
      if ( isdigit( str[0] ) )
      {
        *this += ValueConvert( str );
      }
      else
      {
        *this +=  str;
      }
    }
  }

  CString ValueConvert( const CString& str )
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
          s = "0b";
        }
        else if ( m_DisplayBase == 16 )
        {
          s = "0x";
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

  CDisplayEx( bool isatty ):CDisplay()
  {
    m_DisplayBase = 10;
    m_IsATTY = isatty;
  }
};

int main()
{

  setlocale( LC_NUMERIC, "C" );
  std::cout << "*************************************\n";
  std::cout << "*** LighCAS Console               ***\n";
  std::cout << "*** © Cyril Collineau 2014        ***\n";
  std::cout << "*************************************\n";
  std::cout << "Type \"help\" for help.\n";

  CDisplayEx ds( ISATTY(1) );
  CEvaluator eval;
  CElementDataBase db_root( "Root", NULL, &eval );
  CElementDataBase db( "User", &db_root );

#ifdef _TEST
  db.Test();
  db.Initialize();
#endif

  if ( !db.IsOK() ) { exit( 0 ); }

  CMathExpression equ( &db );
  CParser   IC;
  CElement* simplify = db.GetElement( "SIMPLIFY" );
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
    else if( expression_str == "help" )
    {
      Help();
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

      if( !equ.GetFromString( IC ) )
      {
        std::cerr << "ERROR: " << IC.GetBuffer().GetBufferPtr() << " at line " << IC.GetLineNb() << "\n";
      }
      else
      {
        equ.UnaryOperation( simplify_op );
        ans->SetEquation( equ );
        ds.Clear();
        equ.Display( ds );
        std::cout << ds << "\n";
      }
    }
  }

  return 0;
}

void Help()
{
  std::cout << \
            "Help\n" \
            "----\n" \
            "Assignments:\n" \
            "  a:=5\n" \
            "  a:=b\n" \
            "\n" \
            "Simplifications:\n" \
            "  a:=5\n" \
            "  b:=a\n" \
            "\n" \
            "Supported operators:\n" \
            "a?b:c a|b a~b a+b a-b a&b a*b a//b a^b a<<b a>>b a==b a<>b a<=b a<b a>=b a>b a||b a&&b a[B] -a /a !a ~a a!\n" \
            "\n" \
            "Supported functions:\n" \
            "MIN MAX MOD BOOL LNOT SIN COS TAN ASIN ACOS ATAN SINH COSH TANH ASINH ACOSH ATANH EXP LOG LOG10 SQRT SQR ABS FLOOR CEIL RAND" \
            "\n" \
            "Fonction definition:\n" \
            "  f(x):=SIN(x)\n" \
            "\n" \
            "Derivatives:\n" \
            "  DER(SIN(x),x)\n" \
            "\n" \
            "Taylor series:\n" \
            "  TAYLOR(COS(x),x,0,1)\n" \
            "\n" \
            "Vectors:\n" \
            "  Addition       : {a,b}+{5,6}\n" \
            "  Sunstraction   : {a,b}-{5,6}\n" \
            "\n" \
            "Solving:\n" \
            "  SOLVE(x-2,x)\n" \
            "  SOLVE(x^2-2*x+4,x)\n" \
            "  SOLVE({x-y+1,x-y+1},{x,y})\n" \
            "\n" \
            "Complex numbers:\n" \
            "  IM(x+j*y)\n" \
            "  RE(x+j*y)\n" \
            "\n" \
            "Conversion:\n" \
            "  0b1110\n" \
            "  0xABCE\n" \
            ;
}