/*******************************************************************************/
/*  Copyright (C) 2006 The SIMCAS project                                      */
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

#include "Function.h"
#include "Element.h"

CFunction::CFunction()
{
  m_ParameterNb = 0;
}

CFunction::~CFunction()
{
  Clear();
}

void CFunction::Clear()
{
  m_AlgebraRuleArray.DeleteAll();
}

void CFunction::AddAlgebraRule( const CEquation& src, const CEquation& dst, unsigned line_no )
{
  ASSERT( src.GetSize() );
  CAlgebraRule* rule = new CAlgebraRule( src, dst, line_no );
  AddOrReplaceRule( rule );
}

void CFunction::AddOrReplaceRule( CAlgebraRule* rule2 )
{
  unsigned i;
#ifdef _DEBUG
  CDisplay ds;
#endif
  for( i = 0; i < m_AlgebraRuleArray.GetSize(); i++ )
  {
    CAlgebraRule* rule1 = m_AlgebraRuleArray.GetAt( i );
    if( rule2->m_SrcEquation.Match( rule1->m_SrcEquation ) )
    {
      if( rule2->m_SrcEquation.Compare( rule1->m_SrcEquation ) )
      {
        delete rule1;
        m_AlgebraRuleArray.SetAt( i, rule2 );
#ifdef _DEBUG
        ds += "Replacing Rule #";
#endif
        break;
      }
      else
      {
        m_AlgebraRuleArray.InsertAt( i, rule2 );
#ifdef _DEBUG
        ds += "Inserting Rule #";
#endif
        break;
      }
    }
  }
  if( i == m_AlgebraRuleArray.GetSize() )
  {
    m_AlgebraRuleArray.Append( rule2 );
#ifdef _DEBUG
    ds += "Appending Rule #";
#endif
  }
#ifdef _DEBUG
  ds += CString( i );
  ds += " ";
  rule2->m_SrcEquation.Display( ds );
  ds += "\t=> ";
  rule2->m_DstEquation.Display( ds );
  ds += "\t";
  TRACE( ds.GetBufferPtr() );
#endif
}

