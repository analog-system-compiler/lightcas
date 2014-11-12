/*******************************************************************************/
/*  Copyright (C) 2014 The CASLight project                                     */
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
//#include "Value.h"
#include "Function.h"
#include "LCVector.h"

typedef CVector< class CElement* > CElementArray;

class CElement
{

  private:

  enum ELEM_TYPE
    {
      ELEM_CONST,		// result = m_Value,
      ELEM_VAR,		// result = m_Equation, m_Value = XXXXX
    //  ELEM_FORCED,	// result = m_Value,
      ELEM_FUNCT,
    };

    CString  m_Name;
    CString   m_PseudoName;
    bool      m_Global;
    bool      m_Numeric;
    bool	  m_bLock;				    // for recursivity
    
    CFunction m_Function;			    /* remplacer m_Equation par m_Function puis utiliser l'équation de m_Function */

    //CValue    m_Value;
    //CEvaluatorFunct     m_EvalFunct;
    
    //CString   m_Symbol;

    ELEM_TYPE	m_Type;					/* type de la variable */
    bool		m_bAux;					// TRUE si variable auxiliaire. Utilisé que pour l'affichage !!!

    //unsigned	m_Index;				/* position dans la liste */
    unsigned    m_Ref;
    
  private:

    void CommonInit();

  public:

    void Display( CDisplay& ds ) const;
    void RemoveRules();
    void SetEquation( const CEquation& equ );
    void AddFunction( const CEquation& src, const CEquation& dst, unsigned line_nb=0 );
    void Unlock();
    bool Lock();

  // *INDENT-OFF* 
    const   CString&	GetName() const         { return m_Name; }
    //const CValue&  GetValue()	const           { return m_Value; }
    CFunction* GetFunction()                    { return &m_Function; }
    void    SetPseudoName( const CString& s )   { m_PseudoName = s;  }   
    //void    SetValue( const CValue& value )     {  m_Value = value;    }
    //void    SetConstValue( const CValue& value )   { m_Type = ELEM_CONST; SetValue( value );    }
    //const CValue& GetConstValue() const     {      ASSERT( IsConst() );      return m_Value;    }
    //void SetEvalFunct( CEvaluatorFunct funct )     {      m_EvalFunct = funct;    }
    //const CEvaluatorFunct GetEvalFunction()  const        {      return m_EvalFunct;    }
    void SetOperandNb( unsigned operand_nb )    { m_Type = ELEM_FUNCT;  ASSERT(m_Function.GetParameterNb()==0||m_Function.GetParameterNb()==operand_nb);   m_Function.SetParameterNb( operand_nb );    }
    void SetNumeric()   {      m_Numeric = true;    }
    void SetFunct()     {      m_Type = ELEM_FUNCT;    }
    //void SetForced()    {      m_Type = ELEM_FORCED;   }
    void SetVar()       {      m_Type = ELEM_VAR;      }
    void SetConst()     {      m_Type = ELEM_CONST;    }
    void SetGlobal( bool global )     { m_Global=global; }
    void SetTemporary(bool bAux=true) { m_bAux=bAux;}
    bool IsVar()     const    {      return ( m_Type == ELEM_VAR);    }
    bool IsConst()	 const    {      return ( m_Type == ELEM_CONST);    }
    //bool	IsForced()	const    {      return ( m_Type == ELEM_FORCED );    }
    bool IsFunct()   const    {      return ( m_Type == ELEM_FUNCT );    }
    bool IsNumeric() const    {      return m_Numeric;    }
    bool IsAux()	 const    {      return m_bAux;    }
    bool IsLocked()	 const    {      return m_bLock;    }
    bool IsGlobal()  const    { return m_Global; }
    
    bool IsVoid() const      {      return ( m_Function.GetParameterNb() == 0 );    }
    bool IsUnary() const     {      return ( m_Function.GetParameterNb() == 1 );    }
    bool IsBinary() const    {      return ( m_Function.GetParameterNb() == 2 );    }
   // bool IsElement() const     {      //FIXME ot remove      return ( ToRef() > OP_END_RESERVED );      /*CElement *e=RefToElement( op );      return e && !e->IsFunct();*/    }
   void SetRef( OP_CODE op ) { m_Ref = op; }
    OP_CODE ToRef() const    {      return ( OP_CODE )m_Ref;    }
    
   // *INDENT-ON*
    //constructor / destructor

    CElement( const CString& name );
    //CElement( const CValue& value  );
    virtual ~CElement();

};
