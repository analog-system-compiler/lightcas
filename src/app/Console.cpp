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

#ifndef _WIN32
#define COLOR_CYAN "\e[35m"
#define COLOR_BLUE "\e[36m"
#define COLOR_OFF  "\e[0m"
class CDisplayEx : public CDisplay
{
    void Add( const CString& str )
    {
      *this += CString( COLOR_CYAN ) + str + CString( COLOR_OFF );
    }
    void AddValue( const CString& str )
    {
      *this += CString( COLOR_BLUE ) + str + CString( COLOR_OFF );
    }
};
#endif

int main()
{

  setlocale( LC_NUMERIC, "C" );
  std::cout << "*************************************\n";
  std::cout << "*** LighCAS Console ("__DATE__") ***\n";
  std::cout << "*************************************\n";

#ifdef _WIN32
  CDisplay ds;
#else
  CDisplayEx ds;
#endif

  CEvaluator eval;
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
      exit( 0 );
    }
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

  return 0;
}
