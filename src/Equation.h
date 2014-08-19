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
#include "Vector.h"
#include "Evaluator.h"
#include "ElementDataBase.h"
#define MAX_STACK_SIZE 1024
#define FUNCT_EXT

typedef	CVector< class CEquation* > CEquationArray;
class CAlgebraRule;

class CEquation
{    

  private:
    OP_CODE*    m_StackArray;
    unsigned	m_StackSize;
    unsigned	m_OldStackSize;
    unsigned	m_AllocSize;
        
  public:

    CElementDataBase*	m_ElementDB;
    CValue& Evaluate( CEvaluator& val ) const;    

  protected:

    void        Set( unsigned pos, OP_CODE op )   {  m_StackArray[ pos ] = op;    }
    OP_CODE     Get( unsigned pos )	const         {  return m_StackArray[ pos ];  }
    unsigned    Push( OP_CODE op )                {  SetSize( m_StackSize + 1  );      Set( m_OldStackSize, op );      return m_StackSize;    }
    OP_CODE     Pop( unsigned& pos )  const       {  ASSERT( pos );      ASSERT( m_StackSize );      pos --;      return Get( pos );    }

    CValue&     Evaluate( unsigned pos, CEvaluator& val ) const;
    void		SetSize( unsigned i );
    
    void		PushBranch( const CEquation& equ, unsigned& pos );        
    bool		CompareBranch( unsigned pos1, unsigned pos2 ) const;    
    void		CopyBranch( const CEquation& equ, unsigned& pos )        {        Clear();        PushBranch( equ, pos );        }
    void		NextBranch( unsigned& pos ) const;    

    void		Append( const OP_CODE* array, unsigned size );
    void		PushEquation( const CEquation& equ );
    void        PushElement( CValue& v );

    void        GetLevel( CParser& IC, unsigned priority );
    bool        SearchOperator( CParser& IC, unsigned priority, bool symbol_first );
    void        ParseElement(CParser& IC);
    bool        GetAtom(CParser& IC);
    bool        MatchOperator( CParser& IC, const CString& s, const CEquation& equ, unsigned priority1, bool symbol_first );

    const char*        ParseExpression( const char* sp, OP_CODE* elem_array, unsigned* pos_array, unsigned index, unsigned priority, CParser& IC, bool allow_recursion );

    bool		OptimizeConst();
    void		OptimizeTree(CEquation& equ);
    void		OptimizeTree2(CEquation& equ);
    void		ApplyRule( const CEquation& equ, OP_CODE const elem_array[], unsigned const pos_array[], const CEquation* rule_equ, bool optimize=true );
    unsigned    Match( const CEquation* equ, OP_CODE elem_array[], unsigned pos_array[] ) const;    
    bool        MatchBranch( OP_CODE elem_array[], unsigned pos_array[], OP_CODE op1, unsigned pos2 ) const;
    static void InitParameterLUT( OP_CODE elem_array[] );
    static unsigned MatchParameter( const OP_CODE elem_array[], OP_CODE op );
    

#ifdef FUNCT_EXT
    void		AddZero();
    void		RemoveZero();
#endif

    //static bool	IsElement( OP_CODE op )                  {      return ( op > OP_END_RESERVED );      }
    static CElement* 	RefToElement( OP_CODE op )       {      return CElementDataBase::RefToElement( op );    }
    static OP_CODE	ElementToRef( const CElement* e )    {      return CElementDataBase::ElementToRef( e );    }
    void        Replace( OP_CODE op1, OP_CODE op2 );
    void		Initialize( CElementDataBase* db );

    //display funct
    unsigned	DisplayBranch( unsigned pos , unsigned priority, CDisplay& ds ) const;
    unsigned    DisplaySymbol(  unsigned pos, unsigned priority, CDisplay& ds ) const;

    void		PushOperator( OP_CODE op )          {     Push( op );                   }
    void		PushElement( const CElement* e )    {     Push( ElementToRef( e ) );    }
 
  public:

    void		Clear()                             { m_OldStackSize = 0; m_StackSize = 0; }
    void        Init( const CElement *e )           { Clear(); PushElement(e);             }
    bool		IsNull() const                      { return ( m_StackSize == 0 );         }       
    bool        Compare( const CEquation& equ ) const; 
   
    void		Copy( const CEquation& equ );    
    void		Display( CDisplay& ds ) const;
    void        GetFromText( const char *text )     { CParser IC( text ); GetFromText( IC ); }
    void		GetFromText( CParser& IC );
    void        GetFromTextRPN( CParser& IC );
    void		OptimizeTree();
    bool        Match( const CEquation& equ ) const;
    
#ifdef FUNCT_EXT
    void		BinaryOperation( OP_CODE op, const CEquation& equ );
    void		BinaryOperation( OP_CODE op, const CElement* e );
    void		UnaryOperation( OP_CODE op );
    void		VoidOperation( OP_CODE op );
#endif


    //void		SimplifyComplex();    
    static void        ConvertToRule( CEquation& src, CEquation& dst );
    static bool IsReserved( OP_CODE op )      { return ( op >= CElementDataBase::OP_EXP1 ) && ( op < ( CElementDataBase::OP_EXP1 + CElementDataBase::MAX_EXP ) ); }
    unsigned	GetSize()	const             {      return m_StackSize;    }
    CElementDataBase* GetElementDB() const    {      return m_ElementDB;    }
       OP_CODE		GetLastOperator()	const;
    // constructor
    CEquation( CElementDataBase* db = NULL );

    // copy constructor
    CEquation( const CEquation& equ );
    virtual ~CEquation();

};

struct CSymbolSyntaxStruct
{
char      m_Syntax[8];
CEquation m_Equation;
};

