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

#include <cstring>
#include <cctype>
#include "Debug.h"
#include "Parser.h"
#include "Display.h"

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

const char CParser::m_CharTab[] = {
    /*                              \n       \r                                                      */
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /*   !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ? */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0,

    /*@  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _ */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,

    /*   a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }       */
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};

CParser::CParser()
{
  Init("");
}

CParser::CParser(const char *pText)
{
  Init(pText);
}

CParser::~CParser()
{
  for (unsigned i = 0; i < m_ContextArray.GetSize(); i++)
  {
    delete m_ContextArray[i].m_Text;
  }
}

void CParser::Init(const char *pText)
{
  m_LineNb = 1;
  SetPos(pText);
  m_LineStart = true;
  m_SymbolMacroSeen = false;
}

const CString &CParser::GetWord()
{
  const char *pos;
  SkipSpace();
  pos = m_Pos;
  ASSERT(IsWord(*pos));
  while (IsWord(*pos))
  {
    pos++;
  }
  m_Buffer.Copy(m_Pos, pos - m_Pos);
  m_Pos = pos;
  return m_Buffer;
}

bool CParser::Find(char c)
{
  if (GetChar() != c)
  {
    if (EOT(*m_Pos))
    {
      m_Buffer = "end of file";
    }
    else if (EOL(*m_Pos))
    {
      m_Buffer = "end of line";
    }
    else
    {
      m_Buffer = "character \'";
      m_Buffer += *m_Pos;
      m_Buffer += '\'';
    }

    m_Buffer += " was found instead of \'";
    m_Buffer += c;
    m_Buffer += '\'';
    return false;
  }
  m_Pos++;
  return true;
}

bool CParser::TryFind(char c)
{
  if (IsChar(c))
  {
    Next();
    return true;
  }
  return false;
}

void CParser::SkipSpace()
{
  char c;
  bool char_found = false;

  while (!char_found)
  {
    c = m_Pos[0];
    if (EOL(c))
    {
      m_LineNb++;
      m_Pos++;
      m_LineStart = true;
#if (DEBUG_LEVEL >= 1)
      CDisplay ds;
      ds += "# line ";
      ds += CString(m_LineNb);
      ds += " : ";
      const char *pos = strchr(m_Pos, '\n');
      if (pos)
        ds.Append(m_Pos, pos - m_Pos);
      ds.Log(LOG_INFO);
#endif
    }
    else if (isspace(c)) //\n is considered as space
    {
      m_Pos++;
    }
    else if (c == '/')
    {
      m_LineStart = false;
      c = m_Pos[1];
      if (c == '/')
      {
        SkipLineComment();
      }
      else if (c == '*')
      {
        if (!SkipBlockComment())
        {
          char_found = true;
        }
      }
      else
      {
        char_found = true;
      }
    }
    else if ((c == m_SymbolMacro) && m_LineStart)
    {
      m_LineStart = false;
      ProcessMacro();
    }
    else if (EOT(c))
    {
      m_LineStart = true;
      if (!CloseFile())
      {
        char_found = true;
      }
    }
    else
    {
      m_LineStart = false;
      char_found = true;
    }
  }
}

void CParser::SkipLineComment()
{
  m_Pos += 2;
  while (!EOT(*m_Pos))
  {
    if (EOL(*m_Pos))
    {
      return;
    }
    m_Pos++;
  }
}

bool CParser::SkipBlockComment()
{
  m_Pos += 2;
  while (!EOT(*m_Pos))
  {
    if ((m_Pos[0] == '*') && (m_Pos[1] == '/'))
    {
      m_Pos += 2;
      return true;
    }
    if (EOL(*m_Pos))
    {
      m_LineNb++;
    }
    m_Pos++;
  }
  return false;
}

bool CParser::TryMatchSymbol(const char *&symbol_str)
{
  const char *s1 = symbol_str;
  const char *s2 = m_Pos;

  if (EOT(*s1) || IsWord(*s1))
  {
    return false;
  }

  while (*s1 && !(IsWord(*s1) || (*s1 == '(')))
  {

    if (*s1 == m_OperatorAlpha)
    {
      s1++; // Next char is considered as word
      if (*s2 == *s1)
      {
        s2++;
        s1++;
        if (IsWord(*s2))
          return false;
      }
      else
        return false;
    }
    else if (*s2 == *s1)
    {
      s1++;
      s2++;
    }
    else if (*s1 == m_OperatorExclude)
    {
      s1++;           //'\'
      if (*s2 == *s1) // Check for forbidden characters
      {
        return false;
      }
      s1++;
    }
    else
    {
      return false;
    }
  }

  symbol_str = s1;
  m_Pos = s2;
  return true;
}

int CParser::GetQuotes()
{
  int ret = 0;
  m_Buffer.Clear();

  SkipSpace();
  if (TryFind(m_StringDelimiter))
  {
    const char *pos = ::strchr(m_Pos, m_StringDelimiter);
    if (pos)
    {
      m_Buffer.Copy(m_Pos, pos - m_Pos);
      m_Pos = pos + 1;
      ret = 1;
    }
    else
    {
      CString s(" not found");
      s.Prepend(m_StringDelimiter);
      Error(s);
    }
  }
  else if (TryFind(m_HeaderDelimiterIn))
  {
    const char *pos = ::strchr(m_Pos, m_HeaderDelimiterOut);
    if (pos)
    {
      m_Buffer.Copy(m_Pos, pos - m_Pos);
      m_Pos = pos + 1;
      ret = 2;
    }
    else
    {
      CString s(" not found");
      s.Prepend(m_HeaderDelimiterIn);
      Error(s);
    }
  }
  else
  {
    ret = 0;
  }

  return ret; // return m_Buffer;
}

CString CParser::GetPath(const CString &path)
{
  CString s;
  const char *full_path = path.GetBufferPtr();
  const char *basename_path = ::strrchr(full_path, PATH_SEPARATOR);

  if (basename_path)
  {
    s = full_path;
    s.SetLength(basename_path - full_path + 1);
  }
  else
    s.Clear();

  return s;
}

bool CParser::LoadFromFile(const CString &name)
{
  int size;
  FILE *file;
  CParserContext context;
  char *text;

  file = fopen(name.GetBufferPtr(), "r");
  if (file)
  {
#if (DEBUG_LEVEL >= 1)
    CDisplay::Log(LOG_INFO, "opening file %s", name.GetBufferPtr());
#endif
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    text = new char[size + 1];
    size = fread(text, sizeof(char), size, file);
    fclose(file);
    text[size] = '\0';
    context.m_Text = text;
    context.m_FileName = GetFileName();
    context.m_LineNb = GetLineNb();
    context.m_Pos = GetPos();
    m_ContextArray.Push(context);
    SetPos(text);
    m_LineNb = 1;
    m_FileName = name;
    m_LineStart = true;
    return true;
  }
  Error(CString("failed to open file \"") + name + CString("\""));
  return false;
}

bool CParser::CloseFile()
{
  if (m_ContextArray.GetSize())
  {
    const CParserContext &context = m_ContextArray.Pop();
    delete context.m_Text;
    SetPos(context.m_Pos);
    m_LineNb = context.m_LineNb;
    m_FileName = context.m_FileName;
    return true;
  }
  return false;
}

bool CParser::ProcessMacro()
{
  bool ret = false;
  const char *pos = m_Pos;
  m_Pos++;

  GetWord();
  if (m_Buffer == "include")
  {
    int type = GetQuotes();
    if (type == 1)
    {
      ret = LoadFromFile(GetPath(m_FileName) + GetBuffer());
    }
    else if (type == 2)
    {
      ret = LoadFromFile(m_RootPath + GetBuffer());
    }
  }
  else if (m_Buffer == "symbol")
  {
    if (GetQuotes())
    {
      m_SymbolBuffer = m_Buffer;
      m_SymbolMacroSeen = true;
      ret = true;
    }
  }
  else
  {
    Error(CString("macro ") + GetBuffer());
    m_Pos = pos;
    ret = false;
  }
  return ret;
}

bool CParser::CheckEOT() const
{
  if (!EOT(*m_Pos))
  {
    CString s("extra character found at end of line: ");
    s += *m_Pos;
    Error(s);
    return false;
  }
  return true;
}

void CParser::Error(const CString &s) const
{
  CDisplay ds(s);
  ds += " in line ";
  ds += CDisplay(GetLineNb(), 10);
  ds += " of file \"";
  ds += GetFileName();
  ds += "\"";
  ds.Log(LOG_ERR);
}
