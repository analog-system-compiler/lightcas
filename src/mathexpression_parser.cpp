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

enum
{
  PARSE_NOTFOUND = 0,
  PARSE_FOUND,
  PARSE_ERROR,
};

int CMathExpression::Parse(CParser &IC)
{
  Clear();
  int ret = GetLevel(IC);
  IC.CheckEOT();
  return ret;
}

bool CMathExpression::ParseAtom(CParser &IC)
{
  bool bStatus = false;

  if (IC.IsDigit())
  {
    CEvaluator *eval = m_ElementDB->GetEvaluator();
    const char *pos = IC.GetPos();
    pos = eval->GetValueFromString(pos);
    IC.SetPos(pos);
    PushEvalElement(*eval);
    bStatus = true;
  }
  else if (IC.IsWord())
  {
    bStatus = ParseElement(IC);
  }
  else if (IC.TryFind('('))
  {
    GetLevel(IC);
    bStatus = IC.Find(')');
  }
  return bStatus;
}

int CMathExpression::GetLevel(CParser &IC)
{
  int i = 0;

  while (!IC.IsStopChar())
  {
    if (GetLevel(IC, 0))
      i++;
    else
    {
      IC.Error(IC.GetBuffer());
      return -2; // IC.Error( CParserException::ID_ERROR_OPERATOR_EXPECTED );
    }
  }
  return i;
}

bool CMathExpression::GetLevel(CParser &IC, unsigned priority)
{
  bool var_found;
  int ret = 1;

  while (!IC.IsStopChar() && IC.IsSymbolMacro())
  {
    IC.ClearSymbolMacro();
    m_ElementDB->AssociateSymbol(IC);
  }

  var_found = ParseAtom(IC);

  while (!IC.IsStopChar() && (ret == 1))
  {
    unsigned precedence = var_found ? priority : 0;
    ret = ParseOperator(IC, precedence, var_found);
    if (ret == PARSE_ERROR)
      return false;
    if (ret == PARSE_FOUND)
      var_found = true;
  }

  return var_found;
}

int CMathExpression::ParseOperator(CParser &IC, unsigned &index, bool var_found)
{

  char c;
  const CSymbolSyntaxArray &st = m_ElementDB->GetSymbolTable();
  const char *char_pos = IC.GetPos();

  while (index < st.GetSize())
  {
    const char *sp = st[index]->m_Syntax;
    bool check_var = !CParser::EOT(TryMatchExp(sp));
    if (check_var == var_found)
    {
      while (IC.TryMatchSymbol(sp))
      {
        c = TryMatchExp(sp);
        if (c)
        {
          if (IC.IsStopChar())
            return PARSE_ERROR;

          if (!GetLevel(IC, ::isupper(c) ? 0 : index + 1))
            goto next;
        }
      }

      if (CParser::EOT(*sp))
      {
        Push(st[index]->m_Equation.GetLastOperator());
#if (DEBUG_LEVEL >= 3)
        CDisplay ds;
        ds.Clear();
        Display(ds);
        ds.Log(LOG_DEBUG);
#endif
        return PARSE_FOUND;
      }
    }
  next:
    IC.SetPos(char_pos);
    index++;
  }

  return PARSE_NOTFOUND;
}

bool CMathExpression::ParseElement(CParser &IC)
{
  CElement *e;
  CFunction *f;
  int i = 0;
  bool is_funct = false;

  e = m_ElementDB->GetElement(IC.GetWord());

  if (!e)
  {
    return false;
  }

  f = e->GetFunction();
  if (e->IsFunct())
    IC.SkipSpace();

  // Check if this is a function that needs parameters
  if (IC.TryFind('('))
  {
    is_funct = true;
    i = GetLevel(IC);
    if (i < 0)
    {
      return false;
    }
    if (!IC.Find(')'))
    {
      return false;
    }
  }

  // We got parameters
  if (f->GetParameterNb() != (unsigned)i)
  {
    if (!e->IsFunct())
    {
      f->SetParameterNb(i);
      e->SetFunct();
#if (DEBUG_LEVEL > 0)
      CDisplay ds;
      ds += "SetParamNb   : ";
      e->Display(ds);
      ds += ':';
      ds += CString(e->GetFunction()->GetParameterNb());
      ds.Log(LOG_DEBUG);
#endif
    }
    else
    {
      CDisplay ds;
      if (f->GetParameterNb() > (unsigned)i)
        ds = "Error: not enough parameters for: ";
      else // if (f->GetParameterNb() < (unsigned)i)
        ds = "Error: too much parameters for: ";
      ds += e->GetName();
      ds += ". Expected:";
      ds += CString(f->GetParameterNb());
      ds += ", got:";
      ds += CString(i);
      ds.Log(LOG_ERR);
      return false;
    }
  }
  else if (is_funct) // case with 0 parameters
    e->SetFunct();

  Push(e);
  return true;
}

char CMathExpression::TryMatchExp(const char *&sp)
{
  char c = *sp++;
  if (c == '(') // Parenthesis not used in parsing stage
  {
    c = *sp++;
    if (*sp == ')') // Parenthesis not used in parsing stage
    {
      sp++;
    }
  }

  if (!CParser::IsWord(c))
  {
    c = '\0';
    sp--;
  }
  return c;
}
