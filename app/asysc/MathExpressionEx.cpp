/*******************************************************************************/
/*  Copyright (C) 2006 The SIMCAS project                                      */
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

#include <ctype.h>
#include "Element.h"
#include "MathExpressionEx.h"

bool CMathExpressionEx::ToPython(CDisplay &ds, CAnalysisMode mode, const char *circuit_name)
{
  CElement *eth, *es, *ec, *et, *ee, *ess;
  CDisplay ds1, ds2, ds3, ds4;
  CElementArray element_array;

  m_ElementDB->GetElement("RESOLVE_TRANS")->SetMessage("Solving transient system...");
  m_ElementDB->GetElement("RESOLVE_AC")->SetMessage("Solving AC system...");
  m_ElementDB->GetElement("SIMPLIFY")->SetMessage("Simplifying system...");
  eth = m_ElementDB->GetElement("TANH");
  es = m_ElementDB->GetElement("SIN");
  ec = m_ElementDB->GetElement("COS");
  et = m_ElementDB->GetElement("TIME");
  ee = m_ElementDB->GetElement("EXP");
  ess = m_ElementDB->GetElement("s");
  m_op_hier = m_ElementDB->GetElement("HIER")->ToRef();
  m_op_getv = m_ElementDB->GetElement("_getv")->ToRef();
  OP_CODE op_concat = m_ElementDB->GetElement("CONCAT")->ToRef();
  OP_CODE op_vect = m_ElementDB->GetElement("VECT")->ToRef();
  OP_CODE op_eqv = m_ElementDB->GetElement("EQV")->ToRef();

  eth->SetName("tanh");
  eth->SetNumeric();
  es->SetName("sin");
  es->SetNumeric();
  ec->SetName("cos");
  ec->SetNumeric();
  ee->SetName("exp");
  ee->SetNumeric();
  et->SetName("time");

  CElement *e = m_ElementDB->GetElement(circuit_name);
  if (e->IsFunct())
  {
    const CAlgebraRuleArray &rule_array = e->GetFunction()->GetAlgebraRulesArray();
    Copy(rule_array.GetAt(0)->m_DstEquation);
    // Push(e)

    // ds.Append("\'\'\'\n");
    // Display(ds);
    // ds.Append("\n\'\'\'\n");
    // ds.Print();

    OptimizeTree();
    ess->SetVar();
    // ds.Append("\'\'\'\n");
    // Display(ds);
    // ds.Append("\n\'\'\'\n");
    // ds.Print();

    // Search for _getv and initialize components
    pos_t pos = GetSize();
    while (pos)
    {
      OP_CODE op = Pop(pos);
      if (op == m_op_getv)
      {
        op = Pop(pos);
        ASSERT(pos);
        e = RefToElement(op);
        if (!element_array.Find(e))
        {
          element_array.Append(e);
          ds4.Clear();
          ds4.Append("self.");
          e->Display(ds4);
          ds1.Append("\t\t");
          ds1.Append(ds4);
          ds1.Append(" = element('");
          ds1.Append(ds4);
          ds1.Append("')\n");
        }
      }
    }

    // Search for results
    ds3.SetDebug();
    pos = GetSize();
    OP_CODE op = Pop(pos);
    if (op == op_vect)
    {
      while (pos)
      {
        op = Pop(pos);
        if (op != op_concat)
          pos++;

        op = Pop(pos);
        if (op == op_eqv)
        {

          ds3.Clear();
          ds4.Clear();
          pos = CMathExpression::DisplayBranch(ds3, pos);
          pos = CMathExpression::DisplayBranch(ds4, pos);

          ds1.Append("\t\t");
          ds1.Append(ds4);
          ds1.Append(" = element('");
          ds1.Append(ds4);
          ds1.Append("')\n");

          ds2.Append("\t\t");
          if (mode == CAnalysisMode::AC_ANALYSIS)
          {
            ds2.Append(ds4);
            ds2.Append(".set_f(");
            ds2.Append(ds3);
            ds2.Append(", self.freq)\n");
          }
          else
          {
            ds2.Append("self._setc(");
            ds2.Append(ds4);
            ds2.Append(",");
            ds2.Append(ds3);
            ds2.Append(")\n");
          }
        }
        else
        {
          ds.Print("Resolution error 1");
          ds.Clear();
          Display(ds);
          ds.Print();
          return false;
        }
      }
    }
    else
    {
      ds.Print("Resolution error 2");
      ds.Clear();
      Display(ds);
      ds.Print();
      return false;
    }

    ds.Append("import math\nimport circuit_base\nfrom circuit_base import circuit_base, element\n");
    ds.Append("\nclass circuit( circuit_base ):\n");
    ds.Append("\n\tdef __init__(self):\n\t\tsuper().__init__()\n");
    ds.Append(ds1);
    ds.Append("\n\tdef step(self):\n");
    ds.Append(ds2);
    ds.Print();
  }

  return true;
}

pos_t CMathExpressionEx::DisplaySymbol(CDisplay &ds, pos_t pos, unsigned precedence) const
{

  OP_CODE op = Pop(pos);
  if (op == m_op_hier)
  {
    pos++;
    pos_t pos2 = pos;
    pos = NextBranch(pos);
    pos_t pos3 = pos;
    ds += "self.";
    while (pos3 != pos2)
    {
      op = Get(pos3);
      if (op != m_op_getv)
      {
        RefToElement(op)->Display(ds);
        if (pos3 != pos2 - 2)
          ds += '_';
      }
      pos3++;
      op = Get(pos3);
      if (op == m_op_hier)
        pos3++;
    }
  }
  else
  {
    pos++;
    pos_t pos_array[CElementDataBase::MAX_PAR];
    const CSymbolSyntaxArray &st = m_ElementDB->GetSymbolTable();
    for (unsigned i = 0; i < st.GetSize(); i++)
    {
      const CSymbolSyntaxStruct *ss = st[i];
      pos_t pos1 = Match(pos, ss->m_Equation, pos_array);
      if (pos1 != pos)
      {
        const char *sp = ss->m_Syntax;
        if ((i < precedence) || (ds.IsDebug() && i <= precedence))
        {
          ds += '(';
          DisplaySymbolString(sp, pos_array, i, ds);
          ds += ')';
        }
        else
        {
          DisplaySymbolString(sp, pos_array, i, ds);
        }
        pos = pos1;
        break;
      }
    }
  }
  return pos;
}

pos_t CMathExpressionEx::DisplayBranch(CDisplay &ds, pos_t pos, unsigned priority) const
{
  const CElement *e;
  unsigned pos2;

  ASSERT(pos);
  pos2 = DisplaySymbol(ds, pos, priority);

  if (pos == pos2) // no symbol displayed
  {
    OP_CODE op = Pop(pos);
    e = RefToElement(op);
    if (e->IsFunct() || e->IsVar())
      ds += "self.";
    else if (e->IsNumeric())
      ds += "math.";
    e->Display(ds);

    if (e->IsNumeric() || e->IsFunct()) // For Rand(), n=0
    {
      unsigned n = e->GetFunction()->GetParameterNb();
      ASSERT(n <= CElementDataBase::MAX_PAR);
      ds += '(';
      if (n)
      {
        pos_t pos_array[CElementDataBase::MAX_PAR];
        for (unsigned i = 0; i < n; i++)
        {
          pos_array[i] = pos;
          pos = NextBranch(pos);
        }
        for (unsigned i = 1; i < n; i++)
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
