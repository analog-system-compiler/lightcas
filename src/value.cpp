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

#include <cstdio>
#include "value.h"

const char *CValue::GetFromString(const char *s1)
{
  char *s2;
  if ((s1[0] == '0') && ((s1[1] == 'b') || (s1[1] == 'B')))
  {
    m_Value = strtoul(s1 + 2, &s2, 2);
  }
  else
  {
    m_Value = strtod(s1, &s2);
  }
  return s2;
}