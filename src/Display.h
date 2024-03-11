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

#include <cstdio>
#include <cstdarg>
#include "LCString.h"

#ifdef _WIN32
#define vsnprintf _vsnprintf_s
#endif

class CDisplay : public CString
{
private:
  FILE *m_File;
  bool m_DebugMode;

public:
  virtual void Print(const char *s) { puts(s); }
  void Print()
  {
    if (m_File)
    {
      fwrite(GetBufferPtr(), GetLength(), 1, m_File);
      fputc('\n', m_File);
      // fputs("\r\n", m_File);
    }
    else
      Print(GetBufferPtr());
  }

  void Printf(const char *format, ...)
  {
    static char buffer[2048];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Print(buffer);
  }

  bool StoreToFile(const CString &name)
  {
    if (m_File)
      fclose(m_File);
    m_File = fopen(name.GetBufferPtr(), "w+");
    return m_File != NULL;
  }

  bool IsDebug() { return m_DebugMode; }
  void SetDebug() { m_DebugMode = true; }
  // Constructors
  CDisplay() : CString()
  {
    m_File = NULL;
    m_DebugMode = false;
  }
  CDisplay(const char *s) : CString(s)
  {
    m_File = NULL;
    m_DebugMode = false;
  }
  CDisplay(const CString& s) : CString(s)
  {
    m_File = NULL;
    m_DebugMode = false;
  }
  explicit CDisplay(int i, unsigned base = 10) : CString(i, base)
  {
    m_File = NULL;
    m_DebugMode = false;
  }
  ~CDisplay()
  {
    if (m_File)
      fclose(m_File);
  }
};