/*******************************************************************************/
/*  Copyright (C) 2014 The LightCAS project                                     */
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

  typedef enum 
  {
    ELEM_CONST,
    ELEM_VAR,
    ELEM_FUNCT,
    ELEM_NUM,
  } ELEM_TYPE;

  CString    m_Name;
  CFunction  m_Function;
  ELEM_TYPE  m_Type;
  bool       m_Global;
  bool       m_bLock;     // Prevents recursivity
  bool       m_bAux;
  unsigned   m_Ref;

public:

  void Display( CDisplay& ds ) const;
  void RemoveRules();
  void SetEquation( const CMathExpression& equ );
  void AddFunction( const CMathExpression& src, const CMathExpression& dst, unsigned line_nb = 0 );
  void Unlock();
  void Lock();

  const CString&  GetName() const           { return m_Name;                   }
  void SetFunct()                           { m_Type = ELEM_FUNCT;             }
  void SetVar()                             { m_Type = ELEM_VAR;               }
  void SetConst()                           { m_Type = ELEM_CONST;             }
  void SetNumeric()                         { m_Type = ELEM_NUM;               }
  void SetOperandNb( unsigned operand_nb )  { SetFunct(); ASSERT( m_Function.GetParameterNb() == 0 || m_Function.GetParameterNb() == operand_nb ); m_Function.SetParameterNb( operand_nb ); }

  void SetGlobal( bool global )             { m_Global = global;               }
  void SetAux   ( bool bAux = true )        { m_bAux = bAux;                   }
  void SetRef( OP_CODE op )                 { m_Ref = op;                      }
  bool IsVar()     const                    { return ( m_Type == ELEM_VAR   ); }
  bool IsConst()   const                    { return ( m_Type == ELEM_CONST ); }
  bool IsFunct()   const                    { return ( m_Type == ELEM_FUNCT ); }
  bool IsNumeric() const                    { return ( m_Type == ELEM_NUM );   }
  bool IsAux()     const                    { return m_bAux;                   }
  bool IsLocked()  const                    { return m_bLock;                  }
  bool IsGlobal()  const                    { return m_Global;                 }
  bool IsVoid()    const                    { return ( m_Function.GetParameterNb() == 0 );  }
  bool IsUnary()   const                    { return ( m_Function.GetParameterNb() == 1 );  }
  bool IsBinary()  const                    { return ( m_Function.GetParameterNb() == 2 );  }
  OP_CODE ToRef()  const                    { return ( OP_CODE )m_Ref;         }
  const CFunction* GetFunction() const      { return &m_Function;              }
  CFunction* GetFunction()                  { return &m_Function;              }

  CElement( const CString& name );
  virtual ~CElement();

};
