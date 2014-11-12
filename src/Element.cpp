/*******************************************************************************/
/*  Copyright (C) 2014 The CASLigh project                                     */
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
  m_Type = ELEM_VAR;
  CommonInit();
}
/*
CElement::CElement( const CValue& val )
{
  SetConstValue( val );
  CommonInit();
}*/

CElement::~CElement()
{
  CElementDataBase::UnRef( ToRef() );
}

void CElement::CommonInit()
{
  m_bLock     = false;
  m_bAux      = false;
  m_Ref       = 0;
  m_Global    = false;
  m_Numeric   = false;
}

void CElement::Display( CDisplay& ds ) const
{
  CString s1;

  if( IsConst() )
  {
    if( m_Name.GetLength() )
    {
      ds.Add( m_Name );
    }
    else
    {
      ASSERT( false );
      //m_Value.Display( ds );
    }
  }
  else if( m_PseudoName.GetLength() )
  {
    ds.Add( m_PseudoName );
  }
  else
  {
    ds.Add( m_Name );
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
  m_Type = ELEM_VAR;
}

void CElement::SetEquation( const CEquation& dst )
{
  CEquation src( dst.GetElementDB() );
  src.Init( this ); // f()
  RemoveRules();
  AddFunction( src,  dst );
}

void CElement::AddFunction( const CEquation& src, const CEquation& dst, unsigned line_nb )
{
  m_Type = ELEM_FUNCT;
  Lock(); // to avoid function to be replaced by exp in "ConvertToRule"
  m_Function.AddAlgebraRule( src, dst, line_nb );
  Unlock();
}

