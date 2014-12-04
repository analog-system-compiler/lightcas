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
#define MAX_STACK_SIZE 1024

typedef	CVector< class CMathExpression* > CEquationArray;
class CAlgebraRule;

class CMathExpression
{    

  protected:
    OP_CODE*    m_StackArray;
    unsigned	m_StackSize;
    unsigned	m_OldStackSize;
    unsigned	m_AllocSize;
    CElementDataBase*	m_ElementDB;

  protected:

    void		 SetSize( unsigned i );
    void         Set( unsigned pos, OP_CODE op )   {  m_StackArray[ pos ] = op;    }
    OP_CODE      Get( unsigned pos )	const         {  return m_StackArray[ pos ];  }
    OP_CODE      Pop( unsigned& pos )  const       {  ASSERT( pos > 0 && pos<=m_StackSize ); pos--; return Get( pos );    }
    
    unsigned     Push( OP_CODE op )                {  SetSize( m_StackSize + 1  );      Set( m_OldStackSize, op );      return m_StackSize;    }
    void		 Push( const CElement* e )         {  Push( ElementToRef( e ) );    }
    void		 Push( const CMathExpression& equ );
    void         Push( const CValue& v );    
    void		 Append( const OP_CODE* array, unsigned size );
                 
    void		 PushBranch( const CMathExpression& equ, unsigned& pos );        
    bool		 CompareBranch( unsigned pos1, unsigned pos2 ) const;    
    void		 CopyBranch( const CMathExpression& equ, unsigned& pos );
    void		 NextBranch( unsigned& pos ) const;    
                 
    void         GetLevel( CParser& IC, unsigned priority );
    bool         SearchOperator( CParser& IC, unsigned priority, bool symbol_first );
    bool         ParseElement(CParser& IC);
    bool         MatchOperator( CParser& IC, const CString& s, const CMathExpression& equ, unsigned priority1, bool symbol_first );
                 
    const char*  ParseExpression( const char* sp, unsigned pos_array[], unsigned priority, CParser& IC, bool allow_recursion );

    bool         OptimizeConst();
    bool         OptimizeTree2(CMathExpression& equ);
    void         OptimizeTree(CMathExpression& equ);
    void         Replace( OP_CODE op1, OP_CODE op2 );
    void         ApplyRule( const CMathExpression& equ, unsigned const pos_array[], const CMathExpression* rule_equ, bool optimize=true );
    unsigned     Match( const CMathExpression& equ, unsigned pos_array[] ) const;    
    bool         MatchBranch( unsigned pos_array[], OP_CODE op1, unsigned pos2 ) const;
    virtual bool ExecuteCommand( OP_CODE , CMathExpression&  );
    void		 AddZero();
    void		 RemoveZero();

    static void       InitPositionTable( unsigned pos_array[] );
    static unsigned   ReservedParameterIndex( OP_CODE op );
    static CElement*  RefToElement( OP_CODE op )         { return CElementDataBase::RefToElement( op );  }
    static OP_CODE	  ElementToRef( const CElement* e )  { return CElementDataBase::ElementToRef( e );   }

    //Display funct
    unsigned    DisplayBranch( unsigned pos , unsigned priority, CDisplay& ds ) const;
    unsigned    DisplaySymbol(  unsigned pos, unsigned priority, CDisplay& ds ) const;
    unsigned    DisplaySymbolString(  const CSymbolSyntaxStruct& st, unsigned pos, unsigned priority, CDisplay& ds ) const;

  public:
    void	Initialize( CElementDataBase* db );

    void	BinaryOperation( OP_CODE op, const CMathExpression& equ );
    void	BinaryOperation( OP_CODE op, const CElement* e );
    void	UnaryOperation( OP_CODE op );
    void	VoidOperation( OP_CODE op );

    void	Clear()                             { m_OldStackSize = 0; m_StackSize = 0; }
    void    Init( const CElement *e )           { Clear(); Push(e);                    }
    void	Copy( const CMathExpression& equ );    
    bool	IsNull() const                      { return ( m_StackSize == 0 );         }         
    bool    Compare( const CMathExpression& equ ) const;   
    void	Display( CDisplay& ds ) const;
    void    GetFromString( const char *text )   { CParser IC( text ); GetFromString( IC ); }
    void	GetFromString( CParser& IC );
    void    GetFromTextRPN( CParser& IC );
    void	OptimizeTree();
    bool    Match( const CMathExpression& equ ) const;
    
    const CValue& Evaluate() const;

    static void ConvertToRule( CMathExpression& src, CMathExpression& dst );
    static bool IsReserved( OP_CODE op )      { return ( op >= CElementDataBase::OP_EXP1 ) && ( op < ( CElementDataBase::OP_EXP1 + CElementDataBase::MAX_EXP ) ); }

    unsigned	GetSize()	const             { return m_StackSize;    }
    CElementDataBase* GetElementDB() const    { return m_ElementDB;    }
    OP_CODE		GetLastOperator()	const;
    
    CMathExpression( CElementDataBase* db = NULL );
    CMathExpression( const CMathExpression& equ );
    virtual ~CMathExpression();

};

struct CSymbolSyntaxStruct
{
    char      m_Syntax[8];
    CMathExpression m_Equation;
};

