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

#ifdef _TEST

#include "Debug.h"
#include "MathExpression.h"
#include "ElementDataBase.h"
#include "Element.h"
#include "Parser.h"
#include "Display.h"

void CElementDataBase::Check(const char *s1, const char *s2)
{
  CDisplay ds(true);
  CParser IC(s1);
  CMathExpression equ(this);
  
  ds.Log(LOG_INFO,"** Check %s == %s", s1, s2);

  if (equ.Parse(IC))
  {
    equ.OptimizeTree();
    equ.Display(ds);
    if (ds != s2)
    {
      ASSERT(false);
      ds.Log(LOG_ERR, "FAIL: %s => %s != %s", s1, ds.GetBufferPtr(), s2);
      return;
    }
  }
  else
  {
    ASSERT(false);
    ds.Log(LOG_ERR, "FAIL: Test exception: %s => %s", s1, s2);
    return;
  }
  ds.Log(LOG_INFO,"OK: %s => %s", s1, s2);
}

void CElementDataBase::CheckEval(const char *s1, const CValue &v1)
{
  CDisplay ds;
  CMathExpression equ(this);
  CParser IC(s1);
  equ.Parse(IC);
  equ.Evaluate();
  const CValue &v2 = GetEvaluator()->GetValue();
  if (v2.GetValue() != v1.GetValue())
  {
    ASSERT(false);
    ds.Log(LOG_ERR,"FAIL: %s => %g != %g", s1, v2.GetValue(), v1.GetValue());
    return;
  }
  ds.Log(LOG_INFO,"OK: %s => %g", s1, v1.GetValue());
}

void CElementDataBase::CheckSyntaxError(const char *s1)
{
  CParser IC(s1);
  CMathExpression equ(this);
  if (equ.Parse(IC))
  {
    CDisplay ds;
    ds.Log(LOG_ERR,"Syntax check detected: %s", s1);
  }
}

void CElementDataBase::DisplayStats()
{
  CDisplay ds;
  for (unsigned i = 0; i < m_ElementRefArray.GetSize(); i++)
  {
    CElement *e = m_ElementRefArray.GetAt(i);
    if (e)
    {
      ds.Log(LOG_INFO,"********************* #%04d %s *************************", i, e->GetName().GetBufferPtr());
      const CFunction *funct = e->GetFunction();
      for (unsigned j = 0; j < funct->m_AlgebraRuleArray.GetSize(); j++)
      {
        CAlgebraRule *rule = funct->m_AlgebraRuleArray.GetAt(j);
        ds.Clear();
        rule->Display(j, ds);
        ds.Log(LOG_INFO,"%4d\t%s", rule->m_AccessNb, ds.GetBufferPtr());
      }
    }
    else
    {
      ds.Log(LOG_INFO,"********************* #%04d No element *************************", i);
    }
  }
}

void CElementDataBase::Test()
{
  CDisplay ds;
  ds.Log(LOG_INFO,"Running tests...");

  /***** Some basic tests *****/
  CheckSyntaxError("sin(x");
  // CheckSyntaxError( "sin(x))" );
  CheckSyntaxError("x+");
  CheckSyntaxError("x-*2");
  CheckSyntaxError("{a,b,}");
  CheckSyntaxError("TED(a)");
#if 0
  Check( "10.500e-2", "0.105" );
  Check( "0.5E4", "5000" );
  Check( "0xAA", "170" );
  Check( "0b1010", "10" );
  //Check( "a b c", "a b c" );
  Check( "TED( a b )", "TED(a b)" );
  Check( "a-(b-c)", "a-(b-c)" );
  Check( "a^(b^c)", "a^(b^c)" );
  Check( "a/(b/c)", "a/(b/c)" );
  Check( "(a^b)^c", "(a^b)^c" );
  Check( "SIMPLIFY(0)", "0" );
  Check( "SIMPLIFY(1)", "1" );
  Check( "SIMPLIFY(-1)", "-1" );
  Check( "SIMPLIFY(-0)", "0" );
  Check( "SIMPLIFY(--2)", "2" );
  Check( "SIMPLIFY(2--2)", "4" );
  Check( "SIMPLIFY(-4-8)", "-12" );
  Check( "SIMPLIFY(3!)", "6" );
  Check( "SIMPLIFY(-3!)", "-6" );
  Check( "SIMPLIFY(-3!!)", "-720" );
  Check( "SIMPLIFY(2*2^2*2^2-32)", "0" );
  Check( "SIMPLIFY(a-a)", "0" );
  Check( "SIMPLIFY(a+a)", "2*a" );
  Check( "SIMPLIFY(a+a+a)", "3*a" );
  Check( "SIMPLIFY(a+b)", "a+b" );
  Check( "SIMPLIFY(a-b)", "a-b" );
  Check( "SIMPLIFY(a+a+b+b)", "2*a+2*b" );
  Check( "SIMPLIFY(a+a-(b+b))", "2*a-2*b" );
  Check( "SIMPLIFY(a-b+c-(c+a-b))", "0" );
  Check( "SIMPLIFY(a-(b-c))", "a-b+c" );
  Check( "SIMPLIFY(a-b-c)", "a-b-c" );
  Check( "SIMPLIFY((a+b)*(a-b))", "a^2-b^2" );
  Check( "SIMPLIFY(0*a+a*0+a*1-1*a+a+0-a-0+a^1-a+a^0-a/a+0/a)", "0" );
//  Check( "SIMPLIFY((2*x+3)*(10*x-5)/(2*x-1))", "15+10*x" );
  Check( "SIMPLIFY(2+2-4+0)", "0" );
  Check( "SIMPLIFY(-2*MIN(2+2, 5)*MAX(2, 3)+2^4+8)", "0" );
  Check( "SIMPLIFY(1-0*a*b)", "1" );
  Check( "SIMPLIFY(2*a-2*a)", "0" );
  Check( "SIMPLIFY(b*(a+2)-b*a-2*b)", "0" );
  Check( "SIMPLIFY(-(a-b)-(b-a))", "0" );
  Check( "SIMPLIFY(a/a-1)", "0" );
  Check( "SIMPLIFY(-b/a)", "-(b/a)" );
  Check( "SIMPLIFY(a*b/(a*b)-1)", "0" );
  Check( "SIMPLIFY(a*b/(a*b))", "1" );
  Check( "SIMPLIFY(a*b*c/(a*b*c))", "1" );
  Check( "SIMPLIFY(4*a*b*c/(4*a*b*c))", "1" );
  Check( "SIMPLIFY(4*a*b*c/(4*a*b*c)-1)", "0" );
  Check( "SIMPLIFY(a*b/(4*a*b*c)-1/(4*c))", "0" );
  Check( "SIMPLIFY((4*a*b)/(3*a*b*c))", "1.33333333333/c" ); //"4/(3*c)" );
  Check( "SIMPLIFY((4*c*b)/(3*a*b*c))", "1.33333333333/a" ); //"4/(3*a)" );
  Check( "SIMPLIFY((4*a*b*c+4)/(4*a*b*c))", "1+1/(c*b*a)" );
  Check( "SIMPLIFY(b/a+b/a)", "(2*b)/a" );
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
  Check( "SIMPLIFY(a*(a^b))", "a^(1+b)" );
  Check( "SIMPLIFY(x^(a*b)/x^(a*b))", "1" );
  Check( "SIMPLIFY(x^(a-a))", "1" );
  Check( "SIMPLIFY(x^a)", "x^a" );
  Check( "SIMPLIFY(x^(a+a))", "x^(2*a)" );
  Check( "SIMPLIFY((x+y)^(a+a))", "(x+y)^(2*a)" );
  Check( "SIMPLIFY(1+(x+y)^(a+a))", "1+(x+y)^(2*a)" );
  Check( "SIMPLIFY((x^a)^2)", "x^(2*a)" );
  Check( "SIMPLIFY(x^2.5)", "x^2.5" );
  Check( "SIMPLIFY((x+a)^2.5)", "(x+a)^2.5" );
  Check( "SIMPLIFY(2*x^a-2*x^a)", "0" );
  Check( "SIMPLIFY(x^(1+a)*x^(1-a))", "x^2" );
  Check( "SIMPLIFY((a-a+1)>(a-a))", "1" );
  Check( "SIMPLIFY({a+a,b+4*b,c-d})", "{2*a,5*b,c-d}" );
  Check( "SIMPLIFY(COS(b-b))", "1" );
  Check( "SIMPLIFY((a*1000*1000)/(b*1000*1000))", "a/b" );

  /****** Complex *********/
  Check( "SIMPLIFY(j*j)", "-1" );
  Check( "SIMPLIFY(j^2)", "-1" );
  Check( "SIMPLIFY(j^-2)", "-1" );
  Check( "SIMPLIFY(1+j-(1+j))", "0" );
  Check( "SIMPLIFY(j*j*j*j*j*j*j)", "-j" );
  Check( "SIMPLIFY(j*j+1)", "0" );
  Check( "SIMPLIFY(1/j+j)", "0" );
  Check( "SIMPLIFY(1+j+j^2+j^3)", "0" );
  Check( "SIMPLIFY(1+2*j+j^2+j^7)", "j" );
  Check( "SIMPLIFY(2/(1+j)-(1-j))", "0" );
  Check( "SIMPLIFY(2/((1+1/j)+(1+1/(1/j)))-1)", "0" );

  //FAIL!!Check( "SIN(PI)", "0" );
  //FAIL!!Check( "COS(PI/2)", "0" );
  Check( "SQRT(-25)", "5*j" );
  Check( "SIMPLIFY(SQRT(-25)^2+26)", "1" );

  Check( "SIMPLIFY(RE(4+j))", "4" );
  Check( "SIMPLIFY(-IM(1-5*j))", "5" );
  Check( "SIMPLIFY(RE(4*j^6+j))", "-4" );
  Check( "SIMPLIFY(-IM(1-5*j*(5*j^4)))", "25" );

  /******* In function reduction ******/
  Check( "SIMPLIFY((a+a)*COS(a+a))", "2*COS(2*a)*a" );
  Check( "SIMPLIFY(COS(2*(3*y-3*y+1)*x-x*y/y-x+x*y-x*y-1+1)-1)", "0" );
  Check( "SIMPLIFY((1+2+a+a-2*a)!-6)", "0" );

  /****** Derival *****/
  Check( "DER(SIN(4*a),a)", "4*COS(4*a)" );
  Check( "DER(COS(a*b),b)", "-((b*DER(a,b)+a)*SIN(a*b))" );
  Check( "DER(LOG(a*b+c),b)", "(b*DER(a,b)+a+DER(c,b))/(a*b+c)" );

  /****** Vector *********/
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
  Check( "5*{a,b,c}", "{5*a,5*b,5*c}" );
  Check( "{a,b,c}*5", "{a*5,b*5,c*5}" );
  Check( "SIMPLIFY({-2,2})", "{-2,2}" );
  Check( "MIN({1,2,3,4})", "1" );
  Check( "MIN({5,1,3,4})", "1" );
  Check( "MIN({5,3,1,4})", "1" );
  Check( "MIN({5,3,4,1})", "1" );
  Check( "MAX({1,2,3,4})", "4" );
  Check( "MAX({5,1,3,4})", "5" );
  Check( "MAX({1,5,1,4})", "5" );
  Check( "MAX({4,3,5,1})", "5" );

  /****** Trinary *********/
  Check( "SIMPLIFY(0 ? a : b)", "b" );
  Check( "SIMPLIFY(1 ? a : b)", "a" );
  Check( "SIMPLIFY(5 ? a : b)", "a" );
  Check( "SIMPLIFY(2>3 ? a : b)", "b" );
  Check( "SIMPLIFY(a<=a ? b : c)", "b" );

  /****** Logic *********/
  /*Check( "SIMPLIFY(a|0)", "a" );
  Check( "SIMPLIFY(a|~0)", "-1" );
  Check( "SIMPLIFY(a&0)", "0" );
  Check( "SIMPLIFY(a&~0)", "a" );
  Check( "SIMPLIFY((5<<2)|32)", "52" );*/
  Check( "SIMPLIFY(57>>3)", "7.125" );

  /********** Transforms **********/
  Check( "NORM((4+a*(6+32*a))/(2+a*(2+72*a)),a)", "(2+3*a+(4*a)^2)/(1+a+(6*a)^2)" );

  /*********** Assignment ***/
  CMathExpression equ0( this );
  equ0.Parse( "a+b" );
  GetElement( "y" )->SetEquation( equ0 );
  equ0.Parse( "a-b" );
  GetElement( "z" )->SetEquation( equ0 );
  Check( "SIMPLIFY(y-z)", "2*b" );

  /********** Equation solving **********/
  Initialize();
  Check( "SOLVE(x^2-4,x)", "{-2,2}" );
  Check( "SOLVE(x^2+4,x)", "{-(2*j),2*j}" );
  Check( "SIMPLIFY(SOLVE(x^2-4,x)[1]-2)", "0" );

  /***** System solving *****/
  Check( "SYSTEM_AUTO_SOLVE({x-y+1,x-y+1})", "{y-(x+1),0}" );
  Check( "SYSTEM_AUTO_SOLVE({x-y-z+0,x-y+z-2,x+y-z-2})", "{x-2,y-1,z-1}" );
  Check( "SYSTEM_AUTO_SOLVE({x+y+z-3,x-y+z-1,x+y-z-1})", "{x-1,y-1,z-1}" );
  Check( "SYSTEM_SOLVE({x+y+z-3,x-y+z-1,x+y-z-1},{x,y,z})",    "{1,1,1}" );
  Check( "SYSTEM_SOLVE({x-y-z+0,x-y+z-2,x+y-z-2},{x,y,z})",    "{2,1,1}" );
  Check( "SYSTEM_SOLVE({a*x-b*y-c*z+0,d*x-e*y+f*z-2,g*x+h*y-i*z-2},{x,y,z})", "{((i*e-h*f)*2*c+(i*2+2*f)*(h*c+i*b))/((i*e-h*f)*(g*c-i*a)+(i*d+g*f)*(h*c+i*b)),(((i*e-h*f)*2*c+(i*2+2*f)*(h*c+i*b))*(i*d+g*f)-((i*e-h*f)*(g*c-i*a)+(i*d+g*f)*(h*c+i*b))*(i*2+2*f))/(((i*e-h*f)*(g*c-i*a)+(i*d+g*f)*(h*c+i*b))*(i*e-h*f)),(((i*e-h*f)*2*c+(i*2+2*f)*(h*c+i*b))*((i*e-h*f)*g+(i*d+g*f)*h)-((i*e-h*f)*(g*c-i*a)+(i*d+g*f)*(h*c+i*b))*((i*e-h*f)*2+(i*2+2*f)*h))/(((i*e-h*f)*(g*c-i*a)+(i*d+g*f)*(h*c+i*b))*(i*e-h*f)*i)}" );
  /***** Determinant ******/
  //Check("MATRIX_GETVAR({-x+2*y+5*z, x+2*y+3*z, -2*x+8*y+10*z})", "{x,y,z}");
  Check( "VECT_REVERSE({ x,y,z,w})", "{w,z,y,x}" );
  Check( "VECT_REVERSE1( (a,b,c), (x,y,z,w))", "x,y,z,w,c,b,a" );
  Check( "VECT_REVERSE1( (a,b,c), x)",   "x,c,b,a" );
  Check( "VECT_REVERSE1( a, (x,y,z,w))", "x,y,z,w,a" );
  Check( "DET({3*x+4*y, 2*x+8*y},                     {x,y})",   "16" );
  Check( "DET({-x,         2*y+3*z, 8*y+10*z},        {x,y,z})",  "4" );
  Check( "DET({-x+2*y+5*z, 2*y+3*z, 8*y+10*z},        {x,y,z})",  "4" );
  Check( "DET({-x+2*y+5*z, x+2*y+3*z, -2*x+8*y+10*z}, {x,y,z})", "32" );
  Check( "DET({ x+2*y+5*z-1, -x+2*y+3*z+1, -2*x+8*y+10*z-2}, {1,x,y,z})", "32" );
  Check( "DET({ x+y+z+w, y+z+w, z+w, w}, {x,y,z,w})", "1" );
  Check( "DET({(L1_U+C1_U)-(V_U+V1_U)-R1_U,-(V1_I+C1_I),V1_I-V_I,V_I+R1_I,C1_I-L1_I,R1_U-10*R1_I,L1_U-0.001*0,C1_I-5e-07*0},{1,C1_I,L1_U,R1_I,R1_U,L1_I,V_I,V1_I,C1_U})", "0" );
  Check( "DET({(L1_U+C1_U)-(V_U+V1_U)-R1_U,-(V1_I+C1_I),V1_I-V_I,V_I+R1_I,C1_I-L1_I,R1_U-10*R1_I,L1_U-0.001*0,C1_I-5e-07*0},{1,R1_U,L1_U,R1_I,R1_U,L1_I,V_I,V1_I,C1_U})", "-C1_I" );
  Check( "DET({ {3,4} , {2,8} })", "16" );
  Check( "DET({ {-1,2,5} , {1,2,3} , {-2,8,10} })", "32" );

  /* Matrix inversion */
  Check( "MAT_INV( { {4,-2,3} , {8,-3,5} , {7,-2,4} })", "{{-2,2,-1},{3,-5,4},{5,-6,4}}" );
  Check( "MAT_INV( { {1,2} , {3,4} })", "{{-2,1},{1.5,-0.5}}" );
  Check( "MAT_INV( { 4 } )", "{4}" );

  /*** Matrixes ***/
  Check( "MAT_TRANSPOSE( { { 1 },{ 6 } } )", "{{1,6}}" );
  Check( "MAT_TRANSPOSE( { { 2,3 },{ 4,5 } } )", "{{2,4},{3,5}}" );
  Check( "MAT_TRANSPOSE( { { 2,3,5 },{ 4,5,6 } } )", "{{2,4},{3,5},{5,6}}" );
  Check( "MAT_MUL( { { 2 } }, { { 1 } } )", "{{2}}" );
  Check( "MAT_MUL( { { 2,3 } }, { { 1 },{ 6 } } )", "{{20}}" );
  Check( "MAT_MUL( { { 2,3 } }, { { 1,6 } ,{ 8,9 } } )", "{{26,39}}" );
  Check( "MAT_MUL( { { 2,3 },{ 4,5 } }, { { 1 } ,{ 8 } } )", "{{26},{44}}" );
  Check( "MAT_MUL( { { 2,3 },{ 4,5 } }, { { 1,6 } ,{ 8,9 } } )", "{{26,39},{44,69}}" );

  /**** Taylor suites ****/
  Check( "TAYLOR(COS(x),x,0,1)", "1" );
  Check( "TAYLOR(COS(x),x,0,3)", "1-x^2/2" );
  Check( "TAYLOR(EXP(x),x,0,5)", "1+x+x^2/2+x^3/6+x^4/24+x^5/120" );

  /****** Fonction *********/
  Initialize();
  CElementDataBase db( "test_function", this );
  db.Check( "y:=6", "y" );
  db.Check( "EXECUTE( y:=6 )", "y" );
  db.Check( "EXECUTE( y:=6; SIMPLIFY(y-5) )", "y;1" );
  db.Check( "EXECUTE( POLY(f(a),1) := POLYF( f( SIMPLIFY(a) ) ) )", "POLY(f(a),1)" ); //FIXME
  db.Check( "EXECUTE( f(x):=4*x; SIMPLIFY(f(z)-4*z+1) )", "f(x);1" );
  db.Check( "SIMPLIFY(f(z+z)-8*z)", "0" );
#endif
  CParser IC;
  IC.SetRootPath(m_RootPath);
  if (IC.LoadFromFile(m_RootPath + CString("tests.txt")))
  {
    AddAlgebraRuleTable(IC);
    IC.CloseFile();
  }
  else
  {
    IC.CopyBuffer("test description file not found.");
  }
  // CleanTempElements();
  // DisplayStats();
}

#endif
