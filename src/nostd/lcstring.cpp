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

char const CString::m_NullString[1] = "";

void CString::SetLength(unsigned len)
{
  m_Length = len;
  char *data_ptr = (char *)realloc(m_Data, (m_Length + 1) * sizeof(char));
  if (!data_ptr)
  {
    free(m_Data);
    exit(-1);
  }
  m_Data = data_ptr;
  m_Data[len] = '\0';
}

void CString::Append(const char *s, unsigned i)
{
  if (i != 0)
  {
    ASSERT(s != NULL);

    SetLength(m_Length + i);

    if (i == 1) // char
    {
      m_Data[m_Length - 1] = *s;
    }
    else // string;
    {
      memcpy(&m_Data[m_Length - i], s, i * sizeof(char));
    }

    m_Data[m_Length] = '\0';
  }
}

int CString::Compare(const char *s1, const char *s2)
{
  int bcomp;

  if ((s1 == NULL) && (s2 == NULL))
  {
    bcomp = 0;
  }
  else if (s1 == NULL)
  {
    bcomp = (*s2 == '\0') ? 0 : -1;
  }
  else if (s2 == NULL)
  {
    bcomp = (*s1 == '\0') ? 0 : 1;
  }
  else
  {
    bcomp = strcmp(s1, s2);
  }

  return bcomp;
}

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
