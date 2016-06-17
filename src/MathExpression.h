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
#include "ElementDataBase.h"

#define MAX_STACK_SIZE     (1<<20)
#define OPTIMIZATION_LEVEL  2

typedef	CVector< class CMathExpression* > CEquationArray;
class CAlgebraRule;

class CMathExpression
{    

  friend CElementDataBase;

  protected:
    OP_CODE*    m_StackArray;
    unsigned	m_StackSize;
    unsigned	m_AllocSize;
    CElementDataBase*	m_ElementDB;    

  protected:
    void		 SetSize( unsigned i );
    void         Set( unsigned pos, OP_CODE op )   {  m_StackArray[ pos ] = op;    }
    OP_CODE      Get( unsigned pos )	const      {  return m_StackArray[ pos ];  }
    OP_CODE      Pop( unsigned& pos )  const       {  ASSERT( pos > 0 && pos<=m_StackSize ); pos--; return Get( pos );  }
    
    void         Push( OP_CODE op )                {  SetSize( m_StackSize + 1  );  Set( m_StackSize-1, op );  }
    void		 Push( const CElement* e )         {  Push( ElementToRef( e ) );    }
    void		 Push( const CMathExpression& equ );
    void         Push( const CValue& v );    
    void		 Append( const OP_CODE* array, unsigned size );
                 
    void		 PushBranch( const CMathExpression& equ, unsigned& pos );        
    bool		 CompareBranch( unsigned pos1, unsigned pos2 ) const;    
    void		 NextBranch( unsigned& pos ) const;    
                 
    void         GetLevel( CParser& IC, unsigned priority );
    bool         SearchOperator( CParser& IC, unsigned priority, bool symbol_first );
    bool         ParseElement(CParser& IC);
    unsigned     ParseParenthesis( CParser& IC );
    unsigned     Parse( CParser& IC );
    bool         MatchOperator( CParser& IC, const char * s, const CMathExpression& equ, unsigned priority1 );
                 
    void         StoreStackPointer( char c, unsigned pos_array[] );

    bool         OptimizeConst();
    bool         OptimizeTree2();
    void         OptimizeTree3();

    void         Replace( OP_CODE op1, OP_CODE op2 );
    void         ApplyRule( const CMathExpression& equ, unsigned const pos_array[], const CMathExpression* rule_equ, bool optimize=true );
    unsigned     Match( unsigned pos2, const CMathExpression& equ, unsigned pos_array[] ) const;    
    bool         MatchBranch( unsigned pos_array[], OP_CODE op1, unsigned pos2 ) const;
    virtual bool ExecuteCommand();
    void		 AddZero();
    void		 RemoveZero();

    static void       InitPositionTable( unsigned pos_array[] );
    static CElement*  RefToElement( OP_CODE op )              { return CElementDataBase::RefToElement( op );  }
    static OP_CODE	  ElementToRef( const CElement* e )       { return CElementDataBase::ElementToRef( e );   }
    static unsigned   ReservedParameterIndex( OP_CODE op )    { return op - CElementDataBase::OP_EXP1;  }

    //Display funct
    unsigned    DisplayBranch( unsigned pos , unsigned priority, CDisplay& ds ) const;
    unsigned    DisplaySymbol(  unsigned pos, unsigned priority, CDisplay& ds ) const;
    void        DisplaySymbolString(  const char *sp, unsigned pos_array[], unsigned precedence, CDisplay& ds ) const;

  public:
    void	Initialize( CElementDataBase* db );

    void	BinaryOperation( OP_CODE op, const CMathExpression& equ );
    void	UnaryOperation( OP_CODE op );
    void	VoidOperation( OP_CODE op );

    void	Clear()                             { m_StackSize = 0;  }
    void    Init( const CElement *e )           { Clear(); Push(e); }
    bool	IsEmpty() const                     { return ( m_StackSize == 0 );  }    
    
    bool    Compare( const CMathExpression& equ ) const;   
    void	Copy( const CMathExpression& equ );    
    void	Display( CDisplay& ds ) const;
    void	GetFromString( CParser& IC );
    void    GetFromString( const char *text )   { CParser IC( text ); GetFromString( IC ); }
    void	OptimizeTree();
    bool    Match( const CMathExpression& equ ) const;
    
    const CValue& Evaluate() const;

    static void ConvertToRule( CMathExpression& src, CMathExpression& dst );
    static bool IsReserved( OP_CODE op )      { return ( op >= CElementDataBase::OP_EXP1 ) && ( op < ( CElementDataBase::OP_EXP1 + CElementDataBase::MAX_EXP ) ); }

    unsigned	GetSize()	const             { return m_StackSize;    }
    CElementDataBase* GetElementDB() const    { return m_ElementDB;    }
    OP_CODE		GetLastOperator()	const;
    
    CMathExpression( CElementDataBase* db = NULL );
    virtual ~CMathExpression();

};

struct CSymbolSyntaxStruct
{
    char      m_Syntax[8];
    CMathExpression m_Equation;
};

