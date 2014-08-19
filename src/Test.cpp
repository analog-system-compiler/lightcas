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

#include "Debug.h"
#include "ElementDataBase.h"
#include "Equation.h"
#include "Element.h"
#include "Parser.h"
#include "Display.h"

#ifdef _DEBUG

void CElementDataBase::Check( const char* s1, const char* s2 )
{
  CParser IC( s1 );
  CDisplay ds;
  CEquation equ( this );
  TRACE( "** Check %s == %s", s1, s2 );
  equ.GetFromText( IC );
  equ.OptimizeTree();
  equ.Display( ds );
  if( ds != s2 )
  {
    printf( "Test failed: %s => %s != %s\n", s1, ds.GetBufferPtr(), s2 );
  }
}

void CElementDataBase::CheckCatch( const char* s1 )
{
  CParser IC( s1 );
  CDisplay ds;
  CEquation equ( this );
  TRACE( "** CheckCatch %s", s1 );
  try {
    equ.GetFromText( IC );
  }
  catch(...)
  {
    return;
  }
  printf( "Catch failed: %s\n", s1 );
}

void CElementDataBase::Test()
{
  /***** Syntax checking ****/
  CheckCatch( "a b" );
  CheckCatch( "a.b" );
  
  /***** Some basic tests *****/ 
  Check( "SIMPLIFY(0)", "0" );
  Check( "SIMPLIFY(1)", "1" );
  Check( "SIMPLIFY(-1)", "-1" );
  Check( "SIMPLIFY(--2)", "2" );
  Check( "SIMPLIFY(2--2)", "4" );
  Check( "SIMPLIFY(2*2^2*2^2-32)", "0" );
  Check( "SIMPLIFY(a-a)", "0" );
  //Check( "SIMPLIFY(2a)", "2*a" );
  Check( "SIMPLIFY(a-b+c-(c+a-b))", "0" );
  Check( "SIMPLIFY(0*a+a*0+a*1-1*a+a+0-a-0+a^1-a+a^0-a/a+0/a)", "0" );
  Check( "SIMPLIFY((2*x+3)*(10*x-5)/(2*x-1))","15+10*x");
  Check( "SIMPLIFY(2+2-4+0)", "0" );
  Check( "SIMPLIFY(-2*MIN(4,5)*MAX(2,3)+2^4+8)", "0" );
  Check( "SIMPLIFY(1-0*a*b)", "1" );
  Check( "SIMPLIFY(2*a-2*a)", "0" );
  Check( "SIMPLIFY(b*(a+2)-b*a-2*b)", "0" );
  Check( "SIMPLIFY(-(a-b)-(b-a))", "0" );
  Check( "SIMPLIFY(a/a-1)", "0" );
  Check( "SIMPLIFY(a*b/(a*b)-1)", "0" );
  Check( "SIMPLIFY(a*b/(a*b))", "1" );
  Check( "SIMPLIFY(a*b*c/(a*b*c))", "1" );
  Check( "SIMPLIFY(4*a*b*c/(4*a*b*c)-1)", "0" );
  Check( "SIMPLIFY(a*b/(4*a*b*c)-1/(4*c))", "0" );
  //Check( "SIMPLIFY((4*a*b*c+4)/(4*a*b*c))", "1/(a*b*c)+1" );
  Check( "SIMPLIFY(b/a-b/a)", "0" );
  Check( "SIMPLIFY(b*1/a)", "b/a" );
  Check( "SIMPLIFY(b/a-a/b-(b^2-a^2)/(a*b))", "0" );
  Check( "SIMPLIFY((a-b)^2-(a^2-2*b*a+b^2))", "0" );
  Check( "SIMPLIFY(a^2-a^(1+1))", "0" );
  Check( "SIMPLIFY(1/(a+1)-1/(a+1))", "0" );
  Check( "SIMPLIFY(1/(1/(a+1))-a-1)", "0" );
  Check( "SIMPLIFY(1/(2*1/(2*a+2))-a-1)", "0" );
  Check( "SIMPLIFY(x^(2+2))", "x^4" );
  //Check( "SIMPLIFY(a*(a^b)","a^(b+1)");
  //Check( "SIMPLIFY(x^(a*b)/x^(a*b)","1" );
  Check( "SIMPLIFY(x^(a-a))","1" );
  //Check("2*x^a-2*x^a","0");

  /****** complex *********/
  Check( "SIMPLIFY(j*j)", "-1" );
  Check( "SIMPLIFY(j^2)", "-1" );
  Check( "SIMPLIFY(1+j-(1+j))", "0" );
  Check( "SIMPLIFY(j*j*j*j*j*j*j)", "-j" );
  Check( "SIMPLIFY(j*j+1)", "0" );
  Check( "SIMPLIFY(1/j+j)", "0" );
  Check( "SIMPLIFY(1+j+j^2+j^3)", "0" );
  Check( "SIMPLIFY(1+j+j^2+j^7)", "0" );

  //Check("SIMPLIFY(2/(1+j))","(1-j)"); TODO : do not fix that
  Check( "SIMPLIFY(2/(1+j)-(1-j))", "0" );
  Check( "SIMPLIFY(2/((1+1/j)+(1+1/(1/j)))-1)", "0" );

  //Check("sqrt(-1)-(-1)^0.5","0");
  //Check("sqrt(-25)^2+25","0");

  Check( "SIMPLIFY(RE(1+j))", "1" );
  Check( "SIMPLIFY(-IM(1-j))", "1" );

  /******* In function reduction ******/
  Check( "SIMPLIFY((a+a)*COS(a+a))", "2*COS(2*a)*a" );
  Check( "SIMPLIFY(COS(2*(3*y-3*y+1)*x-x*y/y-x+x*y-x*y-1+1)-1)", "0" );
  Check( "SIMPLIFY((1+2+a+a-2*a)!-6)", "0" );

  /****** derival *****/
  Check( "DER(SIN(4*t),t)", "4*COS(4*t)" );
  Check( "DER(COS(a*t),t)", "-((t*DER(a,t)+a)*SIN(a*t))" );

  /****** vector *********/
  Check( "SIMPLIFY({a,b}[c])", "{a,b}[c]" );
  Check( "VSIZE({a})", "1" );
  Check( "VSIZE({a,b})", "2" );
  Check( "VSIZE({a,b,c})", "3" );
  Check( "{0}[0]", "0" );

  Check( "{0,1,-1}[0]", "0" );
  Check( "{0,1,-1}[1]", "1" );
  Check( "{0,1,-1}[2]", "-1" );
  Check( "{0,1,2}+{1,2,3}", "{1,3,5}" );
  Check( "{0,1,-1}-{0,1,-1}", "{0,0,0}" );
  Check( "{{0,1},{2,3}}+{{4,5},{6,7}}", "{{4,6},{8,10}}" );

  /****** trinary *********/
  Check( "SIMPLIFY(0 ? a : b)", "b" );
  Check( "SIMPLIFY(1 ? a : b)", "a" );

  /****** logic *********/
  Check( "SIMPLIFY(a|0)", "a" );
  Check( "SIMPLIFY(a|~0)", "-1" );
  Check( "SIMPLIFY(a&0)", "0" );
  Check( "SIMPLIFY(a&~0)", "a" );
  Check( "SIMPLIFY(5<<2)", "20" );
  Check( "SIMPLIFY(57>>3)", "7.125" );

  /********** transform **********/
  Check( "NORM((4+p*(6+32*p))/(2+p*(2+72*p)),p)", "(2+3*p+(4*p)^2)/(1+p+(6*p)^2)" );

  /*********** affectation ***/
  CEquation equ0( this );
  equ0.GetFromText( "a+b" );
  GetElement( "y" )->SetEquation( equ0 );
  equ0.GetFromText( "a-b" );
  GetElement( "z" )->SetEquation( equ0 );
  Check( "SIMPLIFY(y-z)", "2*b" );

  /********** solving **********/
  Initialize();
  Check( "SOLVE(x^2-4,x)", "{-2,2}" );
  Check( "SIMPLIFY(SOLVE(x^2-4,x)[1]-2)", "0" );

  /***** system solving *****/
  Check( "SYSTEM_AUTO_SOLVE({x-y-z+0,x-y+z-2,x+y-z-2})", "{x-2,y-1,z-1}" );
  Check( "SYSTEM_AUTO_SOLVE({x+y+z-3,x-y+z-1,x+y-z-1})", "{x-1,y-1,z-1}" );
  Check( "SYSTEM_SOLVE({x+y+z-3,x-y+z-1,x+y-z-1},{x,y,z})", "{1,1,1}" );
  Check( "SYSTEM_SOLVE({x-y-z+0,x-y+z-2,x+y-z-2},{x,y,z})", "{2,1,1}" );
  Check( "SYSTEM_SOLVE({a*x-b*y-c*z+0,d*x-e*y+f*z-2,g*x+h*y-i*z-2},{x,y,z})", "{-(((h*f-i*e)*2*c-(i*2+2*f)*(h*c+i*b))/((i*d+g*f)*(h*c+i*b)-(h*f-i*e)*(g*c-i*a))),(((h*f-i*e)*2*c-(i*2+2*f)*(h*c+i*b))*(i*d+g*f)+((i*d+g*f)*(h*c+i*b)-(h*f-i*e)*(g*c-i*a))*(i*2+2*f))/(((i*d+g*f)*(h*c+i*b)-(h*f-i*e)*(g*c-i*a))*(h*f-i*e)),-((((i*d+g*f)*(h*c+i*b)-(h*f-i*e)*(g*c-i*a))*((h*f-i*e)*2-(i*2+2*f)*h)-((h*f-i*e)*2*c-(i*2+2*f)*(h*c+i*b))*((i*d+g*f)*h-(h*f-i*e)*g))/(((i*d+g*f)*(h*c+i*b)-(h*f-i*e)*(g*c-i*a))*(h*f-i*e)*i))}" );

  /**** taylor suites ****/
  Check( "TAYLOR(COS(x),x,0,1)", "1" );

  /****** fonction *********/
  Initialize();
  CElementDataBase db( this );
  db.Check( "a:=4;SIMPLIFY(a-3)", "1" );
  db.Check( "a:=6;SIMPLIFY(a-5)", "1" );
  db.Check( "f(x):=4*x;SIMPLIFY(f(z)-4*z+1)", "1" );

}

#endif

