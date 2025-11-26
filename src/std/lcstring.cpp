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

#include "lcstring.h"

void CString::Set(unsigned i, unsigned base)
{
  char m_Buffer[256];
#if defined(__GNUG__)
  unsigned j;

  j = sizeof(m_Buffer) - 1;
  m_Buffer[j] = '\0';

  do
  {

    unsigned k = i / base;
    unsigned val = i - k * base;
    i = k;

    if (val < 10)
    {
      val += '0';
    }

    else
    {
      val += 'A' - 10;
    }

    j--;
    m_Buffer[j] = val;

  } while (i && j);

  Copy(m_Buffer + j);
#else
  _itoa_s(i, m_Buffer, base);
  Copy(m_Buffer);
#endif
}
