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

#include "debug.h"
#include "element.h"
#include "elementdatabase.h"
#include "function.h"
#include "display.h"

CElement::CElement(const CString &name) : m_Name(name)
{
  m_bAux = false;
  m_Global = false;
  m_Ref = 0;
  SetVar();
}

CElement::~CElement()
{
#ifdef _DEBUG
  CDisplay ds;
  ds += "Unregistering : ";
  ds += m_Name;
  ds.Log(LOG_DEBUG);
#endif
  CElementDataBase::UnRef(ToRef());
}

void CElement::Display(CDisplay &ds) const
{
  ds += m_Name;
}

void CElement::RemoveRules()
{
  m_Function.Clear();
  SetVar();
}

void CElement::SetEquation(const CMathExpression &dst)
{
  CMathExpression src(dst.GetElementDB());
  src.Init(this); // f()
  RemoveRules();
  AddFunction(src, dst);
}

void CElement::AddFunction(const CMathExpression &src, const CMathExpression &dst, unsigned line_nb)
{
  SetFunct(); // to avoid function to be replaced by exp in "ConvertToRule"
  m_Function.AddAlgebraRule(src, dst, line_nb);
}
