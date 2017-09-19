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

#include "Debug.h"
#include "Parser.h"
#include "LCVector.h"
#include "Evaluator.h"
#include "Display.h"
#include "ElementDataBase.h"

#define OPTIMIZATION_LEVEL 0//2

class CAlgebraRule;

typedef unsigned short pos_t;
//#define MAX_STACK_SIZE (1<<20)

class CMathExpression
{

  friend class CElementDataBase;

protected:
  OP_CODE*  m_StackArray;
  pos_t     m_StackSize;
  pos_t     m_AllocSize;
  CElementDataBase* m_ElementDB;

protected:
  void     SetSize( pos_t i );
  void     Set( pos_t pos, OP_CODE op ) { m_StackArray[ pos ] = op;    }
  OP_CODE  Get( pos_t pos )   const     { return m_StackArray[ pos ];  }
  OP_CODE  Pop( pos_t& pos )  const     { ASSERT( pos > 0 && pos <= m_StackSize ); pos--; return Get( pos );  }
  void     Push( OP_CODE op )           { SetSize( m_StackSize + 1  );  Set( m_StackSize - 1, op );  }
  void     Push( const CElement* e )    { Push( ElementToRef( e ) );    }

  void     Push( const CMathExpression& equ );
  void     PushEvalElement( CEvaluator& eval );
  void     Append( const OP_CODE* array, pos_t size );

  void     PushBranch( const CMathExpression& equ, pos_t& pos );
  bool     CompareBranchElement( pos_t pos1, pos_t pos2 ) const;
  bool     CompareBranch( pos_t pos1, pos_t pos2 ) const;
  void     NextBranch( pos_t& pos ) const;

  int      GetLevel( CParser& IC );
  bool     GetLevel( CParser& IC, unsigned priority );
  bool     SearchOperator( CParser& IC, unsigned priority, bool symbol_first );
  bool     ParseMacro( CParser& IC );
  bool     ParseAtom( CParser& IC );
  bool     ParseElement( CParser& IC );
  unsigned Parse( CParser& IC );
  bool     MatchOperator( CParser& IC, const char* sp, pos_t pos_array[], unsigned precedence, bool symbol_first );
  void     StoreStackPointer( char c, pos_t pos_array[] );

  bool     OptimizeTree2();

  void     Replace( OP_CODE op1, OP_CODE op2, pos_t pos = 0 );
  void     ApplyRule( pos_t pos, pos_t const pos_array[], const CMathExpression& rule_equ, bool optimize = true );
  void     InnerCopy( pos_t pos_dest, pos_t pos_source, pos_t size );
  pos_t    Match( pos_t pos2, const CMathExpression& equ, pos_t pos_array[] ) const;
  bool     RegisterBranch( pos_t pos_array[], OP_CODE op1, pos_t pos2 ) const;
  void     AddZero();
  void     RemoveZero();

  //virtual
  virtual void ExecuteCommand();

  //static
  static CElement*  RefToElement( OP_CODE op )              { return CElementDataBase::RefToElement( op );  }
  static OP_CODE    ElementToRef( const CElement* e )       { return CElementDataBase::ElementToRef( e );   }
  static unsigned   ReservedParameterIndex( OP_CODE op )    { return ( unsigned )( op - CElementDataBase::OP_EXP1 );  }

  //Display funct
  unsigned    DisplayBranch( pos_t pos, unsigned priority, CDisplay& ds ) const;
  unsigned    DisplaySymbol( pos_t pos, unsigned priority, CDisplay& ds ) const;
  void        DisplaySymbolString(  const char* sp, pos_t pos_array[], unsigned precedence, CDisplay& ds ) const;

public:
  void  Initialize( CElementDataBase* db );

  void  BinaryOperation( OP_CODE op, const CMathExpression& equ );
  void  UnaryOperation( OP_CODE op );
  void  VoidOperation( OP_CODE op );

  void  Clear()                             { m_StackSize = 0;  }
  void  Init( OP_CODE op )                  { Clear(); Push( op ); }
  void  Init( const CElement* e )           { Clear(); Push( e ); }
  bool  IsEmpty() const                     { return ( m_StackSize == 0 );  }

  bool  Compare( const CMathExpression& equ ) const;
  void  Copy( const CMathExpression& equ );
  void  Display( CDisplay& ds ) const;
  bool  GetFromString( CParser& IC )        { return ( Parse( IC ) > 0 ); }
  bool  GetFromString( const char* text )   { CParser IC( text ); return GetFromString( IC ); }
  void  OptimizeTree();
  bool  Match( const CMathExpression& equ ) const;
  void  Evaluate() const;

  static void ConvertToRule( CMathExpression& src, CMathExpression& dst );

  OP_CODE GetLastOperator() const         { return IsEmpty() ? CElementDataBase::OP_ZERO : Get( m_StackSize - 1 ); }
  pos_t             GetSize() const       { return m_StackSize;    }
  CElementDataBase* GetElementDB() const  { return m_ElementDB;    }

  CMathExpression( CElementDataBase* db = NULL );
  CMathExpression( const CMathExpression& equ ) { m_ElementDB = NULL; Copy( equ ); }
  virtual ~CMathExpression();

};

struct CSymbolSyntaxStruct
{
  char            m_Syntax[8];
  CMathExpression m_Equation;
};

