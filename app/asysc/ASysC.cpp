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

#include <cstdio>
#include <cstring>
#include <iostream>

#include "Parser.h"
#include "Element.h"
#include "DisplayEx.h"
#include "MathExpressionEx.h"

extern const char help_txt[] asm("_binary_objs_asysc_Help_ztxt_start");

// #define _TEST
typedef enum
{
  LANG_PYTHON,
  LANG_C,
  LANG_UNKNOWN
} lang_t;

void Help();
int FileMode(CElementDataBase &db, const char *input_filename, const char *output_filename, lang_t language, CAnalysisMode analysis_type, const char *circuit_name);
int InteractiveMode(CElementDataBase &db);

int main(int argc, char *argv[])
{
  lang_t language = LANG_PYTHON;
  char *input_filename = NULL;
  const char *output_filename = "out";
  const char *circuit_name = "CIRCUIT";

  CAnalysisMode analysis_type = TRANS_ANALYSIS;

  while (true)
  {
    switch (::getopt(argc, argv, "i:o:t:l:"))
    {
    case 'i':
      input_filename = optarg;
      continue;
    case 'o':
      output_filename = optarg;
      continue;
    case 'l':
      if (!::strcmp(optarg, "python"))
        language = LANG_PYTHON;
      else if (!::strcmp(optarg, "c"))
        language = LANG_C;
      else
        language = LANG_UNKNOWN;
      continue;
    case 't':
      if (!::strcmp(optarg, "ac"))
        analysis_type = CAnalysisMode::AC_ANALYSIS;
      else if (!::strcmp(optarg, "trans"))
        analysis_type = CAnalysisMode::TRANS_ANALYSIS;
      continue;
    case 'c':
      circuit_name = optarg;
      continue;
    case '?':
      if (optopt == 'i' or optopt == 'o')
        ::fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        ::fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        ::fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      return 1;
    case -1:
      break;
    default:
      ::abort();
    }
    break;
  }

  CEvaluator eval;
  CElementDataBase db_root("Root", NULL, &eval, argv[0]);
  if (!db_root.Initialize())
    return -1;
  CElementDataBase db("User", &db_root);

#ifdef _TEST
  db.Test();
  db.Initialize();
#endif

  if (!db.IsOK())
    return 1;

  if (input_filename)
  {
    FileMode(db, input_filename, output_filename, language, analysis_type, circuit_name);
  }
  else
  {
    setlocale(LC_NUMERIC, "C");
    std::cout << "*************************************\n";
    std::cout << "*** LightCAS Console              ***\n";
    std::cout << "*** © Cyril Collineau, 2006-2024  ***\n";
    std::cout << "*************************************\n";
    std::cout << "Type \"help\" for help.\n";
    InteractiveMode(db);
  }

  return 0;
}

int FileMode(CElementDataBase &db, const char *input_filename, const char *output_filename, lang_t language, CAnalysisMode analysis_type, const char *circuit_name)
{
  bool ret = false;
  CMathExpressionEx equ(&db);
  if (db.LoadFromFile(CString(input_filename), equ))
  {
    CDisplay ds;
    if (ds.StoreToFile(output_filename))
    {
      if (language == LANG_PYTHON)
        ret = equ.ToPython(ds, analysis_type, circuit_name);
    }
  }
  return ret ? 0 : 1;
}

int InteractiveMode(CElementDataBase &db)
{
  CMathExpression equ(&db);
  CParser parser;
  parser.SetRootPath(db.GetRootPath());
  CDisplayEx ds((bool)ISATTY(1));
  const CElement *simplify = db.GetElement("SIMPLIFY");
  CElement *ans = db.GetElement("ans");
  OP_CODE simplify_op = simplify->ToRef();
  std::string expression_str;

  while (true)
  {
    std::cout << '>';
    std::getline(std::cin, expression_str);
    if (expression_str == "exit")
    {
      std::cout << "Exiting."
                << "\n";
      break;
    }
    else if (expression_str == "help")
    {
      Help();
    }
    else if (expression_str == "hex")
    {
      ds.m_DisplayBase = 16;
      std::cout << "Switching to hexadecimal mode."
                << "\n";
    }
    else if (expression_str == "bin")
    {
      ds.m_DisplayBase = 2;
      std::cout << "Switching to binary mode."
                << "\n";
    }
    else if (expression_str == "dec")
    {
      ds.m_DisplayBase = 10;
      std::cout << "Switching to decimal mode."
                << "\n";
    }
    else if (expression_str == "clr")
    {
      db.Clear();
      std::cout << "Clearing previous results."
                << "\n";
    }
    else
    {
      parser.SetPos(expression_str.c_str());
      if (!equ.Parse(parser))
      {
        std::cerr << "ERROR: " << parser.GetBuffer().GetBufferPtr() << " at line " << parser.GetLineNb() << "\n";
      }
      else
      {
        equ.UnaryOperation(simplify_op);
        ans->SetEquation(equ);
        ds.Clear();
        equ.Display(ds);
        std::cout << ds << "\n";
      }
    }
  }

  return 0;
}

void Help()
{
  std::cout << help_txt;
}