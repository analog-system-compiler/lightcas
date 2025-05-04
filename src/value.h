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

#pragma once

#include <cstdlib>
#include <cmath>
#include "lcstring.h"

class CValue
{

protected:
  double m_Value;

public:
  void SetValue(double v) { m_Value = v; }
  double GetValue() const { return m_Value; }
  bool IsNegative() const { return m_Value < 0.; }
  void Negate() { m_Value = -m_Value; }
  const char *GetFromString(const char *s1);
  void Display(CString &ds) const
  {
    CString s;
    s.Set(m_Value);
    ds.Append(s);
  }

  explicit CValue(double v = 0.) : m_Value(v) {};
};