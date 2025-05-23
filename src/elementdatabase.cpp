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

#include <string.h>
#include <cctype>
#include "debug.h"
#include "element.h"
#include "elementdatabase.h"

CElementArray CElementDataBase::m_ElementRefArray;
CSymbolSyntaxArray CElementDataBase::m_SymbolSyntaxArray;
unsigned CElementDataBase::m_SecureLimit;

#ifdef EMBED_RULES
extern const char embedded_rules_txt[] asm("_binary_objs_Rules_concat_ztxt_start");
#endif

const SProperties CElementDataBase::m_FunctionProperties[] =
    {
        {"_add", 2, &CEvaluator::Add},
        {"_sub", 2, &CEvaluator::Sub},
        {"_or", 2, &CEvaluator::Or},
        {"_lor", 2, &CEvaluator::LOr},
        {"_xor", 2, &CEvaluator::Xor},
        {"_mul", 2, &CEvaluator::Mul},
        {"_div", 2, &CEvaluator::Div},
        {"_and", 2, &CEvaluator::And},
        {"_land", 2, &CEvaluator::LAnd},
        {"_eq", 2, &CEvaluator::Equal},
        {"_neq", 2, &CEvaluator::NotEqual},
        {"_lte", 2, &CEvaluator::LowerOrEqual},
        {"_shl", 2, &CEvaluator::ShiftLeft},
        {"_lt", 2, &CEvaluator::Lower},
        {"_gte", 2, &CEvaluator::GreaterOrEqual},
        {"_shr", 2, &CEvaluator::ShiftRight},
        {"_gt", 2, &CEvaluator::Greater},
        {"_min", 2, &CEvaluator::Min},
        {"_max", 2, &CEvaluator::Max},
        {"_mod", 2, &CEvaluator::Mod},
        {"_pow", 2, &CEvaluator::Pow},
        {"_par", 2, &CEvaluator::Par},
        {"_neg", 1, &CEvaluator::Neg},
        {"_inv", 1, &CEvaluator::Inv},
        {"_not", 1, &CEvaluator::Not},
        {"_id", 1, &CEvaluator::Id},
        {"_bool", 1, &CEvaluator::Bool},
        {"_lnot", 1, &CEvaluator::LNot},
        {"_sin", 1, &CEvaluator::Sin},
        {"_cos", 1, &CEvaluator::Cos},
        {"_tan", 1, &CEvaluator::Tan},
        {"_asin", 1, &CEvaluator::Asin},
        {"_acos", 1, &CEvaluator::Acos},
        {"_atan", 1, &CEvaluator::Atan},
        {"_sinh", 1, &CEvaluator::SinH},
        {"_cosh", 1, &CEvaluator::CosH},
        {"_tanh", 1, &CEvaluator::TanH},
        {"_asinh", 1, &CEvaluator::AsinH},
        {"_acosh", 1, &CEvaluator::AcosH},
        {"_atanh", 1, &CEvaluator::AtanH},
        {"_exp", 1, &CEvaluator::Exp},
        {"_log", 1, &CEvaluator::Ln},
        {"_log10", 1, &CEvaluator::Log},
        {"_sqrt", 1, &CEvaluator::Sqrt},
        {"_sqr", 1, &CEvaluator::Sqr},
        {"_fact", 1, &CEvaluator::Fact},
        {"_abs", 1, &CEvaluator::Abs},
        {"_floor", 1, &CEvaluator::Floor},
        {"_ceil", 1, &CEvaluator::Ceil},
        {"_rand", 0, &CEvaluator::Rand},
        {"_if2", 3, &CEvaluator::If},
        {"_if", 3, &CEvaluator::If},
        {"_elemid", 1, &CEvaluator::ElemId}};

const unsigned CElementDataBase::m_FunctionPropertiesSize = sizeof(CElementDataBase::m_FunctionProperties) / sizeof(SProperties);

CElementDataBase::CElementDataBase(const CString &name, CElementDataBase *parent, CEvaluator *eval, const CString &exe_path)
{
  m_Parent = parent;
  m_SearchStart = 0;
  m_Evaluator = eval;
  m_SecureLimit = 0;
  m_Error = false;
  m_Precedence = 0;

  if (parent)
  {
    m_RootPath = parent->m_RootPath;
    m_Name = parent->GetName() + '.' + name;
    m_Evaluator = m_Parent->GetEvaluator();
  }
  else
  {
    m_RootPath = CParser::GetPath(exe_path) + "rules/";
    m_Name = name;
  }
}

CElementDataBase::~CElementDataBase()
{
  Clear();
}

bool CElementDataBase::Initialize()
{

  Clear();
  if (m_Parent == NULL)
  {
    AddReservedElements();
    AddReservedFunctions();
    SetSecureLimit(GetSize());
    InitAlgebraRuleTable();
    CMathExpression src(this);
#ifdef EMBED_RULES
    CParser IC;
    IC.SetPos(embedded_rules_txt);
    AddAlgebraRuleTable(IC, src);
#else
    LoadFromFile(m_RootPath + CString("includes.rule"), src);
#endif
    // CleanTempElements();
  }
  return true;
}

bool CElementDataBase::LoadFromFile(const CString &file_name, CMathExpression &src)
{
  CParser IC;
  IC.SetRootPath(m_RootPath);
  if (IC.LoadFromFile(file_name))
  {
    src.Parse(IC);
    src.Compile();
    IC.CloseFile();
    return true;
  }
  CDisplay::Log(LOG_ERR, "rules description file not found.");
  return false;
}

void CElementDataBase::AddReservedElements()
{
  CMathExpression exp(this);
  static const char parameters[] = "_1 _2 _3 _4 _5 _6 _7 _8 _9 _10 _11 _12 _13 _14 _15 _16";
  static const char built_in[] = "_set(0 0) _get(0) _none _const(0) _elem(0) _funct0(0) _funct1(0 0) _funct2(0 0 0) NEG(0) _rank(0 0) _eval(0) _check(0 0) _print(0) _exec(0)";
  bool ret = exp.Parse(parameters);
  if (ret)
  {
    ASSERT(exp.GetSize() == 16);
    m_SearchStart = MAX_PAR;
    GetElement("0")->SetConst();
    exp.Parse(built_in);
    ASSERT(GetSize() == OP_END_RESERVED);
    ASSERT(OP_ELEM == OP_CONST + 1);
    ASSERT(OP_FUNCT0 == OP_ELEM + 1);
    ASSERT(OP_FUNCT1 == OP_FUNCT0 + 1);
    ASSERT(OP_FUNCT2 == OP_FUNCT1 + 1);
  }
  else
    ASSERT(false);
}

void CElementDataBase::AddReservedFunctions()
{
  AddEvalFunctionTable(m_FunctionProperties, m_FunctionPropertiesSize);
}

bool CElementDataBase::AssociateSymbol(CParser &IC)
{
  char c;
  unsigned i;
  CString s;
  CSymbolSyntaxStruct *sss;
  CMathExpression src_equ(this);

  sss = new CSymbolSyntaxStruct();
  sss->m_Precedence = (unsigned char)IC.GetInt();
  sss->m_Equation.Initialize(this);
  sss->m_Equation.ParseElement(IC);
  const char *sp = IC.GetSymbolBuffer().GetBufferPtr();
  i = 0;
  c = *sp++;
  while (c && !isspace(c) && (i < sizeof(sss->m_Syntax) - 1))
  {
    if (::isalpha(c))
    {
      s.Clear();
      s += tolower(c);
      const CElement *e = GetElement(s);
      ASSERT(e);
      if (e)
      {
        src_equ.Push(e);
      }
    }
    sss->m_Syntax[i++] = c;
    c = *sp++;
  }
  sss->m_Syntax[i] = '\0';
  CMathExpression::ConvertToRule(src_equ, sss->m_Equation);
  m_SymbolSyntaxArray.Append(sss);

#ifdef _DEBUG
  CDisplay ds;
  ds += "Adding symbol syntax [";
  ds += CString((int)(m_SymbolSyntaxArray.GetSize() - 1));
  ds += "] ";
  ds += sss->m_Syntax;
  ds += " => ";
  sss->m_Equation.Display(ds);
  ds.Log(LOG_DEBUG);
#endif
  return true;
}

void CElementDataBase::InitAlgebraRuleTable()
{
  CMathExpression src(this);
  src.Parse("_set( _exec( LIST( a b ) ) LIST( _exec( a ) _exec( b ) )  )");
  src.OptimizeTree();
  src.Parse("_set( _exec( _set( a b ) ) _set( a b ) )");
  src.OptimizeTree();
  src.Parse("_set( _exec( a )  a )");
  src.OptimizeTree();
}

void CElementDataBase::CleanTempElements()
{
  for (unsigned i = 0; i < GetSize(); i++)
  {
    CElement *e = GetAt(i);
    if ((e->ToRef() > CElementDataBase::GetSecureLimit()) && !e->IsFunct() && !e->IsConst())
    {
      delete e;
      RemoveAt(i);
      i--;
    }
  }
}

void CElementDataBase::AddEvalFunctionTable(const SProperties *property_table, unsigned size)
{
  for (unsigned i = 0; i < size; i++)
  {
    const SProperties &prop = property_table[i];
    AddEvalFunction(CString(prop.m_FunctionName), prop.m_ParameterNb, prop.m_FunctionCall);
  }
}

void CElementDataBase::AddEvalFunction(const CString &name, unsigned parameter_nb, CEvaluatorFunct funct)
{
  CElement *e = GetElement(name);
  e->SetOperandNb(parameter_nb);
  if (funct)
  {
    e->SetNumeric();
    GetEvaluator()->SetFunction(e->ToRef(), funct);
  }
}

void CElementDataBase::Clear()
{
  DeleteAll();
  if (m_Parent == NULL)
  {
    m_ElementRefArray.RemoveAll();
    m_SymbolSyntaxArray.DeleteAll();
  }
}

CElement *CElementDataBase::ParseElement(CParser &IC)
{
  CElement *e = NULL;

  if (IC.IsWord())
  {
    e = GetElement(IC.GetWord());
  }
  return e;
}

CElement *CElementDataBase::GetElement()
{
  unsigned n;
  CElement *e;
  CString s;

  n = m_ElementRefArray.GetSize();
  s = "_";
  s += CString((int)n);
  e = CreateElement(s, GetSize());
  e->SetAux();
  return e;
}

CElement *CElementDataBase::GetElement(const CString &string)
{
  unsigned pos;
  CElement *e;

  e = SearchElement(string, pos);
  if (e == NULL)
  {
    e = CreateElement(string, pos);
  }
  return e;
}

CElement *CElementDataBase::CreateElement(const CString &string, unsigned pos)
{
  CElement *e;

  e = new CElement(string);
  Register(e, pos);
  m_Evaluator->DeclareElement(e->ToRef());
  return e;
}

CElement *CElementDataBase::SearchElement(const CString &string, unsigned &pos) const
{
  if (m_Parent)
  {
    CElement *e = m_Parent->SearchElement(string, pos);
    if (e)
    {
      return e;
    }
  }

  unsigned start = m_SearchStart;
  unsigned stop = GetSize();
  while (start < stop)
  {
    unsigned mid = (start + stop) / 2;
    CElement *e = GetAt(mid);
    ASSERT(e);
    int compare = string.Compare(e->GetName());
    if (compare == 0)
    {
      return e;
    }
    if (compare < 0)
    {
      stop = mid;
    }
    else
    {
      start = mid + 1;
    }
  }
  pos = stop;
  return NULL;
}

unsigned CElementDataBase::Register(CElement *e, unsigned index)
{
  unsigned n;

  InsertAt(index, e);
  n = m_ElementRefArray.GetSize();
  e->SetRef(n);
  e->SetGlobal(m_Parent == NULL);
  m_ElementRefArray.Append(e);

#ifdef _DEBUG
  CDisplay ds;
  ds += "Register #";
  ds += CString(e->ToRef());
  ds += " : ";
  ds += m_Name;
  ds += '/';
  e->Display(ds);
  ds.Log(LOG_DEBUG);
#endif

  return n;
}
