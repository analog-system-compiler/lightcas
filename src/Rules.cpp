/*******************************************************************************/
/*  Copyright (C) 2014 The CASLight project                                      */
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
  { "ADD",   &CEvaluator::Add           },
  { "SUB",   &CEvaluator::Sub           },
  { "OR",    &CEvaluator::Or            },
  { "LOR",   &CEvaluator::LOr           },
  { "XOR",   &CEvaluator::Xor           },
  { "MUL",   &CEvaluator::Mul           },
  { "PAR",   &CEvaluator::Par           },
  { "DIV",   &CEvaluator::Div           },
  { "AND",   &CEvaluator::And           },
  { "LAND",  &CEvaluator::LAnd          },
  { "EQU",   &CEvaluator::Equal         },
  { "NEQ",   &CEvaluator::NotEqual      },
  { "LTE",   &CEvaluator::LowerOrEqual  },
  { "SHL",   &CEvaluator::ShiftLeft     },
  { "LT",    &CEvaluator::Lower         },
  { "GTE",   &CEvaluator::GreaterOrEqual},
  { "SHR",   &CEvaluator::ShiftRight    },
  { "GT",    &CEvaluator::Greater       },
  { "MIN",   &CEvaluator::Min           },
  { "MAX",   &CEvaluator::Max           },
  { "MOD",   &CEvaluator::Mod           },
  { "POW",   &CEvaluator::Pow           },
  { "NEG",   &CEvaluator::Neg           },
  { "INV",   &CEvaluator::Inv           },
  { "NOT",   &CEvaluator::Not           },
  { "ID",    &CEvaluator::Id            },
  { "BOOL",  &CEvaluator::Bool          },
  { "LNOT",  &CEvaluator::LNot          },
  { "SIN",   &CEvaluator::Sin           },
  { "COS",   &CEvaluator::Cos           },
  { "TAN",   &CEvaluator::Tan           },
  { "ASIN",  &CEvaluator::Asin          },
  { "ACOS",  &CEvaluator::Acos          },
  { "ATAN",  &CEvaluator::Atan          },
  { "SINH",  &CEvaluator::SinH          },
  { "COSH",  &CEvaluator::CosH          },
  { "TANH",  &CEvaluator::TanH          },
  { "ASINH", &CEvaluator::AsinH         },
  { "ACOSH", &CEvaluator::AcosH         },
  { "ATANH", &CEvaluator::AtanH         },
  { "EXP",   &CEvaluator::Exp           },
  { "LOG",   &CEvaluator::Ln            },
  { "LOG10", &CEvaluator::Log           },
  { "SQRT",  &CEvaluator::Sqrt          },
  { "SQR",   &CEvaluator::Sqr           },
  { "FACT",  &CEvaluator::Fact          },
  { "ABS",   &CEvaluator::Abs           },
  { "FLOOR", &CEvaluator::Floor         },
  { "CEIL",  &CEvaluator::Ceil          },
  { "SQUARE",&CEvaluator::Square        },
  { "RAND",  &CEvaluator::Rand          }


};
const unsigned CRules::m_FunctionPropertiesSize = sizeof( CRules::m_FunctionProperties ) / sizeof( SProperties );

const char CRules::m_BinaryFunctions[] =
{
  // TO DO use the following instead ADD 2; NOT 1; ...
  "ADD;"
  "SUB;"
  "MUL;"
  "PAR;"
  "DIV;"
  "POW;"
  "OR;"
  "XOR;"
  "LOR;"
  "AND;"
  "LAND;"
  "PADD;"
  "PMUL;"
  "EQU;"
  "NEQ;"
  "LTE;"
  "SHL;"
  "LT;"
  "GTE;"
  "SHR;"
  "GT;"
  "MIN;"
  "MAX;"
  "DB;"
  "PHASE;"
  "MOD;"
  "TED;"
  "POLY;"
  "POLY2;"
  "IPOLY;"
  "IPOLY2;"
  "IPOLY3;"
  "AT;"
  "CHOOSEVAR;"
  "SET;"
  "CONCAT;"
  "DER;"
  "INVAT;"
  "SIMPLIFY2;"
  "NORM;"
  "RANK;"
  "SUBST;"
};

const char CRules::m_UnaryFunctions[] =
{

  "NEG;"
  "INV;"
  "NOT;"
  "ID;"
  "BOOL;"
  "LNOT;"
  "SIN;"
  "COS;"
  "TAN;"
  "ASIN;"
  "ACOS;"
  "ATAN;"
  "SINH;"
  "COSH;"
  "TANH;"
  "ASINH;"
  "ACOSH;"
  "ATANH;"
  "EXP;"
  "LOG;"
  "LOG10;"
  "SQRT;"
  "SQR;"
  "FACT;"
  "ABS;"
  "FLOOR;"
  "CEIL;"
  "RE;"
  "IM;"
  "SQUARE;"
  "RECT;"
  "CONST;"
  "ELEM;"
  "VECT;"
  "VSIZE;"
  "ERROR;"
  "SIMPLIFY;"
  "GETVAR;"
  "SYSTEM_SOLVE;"
  "SYSTEM_SOLVE2;"
  "SYSTEM_SOLVE3;"
  "SYSTEM_SOLVE4;"
  "SYSTEM_SOLVE5;"
  "SYSTEM_AUTO_SOLVE;"
  "SYSTEM_AUTO_SOLVE2;"
  "SYSTEM_AUTO_SOLVE3;"
  "SYSTEM_AUTO_SOLVE4;"
  "SOLVE;"
  "SOLVE2;"
  "REDUCE;"
  "NORM2;"
  "TAYLOR;"
  "DERN;"

};

const char CRules::m_VoidFunctions[] =
{
  "RAND;"
  // "NEW;"
};

const char CRules::m_FunctionSymbol[] = // priority is defined here
{
  //a,b,c,d,e,f,g,h // priority => priority+1
  //A,B,C,D,E,F,G,H // priority => 0  
  "a,b       =>  a b CONCAT;"
  "a:=b      =>  a b SET;"
  "a+b       =>  a b ADD;"
  "a-b       =>  a b SUB;"
  "a*b       =>  a b MUL;"
  //"ab        =>  a b MUL;"
  //  "a\\b    =>  a b PAR;"
  "a/b       =>  a b DIV;"
  "a^b       =>  a b POW;"
  "a|b       =>  a b OR;"
  "a~b       =>  a b XOR;"
  "a&b       =>  a b AND;"
  "a<<b      =>  a b SHL;"
  "a>>b      =>  a b SHR;"
  "a==b      =>  a b EQU;"
  "a<>b      =>  a b NEQ;"
  "a<=b      =>  a b LTE;"
  "a<b       =>  a b LT;"
  "a>=b      =>  a b GTE;"
  "a>b       =>  a b GT;"
  "a||b      =>  a b LOR;"
  "a&&b      =>  a b LAND;"
  "a[B]      =>  a b AT;"
  "c?A:B     =>  a b CONCAT VECT c BOOL AT;"
  "-a        =>  a NEG;"
  "/a        =>  a INV;"
  "!a        =>  a LNOT;"
  "~a        =>  a NOT;"
  "a!        =>  a FACT;"
  "{A}       =>  a VECT;"
  //  "\\da/\\db =>  a b DER;"
  // "\\Sa\\db  =>  INT(a,b);"
  //"a\\p     =>  MUL(a,1e-12);"
  //"a\\n     =>  MUL(a,1e-9);"
  //"a\\u     =>  MUL(a,1e-6);"
  //"a\\m     =>  MUL(a,1e-3);"
  //"a\\k     =>  MUL(a,1e3);"
  //"a\\K     =>  MUL(a,1e3);"
  //"a\\M     =>  MUL(a,1e6);"
  //"a\\G     =>  MUL(a,1e9);"
};

const char CRules::m_AlgebraRuleString[] =

  "(TED(a,b) / c ) + ( TED(e,f) / c )                  := ( TED(a,b) + TED(e,f) ) / c;"
  "(TED(a,b) / c ) - ( TED(e,f) / c )                  := ( TED(a,b) - TED(e,f) ) / c;"

  "(TED(a,b) / TED(c,d)) + (TED(e,f) / TED(g,h))       := ( TED(a,b) * TED(g,h) + TED(e,f) * TED(c,d) ) / ( TED(c,d) * TED(g,h) ) ;"
  "(TED(a,b) / TED(c,d)) - (TED(e,f) / TED(g,h))       := ( TED(a,b) * TED(g,h) - TED(e,f) * TED(c,d) ) / ( TED(c,d) * TED(g,h) ) ;"

  "(TED(a,b) / TED(c,d)) * (TED(e,f) / TED(g,h))   := ( TED(a,b) * TED(e,f) ) / ( TED(c,d) * TED(g,h) );"
  "(TED(a,b) / TED(c,d)) / (TED(g,h) / TED(e,f))   := ( TED(a,b) * TED(e,f) ) / ( TED(c,d) * TED(g,h) );"

  "TED( TED(a,b),0 )   / TED( TED(c,d), 0 )        := TED(a,b)   / TED(c,d);"

  "TED( a,0)           / TED( TED(b,c),0)          := TED(0,a)   / TED(b,c);"
  "TED( TED(a,b),0)    / TED(c,0)                  := TED(a,b)   / TED(0,c);"
  "TED( a,0)           / TED(b,0)                  := TED(0,a/b) / TED(0,1);"

  // euclidian division
  //"(TED(a,b) / TED(0,1)) + (TED(0,f) / TED(g,h))       := NONE;"
  //"TED( TED(a,b),c )   / TED( d, e )               := TED(a/d,a/d*(b/a-e/d)) / TED(0,1) + TED( 0, c-e*a/d*(b/a-e/d)) / TED(d, e) ;"
  
  "TED(a,b)              / TED(0,1)                := NONE     ; "
  "TED(a,b)              / TED(0,c)                := TED(a/c,b/c) / TED(0,1);" // recursion*/
  "TED(a,b)/CONST(c)      := TED(a/c,b/c);"

  "TED(a,b)*TED(c,d)      := TED(TED(a*c,0),b*d) + TED(a*d+b*c,0);"
  "c*TED(a,b)             := TED(a*c,b*c);"
  "TED(a,b)*c             := TED(a*c,b*c);"

  "TED(a,b)+TED(c,d)    := TED(a+c,b+d);"
  "c+TED(a,b)        := TED(a,b+c);"
  "TED(a,b)+c        := TED(a,b+c);"

  "TED(a,b)-TED(c,d)    := TED(a-c,b-d);"
  "c-TED(a,b)        := TED(-a,c-b);"
  "TED(a,b)-c        := TED(a,b-c);"

  //"TED(a,TED(b,c))      := TED(a+b,c);"
  "TED( TED(0,a),b )    := TED(a,b);"

  /*"TED(a,b)+c   := TED(a,b+c);"
  "TED(a,b)-c   := TED(a,b-c);"
  "TED(a,b)*c   := TED(a*c,b*c);"
  "TED(a,b)/c   := TED(a/c,b/c);"*/
  //"(TED(a,b)/TED(c,d))^0         := TED(0,1)/TED(0,1);"
  "(TED(a,b)/TED(c,d))^1         := TED(a,b)/TED(c,d);"
  "(TED(a,b)/TED(c,d))^-CONST(e) := (TED(c,d)/TED(a,b))^e;"
  "(TED(a,b)/TED(c,d))^CONST(e)  := (TED(a,b)/TED(c,d)) * (TED(a,b)/TED(c,d))^(e-1);"

  /* "c+TED(a,b)   := TED(a,b+c);"
   "c-TED(a,b)   := TED(-a,c-b);"
   "c*TED(a,b)   := TED(a*c,b*c);"
   "c/TED(a,b)   := /TED(a/c,b/c );"*/
  "-TED(a,b)                 := TED(-a,-b);"
  "-(TED(a,b)/TED(c,d))    := (-TED(a,b))/TED(c,d);"
  "/(TED(a,b)/TED(c,d))    := TED(c,d)/TED(a,b);"

  "POLY(-a,v)     := -POLY(a,v);"
  "POLY(/a,v)     := /POLY(a,v);"
  "POLY(a+b,v)    := POLY(a,v)+POLY(b,v);"
  "POLY(a-b,v)    := POLY(a,v)-POLY(b,v);"
  "POLY(a*b,v)    := POLY(a,v)*POLY(b,v);"
  "POLY(a/b,v)    := POLY(a,v)/POLY(b,v);"
  "POLY(a^b,v)           := POLY2(a^SIMPLIFY(b),v);"

  "POLY2(a^CONST(b),v)   := POLY(a,v)^b;"
  "POLY2(a^-CONST(b),v)  := POLY(a,v)^-b;"
  "POLY2(a^b,v)          := TED(0,a^b)/TED(0,1);"
  "POLY2(a,v)            := POLY(a,v);"

  //"POLY(a!,v)     := POLY(a,v)!;"

  "POLY(COS(a),v)      := TED(0,COS( SIMPLIFY(a)))/TED(0,1);"
  "POLY(SIN(a),v)      := TED(0,SIN( SIMPLIFY(a)))/TED(0,1);"
  "POLY(EXP(a),v)      := TED(0,EXP( SIMPLIFY(a)))/TED(0,1);"
  "POLY(FACT(a),v)     := TED(0,FACT(SIMPLIFY(a)))/TED(0,1);"
  //"POLY({a,b},v)       := TED(0,{SIMPLIFY(a),SIMPLIFY(b)})/TED(0,1);"
  "POLY(a[b],v)        := TED(0,SIMPLIFY(a)[SIMPLIFY(b)])/TED(0,1);"
  //"POLY(TED(a,b),v) := TED(POLY(a,v),POLY(b,v));"
  "POLY({a},v)         := {POLY(a,v)};"
  "POLY((a,b),v)       := POLY(a,v),POLY(b,v);"
  "POLY(v,v)           := TED(1,0)/TED(0,1);"
  "POLY(a,v)           := TED(0,a)/TED(0,1);"

  "SOLVE2(TED(TED(a,b),c)/e,v)        := { (-b-SQRT(b^2-4*a*c))/(2*a), (-b+SQRT(b^2-4*a*c))/(2*a) };"
  "SOLVE2(TED(a,b)/e,v)               := (-b)/a;"
  //"SOLVE2(a,b,v)                      := SOLVE2(a,v),SOLVE2(b,v);"
  "SOLVE(a,v)                         := SOLVE2(POLY(a,v),v);"
  //"SOLVE(a)                           := SOLVE(a,GETVAR(a));"

  "--a        := a  ;"
  "!(a==b)      := a<>b  ;"
  "!(a<>b)      := a==b  ;"
  "!(a<=b)      := a>b  ;"
  "!(a<b)      := a>=b  ;"
  "!(a>=b)      := a<b  ;"
  "!(a>b)      := a<=b  ;"

  "SIN(-a)      := -SIN(a)  ;"
  "COS(-a)      := COS(a)  ;"
  "EXP(-a)      := 1/EXP(a)  ;"

  "0+b        := b  ;"
  "a+0        := a  ;"
  "(-a+-b)      := -(a+b)  ;"
  "(-a+b)      := b-a  ;"
  "a+-b        := a-b  ;"
  "SIN(a)+SIN(b)    := COS((a-b)/2)*SIN((a+b)/2)*2  ;"
  "LOG10(a)+LOG10(b)  := LOG10(a*b)  ;"

  "0-b        := -b  ;"
  //"1-1        := 0  ;"
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
  "a*1/b        := a/b  ;"
  "1/a*b        := b/a  ;"
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
  //"1/a/b        := 1/(b*a)  ;"

  "LOG(EXP(1))           := 1;"
  "EXP(a)/EXP(b)    := EXP(a-b)  ;"
  "SQRT(a)/SQRT(b)      := SQRT(a/b)  ;"
  "SQRT(-CONST(a))      := j*SQRT(a);"
  "SQR(a)/SQR(b)    := SQR(a/b)  ;"

  "a^0            := 1  ;"
  "0^b            := 0  ;"
  "1^b            := 1  ;"
  "a^1            := a  ;"
  "a^-b            := 1/(a^b)  ;"
  "x^a*x^b              := x^(a+b);"
  "x^a/x^b              := x^(a-b);"

  //"a*(a^b)              := a^(b+1)  ;"
  //"(a^b)*a              := a^(b+1)  ;"
  //"a*(a^b)              := a^(b+1)  ;"
  //"(a*b)^c          := (a^c)*(b^c)  ;"

  "BOOL(0)              := 0;"
  "BOOL(CONST(a))       := 1;"

  "RE(TED(a,b)/TED(0,1))     := b;"
  "RE(a)                     := RE(POLY(a,j));"

  "IM(TED(a,b)/TED(0,1))     := a;"
  "IM(a)                     := IM(POLY(a,j));"

  "IPOLY(a,b,v)                       := IPOLY(a,v),IPOLY(b,v);"
  "IPOLY({a},v)                       := {IPOLY(a,v)};"
  "IPOLY(TED(a,b)/TED(c,d),e)          := IPOLY(TED(a,b),e) / IPOLY(TED(c,d),e);" //TODO:IPOLY(a/c,p)
  "IPOLY(TED(TED(TED(TED(a,b),c),d),e),j) := SIMPLIFY(e-c+j*(d-b)+IPOLY(a,j));"
  "IPOLY(       TED(TED(TED(b,c),d),e),j) := e-c+j*(d-b);"
  "IPOLY(              TED(TED(c,d),e),j) := e-c+j*d;"
  "IPOLY(                     TED(d,e),j) := e+j*d;"
  "IPOLY(TED(a,b),q)                   := b+IPOLY3(a,q);"

  "IPOLY3(TED(a,b),q^CONST(n))         := IPOLY3(b,q^n)+IPOLY3(a,q^(n+1));"
  "IPOLY3(TED(a,b),q)                  := IPOLY3(b,q)+IPOLY3(a,q^2);"
  "IPOLY3(a,q)                         := a*q;"

  "IPOLY2(TED(a,b),{q,n})              := IPOLY2(b,{q,n})+IPOLY2(a,{q,n+1});"
  "IPOLY2(-a,{q,n})                    := -IPOLY2(a,{q,n});"
  "IPOLY2(a,{q,n})                     := (a^(1/n)*q)^n;"

  "NORM(TED(a,b)/TED(c,CONST(d)),q)    := ( b/d + IPOLY2(a/d,{q,1}) ) / ( 1 + IPOLY2(c/d,{q,1}) );"
  "NORM(a,q)                           := NORM(POLY(a,q),q);"

  "NORM2(TED(a,b)/c,q)                 := q+b/a;"
  "NORM2(a,q)                          := NORM2(POLY(a,q),q);"

  "SIMPLIFY(ELEM(a))                    := a ;"
  "SIMPLIFY(-ELEM(a))                   := -a ;"
  "SIMPLIFY(CONST(a))                   := a ;"
  "SIMPLIFY(-CONST(a))                  := -a ;"
  "SIMPLIFY(TED(a,b))                   := TED(SIMPLIFY(a),SIMPLIFY(b));"
  "SIMPLIFY(TED(a,b)/TED(0,1))          := TED(SIMPLIFY(a),SIMPLIFY(b));"
  "SIMPLIFY(TED(a,b)/TED(c,d))          := TED(SIMPLIFY(a),SIMPLIFY(b))/TED(SIMPLIFY(c),SIMPLIFY(d));"
  "SIMPLIFY(a)                          := SIMPLIFY2(a,GETVAR(a));"

  "SIMPLIFY2(a,1)                       := IPOLY(POLY(a,1),1);"
  "SIMPLIFY2(a,v)                       := IPOLY(SIMPLIFY(POLY(a,v)),v);"

  "GETVAR(a+b)                         := CHOOSEVAR( GETVAR(a), GETVAR(b) );"
  "GETVAR(a-b)                         := CHOOSEVAR( GETVAR(a), GETVAR(b) );"
  "GETVAR(a*b)                         := CHOOSEVAR( GETVAR(a), GETVAR(b) );"
  "GETVAR(a/b)                         := CHOOSEVAR( GETVAR(a), GETVAR(b) );"
  "GETVAR(a^b)                         := GETVAR(a);"
  "GETVAR(-a)                          := GETVAR(a);"
  "GETVAR(CONST(a))                    := 1;"
  "GETVAR(ELEM(a))                     := a;"
  "GETVAR(a)                           := 1;"

  "CHOOSEVAR(1,b)                      := b;"
  "CHOOSEVAR(a,1)                      := a;"
  "CHOOSEVAR(a,b)                      := RANK(a,b);"
  ;

const char CRules::m_AlgebraRuleDerivals[] =
  "DERN(a,b,0)     := a;"
  "DERN(a,b,n)     := DERN(DER(a,b),b,n-1);"
  "DER(CONST(a),c) := 0;"
  "DER(c,c)        := 1;"
  "DER(a+b,c)      := DER(a,c)+DER(b,c)  ;"
  "DER(a-b,c)      := DER(a,c)-DER(b,c)  ;"
  "DER(a*b,c)      := b*DER(a,c)+DER(b,c)*a  ;"
  "DER(a/b,c)      := (b*DER(a,c)-DER(b,c)*a)/SQR(b)  ;"
  "DER(a^b,c)      := DER(a,c)*b*a^(b-1)  ;"
  "DER(-a,c)       := -DER(a,c)  ;"
  "DER(SIN(b),c)   := DER(b,c)*COS(b)  ;"
  "DER(COS(b),c)   := -(DER(b,c)*SIN(b))  ;"
  "DER(TAN(b),c)   := DER(b,c)*(1+TAN(b)^2)  ;"
  "DER(ASIN(b),c)  := DER(b,c)/SQRT(1-b^2)  ;"
  "DER(ACOS(b),c)  := -(DER(b,c)/SQRT(1-b^2))  ;"
  "DER(ATAN(b),c)  := DER(b,c)/(1+b^2)  ;"
  "DER(TANH(b),c)  := DER(b,c)*(1-TANH(b)^2)  ;"
  "DER(EXP(b),c)   := DER(b,c)*EXP(b)  ;"
  "DER(LOG(a),c)   := DER(a,c)/a;"
  ;

const char CRules::m_AlgebraRuleSystems[] =

  "REDUCE(    TED(a,b)/TED(0,1),TED(0,d)/TED(0,1))    := TED(a,b),TED(0,d);"
  "REDUCE(    TED(a,b)/TED(0,1),TED(c,d)/TED(0,1))    := TED(c,d),TED(0,c*b-d*a);"
  "REDUCE( e, TED(a,b)/TED(0,1),TED(0,d)/TED(0,1))    := REDUCE( e, TED(a,b)/TED(0,1)) , TED(0,d);"
  "REDUCE( e, TED(a,b)/TED(0,1),TED(c,d)/TED(0,1))    := REDUCE( e, TED(c,d)/TED(0,1)) , TED(0,c*b-d*a);"

  "SYSTEM_SOLVE3({a},{x,y})   := SYSTEM_SOLVE3({IPOLY(REDUCE(POLY(a,y)),y)},{x});"
  "SYSTEM_SOLVE3({a},{x})     := IPOLY(REDUCE(POLY(a,x)),x);"

  "SYSTEM_SOLVE4({a,b},{x,y})   := SYSTEM_SOLVE4({a},{x}),SYSTEM_SOLVE4({b},{y});"
  "SYSTEM_SOLVE4({a},{x})       := SYSTEM_SOLVE5(POLY(a,x));"

  "SYSTEM_SOLVE5(TED(a,b)/TED(0,1)) := -b/a;"

  "SYSTEM_SOLVE ({a},{x})          := {SYSTEM_SOLVE4({SYSTEM_SOLVE3({a},{x})},{x})};"

  "SYSTEM_AUTO_SOLVE4({a,b})       := SYSTEM_AUTO_SOLVE4({a}),SYSTEM_AUTO_SOLVE4({b});"
  "SYSTEM_AUTO_SOLVE4({a})         := NORM2(a,GETVAR(a));"
  "SYSTEM_AUTO_SOLVE3({a,b})       := SYSTEM_SOLVE3({a,b},{GETVAR(b)});"
  "SYSTEM_AUTO_SOLVE2({a},{c,d})   := SYSTEM_AUTO_SOLVE2({SYSTEM_AUTO_SOLVE3({a})},{c});"
  "SYSTEM_AUTO_SOLVE2({a},{c})     := SYSTEM_AUTO_SOLVE3({a});"
  "SYSTEM_AUTO_SOLVE({a})          := {SYSTEM_AUTO_SOLVE4({SYSTEM_AUTO_SOLVE2({a},{a})})};"
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
  "TED(1,0)|TED(1,0)  := TED(1,0);"
  "TED(0,a)|TED(0,b)  := TED(0,a|b);"
  "TED(1,0)&TED(1,0)  := TED(1,0);"
  "TED(0,a)&TED(0,b)  := TED(0,a&b);"
  "~TED(0,a)          := TED(0,~a);"
  "~TED(1,0)|TED(1,0) := TED(0,1);"
  "TED(1,0)|~TED(1,0) := TED(0,1);"
  "~TED(1,0)&TED(1,0) := TED(0,0);"
  "TED(1,0)&~TED(1,0) := TED(0,0);"
/*
  "TED(a,b)|TED(c,d) := TED(a|c,b|d);"
  "TED(a,b)&TED(c,d) := TED(a&c|b&c|a&d,b&d);"
  "~TED(0,b)      := TED(0,~b);"*/
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