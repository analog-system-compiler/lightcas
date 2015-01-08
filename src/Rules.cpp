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

#include "Rules.h"

const SProperties CRules::m_FunctionProperties[] =
{
  { "ADD",   2, &CEvaluator::Add           },
  { "SUB",   2, &CEvaluator::Sub           },
  { "OR",    2, &CEvaluator::Or            },
  { "LOR",   2, &CEvaluator::LOr           },
  { "XOR",   2, &CEvaluator::Xor           },
  { "MUL",   2, &CEvaluator::Mul           },
  { "DIV",   2, &CEvaluator::Div           },
  { "AND",   2, &CEvaluator::And           },
  { "LAND",  2, &CEvaluator::LAnd          },
  { "EQ",    2, &CEvaluator::Equal         },
  { "NEQ",   2, &CEvaluator::NotEqual      },
  { "LTE",   2, &CEvaluator::LowerOrEqual  },
  { "SHL",   2, &CEvaluator::ShiftLeft     },
  { "LT",    2, &CEvaluator::Lower         },
  { "GTE",   2, &CEvaluator::GreaterOrEqual},
  { "SHR",   2, &CEvaluator::ShiftRight    },
  { "GT",    2, &CEvaluator::Greater       },
  { "MIN",   2, &CEvaluator::Min           },
  { "MAX",   2, &CEvaluator::Max           },
  { "MOD",   2, &CEvaluator::Mod           },
  { "POW",   2, &CEvaluator::Pow           },
  { "NEG",   1, &CEvaluator::Neg           },
  { "INV",   1, &CEvaluator::Inv           },
  { "NOT",   1, &CEvaluator::Not           },
  { "ID",    1, &CEvaluator::Id            },
  { "BOOL",  1, &CEvaluator::Bool          },
  { "LNOT",  1, &CEvaluator::LNot          },
  { "SIN",   1, &CEvaluator::Sin           },
  { "COS",   1, &CEvaluator::Cos           },
  { "TAN",   1, &CEvaluator::Tan           },
  { "ASIN",  1, &CEvaluator::Asin          },
  { "ACOS",  1, &CEvaluator::Acos          },
  { "ATAN",  1, &CEvaluator::Atan          },
  { "SINH",  1, &CEvaluator::SinH          },
  { "COSH",  1, &CEvaluator::CosH          },
  { "TANH",  1, &CEvaluator::TanH          },
  { "ASINH", 1, &CEvaluator::AsinH         },
  { "ACOSH", 1, &CEvaluator::AcosH         },
  { "ATANH", 1, &CEvaluator::AtanH         },
  { "EXP",   1, &CEvaluator::Exp           },
  { "LOG",   1, &CEvaluator::Ln            },
  { "LOG10", 1, &CEvaluator::Log           },
  { "SQRT",  1, &CEvaluator::Sqrt          },
  { "SQR",   1, &CEvaluator::Sqr           },
  { "FACT",  1, &CEvaluator::Fact          },
  { "ABS",   1, &CEvaluator::Abs           },
  { "FLOOR", 1, &CEvaluator::Floor         },
  { "CEIL",  1, &CEvaluator::Ceil          },
  { "SQUARE", 1, &CEvaluator::Square        },
  { "RAND",  0, &CEvaluator::Rand          },
  { "IF2",   2, &CEvaluator::If            },
  { "IF",    2, &CEvaluator::If            },
  { "CONCAT", 2, &CEvaluator::Concat        },
  { "SET",   2, NULL                       },
  { "AT",    2, NULL                       },
  { "VECT",  1, NULL                       }

};

const unsigned CRules::m_FunctionPropertiesSize = sizeof( CRules::m_FunctionProperties ) / sizeof( SProperties );

const char CRules::m_Functions[] =
{

  "ADD(a,b)"
  "SUB(a,b)"
  "MUL(a,b)"
  "PAR(a,b)"
  "DIV(a,b)"
  "POW(a,b)"
  "OR(a,b)"
  "XOR(a,b)"
  "LOR(a,b)"
  "AND(a,b)"
  "LAND(a,b)"
  "PADD(a,b)"
  "PMUL(a,b)"
  "EQ(a,b)"
  "NEQ(a,b)"
  "LTE(a,b)"
  "SHL(a,b)"
  "LT(a,b)"
  "GTE(a,b)"
  "SHR(a,b)"
  "GT(a,b)"
  "MIN(a,b)"
  "MAX(a,b)"
  "DB(a,b)"
  "PHASE(a,b)"
  "MOD(a,b)"
  "TED(a,b)"
  "POLY(a,b)"
  "POLY_POW(a,b)"
  "IPOLY(a,b)"
  "IPOLY2(a,b)"
  "IPOLY3(a,b)"
  "IPOLY4(a,b)"
  "TO_POLY_TED(a,b)"
  "IPOLY_TED(a,b)"
  "AT(a,b)"
  "CHOOSEVAR(a,b)"
  "CHOOSEVARDIV(a,b)"
  "SET(a,b)"
  "CONCAT(a,b)"
  "DER(a,b)"
  "INVAT(a,b)"
  "SIMPLIFY2(a,b)"
  "NORM(a,b)"
  "NORM2(a,b)"
  "RANK(a,b)"
  "SUBST(a,b)"
  "IF(a,b)"
  "IF2(a,b)"
  "MON(a,b)"
  "MONADD(a,b)"
  "POLYSTORE(a,b)"
  "MONMUL(a,b)"
  "MONDIV(a,b)"
  "MONPOW(a,b)"
  "REDUCE_COMPLEX(a)"
  "MONNEG(a)"
  "POLY_TED(a)"
  "GET_POWER(a,b)"

  //Unary
  "NEG(a)"
  "INV(a)"
  "NOT(a)"
  "ID(a)"
  "BOOL(a)"
  "LNOT(a)"
  "SIN(a)"
  "COS(a)"
  "TAN(a)"
  "ASIN(a)"
  "ACOS(a)"
  "ATAN(a)"
  "SINH(a)"
  "COSH(a)"
  "TANH(a)"
  "ASINH(a)"
  "ACOSH(a)"
  "ATANH(a)"
  "EXP(a)"
  "LOG(a)"
  "LOG10(a)"
  "SQRT(a)"
  "SQR(a)"
  "FACT(a)"
  "ABS(a)"
  "FLOOR(a)"
  "CEIL(a)"
  "RE(a)"
  "IM(a)"
  "SQUARE(a)"
  "RECT(a)"
  "CONST(a)"
  "CONSTINT(a)"
  "ELEM(a)"
  "VECT(a)"
  "VSIZE(a)"
  "ERROR(a)"
  "SIMPLIFY(a)"
  "GETVAR(a)"
  "SYSTEM_SOLVE(a)"
  "SYSTEM_SOLVE2(a)"
  "SYSTEM_SOLVE3(a)"
  "SYSTEM_SOLVE4(a)"
  //"SYSTEM_SOLVE5(a)"
  "SYSTEM_AUTO_SOLVE(a)"
  "SYSTEM_AUTO_SOLVE2(a)"
  "SYSTEM_AUTO_SOLVE3(a)"
  "SYSTEM_AUTO_SOLVE4(a)"
  "SOLVE(a)"
  "SOLVE2(a)"
  "REDUCE(a)"
  "TAYLOR(a)"
  "DERN(a)"

  //void
  "RAND()"

};

const char CRules::m_FunctionSymbol[] = // precedence is defined here
{
  // lowercase: new precedence = precedence + 1
  // uppercase: new precedence = 0
  "a,b       =>  a b CONCAT;"
  "a:=b      =>  a b SET;"
  "a?B:C     =>  a b c CONCAT IF;"
  "a|b       =>  a b OR;"
  "a~b       =>  a b XOR;"
  "a+b       =>  a b ADD;"
  "a-b       =>  a b SUB;"
  "a&b       =>  a b AND;"
  "a*b       =>  a b MUL;"
  "a/b       =>  a b DIV;"
  "a^b       =>  a b POW;"
  "a<<b      =>  a b SHL;"
  "a>>b      =>  a b SHR;"
  "a==b      =>  a b EQ;"
  "a<>b      =>  a b NEQ;"
  "a<=b      =>  a b LTE;"
  "a<b       =>  a b LT;"
  "a>=b      =>  a b GTE;"
  "a>b       =>  a b GT;"
  "a||b      =>  a b LOR;"
  "a&&b      =>  a b LAND;"
  "a[B]      =>  a b AT;"
  "-a        =>  a NEG;"
  "/a        =>  a INV;"
  "!a        =>  a LNOT;"
  "~a        =>  a NOT;"
  "a!        =>  a FACT;"
  "{A}       =>  a VECT;"
};

const char CRules::m_AlgebraRulePolynom[] =

  //Mult
  "MONMUL( MON(a,b),MON(c,d))                      := MON(a*c,b+d);"
  "MONMUL( a,POLYSTORE(b,MON(c,d)) )               := MONADD(MONMUL(a,b),MONMUL(a,MON(c,d)));"
  "MONMUL( POLYSTORE(a,MON(b,c)),d )               := MONADD(MONMUL(a,d),MONMUL(d,MON(b,c)));"

  //Add
  "MONADD( MON(a,b),MON(0,c)      )                := MON(a,b);"
  "MONADD( MON(0,c),MON(a,b)      )                := MON(a,b);"
  "MONADD( MON(a,b),MON(c,b)      )                := MON(a+c,b);"
  "MONADD( MON(a,b),MON(c,d)      )                := POLYSTORE(MON(a,b),MON(c,d));"
  "MONADD( POLYSTORE(a,MON(b,c)),MON(d,c)  )       := POLYSTORE( a,MON(d+b,c));"
  "MONADD( POLYSTORE(a,b),MON(d,e)  )              := POLYSTORE( MONADD(a,MON(d,e)), b);"
  "MONADD( a,POLYSTORE(b,c)  )                     := MONADD( MONADD(a,b), c );"

  //Neg
  "MONNEG( MON(a,b))             := MON(-a,b);"
  "MONNEG( POLYSTORE(a,b))       := POLYSTORE(MONNEG(a),MONNEG(b));"
  "MONNEG( MONDIV(a,b))          := MONDIV(MONNEG(a),b);"

  //Div
  "MONDIV( a,MON(1,0))           := a;"
#if 0
  "MONDIV( a,MON(b,0))           := NONE;"
  "MONDIV( MON(a,b),MON(c,0))    := MON(a/c,b);"
  "MONDIV( a,MON(b,c))           := MONDIV( MONMUL(a,MON(1,-c)), MON(b,0) );"
#else
  "MONDIV( a,MON(b,c))           := MONMUL(a,MON(1/b,-c));"
#endif

  //Pow
  "MONPOW( MON(a,b), c )         := MON(a^c,b*c);"
  "MONPOW( a, 1 )                := a;"
  "MONPOW( a,CONSTINT(e) )          := MONMUL( MONPOW(a,e-1), a );"
  "MONPOW( a,-CONSTINT(e) )         := MONDIV( MON(1,0), MONPOW(a,e) );"

  //Fract
  "MONADD(a,MONDIV(b,c))            := MONDIV( MONADD( MONMUL(a,c) ,b ), c );"                        //a+b/c   := (a*c+b)/c;"
  "MONADD(MONDIV(a,c),b)            := MONDIV( MONADD( MONMUL(b,c) ,a ), c );"                        //"a/c+b   := (a+b*c)/c;"
  "MONADD(MONDIV(a,b),MONDIV(c,d))  := MONDIV( MONADD( MONMUL(a,d) , MONMUL(b,c) ), MONMUL( b,d ) );" //"a/b+c/d := (a*d+b*c)/(b*d);"
  "MONADD(MONDIV(a,b),MONDIV(c,b))  := MONDIV( MONADD(a,c), b );"                                     //"a/b+c/b := (a+c)/b;"
  "MONMUL(a,MONDIV(b,c))            := MONDIV( MONMUL(a,b) ,c );"                                     //"a*b/c   := (a*b)/c;"
  "MONMUL(MONDIV(a,c),b)            := MONDIV( MONMUL(a,b) ,c );"                                     //"a/c*b   := (a*b)/c;"
  "MONMUL(MONDIV(a,b),MONDIV(c,d))  := MONDIV( MONMUL(a,c) , MONMUL(b,d) );"                          //"a/b*c/d := (a*c)/(b*d);"
  "MONDIV(MONDIV(a,b),c)            := MONDIV( a,MONMUL(c,b ) );"                                     //"(a/b)/c := a/(c*b);"

  //Poly
  "POLY(v,v)             := MON(1,1);"
  "POLY(a,v)             := MON(a,0);"
  "POLY(-a,v)            := MONNEG( POLY(a,v) );"
  "POLY(/a,v)            := MONDIV( MON(1,0)  , POLY(a,v)    );"
  "POLY(a+b,v)           := MONADD( POLY(a,v) , POLY(b,v)    );"
  "POLY(a-b,v)           := MONADD( POLY(a,v) , MONNEG(POLY(b,v)));"
  "POLY(a*b,v)           := MONMUL( POLY(a,v) , POLY(b,v)    );"
  "POLY(a/b,v)           := MONDIV( POLY(a,v) , POLY(b,v)    );"
  "POLY(a^b,v)           := MONPOW( POLY(a,v) , SIMPLIFY(b)  );"
  "POLY(a[b],v)          := POLY(a,v)[SIMPLIFY(b)];"

  //IPoly
  "IPOLY2(MON(a,b),v)             := a*v^b;"
  "IPOLY2(POLYSTORE(a,b),v)       := IPOLY2(a,v)+IPOLY2(b,v);"

  //IPoly if simplication failure
  "IPOLY2(MONADD(a,b),v)           := IPOLY(a,v)+IPOLY(b,v);"
  "IPOLY2(MONDIV(a,b),v)           := IPOLY(a,v)/IPOLY(b,v);"
  "IPOLY2(MONMUL(a,b),v)           := IPOLY(a,v)*IPOLY(b,v);"
  "IPOLY2(MONPOW(a,b),v)           := IPOLY(a,v)^b;"
  "IPOLY2(a[b],v)                  := IPOLY(a,v)[b];"

  "IPOLY(a,j)                     := IPOLY2(REDUCE_COMPLEX(a),j);"
  "IPOLY(a,v)                     := IPOLY2(a,v);"

  "REDUCE_COMPLEX(MON(a,0))         := MON(a,0);"
  "REDUCE_COMPLEX(MON(a,1))         := MON(a,1);"
  "REDUCE_COMPLEX(MON(a,CONST(b)))  := REDUCE_COMPLEX(MON(-a,b-2));"
  "REDUCE_COMPLEX(MON(a,-CONST(b))) := REDUCE_COMPLEX(MON(-a,2-b));"
  "REDUCE_COMPLEX(POLYSTORE(a,b))   := MONADD(REDUCE_COMPLEX(a),REDUCE_COMPLEX(b));"
  "REDUCE_COMPLEX(MONDIV(a,b))      := MONDIV(REDUCE_COMPLEX(a),REDUCE_COMPLEX(b));"

  "SIMPLIFY(MON(a,b))               := MON(SIMPLIFY(a),b);"
  "SIMPLIFY(POLYSTORE(a,b))         := POLYSTORE(SIMPLIFY(a),SIMPLIFY(b));"

  "RE(POLYSTORE(a,b))                  := RE(a)+RE(b);"
  "RE(MON(a,1))                        := 0;"
  "RE(MON(b,0))                        := b;"

  "IM(POLYSTORE(a,b))                  := IM(a)+IM(b);"
  "IM(MON(a,1))                        := a;"
  "IM(MON(b,0))                        := 0;"

  "GET_POWER(POLYSTORE(a,b),c)            := GET_POWER(a,c)+GET_POWER(b,c);"
  "GET_POWER(MON(a,b),b)                  := a;"       
  "GET_POWER(a,b)                         := 0;"       

  //"IPOLY4(MON(a,b),q)                  := q+b/a;"
  "NORM2(a,q)                          := IPOLY(POLY(a,q),q);"

  "NORM(a,q)                           := IPOLY3(POLY(a,q),q);"
  "IPOLY3(MON(a,b),v)                  := (a^(1/b)*v)^b;"
  "IPOLY3(POLYSTORE(a,b),v)            := IPOLY3(a,v)+IPOLY3(b,v);"
  "IPOLY3(MONDIV(a,b),v)               := IPOLY3(MONDIV(a,GET_POWER(b,0)),v)/IPOLY3(MONDIV(b,GET_POWER(b,0)),v);"
  ;

const char CRules::m_AlgebraRuleAcrossFunct[] =
  "POLY(COS(a),1)           := MON(COS( SIMPLIFY(a)),0);"
  "POLY(SIN(a),1)           := MON(SIN( SIMPLIFY(a)),0);"
  "POLY(TAN(a),1)           := MON(TAN( SIMPLIFY(a)),0);"
  "POLY(ACOS(a),1)           := MON(ACOS( SIMPLIFY(a)),0);"
  "POLY(ASIN(a),1)           := MON(ASIN( SIMPLIFY(a)),0);"
  "POLY(ATAN(a),1)           := MON(ATAN( SIMPLIFY(a)),0);"

  "POLY(COSH(a),1)           := MON(COSH( SIMPLIFY(a)),0);"
  "POLY(SINH(a),1)           := MON(SINH( SIMPLIFY(a)),0);"
  "POLY(TANH(a),1)           := MON(TANH( SIMPLIFY(a)),0);"
  "POLY(ACOSH(a),1)           := MON(ACOSH( SIMPLIFY(a)),0);"
  "POLY(ASINH(a),1)           := MON(ASINH( SIMPLIFY(a)),0);"
  "POLY(ATANH(a),1)           := MON(ATANH( SIMPLIFY(a)),0);"

  "POLY(EXP(a),1)           := MON(EXP( SIMPLIFY(a) ),0);"
  "POLY(FACT(a),1)          := MON(FACT(SIMPLIFY(a) ),0);"
  "POLY(LOG(a),1)           := MON(LOG( SIMPLIFY(a) ),0);"
  "POLY(LOG10(a),1)           := MON(LOG10( SIMPLIFY(a)),0);"
  "POLY(VECT(a),1)           := MON(VECT( SIMPLIFY(a) ),0);"

  "POLY(EQ(a,b),1)           := MON(EQ( SIMPLIFY(a), SIMPLIFY(b)),0);"
  "POLY(NEQ(a,b),1)           := MON(NEQ( SIMPLIFY(a), SIMPLIFY(b)),0);"
  "POLY(GT(a,b),1)           := MON(GT( SIMPLIFY(a), SIMPLIFY(b)),0);"
  "POLY(GTE(a,b),1)           := MON(GTE( SIMPLIFY(a), SIMPLIFY(b)),0);"
  "POLY(LT(a,b),1)           := MON(LT( SIMPLIFY(a), SIMPLIFY(b)),0);"
  "POLY(LTE(a,b),1)           := MON(LTE( SIMPLIFY(a), SIMPLIFY(b)),0);"
  "POLY(SHL(a,b),1)           := MON(SHL( SIMPLIFY(a), SIMPLIFY(b)),0);"
  "POLY(SHR(a,b),1)           := MON(SHR( SIMPLIFY(a), SIMPLIFY(b)),0);"
  "POLY(CONCAT(a,b),1)          := MON(CONCAT( SIMPLIFY(a), SIMPLIFY(b)),0);"
  ;

const char CRules::m_AlgebraRuleMisc[] =
  //"SOLVE2(a,b,v)                      := SOLVE2(a,v),SOLVE2(b,v);"
  //"SOLVE(a,v)                         := SOLVE2(POLY(a,v),v);"
  //"SOLVE(a)                           := SOLVE(a,GETVAR(a));"

  "CONCAT(CONST(a),CONST(b)) := NONE;" // no simplification for concatenation op

  "--a          := a  ;"
  "-0           := 0  ;"
  "-CONST(a)    := NONE;"

  "a==a         :=1;"
  "a>=a         :=1;"
  "a<=a         :=1;"
  "a<>a         :=0;"
  "a>a          :=0;"
  "a<a          :=0;"
  "!0           :=1;"
  "!1           :=0;"
  "!(a==b)      := a<>b  ;"
  "!(a<>b)      := a==b  ;"
  "!(a<=b)      := a>b  ;"
  "!(a<b)       := a>=b  ;"
  "!(a>=b)      := a<b  ;"
  "!(a>b)       := a<=b  ;"

  "SIN(-a)      := -SIN(a)  ;"
  "COS(-a)      := COS(a)  ;"
  //"SIN(a+PI/2)  := COS(a)  ;"
  //"COS(a+PI/2)  := -SIN(a)  ;"
  //"SIN(PI/2+a)  := COS(a)  ;"
  //"COS(PI/2+a)  := -SIN(a)  ;"
  "EXP(-a)      := 1/EXP(a)  ;"

  "0+b        := b  ;"
  "a+0        := a  ;"
  "(-a+-b)      := -(a+b)  ;"
  "(-a+b)      := b-a  ;"
  "a+-b        := a-b  ;"
  "SIN(a)+SIN(b)    := COS((a-b)/2)*SIN((a+b)/2)*2  ;"
  "LOG10(a)+LOG10(b)  := LOG10(a*b)  ;"

  "0-b        := -b  ;"
  // "a-a        := 0  ;" pbm with vector
  "a-0        := a  ;"
  "(-a--b)        := b-a  ;"
  "(-a-b)        := -(a+b)  ;"
  "a--b            := a+b  ;"
  "SIN(a)-SIN(b)    := COS((a+b)/2)*SIN((a-b)/2)*2  ;"
  "LOG10(a)-LOG10(b)  := LOG10(a/b)  ;"

  "0*b        := 0  ;"
  "a*0        := 0  ;"
  "1*b        := b  ;"
  "a*1        := a  ;"
  "(-a*-b)        := a*b  ;"
  "(-a*b)        := -(a*b)  ;"
  "a*-b        := -(a*b)  ;"
  "a*/b             := (a/b);" //doesn't work
  "/a*b            := (b/a);"
  "/a*/b            := /(b*a);"

  // "(b/c)*a            := (b*a)/c  ;"
  // "a*(b/c)            := (a*b)/c  ;"
  //  "1/a*1/b      := 1/(a*b)  ;"
  "a*1/b            := a/b  ;"
  "1/a*b            := b/a  ;"
  "EXP(a)*EXP(b)    := EXP(a+b)  ;"
  "SQRT(a)*SQRT(b)  := SQRT(a*b)  ;"
  "SQR(a)*SQR(b)    := SQR(a*b)  ;"

  "0/b        := 0  ;"
  //"/b            := 1/b  ;"
  "a/1        := a  ;"
  //"-a/-b        := a/b  ;"
  "-a/b        := -(a/b)  ;"
  "a/-b        := -(a/b)  ;"
  //"1/a/1/b      := a/b  ;"
  //"a/1/b        := a*b  ;"
  "(a/b)/c        := a/(b*c)  ;"

  "LOG(EXP(1))      := 1;"
  "EXP(a)/EXP(b)    := EXP(a-b)  ;"
  "SQRT(a)/SQRT(b)  := SQRT(a/b)  ;"
  "SQRT(-CONST(a))  := SQRT(a)*j;"
  "SQR(a)/SQR(b)    := SQR(a/b)  ;"

  "a^0              := 1  ;"
  "0^b              := 0  ;"
  "1^b              := 1  ;"
  "a^1              := a  ;"
  "a^-b             := 1/(a^b)  ;"
  "x^a*x^b          := x^(a+b);"
  "x^a/x^b          := x^(a-b);"
  "(x^a)^b          := x^(a*b);"

  //"a*(a^b)              := a^(b+1)  ;"
  //"(a^b)*a              := a^(b+1)  ;"
  //"a*(a^b)              := a^(b+1)  ;"
  //"(a*b)^c          := (a^c)*(b^c)  ;"

  "BOOL(a==b) := (a==b);"
  "BOOL(a<>b) := (a<>b);"
  "BOOL(a>=b) := (a>=b);"
  "BOOL(a<=b) := (a<=b);"
  "BOOL(a>b)  := (a>b);"
  "BOOL(a<b)  := (a<b);"
  "BOOL(!a)   := !a;"
  "BOOL(a)    := !(a==0);"

  "RE(a)                     := RE(REDUCE_COMPLEX(POLY(a,j)));"
  "IM(a)                     := IM(REDUCE_COMPLEX(POLY(a,j)));"

  "IF2(0,(a,b)) := b;"
  "IF2(1,(a,b)) := a;"
  "IF(c,(a,b))  := IF2(BOOL(c),(a,b));"

  "POLY((a,b),v)         := (POLY(a,v),POLY(b,v));" //parenthesis are important there!!!
  "POLY({a},v)           := {POLY(a,v)};"  

  "IPOLY((a,b),v)                         := (IPOLY(a,v),IPOLY(b,v));" //parenthesis are important there!!!
  "IPOLY({a},v)                           := {IPOLY(a,v)};"

  "SIMPLIFY(ELEM(a))                   := a ;"
  "SIMPLIFY(-ELEM(a))                  := -a ;"
  "SIMPLIFY(CONST(a))                  := a ;"
  "SIMPLIFY(-CONST(a))                 := -a ;"
  "SIMPLIFY(a)                         := SIMPLIFY2(a,GETVAR(a));"
  //"SIMPLIFY(a/b)                       := SIMPLIFY(a)/SIMPLIFY(b);"

  "SIMPLIFY2(a,1)                      := IPOLY(POLY(a,1),1);"
  "SIMPLIFY2(a,v)                      := IPOLY(SIMPLIFY(POLY(a,v)),v);"

  "GETVAR(a+b)                         := CHOOSEVAR( GETVAR(a), GETVAR(b) );"
  "GETVAR(a-b)                         := CHOOSEVAR( GETVAR(a), GETVAR(b) );"
  "GETVAR(a*b)                         := CHOOSEVAR( GETVAR(a), GETVAR(b) );"
  "GETVAR(a/b)                         := CHOOSEVARDIV( GETVAR(a), GETVAR(b) );"
#if 1
  "GETVAR(a^b)                  := GETVAR(a);"
#else
  "GETVAR(a^CONST(b))                  := GETVAR(a);"
  "GETVAR(a^-CONST(b))                 := GETVAR(a);"
  //"GETVAR(a^b)                         := GETVAR(a^SIMPLIFY(b));"

#endif
  "GETVAR(-a)                          := GETVAR(a);"
  "GETVAR(ELEM(a))                     := a;"
  "GETVAR(a)                           := 1;"

  "CHOOSEVAR(1,b)                      := b;"
  "CHOOSEVAR(a,1)                      := a;"
  "CHOOSEVAR(a,b)                      := RANK(a,b);"
  "CHOOSEVARDIV(1,b)                   := b;"
  "CHOOSEVARDIV(a,b)                   := a;" //Not RANK there !!!
  ;

const char CRules::m_AlgebraRuleDerivals[] =
  "DERN(a,b,0)     := a;"
  "DERN(a,b,n)     := DERN(DER(a,b),b,n-1);"
  "DER(CONST(a),c) := 0;"
  "DER(c,c)        := 1;"
  "DER(a+b,c)      := DER(a,c)+DER(b,c);"
  "DER(a-b,c)      := DER(a,c)-DER(b,c);"
  "DER(a*b,c)      := b*DER(a,c)+DER(b,c)*a;"
  "DER(a/b,c)      := (b*DER(a,c)-DER(b,c)*a)/SQR(b);"
  "DER(a^b,c)      := DER(a,c)*b*a^(b-1);"
  "DER(-a,c)       := -DER(a,c);"
  "DER(/a,c)       := -DER(a,c)/SQR(a);"
  "DER(SIN(b),c)   := DER(b,c)*COS(b);"
  "DER(COS(b),c)   := -(DER(b,c)*SIN(b));"
  "DER(TAN(b),c)   := DER(b,c)*(1+TAN(b)^2);"
  "DER(ASIN(b),c)  := DER(b,c)/SQRT(1-b^2);"
  "DER(ACOS(b),c)  := -(DER(b,c)/SQRT(1-b^2));"
  "DER(ATAN(b),c)  := DER(b,c)/(1+b^2);"
  "DER(TANH(b),c)  := DER(b,c)*(1-TANH(b)^2);"
  "DER(EXP(b),c)   := DER(b,c)*EXP(b);"
  "DER(LOG(a),c)   := DER(a,c)/a;"
  ;

const char CRules::m_AlgebraRuleSystems[] =

   /* "POLY_TED(MON(a,0))                     := a;"
    "POLY_TED(MON(a,1))                     := TED(a,0);"
    "POLY_TED(MON(a,2))                     := TED(a,TED(0,0));"
    "POLY_TED(POLYSTORE(MON(a,1),MON(b,0))) := TED(a,b);"
    "POLY_TED(POLYSTORE(MON(a,2),b)         := TED(a,POLY_TED(b));"
    "POLY_TED(POLYSTORE(b,MON(a,2))         := POLY_TED(POLYSTORE(MON(a,2),b);"
    "POLY_TED(POLYSTORE(b,MON(a,1))         := POLY_TED(POLYSTORE(MON(a,1),b);"
    "POLY_TED(a)                            := ERROR(1);" //power > 1*/  
    "TED(TED(0,a),b)                        := TED(a,b);"
    "POLY_TED(a)                            := TED(TED(GET_POWER(a,2),GET_POWER(a,1)),GET_POWER(a,0));"

    "TO_POLY_TED((a,b),v)                   := (TO_POLY_TED(a,v),TO_POLY_TED(b,v));"
    "TO_POLY_TED(a,v)                       := POLY_TED(POLY(a,v));"

    "IPOLY_TED((a,b),v)                     := (IPOLY_TED(a,v),IPOLY_TED(b,v));" //parenthesis are important there!!!
    "IPOLY_TED(TED(a,b),v)                  := a*v+b;"

    "REDUCE(    TED(a,b),TED(0,d))    := (TED(a,b),TED(0,d));"
    "REDUCE(    TED(a,b),TED(c,d))    := (TED(c,d),TED(0,c*b-d*a));"
    "REDUCE( e, TED(a,b),TED(0,d))    := (REDUCE( e, TED(a,b)) , TED(0,d));"
    "REDUCE( e, TED(a,b),TED(c,d))    := (REDUCE( e, TED(c,d)) , TED(0,c*b-d*a));"

  "SOLVE2(TED(TED(a,b),c)/e)         := SOLVE2(TED(TED(a,b),c));"
  "SOLVE2(TED(TED(a,b),c))           := { SIMPLIFY((-b-SQRT(b^2-4*a*c))/(2*a)), SIMPLIFY((-b+SQRT(b^2-4*a*c))/(2*a)) };"
  "SOLVE2(TED(a,b)/e)                := SOLVE2(TED(a,b));"
  "SOLVE2(TED(a,b))                  := SIMPLIFY((-b)/a);"  

  "SYSTEM_SOLVE3({a},{x,y})          := SYSTEM_SOLVE3({IPOLY_TED(REDUCE(TO_POLY_TED(a,y)),y)},{x});"
  "SYSTEM_SOLVE3({a},{x})            := IPOLY_TED(REDUCE(TO_POLY_TED(a,x)),x);"
  "SYSTEM_SOLVE4({a,b},{x,y})        := (SYSTEM_SOLVE4({a},{x}),SYSTEM_SOLVE4({b},{y}));"
  "SYSTEM_SOLVE4({a},{x})            := SOLVE(a,x);"

  "SYSTEM_AUTO_SOLVE4({a,b})         := (SYSTEM_AUTO_SOLVE4({a}),SYSTEM_AUTO_SOLVE4({b}));"  
  "SYSTEM_AUTO_SOLVE4({a})           := NORM2(a,GETVAR(a));"
  "SYSTEM_AUTO_SOLVE3({a,b})         := SYSTEM_SOLVE3({a,b},{GETVAR(b)});"
  "SYSTEM_AUTO_SOLVE2({a},{c,d})     := SYSTEM_AUTO_SOLVE2({SYSTEM_AUTO_SOLVE3({a})},{c});"
  "SYSTEM_AUTO_SOLVE2({a},{c})       := SYSTEM_AUTO_SOLVE3({a});"

  //entry points
  "SOLVE(a,v)                        := SOLVE2(TO_POLY_TED(a,v));"
  "SYSTEM_SOLVE ({a},{x})            := {SYSTEM_SOLVE4({SYSTEM_SOLVE3({a},{x})},{x})};"
  "SYSTEM_AUTO_SOLVE({a})            := {SYSTEM_AUTO_SOLVE4({SYSTEM_AUTO_SOLVE2({a},{a})})};"
  ;

const char CRules::m_AlgebraRuleLogic[] =
  "0|b    := b  ;"
  "a|0    := a  ;"
  "~0|b    := ~0  ;"
  "a|~0    := ~0  ;"
  "0&b    := 0  ;"
  "a&0    := 0  ;"
  "a&~0     := a    ;"
  "~0&b     := b    ;"

  "POLY(a&b,v)        := POLY(a,v)&POLY(b,v);"
  "POLY(a|b,v)        := POLY(a,v)|POLY(b,v);"
  "POLY(~a,v)         := ~POLY(a,v);"
  " TED(1,0)/TED(0,1) |  TED(1,0)/TED(0,1)  := TED(1,0)/TED(0,1);"
  " TED(0,a)/TED(0,1) |  TED(0,b)/TED(0,1)  := TED(0,a|b)/TED(0,1);"
  " TED(1,0)/TED(0,1) &  TED(1,0)/TED(0,1)  := TED(1,0)/TED(0,1);"
  " TED(0,a)/TED(0,1) &  TED(0,b)/TED(0,1)  := TED(0,a&b)/TED(0,1);"
  "~TED(0,a)/TED(0,1)                       := TED(0,~a)/TED(0,1);"
  "~TED(1,0)/TED(0,1) |  TED(1,0)/TED(0,1)  := TED(0,1)/TED(0,1);"
  " TED(1,0)/TED(0,1) | ~TED(1,0)/TED(0,1)  := TED(0,1)/TED(0,1);"
  "~TED(1,0)/TED(0,1) &  TED(1,0)/TED(0,1)  := TED(0,0)/TED(0,1);"
  " TED(1,0)/TED(0,1) & ~TED(1,0)/TED(0,1)  := TED(0,0)/TED(0,1);"
  ;

const char CRules::m_AlgebraRuleVectors[] =
  "(a,b)+(c,d)               := (a+c,b+d);"
  "{a}+{b}                   := {a+b};"
  "(a,b)-(c,d)               := (a-c,b-d);"
  "{a}-{b}                   := {a-b};"
  "VSIZE({a,b})              := VSIZE({a})+1;"
  "VSIZE(a)                  := 1;"
  "{a}[CONST(c)]             := INVAT({a}, VSIZE({a}) -1 - c );"
  "INVAT({a,b},0)            := b;"
  "INVAT({a,b},CONST(c))     := INVAT({a},c-1);"
  "INVAT({a},0)              := a;"
  "INVAT({a},-CONST(b))      := ERROR(0);"
  ;

const char CRules::m_AlgebraRuleTaylorSeries[] =
  "TAYLOR(a,x,xo,0) := SUBST(a,x,xo);"
  "TAYLOR(a,x,xo,n) := TAYLOR(a,x,xo,n-1)+SUBST(DERN(a,x,n),x,xo)*(x-xo)^n/n!;"
  ;
