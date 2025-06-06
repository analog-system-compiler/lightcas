/*
 * Copyright (C) 2006-2025 The LightCAS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <https://www.gnu.org/licenses/>
 */

#include "function.h"
#include "element.h"

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

void CFunction::AddAlgebraRule(const CMathExpression &src, const CMathExpression &dst, unsigned line_no)
{
  ASSERT(src.GetSize());
  CAlgebraRule *rule = new CAlgebraRule(src, dst, line_no);
  AddOrReplaceRule(rule);
}

void CFunction::AddOrReplaceRule(CAlgebraRule *rule2)
{
  unsigned i;
#ifdef _DEBUG
  CDisplay ds;
#endif
  for (i = 0; i < m_AlgebraRuleArray.GetSize(); i++)
  {
    CAlgebraRule *rule1 = m_AlgebraRuleArray.GetAt(i);
    if (rule2->m_SrcEquation.Match(rule1->m_SrcEquation))
    {
      if (rule2->m_SrcEquation.Compare(rule1->m_SrcEquation))
      {
        delete rule1;
        m_AlgebraRuleArray.SetAt(i, rule2);
#ifdef _DEBUG
        ds += "Replacing Rule ";
#endif
        break;
      }
      else
      {
        m_AlgebraRuleArray.InsertAt(i, rule2);
#ifdef _DEBUG
        ds += "Inserting Rule ";
#endif
        break;
      }
    }
  }
  if (i == m_AlgebraRuleArray.GetSize())
  {
    m_AlgebraRuleArray.Append(rule2);
#ifdef _DEBUG
    ds += "Appending Rule ";
#endif
  }
#ifdef _DEBUG
  rule2->Display(i, ds);
  ds.Log(LOG_INFO);
#endif
}
