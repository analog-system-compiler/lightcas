

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

#include "lcstring.h"
#include "lcvector.h"

struct CParserContext
{
  const char *m_FileName;
  unsigned m_LineNb;
  const char *m_Text;
  const char *m_Pos;
};

typedef CVector<CParserContext> CParserContextArray;

class CParser
{

protected:
  static const char m_CharTab[];
  unsigned m_LineNb;
  CString m_FileName;
  CString m_RootPath;
  CString m_Buffer;
  CString m_SymbolBuffer;
  const char *m_Pos;
  CParserContextArray m_ContextArray;
  bool m_LineStart;
  bool m_SymbolMacroSeen;

public:
  static const char m_StringDelimiter = '"';
  static const char m_HeaderDelimiterIn = '<';
  static const char m_HeaderDelimiterOut = '>';
  static const char m_SymbolMacro = '`';
  static const char m_OperatorExclude = '\\';
  static const char m_OperatorAlpha = '$';

protected:
  void SkipLineComment();
  bool SkipBlockComment();

public:
  bool Find(char c);
  bool TryFind(char c);
  void Init(const char *pText);
  bool TryMatchSymbol(const char *&symbol_str);
  bool LoadFromFile(const CString &name);
  bool CloseFile();
  void SkipSpace();
  void Error(const CString &s) const;
  bool ProcessMacro();
  int GetQuotes();
  bool CheckEOT() const;
  int GetInt(int base = 10) { return (int)strtol(m_Pos, (char **)&m_Pos, base); }
  const CString &GetWord();
  const CString &GetBuffer() const { return m_Buffer; }
  const CString &GetSymbolBuffer() const { return m_SymbolBuffer; }
  const CString &GetFileName() const { return m_FileName; }
  char GetChar() const { return (*m_Pos); }
  void CopyBuffer(const char s[], unsigned len) { m_Buffer.Copy(s, len); }
  void CopyBuffer(const CString &s) { m_Buffer = s; }
  unsigned GetLineNb() const { return m_LineNb; }
  void SetRootPath(const CString &root_path) { m_RootPath = root_path; }
  static CString GetPath(const CString &path);

  static bool IsWord(char c) { return (m_CharTab[(int)c] & 1) != 0; }
  static bool IsStopChar(char c) { return (m_CharTab[(int)c] & 2) != 0; }
  static bool IsDigit(char c) { return (m_CharTab[(int)c] & 4) != 0; }

  bool IsWord() const { return IsWord(GetChar()); }
  bool IsStopChar()
  {
    SkipSpace();
    return IsStopChar(GetChar());
  }
  bool IsDigit() const { return IsDigit(GetChar()); }
  bool IsChar(char c) const { return GetChar() == c; }
  bool IsSymbolMacro() const { return m_SymbolMacroSeen; }
  void ClearSymbolMacro() { m_SymbolMacroSeen = false; }

  const char *GetPos() const { return m_Pos; }
  void SetPos(const char *pText) { m_Pos = pText; }
  void Next()
  {
    if (*m_Pos)
      m_Pos++;
  }

  static bool EOT(char x) { return x == '\0'; }
  static bool EOL(char x) { return x == '\n'; }

  CParser();
  explicit CParser(const char *pText);
  virtual ~CParser();
};
