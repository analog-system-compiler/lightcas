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

#include "Debug.h"
#include "ElementDataBase.h"
#include "MathExpression.h"
#include "Element.h"
#include "Parser.h"
#include "Display.h"

#ifdef _DEBUG

#include <cstdarg>

#ifdef _WIN32
#define vsnprintf _vsnprintf_s
#endif

void CElementDataBase::Printf( const char* format, ... )
{
    static char buffer[2048];
    va_list args;
    va_start ( args, format );
    vsnprintf ( buffer, sizeof(buffer), format, args );
    PUTS(buffer);
    va_end ( args );
}

void CElementDataBase::Check( const char* s1, const char* s2 )
{
  CDisplay ds;
  CParser IC( s1 );
  CMathExpression equ( this );
  TRACE( "** Check %s == %s", s1, s2 );
  try
  {
    equ.GetFromString( IC );
    equ.OptimizeTree();
    equ.Display( ds );
    if( ds != s2 )
    {
      ASSERT( false );
      Printf( "FAIL: %s => %s != %s", s1, ds.GetBufferPtr(), s2 );
      return;
    }
  }
  catch( ... )
  {
    ASSERT( false );
    Printf( "FAIL: Test exception: %s => %s", s1, s2 );
    return;
  }
  Printf( "OK: %s => %s", s1, s2 );
}

void CElementDataBase::Check( const char* s1, const CValue& v1 )
{
  CDisplay ds;
  CMathExpression equ( this );
  equ.GetFromString( s1 );
  CValue v2 = equ.Evaluate();
  if( v2.GetValue() != v1.GetValue() )
  {
    ASSERT( false );
    Printf( "FAIL: %s => %g != %g", s1, v2.GetValue(), v1.GetValue() );
    return;
  }
  Printf( "OK: %s => %g", s1, v1.GetValue() );
}

void CElementDataBase::CheckCatch( const char* s1 )
{
  CDisplay ds;
  CParser IC( s1 );
  CMathExpression equ( this );
  TRACE( "** CheckCatch %s", s1 );
  try
  {
    equ.GetFromString( IC );
  }
  catch( ... )
  {
    return;
  }
  ASSERT( false );
  Printf( "Catch failed: %s", s1 );
}

void CElementDataBase::DisplayStats()
{
  CDisplay ds;
  for( unsigned i = 0; i < m_ElementRefArray.GetSize(); i++ )
  {
    CElement* e = m_ElementRefArray.GetAt( i );
    if( e )
    {
      CFunction* funct = e->GetFunction();
      for( unsigned j = 0; j < funct->m_AlgebraRuleArray.GetSize(); j++ )
      {
        if( j == 0 )
        {
          Printf( "**************************" );
        }
        CAlgebraRule* rule = funct->m_AlgebraRuleArray.GetAt( j );
        ds.Clear();
        rule->m_SrcEquation.Display( ds );
        Printf( "%4d \t%s\t", rule->m_AccessNb, ds.GetBufferPtr() );
      }
    }
  }
}

void CElementDataBase::Test()
{
  Printf( "Running tests..." );

  /***** Syntax checking ****/
  //CheckCatch( "a b" );
  //CheckCatch( "a.b" );
  /***** Some basic tests *****/
  Check( "SIMPLIFY(0)", "0" );
  Check( "SIMPLIFY(1)", "1" );
  Check( "SIMPLIFY(-1)", "-1" );
  Check( "SIMPLIFY(-0)", "0" );
  Check( "SIMPLIFY(--2)", "2" );
  Check( "SIMPLIFY(2--2)", "4" );
  Check( "SIMPLIFY(2*2^2*2^2-32)", "0" );
  Check( "SIMPLIFY(a-a)", "0" );
  Check( "SIMPLIFY(a+a)", "2*a" );
  Check( "SIMPLIFY(a+a+a)", "3*a" );
  Check( "SIMPLIFY(a+b)", "a+b" );
  Check( "SIMPLIFY(a-b)", "a-b" );
  Check( "SIMPLIFY(a+a+b+b)", "2*a+2*b" );
  Check( "SIMPLIFY(a+a-(b+b)", "2*a-2*b" );
  Check( "SIMPLIFY(a-b+c-(c+a-b))", "0" );
  Check( "SIMPLIFY(a-(b-c))", "a-b+c" );
  Check( "SIMPLIFY(a-b-c)", "a-b-c" );
  Check( "SIMPLIFY(0*a+a*0+a*1-1*a+a+0-a-0+a^1-a+a^0-a/a+0/a)", "0" );
  //Check( "SIMPLIFY((2*x+3)*(10*x-5)/(2*x-1))", "15+10*x" );
  Check( "SIMPLIFY(2+2-4+0)", "0" );
  Check( "SIMPLIFY(-2*MIN(4,5)*MAX(2,3)+2^4+8)", "0" );
  Check( "SIMPLIFY(1-0*a*b)", "1" );
  Check( "SIMPLIFY(2*a-2*a)", "0" );
  Check( "SIMPLIFY(b*(a+2)-b*a-2*b)", "0" );
  Check( "SIMPLIFY(-(a-b)-(b-a))", "0" );
  Check( "SIMPLIFY(a/a-1)", "0" );
  Check( "SIMPLIFY(-b/a)", "-(b/a)" );
  Check( "SIMPLIFY(a*b/(a*b)-1)", "0" );
  Check( "SIMPLIFY(a*b/(a*b))", "1" );
  Check( "SIMPLIFY(a*b*c/(a*b*c))", "1" );
  Check( "SIMPLIFY(4*a*b*c/(4*a*b*c)-1)", "0" );
  Check( "SIMPLIFY(a*b/(4*a*b*c)-1/(4*c))", "0" );
  Check( "SIMPLIFY((4*a*b)/(3*a*b*c))", "1.33333333333/c" ); //"4/(3*c)" );
  Check( "SIMPLIFY((4*c*b)/(3*a*b*c))", "1.33333333333/a" ); //"4/(3*a)" );
  Check( "SIMPLIFY((4*a*b*c+4)/(4*a*b*c))", "1+1/(c*b*a)" ); //"1+1/(a*b*c)" );
  Check( "SIMPLIFY(b/a+b/a)", "(2*b)/a"); //"2/a*b" );
  Check( "SIMPLIFY(b/a-b/a)", "0" );
  Check( "SIMPLIFY(b*1/a)", "b/a" );
  Check( "SIMPLIFY(x^(2+2))", "x^4" );
  Check( "SIMPLIFY((a+b)^2)", "a^2+b^2+2*a*b" );
  Check( "SIMPLIFY(a^2-a^(1+1))", "0" );
  Check( "SIMPLIFY((a-b)^2-(a^2-2*b*a+b^2))", "0" );
  Check( "SIMPLIFY(b/a-a/b-(b^2-a^2)/(a*b))", "0" );
  Check( "SIMPLIFY(1/(a+1)-1/(a+1))", "0" );
  Check( "SIMPLIFY(1/(1/(a+1))-a-1)", "0" );
  Check( "SIMPLIFY(1/(2*1/(2*a+2))-a-1)", "0" );
  Check( "SIMPLIFY(a*(a^b)","a^(1+b)");
  Check( "SIMPLIFY(x^(a*b)/x^(a*b)","1" );
  Check( "SIMPLIFY(x^(a-a))", "1" );
  Check( "SIMPLIFY(x^a)", "x^a" );
  Check( "SIMPLIFY(x^(a+a))", "x^(2*a)" );
  Check( "SIMPLIFY((x+y)^(a+a))", "(x+y)^(2*a)" );
  Check( "SIMPLIFY(1+(x+y)^(a+a))", "1+(x+y)^(2*a)" );
  Check( "SIMPLIFY((x^a)^2", "x^(2*a)" );
  Check( "SIMPLIFY(x^2.5)", "x^2.5" );
  Check( "SIMPLIFY((x+a)^2.5)", "(x+a)^2.5" );
  Check( "SIMPLIFY(2*x^a-2*x^a)","0");
  Check( "SIMPLIFY(x^(1+a)*x^(1-a))","x^2");
  Check( "SIMPLIFY((a-a+1)>(a-a))", "1" );
  Check( "SIMPLIFY({a+a,b+4*b,c-d})", "{2*a,5*b,c-d}" );
  Check( "SIMPLIFY(a*cos(b)", "cos(b)*a" );

  /****** complex *********/
  Check( "SIMPLIFY(j*j)", "-1" );
  Check( "SIMPLIFY(j^2)", "-1" );
  Check( "SIMPLIFY(j^-2)", "-1" );
  Check( "SIMPLIFY(1+j-(1+j))", "0" );
  Check( "SIMPLIFY(j*j*j*j*j*j*j)", "-j" );
  Check( "SIMPLIFY(j*j+1)", "0" );
  Check( "SIMPLIFY(1/j+j)", "0" );
  Check( "SIMPLIFY(1+j+j^2+j^3)", "0" );
  Check( "SIMPLIFY(1+2*j+j^2+j^7)", "j" );

  //Check("SIMPLIFY(2/(1+j))","(1-j)"); TODO : do not fix that
  Check( "SIMPLIFY(2/(1+j)-(1-j))", "0" );
  Check( "SIMPLIFY(2/((1+1/j)+(1+1/(1/j)))-1)", "0" );

  //FAIL!!Check( "SIN(PI)", "0" );
  //FAIL!!Check( "COS(PI/2)", "0" );
  Check( "SQRT(-25)", "5*j" );
  Check( "SIMPLIFY(SQRT(-25)^2+25)", "0" );

  Check( "SIMPLIFY(RE(4+j))", "4" );
  Check( "SIMPLIFY(-IM(1-5*j))", "5" );
  Check( "SIMPLIFY(RE(4*j^6+j))", "-4" );
  Check( "SIMPLIFY(-IM(1-5*j*(5*j^4))", "25" );

  /******* In function reduction ******/
  Check( "SIMPLIFY((a+a)*COS(a+a))", "2*COS(2*a)*a" );
  Check( "SIMPLIFY(COS(2*(3*y-3*y+1)*x-x*y/y-x+x*y-x*y-1+1)-1)", "0" );
  Check( "SIMPLIFY((1+2+a+a-2*a)!-6)", "0" );

  /****** derival *****/
  Check( "DER(SIN(4*a),a)", "4*COS(4*a)" );
  Check( "DER(COS(a*b),b)", "-((b*DER(a,b)+a)*SIN(a*b))" );

  /****** trigo *********/
  //Check( "(SIN(PI/2+a))", "COS(a)" );

  /****** vector *********/
  Check( "SIMPLIFY({a,b}[c])", "{a,b}[c]" );
  Check( "SIMPLIFY({a+a,b-b}[c+c])", "{2*a,0}[2*c]" );
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
  Check( "SIMPLIFY({-2,2})", "{-2,2}" );
  //Check( "{a,b}:={2,3};{a,b}", "{2,3}" );

  /****** trinary *********/
  Check( "SIMPLIFY(0 ? a : b)", "b" );
  Check( "SIMPLIFY(1 ? a : b)", "a" );
  Check( "SIMPLIFY(5 ? a : b)", "a" );
  Check( "SIMPLIFY(2>3 ? a : b)", "b" );
  Check( "SIMPLIFY(a<=a ? b : c)", "b" );

  /****** logic *********/
  Check( "SIMPLIFY(a|0)", "a" );
  Check( "SIMPLIFY(a|~0)", "-1" );
  Check( "SIMPLIFY(a&0)", "0" );
  Check( "SIMPLIFY(a&~0)", "a" );
  Check( "SIMPLIFY((5<<2)|32", "52" );
  Check( "SIMPLIFY(57>>3)", "7.125" );

  /********** transform **********/
  Check( "NORM((4+a*(6+32*a))/(2+a*(2+72*a)),a)", "(2+3*a+(4*a)^2)/(1+a+(6*a)^2)" );

  /*********** assignment ***/
  CMathExpression equ0( this );
  equ0.GetFromString( "a+b" );
  GetElement( "y" )->SetEquation( equ0 );
  equ0.GetFromString( "a-b" );
  GetElement( "z" )->SetEquation( equ0 );
  Check( "SIMPLIFY(y-z)", "2*b" );

  /********** solving **********/
  Initialize();
  Check( "SOLVE(x^2-4,x)", "{-2,2}" );
  Check( "SOLVE(x^2+4,x)", "{-(2*j),2*j}" );
  Check( "SIMPLIFY(SOLVE(x^2-4,x)[1]-2)", "0" );

  /***** system solving *****/
  Check( "SYSTEM_AUTO_SOLVE({x-y+1,x-y+1})", "{y-(x+1),0}" );
  Check( "SYSTEM_AUTO_SOLVE({x-y-z+0,x-y+z-2,x+y-z-2})", "{x-2,y-1,z-1}" );
  Check( "SYSTEM_AUTO_SOLVE({x+y+z-3,x-y+z-1,x+y-z-1})", "{x-1,y-1,z-1}" );
  Check( "SYSTEM_SOLVE({x+y+z-3,x-y+z-1,x+y-z-1},{x,y,z})", "{1,1,1}" );
  Check( "SYSTEM_SOLVE({x-y-z+0,x-y+z-2,x+y-z-2},{x,y,z})", "{2,1,1}" );
  //Check( "SYSTEM_SOLVE({a*x-b*y-c*z+0,d*x-e*y+f*z-2,g*x+h*y-i*z-2},{x,y,z})", "{-(((h*f-i*e)*2*c-(i*2+2*f)*(h*c+i*b))/((i*d+g*f)*(h*c+i*b)-(h*f-i*e)*(g*c-i*a))),(((h*f-i*e)*2*c-(i*2+2*f)*(h*c+i*b))*(i*d+g*f)+((i*d+g*f)*(h*c+i*b)-(h*f-i*e)*(g*c-i*a))*(i*2+2*f))/(((i*d+g*f)*(h*c+i*b)-(h*f-i*e)*(g*c-i*a))*(h*f-i*e)),-((((i*d+g*f)*(h*c+i*b)-(h*f-i*e)*(g*c-i*a))*((h*f-i*e)*2-(i*2+2*f)*h)-((h*f-i*e)*2*c-(i*2+2*f)*(h*c+i*b))*((i*d+g*f)*h-(h*f-i*e)*g))/(((i*d+g*f)*(h*c+i*b)-(h*f-i*e)*(g*c-i*a))*(h*f-i*e)*i))}" );
  //Check( "SYSTEM_SOLVE({a*x-b*y-c*z+0,d*x-e*y+f*z-2,g*x+h*y-i*z-2},{x,y,z})", "{-(((f*h-i*e)*c*2-(i*b+c*h)*(f*2+i*2))/((i*d+g*f)*(i*b+c*h)-(f*h-i*e)*(g*c-i*a))),(((f*h-i*e)*c*2-(i*b+c*h)*(f*2+i*2))*(i*d+g*f)+((i*d+g*f)*(i*b+c*h)-(f*h-i*e)*(g*c-i*a))*(f*2+i*2))/(((i*d+g*f)*(i*b+c*h)-(f*h-i*e)*(g*c-i*a))*(f*h-i*e)),-((((i*d+g*f)*(i*b+c*h)-(f*h-i*e)*(g*c-i*a))*((f*h-i*e)*2-h*(f*2+i*2))-((f*h-i*e)*c*2-(i*b+c*h)*(f*2+i*2))*((i*d+g*f)*h-(f*h-i*e)*g))/(((i*d+g*f)*(i*b+c*h)-(f*h-i*e)*(g*c-i*a))*(f*h-i*e)*i))}" );
  Check( "SYSTEM_SOLVE({a*x-b*y-c*z+0,d*x-e*y+f*z-2,g*x+h*y-i*z-2},{x,y,z})", "{((i*b+c*h)*(f*2+i*2)+(i*e-f*h)*c*2)/((i*e-f*h)*(g*c-i*a)+(i*d+g*f)*(i*b+c*h)),(((i*b+c*h)*(f*2+i*2)+(i*e-f*h)*c*2)*(i*d+g*f)-((i*e-f*h)*(g*c-i*a)+(i*d+g*f)*(i*b+c*h))*(f*2+i*2))/(((i*e-f*h)*(g*c-i*a)+(i*d+g*f)*(i*b+c*h))*(i*e-f*h)),(((i*b+c*h)*(f*2+i*2)+(i*e-f*h)*c*2)*((i*e-f*h)*g+(i*d+g*f)*h)-((i*e-f*h)*(g*c-i*a)+(i*d+g*f)*(i*b+c*h))*(h*(f*2+i*2)+(i*e-f*h)*2))/(((i*e-f*h)*(g*c-i*a)+(i*d+g*f)*(i*b+c*h))*(i*e-f*h)*i)}" );
  /**** taylor suites ****/
  Check( "TAYLOR(COS(x),x,0,1)", "1" );
  Check( "TAYLOR(COS(x),x,0,3)", "1-x^2/2" );
  Check( "TAYLOR(EXP(x),x,0,5)", "1+x+x^2/2+x^3/6+x^4/24+x^5/120" );

  /****** fonction *********/
  Initialize();
  CElementDataBase db( "test_function", this );
  db.Check( "a:=4 SIMPLIFY(a-3)", "1" );
  db.Check( "a:=6 SIMPLIFY(a-5)", "1" );
  db.Check( "f(x):=4*x SIMPLIFY(f(z)-4*z+1)", "1" );
  db.Check( "SIMPLIFY(f(z+z)-8*z)", "0" );

  /****** evaluator ******/
  Initialize();
  SetValue( GetElement( "a" ), CValue( 4 ) );
  SetValue( GetElement( "b" ), CValue( 8 ) );
  SetValue( GetElement( "c" ), CValue( 9 ) );

  Check( "a+b-c+2", CValue( 5 ) );
  Check( "a>b?2:3", CValue( 3 ) );

  DisplayStats();

}

#endif

