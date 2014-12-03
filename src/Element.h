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
#include "Function.h"
#include "LCVector.h"

typedef CVector< class CElement* > CElementArray;

class CElement
{

  private:

    enum ELEM_TYPE
    {
      ELEM_CONST,
      ELEM_VAR,
      ELEM_FUNCT
    };

    CString   m_Name;
    CString   m_PseudoName;
    CFunction m_Function;
    bool      m_Global;
    bool      m_Numeric;
    bool	    m_bLock;				    // for recursivity    
    ELEM_TYPE	m_Type;
    bool		  m_bAux;
    unsigned  m_Ref;
    
  private:

    void CommonInit();

  public:

    void Display( CDisplay& ds ) const;
    void RemoveRules();    
    void SetEquation( const CMathExpression& equ );
    void AddFunction( const CMathExpression& src, const CMathExpression& dst, unsigned line_nb=0 );
    void Unlock();
    bool Lock();

    // *INDENT-OFF* 
    const CString&	GetName() const             { return m_Name;                   }    
    void SetPseudoName( const CString& s )      { m_PseudoName = s;                }   
    void SetOperandNb( unsigned operand_nb )    { m_Type = ELEM_FUNCT;  ASSERT(m_Function.GetParameterNb()==0||m_Function.GetParameterNb()==operand_nb);   m_Function.SetParameterNb( operand_nb );    }
    void SetNumeric()                           { m_Numeric = true;                }
    void SetFunct()                             { m_Type = ELEM_FUNCT;             }
    void SetVar()                               { m_Type = ELEM_VAR;               }
    void SetConst()                             { m_Type = ELEM_CONST;             }
    void SetGlobal( bool global )               { m_Global=global;                 }
    void SetTemporary(bool bAux=true)           { m_bAux=bAux;                     }
    void SetRef( OP_CODE op )                   { m_Ref = op;                      }
    bool IsVar()     const                      { return ( m_Type == ELEM_VAR);    }
    bool IsConst()	 const                      { return ( m_Type == ELEM_CONST);  }
    bool IsFunct()   const                      { return ( m_Type == ELEM_FUNCT ); }
    bool IsNumeric() const                      { return m_Numeric;                }
    bool IsAux()	 const                        { return m_bAux;                   }
    bool IsLocked()	 const                      { return m_bLock;                  }
    bool IsGlobal()  const                      { return m_Global;                 }                      
    bool IsVoid() const                         { return ( m_Function.GetParameterNb() == 0 );  }
    bool IsUnary() const                        { return ( m_Function.GetParameterNb() == 1 );  }
    bool IsBinary() const                       { return ( m_Function.GetParameterNb() == 2 );  }
    CFunction* GetFunction()                    { return &m_Function;              }
    OP_CODE ToRef() const                       { return ( OP_CODE )m_Ref;         }  
    // *INDENT-ON*
    
    CElement( const CString& name );    
    virtual ~CElement();

};
