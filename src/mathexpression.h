

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

#include "debug.h"
#include "parser.h"
#include "lcvector.h"
#include "evaluator.h"
#include "display.h"
#include "elementdatabase.h"

// #define RECURSIVE_ALGO

#if 1
#define MAX_STACK_SIZE UINT_MAX
typedef unsigned pos_t;
#else
#define MAX_STACK_SIZE USHRT_MAX
typedef unsigned short pos_t;
#endif

class CAlgebraRule;

typedef struct
{
  pos_t m_Pos;
  pos_t m_Size;
  pos_t m_RuleDstPos;
  pos_t m_PosArray[CElementDataBase::MAX_PAR];
#ifdef _DEBUG
  CAlgebraRule *m_Rule;
  unsigned m_RuleNb;
#endif
  const CMathExpression *m_RuleDstExp;
} context_t;

typedef CVector<context_t> CContextArray;

class CMathExpression
{

  friend class CElementDataBase;

protected:
  static CContextArray m_ContextStack;
  OP_CODE *m_StackArray;
  pos_t m_StackSize;
  pos_t m_AllocSize;
  CElementDataBase *m_ElementDB;

protected:
  void Set(pos_t pos, OP_CODE op) { m_StackArray[pos] = op; }
  OP_CODE Get(pos_t pos) const { return m_StackArray[pos]; }
  OP_CODE Pop(pos_t &pos) const
  {
    ASSERT(pos > 0 && pos <= m_StackSize);
    pos--;
    return Get(pos);
  }
  void SetSize(pos_t i)
  {
    m_StackSize = i;
    if (i > m_AllocSize)
      AllocSize(i);
  }
  void Push(OP_CODE op)
  {
    SetSize(m_StackSize + 1);
    Set(m_StackSize - 1, op);
  }

  void AllocSize(pos_t i);
  void Push(const CElement *e);
  void Push(const CMathExpression &equ);
  void PushEvalElement(CEvaluator &eval);
  void Append(const OP_CODE *array, pos_t size);
  void PushBranch(const CMathExpression &equ, pos_t &pos); // TODO: should return pos
  bool CompareBranchElement(pos_t pos1, pos_t pos2) const;
  bool CompareBranch(pos_t pos1, pos_t pos2) const;
  pos_t NextBranch(pos_t pos) const;
  int GetLevel(CParser &IC);
  bool GetLevel(CParser &IC, unsigned priority);
  bool ParseAtom(CParser &IC);
  bool ParseElement(CParser &IC);
  int ParseOperator(CParser &IC, unsigned &priority, bool var_found);
  void Replace(OP_CODE op1, OP_CODE op2, pos_t pos = 0);
  char TryMatchExp(const char *&sp);
  void Move(pos_t pos_dest, pos_t pos_source, pos_t size);
  pos_t Match(pos_t pos, const CMathExpression &equ, pos_t pos_array[CElementDataBase::MAX_PAR]) const;
  void Zero();
  void RemoveZero();
  bool ExecuteDirective();
  void DisplayRuleMessage(const CElement *e) const;
  void ReduceTree(const context_t &save_context);

#ifdef RECURSIVE_ALGO
  CAlgebraRule *RuleSearch(pos_t &pos, pos_t pos_array[CElementDataBase::MAX_EXP]);
  void ApplyRule(pos_t pos2, pos_t pos_array[CElementDataBase::MAX_EXP], const CMathExpression &rule_equ);
#else
  bool RuleSearch(const CElement *e, context_t &save_context);
#endif

  // static
  static CElement *RefToElement(OP_CODE op) { return CElementDataBase::RefToElement(op); }
  static unsigned ReservedParameterIndex(OP_CODE op)
  {
    ASSERT(op <= CElementDataBase::OP_PARF);
    return (unsigned)(op - CElementDataBase::OP_PAR0);
  }

  // Display funct
  virtual pos_t DisplayBranch(CDisplay &ds, pos_t pos, unsigned char precedence = 0) const;
  virtual pos_t DisplaySymbol(CDisplay &ds, pos_t pos, unsigned char precedence = 0) const;
  void DisplaySymbolString(const char *sp, pos_t pos_array[CElementDataBase::MAX_PAR], unsigned char precedence, CDisplay &ds) const;

public:
  void Initialize(CElementDataBase *db);

  void BinaryOperation(OP_CODE op, const CMathExpression &equ);
  void UnaryOperation(OP_CODE op);
  void VoidOperation(OP_CODE op);

  void Clear() { m_StackSize = 0; }
  void Init(OP_CODE op)
  {
    Clear();
    Push(op);
  }
  void Init(const CElement *e)
  {
    Clear();
    Push(e);
  }
  bool IsEmpty() const { return (m_StackSize == 0); }
  bool Compare(const CMathExpression &equ) const;
  void Copy(const CMathExpression &equ);
  void Display(CDisplay &ds, bool bAll = true) const;
  int Parse(CParser &IC);
  int Parse(const char *text)
  {
    CParser IC(text);
    return Parse(IC);
  }
  void OptimizeTree();
  bool Match(const CMathExpression &equ) const;
  void Evaluate() const;
  static unsigned ConvertToRule(CMathExpression &src, CMathExpression &dst);

  OP_CODE GetLastOperator() const { return IsEmpty() ? CElementDataBase::OP_ZERO : Get(m_StackSize - 1); }
  pos_t GetSize() const { return m_StackSize; }
  void Compile() { UnaryOperation(CElementDataBase::OP_EXEC); }
  CElementDataBase *GetElementDB() const { return m_ElementDB; }

  explicit CMathExpression(CElementDataBase *db = NULL);
  explicit CMathExpression(const CMathExpression &equ) : m_ElementDB(NULL)
  {
    Copy(equ);
  }
  virtual ~CMathExpression();
};

struct CSymbolSyntaxStruct
{
  char m_Syntax[9];
  unsigned char m_Precedence;
  CMathExpression m_Equation;
};
