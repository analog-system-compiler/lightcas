/*******************************************************************************/
/*  Copyright (C) 2014 The CASLight project                                     */
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

#include <cstring>
#include "Debug.h"
#include "LCVector.h"
#include "MathExpression.h"
#include "Element.h"
#include "Function.h"

#ifdef RECURSIVE_ALGO

void CMathExpression::OptimizeTree()
{
  pos_t pos;
  pos_t pos_array[CElementDataBase::MAX_PAR + 1];
  CAlgebraRule *rule;

  if (ExecuteCommand())
  {
    rule = RuleSearch(pos, pos_array);
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
  unsigned i;
  unsigned n;

  OP_CODE op = GetLastOperator();
  const CElement *e = RefToElement(op);
  ASSERT(e);
  const CFunction *funct = e->GetFunction();

  if (funct)
  {
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
  OP_CODE op3;
  context_t save_context;
  unsigned j;

  if (!IsEmpty() && ExecuteCommand())
  {
    RuleSearch();
    while (m_ContextStack.GetSize())
    {
      save_context = m_ContextStack.Pop();
      pos_t n = save_context.m_RuleDstExp->GetSize();
      while (save_context.m_RuleDstPos < n)
      {
        op3 = save_context.m_RuleDstExp->Get(save_context.m_RuleDstPos++);

        if (CElementDataBase::IsReservedOP(op3))
        {
          j = ReservedParameterIndex(op3);
          ASSERT(j < CElementDataBase::MAX_PAR);
          unsigned pos2 = save_context.m_PosArray[j];
          ASSERT(pos2 <= GetSize());
          pos_t pos5 = save_context.m_RuleDstPos; // already incremented
          if (pos5 < n)                           // if followed by FUNCT1 or FUNCT2
          {
            op3 = save_context.m_RuleDstExp->Get(pos5);
            if (CElementDataBase::IsFunctionOP(op3))
            {
              save_context.m_RuleDstPos++;
              op3 = Get(save_context.m_PosArray[j] - 1);
              goto L1;
            }
          }

          pos_t pos1 = pos2;
          pos1 = NextBranch(pos1);
          Move(GetSize(), pos1, pos2 - pos1); // Append to end of equation
        }
        else
        {
        L1:
          ASSERT(RefToElement(op3) != NULL);
          Push(op3);
          bool need_post_optim = ExecuteCommand();
          if (need_post_optim)
          {
            m_ContextStack.Push(save_context);
            RuleSearch();
            save_context = m_ContextStack.Pop();
            n = save_context.m_RuleDstExp->GetSize();
          }
        } // if ( CElementDataBase::IsReservedOP( op3 ) )
      }   // while ( save_context.m_RuleDstPos < n )

      pos_t pos3 = save_context.m_Size;
      pos_t pos2 = GetSize();
#if (DEBUG_LEVEL >= 1)
      CDisplay ds;
      ds += "OptimizeTree rule";
      //        ds += CString( rule->m_LineNo );
      save_context.m_Rule->Display(0, ds);
      ds += " : ";
      SetSize(pos3);
      Display(ds, false);
      ds += " => ";
#endif
      Move(save_context.m_Pos, pos3, pos2 - pos3);
#if (DEBUG_LEVEL >= 1)
      Display(ds, false);
      TRACE(ds.GetBufferPtr());
#endif
    } // while ( m_ContextStack.GetSize() )
  }
  if (NextBranch(m_StackSize) != 0)
  {
    CDisplay ds;
    ds.Print("Simplify error:");
    Display(ds, false);
    TRACE(ds.GetBufferPtr());
  }
}

bool CMathExpression::RuleSearch()
{
  OP_CODE op = GetLastOperator();
  const CElement *e;

  e = RefToElement(op);
  return RuleSearch(e);
}

bool CMathExpression::RuleSearch(const CElement *e)
{
  unsigned i;
  unsigned n;
  context_t save_context;

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
        if (rule->m_bHasRule)
        {
          save_context.m_Pos = pos;        // low stack limit
          save_context.m_Size = GetSize(); // high stack limit, start of subsquent expressions
          save_context.m_RuleDstPos = 0;   // index in the rule. From 0 to m_DstEquation.GetSize()
          save_context.m_RuleDstExp = &(rule->m_DstEquation);
#ifdef _DEBUG
          save_context.m_Rule = rule;
#endif
          m_ContextStack.Push(save_context);
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
  OP_CODE op1, op2, op3;
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
    op1 = equ.Pop(pos1);

    if ((op1 >= CElementDataBase::OP_CONST) && (op1 <= CElementDataBase::OP_FUNCT2)) // IsFunctionOP( OP_CODE op )
    {
      op3 = equ.Pop(pos1);
      ASSERT(CElementDataBase::IsReservedOP(op3));
      if (CElementDataBase::IsReservedOP(op3))
      {
        match = RegisterBranch(pos_array, op3, pos2);
        if (match)
        {
          op2 = Pop(pos2);
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
              match = (op1 == ( CElementDataBase::OP_FUNCT0 + e->GetFunction()->GetParameterNb() ) );
            }
          }
        }
      }
    }
    else if (CElementDataBase::IsReservedOP(op1)) // a,b or c
    {
      match = RegisterBranch(pos_array, op1, pos2);
      if (match)
      {
        pos2 = NextBranch(pos2);
      }
    }
    else
    {
      op2 = Pop(pos2);
      match = (op2 == op1);
    }
  }

  return match ? pos2 : pos3;
}

bool CMathExpression::ExecuteCommand()
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
    pos_t pos = m_StackSize;
    equ_src.PushBranch(*this, pos);
    OP_CODE op1 = equ_src.GetLastOperator();
    CElement *e = RefToElement(op1);
    e->AddFunction(equ_src, equ_dst);
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
      pos_t pos = m_StackSize;
      pos = NextBranch(pos);
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
    CDisplay ds;
    ds.SetDebug();
    if (!CompareBranch(pos1, pos2))
    {
      ds = "Assertion error : ";
      DisplayBranch(ds, pos2);
      ds += " != ";
      DisplayBranch(ds, pos1);
    }
    else
    {
      ds = "Assertion OK : ";
      DisplayBranch(ds, pos2);
      ds += " == ";
      DisplayBranch(ds, pos1);
    }
    PUTS(ds.GetBufferPtr());
    m_StackSize++;
    return false;
  }

  if (op3 == CElementDataBase::OP_PRINT)
  {
    CDisplay ds;
    CMathExpression equ(m_ElementDB);
    pos_t pos= m_StackSize;
    equ.PushBranch(*this, pos);
    equ.Display(ds);
    ds.Print();
    return false;
  }

  m_StackSize++;

#if (DEBUG_LEVEL >= 2)
  if ((op3 == CElementDataBase::OP_SET) || (op3 == CElementDataBase::OP_GET) || (op3 == CElementDataBase::OP_RANK) || (op3 == CElementDataBase::OP_EVAL))
  {
    Display(ds);
    TRACE(ds.GetBufferPtr());
  }
#endif
  return true;
}

bool CMathExpression::CompareBranchElement(pos_t pos1, pos_t pos2) const
{
  OP_CODE op3, op4;
  const CElement *e;

  op4 = Pop(pos2);
  e = RefToElement(op4);

  if (e && e->IsVoid())
  {
    op3 = Pop(pos1);
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

  pos11 = pos1;
  pos22 = pos2;
  pos11 = NextBranch(pos11);
  pos22 = NextBranch(pos22);

  return ((pos1 - pos11) == (pos2 - pos22)) && !memcmp(&m_StackArray[pos11], &m_StackArray[pos22], (pos2 - pos22) * sizeof(OP_CODE));
}

bool CMathExpression::RegisterBranch(pos_t pos_array[CElementDataBase::MAX_PAR], OP_CODE op1, pos_t pos2) const
{
  unsigned i;
  bool match;

  i = ReservedParameterIndex(op1);
  ASSERT(i < CElementDataBase::MAX_PAR);

  if (pos_array[i] == 0)
  {
    pos_array[i] = pos2;
    ASSERT(pos2 != 0);
    match = true;
  }
  else
  {
    match = CompareBranch(pos2, pos_array[i]);
  }

  return match;
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
    CDisplay ds;
    ds.Print(e->GetMessage().GetBufferPtr());
  }
}