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
  bool bStatus;

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
    if (bStatus && !IC.IsStopChar())
    {
      if (IC.GetChar() == CParser::m_StringDelimiter)
      {
        m_ElementDB->AssociateSymbol(IC, *this);
        m_StackSize = NextBranch(m_StackSize);
        Push(CElementDataBase::OP_NONE);
      }
    }
  }
  else if (IC.TryFind('('))
  {
    GetLevel(IC);
    if (IC.IsStopChar())
    {
      bStatus = IC.Find(')');
    }
    else
    {
      bStatus = false;
    }
  }
  else
  {
    bStatus = false;
  }
  return bStatus;
}

int CMathExpression::GetLevel(CParser &IC)
{
  int i = 0;
  while (!IC.IsStopChar())
  {
    if (!GetLevel(IC, 0))
    {
      return -2; // IC.Error( CParserException::ID_ERROR_OPERATOR_EXPECTED );
    }
    i++;
  }
  return i;
}

bool CMathExpression::GetLevel(CParser &IC, unsigned priority)
{
  unsigned precedence;
  const char *char_pos;
  char c;
  bool var_found;
  const CSymbolSyntaxArray &st = m_ElementDB->GetSymbolTable();

  if (IC.IsStopChar())
  {
    return false;
  }

  var_found = ParseAtom(IC);

  do
  {

    if (var_found)
    {
      if (IC.IsStopChar()) // when ')' found, set precedence to maximum to avoid searching symbols
      {
        precedence = st.GetSize();
      }
      else
      {
        precedence = priority;
      }
    }
    else
    {
      precedence = 0;
    }

    char_pos = IC.GetPos();

    while (precedence < st.GetSize())
    {
      const char *sp = st[precedence]->m_Syntax;
      bool check_var = (TryMatchExp(sp) != '\0');
      if (check_var == var_found)
      {
        while (*sp && IC.TryMatchSymbol(sp))
        {
          c = TryMatchExp(sp);
          if (c)
          {
            if (!GetLevel(IC, (c < 'a') ? 0 : precedence + 1))
            {
              return false;
            }
          }
        }

        if (*sp == '\0')
        {
          Push(st[precedence]->m_Equation.GetLastOperator());
          var_found = true;
#if (DEBUG_LEVEL >= 3)
          CDisplay ds;
          ds.Clear();
          Display(ds);
          TRACE(ds.GetBufferPtr());
#endif
          break;
        }
      }

      IC.SetPos(char_pos);
      precedence++;
    }
  } while (precedence < st.GetSize());

  return var_found;
}

bool CMathExpression::ParseElement(CParser &IC)
{
  CElement *e;
  CFunction *f;
  int i = 0;

  ASSERT(IC.IsWord());
  CEvaluator *eval = m_ElementDB->GetEvaluator();
  eval->ClearValue();
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
      TRACE(ds.GetBufferPtr());
#endif
      return false;
    }
  }

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
