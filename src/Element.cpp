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

#include "Debug.h"
#include "Element.h"
#include "ElementDataBase.h"
#include "Function.h"
#include "Display.h"

CElement::CElement( const CString& name )
{
  m_Name = name;
  SetVar();
  CommonInit();
}

CElement::~CElement()
{
#ifdef _DEBUG
  CDisplay ds;
  ds.Append( "Unregistering : " );
  ds.Append( m_Name );
  TRACE( ds.GetBufferPtr() );
#endif
  CElementDataBase::UnRef( ToRef() );
}

void CElement::CommonInit()
{
  m_bLock     = false;
  m_bAux      = false;
  m_Global    = false;
  m_Numeric   = false;
  m_Ref       = 0;
}

void CElement::Display( CDisplay& ds ) const
{
  CString s1;

  if( IsConst() )
  {
    ASSERT( m_Name.GetLength() );
    ds.Append( m_Name );
  }
  else if( m_PseudoName.GetLength() )
  {
    ds.Append( m_PseudoName );
  }
  else
  {
    ds.Append( m_Name );
  }
}

bool CElement::Lock()
{
  ASSERT( !m_bLock );
  m_bLock = true;
  return true;
}

void CElement::Unlock()
{
  ASSERT( m_bLock );
  m_bLock = false;
}

void CElement::RemoveRules()
{
  m_Function.Clear();
  SetVar();
}

void CElement::SetEquation( const CMathExpression& dst )
{
  CMathExpression src( dst.GetElementDB() );
  src.Init( this ); // f()
  RemoveRules();
  AddFunction( src,  dst );
}

void CElement::AddFunction( const CMathExpression& src, const CMathExpression& dst, unsigned line_nb )
{
  SetFunct(); // to avoid function to be replaced by exp in "ConvertToRule"
  m_Function.AddAlgebraRule( src, dst, line_nb );
}

