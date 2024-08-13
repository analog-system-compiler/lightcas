/*
 * Copyright (C) 2006-2024 The LightCAS project                        
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
#include <climits>
#include "Debug.h"
#include "LCVector.h"
#include "MathExpression.h"
#include "Element.h"
#include "Function.h"

CMathExpression::CMathExpression(CElementDataBase *db)
{
  m_StackArray = NULL;
  m_ElementDB = db;
  m_AllocSize = 0;
  m_StackSize = 0;
}

CMathExpression::~CMathExpression()
{
  if (m_StackArray)
  {
    ::free(m_StackArray);
  }
}

void CMathExpression::Initialize(CElementDataBase *db)
{
  Clear();
  m_ElementDB = db;
}

void CMathExpression::Push(const CElement *e)
{
  Push(e->ToRef());
}

void CMathExpression::Push(const CMathExpression &equ)
{
  if (equ.IsEmpty())
  {
    Push(CElementDataBase::OP_ZERO);
  }
  else
  {
    Append(equ.m_StackArray, equ.m_StackSize);
  }
}

void CMathExpression::Append(const OP_CODE *src, pos_t size)
{
  if (size)
  {
    pos_t old_size = m_StackSize;
    SetSize(m_StackSize + size);
    ::memcpy(&m_StackArray[old_size], src, size * sizeof(OP_CODE));
  }
}

void CMathExpression::Move(pos_t pos_dest, pos_t pos_source, pos_t size)
{
  if (size)
  {
    SetSize(pos_dest + size);
    ::memmove(&m_StackArray[pos_dest], &m_StackArray[pos_source], size * sizeof(OP_CODE));
  }
}

void CMathExpression::AllocSize(pos_t i)
{
  m_AllocSize = i * 2;
  m_StackArray = (OP_CODE *)::realloc(m_StackArray, m_AllocSize * sizeof(OP_CODE));
  ASSERT(m_StackArray);
}

bool CMathExpression::Compare(const CMathExpression &equ) const
{
  return (equ.m_StackSize == m_StackSize) && !::memcmp(equ.m_StackArray, m_StackArray, m_StackSize * sizeof(OP_CODE));
}

void CMathExpression::Copy(const CMathExpression &equ)
{
  if (m_ElementDB == NULL)
  {
    m_ElementDB = equ.m_ElementDB;
  }

  Clear();

  if (!equ.IsEmpty())
  {
    Push(equ); // Push adds zero if empty
  }
}

void CMathExpression::PushBranch(const CMathExpression &equ, pos_t &pos)
{
  pos_t pos1 = pos;
  ASSERT(pos);
  pos = equ.NextBranch(pos);
  Append(&equ.m_StackArray[pos], pos1 - pos);
}

pos_t CMathExpression::NextBranch(pos_t pos) const
{
  unsigned i = 1;

  ASSERT(pos);
  do // while( pos && i )
  {
    OP_CODE op = Pop(pos);
    const CElement * e = RefToElement(op);
    ASSERT(e);
    i += e->GetOperandNb();
    i--;
  } while (pos && i);

  return pos;
}

void CMathExpression::BinaryOperation(OP_CODE op, const CMathExpression &equ)
{
  ASSERT(RefToElement(op)->IsBinary());
  Zero();
  Push(equ);
  Push(op);
  OptimizeTree();
  RemoveZero();
}

void CMathExpression::UnaryOperation(OP_CODE op)
{
  ASSERT(RefToElement(op)->IsUnary());
  Zero();
  Push(op);
  OptimizeTree();
  RemoveZero();
}

void CMathExpression::VoidOperation(OP_CODE op)
{
  Push(op);
  RemoveZero();
}

void CMathExpression::Zero()
{
  if (IsEmpty())
  {
    Push(CElementDataBase::OP_ZERO);
  }
}

void CMathExpression::RemoveZero()
{
  if ((m_StackSize == 1) && (Get(0) == CElementDataBase::OP_ZERO))
  {
    Clear();
  }
}

void CMathExpression::Evaluate() const
{
  m_ElementDB->GetEvaluator()->Evaluate(m_StackSize, m_StackArray);
}

unsigned CMathExpression::ConvertToRule(CMathExpression &src, CMathExpression &dst)
{
  unsigned i = 0;
  pos_t pos;

  for (pos = 0; pos < src.GetSize(); pos++)
  {
    OP_CODE op = src.Get(pos);
    if (op >= CElementDataBase::GetSecureLimit())
    {
      const CElement * e = RefToElement(op);
      if (e->IsVar())
      {
        src.Replace(op, (OP_CODE)(CElementDataBase::OP_PAR0 + i));
        dst.Replace(op, (OP_CODE)(CElementDataBase::OP_PAR0 + i));
        i++;
      }
    }
  }
  return i;
}

void CMathExpression::Replace(OP_CODE op1, OP_CODE op2, pos_t pos)
{
  for (; pos < m_StackSize; pos++)
  {
    if (Get(pos) == op1)
    {
      Set(pos, op2);
    }
  }
}

void CMathExpression::Compile()
{
  Push(CElementDataBase::OP_EXEC);
  OptimizeTree();
}
