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

#pragma once

#include "LCString.h"
#include "Parser.h"
#include "Evaluator.h"
#include "Rules.h"
#include "LCVector.h"

class CMathExpression;
class CElement;
struct CSymbolSyntaxStruct;

typedef CVector< class CElement* >		            CElementArray;
typedef CVector< struct CSymbolSyntaxStruct* >		CSymbolSyntaxArray;

class CElementDataBase : public CElementArray
{
    
  protected:

    CElementDataBase*         m_Parent;
    CEvaluator*               m_Evaluator;
    static CElementArray      m_ElementRefArray;
    static CSymbolSyntaxArray m_SymbolSyntaxArray;

  public :
    
    enum PRIVATE_OPCODE {
      OP_ZERO=0,
      OP_EXP1,
      OP_EXP2,
      OP_EXP3,
      OP_EXP4,
      OP_EXP5,
      OP_EXP6,
      OP_EXP7,
      OP_EXP8,  
      OP_CONCAT,
      OP_SET,
      OP_NONE,
      OP_CONST,
      //OP_CONSTINT,
      OP_ELEM,
      OP_NEG,
      OP_CPLX,
      OP_RANK,
      OP_SUBST,
      OP_ERROR_SIZE,
      OP_END_RESERVED
    };

    enum { MAX_EXP = OP_EXP8-OP_EXP1+1 };

    CString			m_Name;

  protected:

    unsigned      Register( CElement* e , unsigned index );
    CElement *    CreateElement( const CString& string, unsigned pos );
    virtual void  AddReservedElements();
    virtual void  AddSyntaxSymbolTable(const char *symbol_table);
    virtual void  AddOperandTable(const char *operand_table);    
    virtual void  AddAlgebraRuleTable(const char *rule_table);
    virtual void  AddEvalFunctionTable( const SProperties *property_table, unsigned size );
    virtual void  AddEvalFunction( const CString& name, unsigned parameter_nb, CEvaluatorFunct funct );
    void          SetValue( const CElement* e, const CValue& v );
    void          SetConstValue( CElement* e, const CValue& v );
    CElementDataBase(const CElementDataBase& db );

  public:

    void			Clear();
    void            SetName( const CString& name ) { m_Name=name; }
    void            Initialize();
    CEvaluator*		GetEvaluator() const  {      return m_Evaluator;    }    
    CElement*       ParseElement( CParser& IC );
    CElement* 		GetElement();
    CElement* 		GetElement( const CValue& value );
    CElement* 		GetElement( const CString& );
    CElement* 		SearchElement( const CString&, unsigned& pos ) const;
    
    static CElement* RefToElement( OP_CODE op )         { return m_ElementRefArray[ ( unsigned )op ];    }
    static void      UnRef( OP_CODE op )                { m_ElementRefArray[ ( unsigned )op ] = NULL;    }
    static OP_CODE   ElementToRef( const CElement* e );    
    static const CSymbolSyntaxArray& GetSymbolTable()   { return m_SymbolSyntaxArray; }
    CElementDataBase* GetParent() const                 { return m_Parent;    }

#ifdef _DEBUG    
        void Printf( const char* format, ... );
        void Test();
        void CheckPrintf( const char * format, ... );
        void Check( const char*s1, const char *s2 );
        void Check( const char* s1, const CValue& v1 );
        void CheckCatch( const char* s1 );
        void DisplayStats();
#endif

    CElementDataBase( const CString& name = CString(), CElementDataBase *db=NULL, CEvaluator*eval=NULL, bool bInitialize=true);
    virtual ~CElementDataBase();

};
