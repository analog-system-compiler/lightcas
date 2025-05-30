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

#include <cctype>
#include "element.h"
#include "mathexpression.h"

void CMathExpression::Display(CDisplay &ds, bool bAll) const
{
  unsigned pos = m_StackSize;

  if (pos == 0)
  {
    ds += '0';
  }
  else
  {
    while (pos)
    {
      pos = DisplayBranch(ds, pos);
      if (!bAll)
      {
        return;
      }
      if (pos)
      {
        ds += ' ';
      }
    }
  }
}

pos_t CMathExpression::DisplayBranch(CDisplay &ds, pos_t pos, unsigned char precedence) const
{
  const CElement *e;
  unsigned pos2;

  ASSERT(pos);
  pos2 = DisplaySymbol(ds, pos, precedence);

  if (pos == pos2) // no symbol displayed
  {
    OP_CODE op = Pop(pos);
    e = RefToElement(op);
    e->Display(ds);

    if (e->IsNumeric() || e->IsFunct()) // For Rand(), n=0
    {
      unsigned n = e->GetFunction()->GetParameterNb();
      ASSERT(n <= CElementDataBase::MAX_PAR);
      ds += '(';
      if (n)
      {
        unsigned i;
        pos_t pos_array[CElementDataBase::MAX_PAR];
        for (i = 0; i < n; i++)
        {
          pos_array[i] = pos;
          pos = NextBranch(pos);
        }
        for (i = 1; i < n; i++)
        {
          DisplayBranch(ds, pos_array[n - i]);
          ds += ' ';
        }
        DisplayBranch(ds, pos_array[0]);
      }
      ds += ')';
    }
  }
  else
  {
    pos = pos2;
  }

  return pos;
}

pos_t CMathExpression::DisplaySymbol(CDisplay &ds, pos_t pos, unsigned char index) const
{
  pos_t pos_array[CElementDataBase::MAX_PAR];

  const CSymbolSyntaxArray &st = m_ElementDB->GetSymbolTable();
  for (unsigned i = 0; i < st.GetSize(); i++)
  {
    const CSymbolSyntaxStruct *ss = st[i];
    pos_t pos1 = Match(pos, ss->m_Equation, pos_array);
    if (pos1 != pos)
    {
      const char *sp = ss->m_Syntax;
      unsigned char precedence = ss->m_Precedence;
      if ((precedence < index) || (ds.IsDebug() && (precedence == index)))
      {
        ds += '(';
        DisplaySymbolString(sp, pos_array, precedence, ds);
        ds += ')';
      }
      else
      {
        DisplaySymbolString(sp, pos_array, precedence, ds);
      }
      pos = pos1;
      break;
    }
  }
  return pos;
}

void CMathExpression::DisplaySymbolString(const char *sp, pos_t pos_array[CElementDataBase::MAX_PAR], unsigned char precedence, CDisplay &ds) const
{
  char c;
  unsigned i;
  unsigned char precedence2;

  while ((c = *sp++))
  {
    if (c == '(')
    {
      precedence++;
    }
    else if (c == ')')
    {
      if (precedence)
      {
        precedence--;
      }
      else
        ASSERT(false);
    }
    else if ((c == CParser::m_OperatorExclude) || (c == CParser::m_OperatorAlpha))
    {
      if (c == CParser::m_OperatorAlpha)
      {
        ds += *sp;
      }
      sp++; // Exclude operator
    }
    else if (::isalpha(c))
    {
      precedence2 = precedence;
      if (::isupper(c))
      {
        precedence2 = 0;
      }
      ASSERT(c >= 'A');
      i = ::tolower(c) - 'a';
      DisplayBranch(ds, pos_array[i], precedence2);
    }
    else
    {
      ds += c;
    }
  }
}
