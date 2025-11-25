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

#include <cstdio>
#include <cstdarg>
#include "lcstring.h"

#ifdef _WIN32
#define vsnprintf _vsnprintf_s
#endif

typedef enum
{
  LOG_INFO,
  LOG_WARN,
  LOG_ERR,
  LOG_DEBUG
} log_t;

class CDisplay : public CString
{
private:
  FILE *m_File;
  bool m_DebugMode;
  static FILE *m_DebugLogFile;
  static char m_Buffer[2048];

public:
  static void Log(log_t type, const char *format, ...)
  {
    va_list args;
    va_start(args, format);
    vsnprintf(m_Buffer, sizeof(m_Buffer), format, args);
    va_end(args);
    CDisplay ds(m_Buffer);
    ds.Log(type);
  }

  void Log(log_t type)
  {
    if (type == LOG_DEBUG)
    {
      if (!m_DebugLogFile)
      {
        m_DebugLogFile = ::fopen("debug.log", "w+");
      }

      if (m_DebugLogFile)
      {
        ::fputs(GetBufferPtr(), m_DebugLogFile);
        ::fputc('\n', m_DebugLogFile);
      }
    }
    else
    {
      Prepend(type == LOG_INFO ? CString("Info: ") : (type == LOG_WARN ? CString("Warning: ") : CString("Error: ")));
      Print();
    }
  }

  void Print()
  {
    if (m_File)
    {
      ::fputs(GetBufferPtr(), m_File);
      ::fputc('\n', m_File);
    }
    else
      ::puts((GetBufferPtr()));
  }

  bool StoreToFile(const CString &name)
  {
    if (m_File)
      ::fclose(m_File);
    m_File = ::fopen(name.GetBufferPtr(), "w+");
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
  CDisplay(const CString &s) : CString(s)
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
      ::fclose(m_File);
  }
};
