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
#include "Equation.h"
#include "Element.h"
#include "ElementDataBase.h"

//ToDO
//allow redefinition of function:f(x):=4*x;f(x):=2*x
//add CLEAR()
//add mul 2a

#ifndef _WIN32
#define COLOR_CYAN "\e[35;m"
#define COLOR_BLUE "\e[36;m"
#define COLOR_OFF  "\e[0;m"
class CDisplayEx : public CDisplay
{
    void AddString( const CString& str )
    {
      *this += COLOR_CYAN + str + COLOR_OFF;
    }
    void AddValue( const CString& str )
    {
      *this += COLOR_BLUE + str + COLOR_OFF;
    }
};
#endif

int main()
{

  setlocale( LC_NUMERIC, "C" );

#ifdef _WIN32
  CDisplay ds;
#else
  CDisplayEx ds;
#endif

  CElementDataBase db_main( NULL );
  CElementDataBase db( &db_main );
#ifdef _DEBUG
  db.Test();
  db.Initialize();
#endif
  CEquation equ( &db );
  CParser IC;
  CElement* simplify = db_main.GetElement( "SIMPLIFY" );
  CElement* ans      = db.GetElement( "ans" );
  OP_CODE simplify_op = simplify->ToRef();
  std::string expression_str;

  while( true )
  {
    std::cout << '>';
    std::getline ( std::cin, expression_str );
    if( expression_str == "exit" ) exit(0);
    IC.SetPos( expression_str.c_str() );
    try
    {
      equ.GetFromText( IC );
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
