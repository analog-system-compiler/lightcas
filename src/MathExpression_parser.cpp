/*******************************************************************************/
/*  Copyright (C) 2014 The LightCAS project                                    */
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

#include <cctype>
#include "Element.h"
#include "MathExpression.h"

int CMathExpression::Parse(CParser &IC)
{
  Clear();
  return GetLevel(IC);
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
  
  //m_ElementDB->GetEvaluator()->ClearValue();

  while (!IC.IsStopChar())
  {
    if (GetLevel(IC, 0))
      i++;
    else
    {
      CDisplay ds;
      ds += "Error: ";
      ds += IC.GetBuffer();
      ds += " in line ";
      ds += CDisplay(IC.GetLineNb(), 10);
      ds += " in file ";
      ds += IC.GetFileName();
      ds.Print();
      TRACE(ds.GetBufferPtr());
      return -2; // IC.Error( CParserException::ID_ERROR_OPERATOR_EXPECTED );
    }
  }
  return i;
}

bool CMathExpression::GetLevel(CParser &IC, unsigned priority)
{
  unsigned precedence;
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
    precedence = var_found ? priority : 0;
    ret = ParseOperator(IC, precedence, var_found);
    if (ret == 2)
      return false;
    if (ret == 1)
      var_found = true;
  }

  return var_found;
}

int CMathExpression::ParseOperator(CParser &IC, unsigned &precedence, bool var_found)
{

  char c;
  const CSymbolSyntaxArray &st = m_ElementDB->GetSymbolTable();
  const char *char_pos = IC.GetPos();

  while (precedence < st.GetSize())
  {
    const char *sp = st[precedence]->m_Syntax;
    bool check_var = !CParser::EOT(TryMatchExp(sp));
    if (check_var == var_found)
    {
      while (IC.TryMatchSymbol(sp))
      {
        c = TryMatchExp(sp);
        if (c)
        {
          if (IC.IsStopChar())
            return 2;

          if (!GetLevel(IC, ::isupper(c) ? 0 : precedence + 1))
            // return 2;
            goto next;
        }
      }

      if (CParser::EOT(*sp))
      {
        Push(st[precedence]->m_Equation.GetLastOperator());
#if (DEBUG_LEVEL >= 3)
        CDisplay ds;
        ds.Clear();
        Display(ds);
        TRACE(ds.GetBufferPtr());
#endif
        return 1;
      }
    }
  next:
    IC.SetPos(char_pos);
    precedence++;
  }

  return 0;
}

bool CMathExpression::ParseElement(CParser &IC)
{
  CElement *e;
  CFunction *f;
  int i = 0;
  bool is_funct = false;

  ASSERT(IC.IsWord());
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
#if (DEBUG_LEVEL >= 0)
      CDisplay ds;
      ds += "SetParamNb   : ";
      e->Display(ds);
      ds += ':';
      ds += CString(e->GetFunction()->GetParameterNb());
      TRACE(ds.GetBufferPtr());
#endif
    }
    else
    {
#if (DEBUG_LEVEL >= 0)
      CDisplay ds;
      if (f->GetParameterNb() > (unsigned)i)
        ds = "Error: not enough parameters for: ";
      else if (f->GetParameterNb() < (unsigned)i)
        ds = "Error: too much parameters for: ";
      ds += e->GetName();
      ds += ". Expected:";
      ds += CString(f->GetParameterNb());
      ds += ", got:";
      ds += CString(i);
      ds.Print();
      TRACE(ds.GetBufferPtr());
#endif
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
