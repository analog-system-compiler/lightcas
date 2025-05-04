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

#include <time.h>
#include <cstring>
#include "Debug.h"
#include "LCVector.h"
#include "MathExpression.h"
#include "Element.h"
#include "Function.h"

static clock_t clock_begin;
static constexpr unsigned MAX_ITERATION = 100000000;

#ifdef RECURSIVE_ALGO

void CMathExpression::OptimizeTree()
{
  pos_t pos;
  pos_t pos_array[CElementDataBase::MAX_PAR + 1];

  if (ExecuteCommand())
  {
    const CAlgebraRule *rule = RuleSearch(pos, pos_array);
    if (rule)
    {
#ifdef _TEST
      rule->m_AccessNb++;
#endif
#if (DEBUG_LEVEL >= 1)
      CDisplay ds;
      ds += "OptimizeTree rule";
      //        ds += CString( rule->m_LineNo );
      ds += " [";
      e->Display(ds);
      ds += "] ";
      rule->Display(i, ds);
      ds += " : ";
      Display(ds);
      ds += " => ";
#endif
      if (rule->m_bHasRule)
      {
        ApplyRule(pos, pos_array, rule->m_DstEquation);
      }

#if (DEBUG_LEVEL >= 1)
      Display(ds);
      TRACE(ds.GetBufferPtr());
#endif
    }
  }
}

CAlgebraRule *CMathExpression::RuleSearch(pos_t &pos, pos_t pos_array[CElementDataBase::MAX_PAR])
{

  OP_CODE op = GetLastOperator();
  const CElement *e = RefToElement(op);
  ASSERT(e);
  const CFunction *funct = e->GetFunction();

  if (funct)
  {
    unsigned i;
    unsigned n;
    n = funct->m_AlgebraRuleArray.GetSize();
    for (i = 0; i < n; i++)
    {
      CAlgebraRule *rule = funct->m_AlgebraRuleArray[i];
      const CMathExpression *equ = &(rule->m_SrcEquation);

      pos = Match(GetSize(), *equ, pos_array);
      if (pos != GetSize())
      {
        return rule;
      }
    }
  }
  return NULL;
}
void CMathExpression::ApplyRule(pos_t pos2, pos_t pos_array[CElementDataBase::MAX_PAR], const CMathExpression &rule_equ)
{
  CMathExpression equ(m_ElementDB);
  OP_CODE op3;
  pos_t pos;
  unsigned j;

#if (DEBUG_LEVEL >= 2)
  CDisplay ds;
  ds += "Apply rule ";
  rule_equ.Display(ds);
  ds += " : ";
  equ.Display(ds);
  ds += " => ";
#endif

  equ.SetSize(GetSize());
  equ.Clear();

  unsigned n = rule_equ.GetSize();
  for (pos = 0; pos < n; pos++)
  {

    op3 = rule_equ.Get(pos);

    if (CElementDataBase::IsReservedOP(op3))
    {
      j = ReservedParameterIndex(op3);
      pos_t pos2 = pos_array[j];
      ASSERT(pos2 <= GetSize());
      pos_t pos3 = pos + 1;
      if (pos3 < n) // if followed by FUNCT1 or FUNCT2
      {
        op3 = rule_equ.Get(pos3);
        if (CElementDataBase::IsFunctionOP(op3))
        {
          pos = pos3;
          op3 = Get(pos_array[j] - 1);
          goto L1;
        }
      }
      equ.PushBranch(*this, pos2); // WARNING pos2 is modified
    }
    else
    {
    L1:
      ASSERT(RefToElement(op3) != NULL);
      equ.Push(op3);
      equ.OptimizeTree(); // optimization is only made on the top branch
    }
  }

  SetSize(pos2);
  Push(equ);

#if (DEBUG_LEVEL >= 2)
  Display(ds);
  TRACE(ds.GetBufferPtr());
#endif
}

#else

CContextArray CMathExpression::m_ContextStack;

void CMathExpression::OptimizeTree()
{
  context_t save_context, save_context_new;
  clock_begin = clock();
  unsigned iteration = MAX_ITERATION;

  if (IsEmpty())
    return;

  if (ExecuteDirective())
  {
    if (RuleSearch(RefToElement(GetLastOperator()), save_context_new))
    {
      m_ContextStack.Push(save_context_new);
    }
  }

L1:
  while (m_ContextStack.GetSize() && iteration)
  {
    save_context = m_ContextStack.Pop();
    pos_t n = save_context.m_RuleDstExp->GetSize();
    pos_t pos5 = save_context.m_RuleDstPos;
    while (pos5 < n)
    {
      OP_CODE op3 = save_context.m_RuleDstExp->Get(pos5++);
      if (CElementDataBase::IsReservedOP(op3))
      {
        unsigned j = ReservedParameterIndex(op3);
        ASSERT(j < CElementDataBase::MAX_PAR);
        pos_t pos2 = save_context.m_PosArray[j];
        ASSERT(pos2 <= GetSize());
        op3 = save_context.m_RuleDstExp->Get(pos5);
        if (!CElementDataBase::IsFunctionOP(op3))
        {
          pos_t pos1 = NextBranch(pos2);      // TODO: optimize
          Move(GetSize(), pos1, pos2 - pos1); // Append at the end of equation
          continue;
        }
        pos5++;
        op3 = Get(pos2 - 1); // Get real function OP
      }

      ASSERT(RefToElement(op3) != NULL);
      Push(op3);
      if (ExecuteDirective())
      {
        if (RuleSearch(RefToElement(GetLastOperator()), save_context_new))
        {
          save_context.m_RuleDstPos = pos5;
          m_ContextStack.Push(save_context); // TODO: avoid push/pop
          m_ContextStack.Push(save_context_new);
          iteration--;
          goto L1; // Do recursion
        }
      }
    }
    ReduceTree(save_context);
  }
  if (iteration == 0)
  {
    Init(m_ElementDB->GetElement("ITERATION_LIMIT_ERROR"));
  }
#if (DEBUG_LEVEL >= 1)
  if (NextBranch(m_StackSize) != 0)
  {
    CDisplay ds("Simplify error:");
    Display(ds, false);
    ds.Log(LOG_INFO);
  }
#endif
}

void CMathExpression::ReduceTree(const context_t &save_context)
{
  pos_t pos1 = save_context.m_Size;
  pos_t pos2 = GetSize();
#if (DEBUG_LEVEL >= 1)
  CDisplay ds;
  ds += "OptimizeTree rule";
  save_context.m_Rule->Display(save_context.m_RuleNb, ds);
  ds += " : ";
  SetSize(pos1);
  Display(ds, false);
  ds += " => ";
#endif
  Move(save_context.m_Pos, pos1, pos2 - pos1);
#if (DEBUG_LEVEL >= 1)
  Display(ds, false);
  ds.Log(LOG_DEBUG);
#endif
}

bool CMathExpression::RuleSearch(const CElement *e, context_t &save_context)
{
  unsigned i;
  unsigned n;

  ASSERT(e);
  const CFunction *funct = e->GetFunction();

  if (funct)
  {
    n = funct->m_AlgebraRuleArray.GetSize();
    for (i = 0; i < n; i++)
    {
      CAlgebraRule *rule = funct->m_AlgebraRuleArray[i];
      const CMathExpression *equ = &(rule->m_SrcEquation);

      pos_t pos = Match(GetSize(), *equ, save_context.m_PosArray);
      if (pos != GetSize())
      {
#ifdef _TEST
        rule->m_AccessNb++;
#endif
        ASSERT(rule->m_bHasRule);
        if (rule->m_bHasRule)
        {
          save_context.m_Pos = pos;        // low stack limit
          save_context.m_Size = GetSize(); // high stack limit, start of subsequent expressions
          save_context.m_RuleDstPos = 0;   // index in the rule. From 0 to m_DstEquation.GetSize()
          save_context.m_RuleDstExp = &(rule->m_DstEquation);
#ifdef _DEBUG
          save_context.m_Rule = rule;
          save_context.m_RuleNb = i;
#endif
          DisplayRuleMessage(e);
          return true;
        }
      }
    }
  }
  return false;
}

#endif

bool CMathExpression::Match(const CMathExpression &equ) const
{
  pos_t pos_array[CElementDataBase::MAX_PAR];
  return Match(GetSize(), equ, pos_array) != GetSize();
}

pos_t CMathExpression::Match(pos_t pos3, const CMathExpression &equ, pos_t pos_array[CElementDataBase::MAX_PAR]) const
{
  unsigned i;
  bool match = true;
  pos_t pos1 = equ.GetSize();
  pos_t pos2 = pos3;

  for (i = 0; i < CElementDataBase::MAX_PAR; i++)
  {
    pos_array[i] = 0;
  }

  // Try to match pattern
  while (match && pos1)
  {
    OP_CODE op1 = equ.Pop(pos1);
    if (CElementDataBase::IsReservedOP(op1)) // a,b or c
    {
      i = ReservedParameterIndex(op1);
      if (!pos_array[i])
        pos_array[i] = pos2;
      else
        match = CompareBranch(pos2, pos_array[i]);

      if (match)
      {
        pos2 = NextBranch(pos2);
      }
    }
    else if (CElementDataBase::IsFunctionOP(op1))
    {
      OP_CODE op3 = equ.Pop(pos1);
      if (CElementDataBase::IsReservedOP(op3))
      {
        i = ReservedParameterIndex(op3);
        if (!pos_array[i])
          pos_array[i] = pos2;
        else
          match = CompareBranch(pos2, pos_array[i]);

        if (match)
        {
          OP_CODE op2 = Pop(pos2);
          match = false;
          if (!CElementDataBase::IsReservedOP(op2))
          {
            CElement *e = RefToElement(op2);
            if (e->IsConst())
            {
              match = (op1 == CElementDataBase::OP_CONST);
            }
            else if (e->IsVar())
            {
              match = (op1 == CElementDataBase::OP_ELEM);
            }
            else
            {
              match = (op1 == (CElementDataBase::OP_FUNCT0 + e->GetFunction()->GetParameterNb()));
            }
          }
        }
      }
      else
      {
        match = false;
        ASSERT(false);
      }
    }
    else
    {
      OP_CODE op2 = Pop(pos2);
      match = (op2 == op1);
    }
  }

  return match ? pos2 : pos3;
}

bool CMathExpression::ExecuteDirective()
{
  OP_CODE op3 = Pop(m_StackSize);

#if (DEBUG_LEVEL >= 2)
  CDisplay ds;
  if ((op3 == CElementDataBase::OP_SET) || (op3 == CElementDataBase::OP_GET) || (op3 == CElementDataBase::OP_RANK) || (op3 == CElementDataBase::OP_EVAL))
  {
    ds += "Execute Command ";
    RefToElement(op3)->Display(ds);
    ds += " : ";
    Display(ds);
    ds += " => ";
  }
#endif

  if (op3 == CElementDataBase::OP_SET)
  {
    CMathExpression equ_dst(m_ElementDB);
    CMathExpression equ_src(m_ElementDB);
    equ_dst.PushBranch(*this, m_StackSize);
    equ_src.PushBranch(*this, m_StackSize);
    OP_CODE op1 = equ_src.GetLastOperator();
    CElement *e = RefToElement(op1);
    e->AddFunction(equ_src, equ_dst);
    Push(equ_dst);
    return false;
  }

  if (op3 == CElementDataBase::OP_GET)
  {
    return true;
  }

  if (op3 == CElementDataBase::OP_RANK)
  {
    OP_CODE op1 = Pop(m_StackSize);
    OP_CODE op2 = Pop(m_StackSize);
    Push(op1 > op2 ? op1 : op2);
    return false;
  }

  if (op3 == CElementDataBase::OP_EVAL)
  {
    CEvaluator *eval = m_ElementDB->GetEvaluator();
    if (eval)
    {
      pos_t pos = NextBranch(m_StackSize);
      eval->Evaluate(m_StackSize - pos, m_StackArray + pos);
      m_StackSize = pos;
      PushEvalElement(*eval);
    }
    return false;
  }

  if (op3 == CElementDataBase::OP_CHECK)
  {
    pos_t pos1 = m_StackSize;
    pos_t pos2 = NextBranch(pos1);
    CDisplay ds1;
    ds1.SetDebug();
    double time_spent = (double)(clock() - clock_begin) / (CLOCKS_PER_SEC / 1000);
    clock_begin = clock();
    ds1 += "[";
    ds1 += CString((int)time_spent);
    ds1 += "ms] ";
    if (!CompareBranch(pos1, pos2))
    {
      ds1 += "assertion failure: ";
      DisplayBranch(ds1, pos2);
      ds1 += " != ";
      DisplayBranch(ds1, pos1);
      ds1.Log(LOG_ERR);
    }
    else
    {
      ds1 += "assertion success: ";
      DisplayBranch(ds1, pos1);
      ds1.Log(LOG_INFO);
    }
    m_StackSize++;
    return false;
  }

  if (op3 == CElementDataBase::OP_PRINT)
  {
    CDisplay ds1;
    CMathExpression equ(m_ElementDB);
    pos_t pos = m_StackSize;
    equ.PushBranch(*this, pos);
    equ.Display(ds1);
    ds1.Print();
    return false;
  }

  m_StackSize++;

#if (DEBUG_LEVEL >= 2)
  if ((op3 == CElementDataBase::OP_SET) || (op3 == CElementDataBase::OP_GET) || (op3 == CElementDataBase::OP_RANK) || (op3 == CElementDataBase::OP_EVAL))
  {
    Display(ds);
    ds.Log(LOG_DEBUG);
  }
#endif
  return true;
}

bool CMathExpression::CompareBranchElement(pos_t pos1, pos_t pos2) const
{

  OP_CODE op4 = Pop(pos2);
  const CElement *e = RefToElement(op4);

  if (e && e->IsVoid())
  {
    OP_CODE op3 = Pop(pos1);
    return (op3 == op4);
  }

  return false;
}

bool CMathExpression::CompareBranch(pos_t pos1, pos_t pos2) const
{
  pos_t pos11, pos22;

  if (CompareBranchElement(pos1, pos2))
  {
    return true;
  }

  pos11 = NextBranch(pos1);
  pos22 = NextBranch(pos2);

  return ((pos1 - pos11) == (pos2 - pos22)) && !::memcmp(&m_StackArray[pos11], &m_StackArray[pos22], (pos2 - pos22) * sizeof(OP_CODE));
}

void CMathExpression::PushEvalElement(CEvaluator &eval)
{
  CString s;
  bool minus = eval.Display(s);
  CElement *e = m_ElementDB->GetElement(s);
  eval.SetElementValue(e->ToRef());
  e->SetConst();
  Push(e);
  if (minus)
  {
    Push(CElementDataBase::OP_NEG);
  }
}

void CMathExpression::DisplayRuleMessage(const CElement *e) const
{
  if (e->HasMessage())
  {
    CDisplay ds(e->GetMessage().GetBufferPtr());
    ds.Print();
  }
}