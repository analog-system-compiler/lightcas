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

unsigned CMathExpression::Parse( CParser& IC )
{
  Clear();	  
  return GetLevel(IC);
}

/*unsigned CMathExpression::ParseParenthesis( CParser& IC )
{
  unsigned i;
  if( IC.TryFind( '(' ) )
  {
    i = GetLevel(IC);
    IC.Find( ')' );
  }
  else 
	  i = GetLevel(IC);
  return i;
}*/

void CMathExpression::Display( CDisplay& ds ) const
{
  unsigned pos = m_StackSize;

  if( pos == 0 )
  {
    ds += '0';
  }
  else
  {
    while( pos )
    {
      pos = DisplayBranch( pos, 0, ds );
      if( pos )
      {
        ds += ' ';
      }
    }
  }
}

unsigned CMathExpression::DisplayBranch( unsigned pos , unsigned priority, CDisplay& ds ) const
{
  unsigned i, n;
  CElement* e;
  unsigned pos2;
  CDisplay ds2, ds3;

  ASSERT( pos );
  pos2 = DisplaySymbol( pos, priority, ds );

  if( pos == pos2 ) //no symbol displayed
  {
    OP_CODE op = Pop( pos );
    e = RefToElement( op );
    e->Display( ds );
    n = e->GetFunction()->GetParameterNb();
    if( n || e->IsNumeric() ) // For Rand(), n=0
    {
      for( i = 0; i < n; i++ )
      {
        ds2.Clear();
        pos = DisplayBranch( pos, 0, ds2 );
        if( i != 0 )
        {
          ds2 += ' ' ;
        }
        ds3.Prepend( ds2 );
      }
	  ds += '(';
      ds += ds3 ;
      ds += ')' ;
    }
  }
  else
  {
    pos = pos2;
  }

  return pos;
}

unsigned  CMathExpression::DisplaySymbol(  unsigned pos, unsigned precedence, CDisplay& ds ) const
{
  unsigned pos_array[ CElementDataBase::MAX_EXP ];

  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();
  for( unsigned i = 0; i < st.GetSize(); i++ )
  {
    const CSymbolSyntaxStruct* ss = st[i];
    const CMathExpression* equ =  &ss->m_Equation;    
    unsigned pos1 = Match( pos, *equ, pos_array );
    if( pos1 != pos )
    {
      const char* sp = ss->m_Syntax;
      if( i < precedence )
      {
        ds += '(' ;
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

  return pos;
}

void  CMathExpression::DisplaySymbolString(  const char* sp, unsigned pos_array[], unsigned precedence, CDisplay& ds ) const
{
  char c;
  unsigned i;
  unsigned precedence2;

  while ( ( c = *sp++ ) )
  {
	if (c == '(') {
		precedence++;
	}
	else if (c == ')') {
		if (precedence) precedence--;		
	}	 
    else if( c == CParser::m_OperatorExclude )
    {
	  sp++; //Exclude operator
    }
    else if( CParser::IsWord( c ) )
    {
		precedence2 = precedence;
      if( c < 'a' )
      {
        precedence2 = 0;
      }
      i = tolower( c ) - 'a';
      DisplayBranch( pos_array[i], precedence2, ds );
    }
    else
    {
      ds += c;
    }
  }
}

unsigned CMathExpression::GetLevel(CParser& IC)
{
	unsigned i = 0;
	while (!IC.IsStopChar())
	{
		GetLevel(IC, 0);
		i++;
	}
	return i;
}

void CMathExpression::GetLevel( CParser& IC, unsigned priority )
{
  bool symbol_first = true;

  while( !IC.IsStopChar() )
  {
	  //if (IC.GetChar() == '-') ASSERT(false);
	  if (!SearchOperator(IC, symbol_first?0:priority, symbol_first))
	  {	    
		  if (symbol_first) {
			  if (IC.TryFind('('))
			  {
				  GetLevel(IC);
				  IC.Find(')');
			  }
			  else if (IC.IsWord())
			  {
				  ParseElement(IC);
			  }
			  else
				  IC.Error(CParserException::ID_ERROR_OPERATOR_EXPECTED);
		  }
	      else break; //exit level
	  }
	  symbol_first = false;
  }
}
#if 0
unsigned CMathExpression::GetLevelBis(CParser& IC)
{
	CString stack;
	CMathExpression equ(m_ElementDB);

	//Read the whole string
	while (IC.GetChar())
	{
		if (IC.TryFind(CParser::m_SymbolDelimiter))
		{			
			CParser IC2(IC.GetPos());
			while (!IC.TryFind(CParser::m_SymbolDelimiter)) IC.Next();
			CMathExpression equ2(m_ElementDB);			
			equ2.GetLevelBis(IC);			
			m_ElementDB->AssociateSymbol(IC2, equ2);
		}
		else if (IC.IsWord()) {
			stack.Append('a');
			equ.Push(m_ElementDB->ParseElement(IC));			
		}
		else {
			stack.Append(IC.GetChar());
			IC.Next();
		}
	}
	const char *sp = stack.GetBufferPtr();
	unsigned elem_index = 0;
	unsigned par_nb = 0;
	//s = m_StackSize;
	const char *sp = IC.GetPos();
	while (!CParser::IsStopChar(*sp)) {
		if (!Decode(equ, sp, elem_index, 0))
			IC.Error(CParserException::ID_ERROR_OPERATOR_EXPECTED);		
		par_nb++;
	}
	ASSERT(elem_index == equ.GetSize());
	ASSERT(*sp=='\0');	
	return par_nb;
}

	//Decode 
bool CMathExpression::Decode( const CMathExpression& equ, const char * &s, unsigned& elem_index, unsigned priority)
{
	CElement * e;
	char c;
	unsigned i;
	const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();
	unsigned pos_array[CElementDataBase::MAX_EXP];
	InitPositionTable(pos_array);
	
	for (i = priority; i < st.GetSize(); i++)
	{
		const char* sp = st[i]->m_Syntax;
			
		while (*sp)
		{
			if (CParser::IsWord(*sp) )
			{
				c=*sp++;
				if (c > 'A')
					priority = 0;
				
				if (!Decode(equ,s, elem_index, priority)) return false;

				StoreStackPointer(c, pos_array);
			}
			else if (*sp == *s)
			{
				sp++;
				s++;
			}
			else
				break;
		}

		if (*sp == '\0')
		{
			const CMathExpression& rule_equ = st[i]->m_Equation;
			ApplyRule(equ, pos_array, rule_equ, false);
			return true;
		}
	}

	if (*s == '(') {
		s++;
		if (!Decode(equ, s, elem_index, 0)) return false;
		if (*s == ')')
			s++;
		else
			return false;
	}
	else if (CParser::IsWord(*s))
	{
		OP_CODE op = equ.m_StackArray[elem_index++];
		if (*s == '(') // if fonction
		{
			s++;
			i = 0;
			while (*s && *s != ')') {
				i++;
				if (!Decode(equ, s, elem_index, 0)) return false;
			}
			s++; //')'
			e = RefToElement(op);
			e->SetFunct();
			CFunction *f = e->GetFunction();
			f->SetParameterNb(i);
			if ((f->GetParameterNb() == 2) && (GetLastOperator() == CElementDataBase::OP_CONCAT))
			{
				m_StackSize--;
				ASSERT(e->IsNumeric()); // for numeric function, it is allowed to use comma without concatenation op.
				ASSERT(f->GetParameterNb() == i + 1);
			}
		}		
		Push(op);
		return true;
	}
	else if (*s != '\0')
		return false;

	return false;	
}

unsigned CMathExpression::SymbolMatch(CParser& IC, unsigned par_nb, const char *& pos, unsigned& priority )
{
	CElement * e;
	char c;
	unsigned i;
	const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();

	priority = i;
	for (i = 0; i < st.GetSize(); i++)
	{
		const char* sp = st[i]->m_Syntax;

		while (*sp)
		{
			if (CParser::IsWord(*sp))
			{
				c = *sp++;
				if (c > 'A')
					priority = 0;
			}
			else if (IC.TryFind( *sp ) )
			{
				sp++;
			}
			else
				break;
		}

		if (*sp == '\0')
		{
			return i;
		}
	}
}

void CMathExpression::Build(CMathExpression& equ, unsigned i, unsigned pos )
{
	unsigned pos_array[CElementDataBase::MAX_EXP];
	const CMathExpression& rule_equ = st[i]->m_Equation;
	//InitPositionTable(pos_array);
	ASSERT(pos <= 2);
	if(pos==2)
	{
		equ.PushBranch(m_StackSize);
		pos_array[1] = equ.m_StackSize;
	}
	equ.PushBranch(m_StackSize);
	pos_array[0] = equ.m_StackSize;
	ApplyRule(equ, pos_array, rule_equ, false);
}

bool CMathExpression::Decode(CMathExpression& equ, CParser& IC, unsigned priority)
{
	char c;
	unsigned i;
	unsigned n = m_ElementDB->GetSymbolTable()->GetSize();

	i = SymbolMatch( IC, 1, z); //pre
	if (i != n)
	{
		Decode(equ,IC,0);
		Build(i, 1);
		//SymbolEOS(i, IC, z);
	}
	else {

		if (IC.TryFind('('))
		{
			Decode(equ,IC, 0);
			IC.Find(')');
		}
		else
			ParseElement(IC);

		i = SymbolMatch(IC, 1, z); //post
		if (i != n)
		{
			Build(i, 1);
			//SymbolEOS(i, IC, z);
		}
	}

	i = SymbolMatch(IC, 2, z); //pre
	if (i != n) {

		if (i < priority)
		{
			Decode(equ,IC, i);
			//Build(i, 2);
		}
		else
		{
			Build(equ,priority, 2);
			Decode(IC, 0);
			Build(i, 2);
		}
		//SymbolEOS(i, IC, z);
	}
	//a*b+c a b mul c add
	//a+b*c a b c mul add
	return true;
}
#endif

bool CMathExpression::SearchOperator( CParser& IC, unsigned priority, bool symbol_first )
{
  unsigned i,k,n;
  unsigned pos_array[CElementDataBase::MAX_EXP];
  const CSymbolSyntaxArray& st = m_ElementDB->GetSymbolTable();
  const char * init_pos = IC.GetPos();
  InitPositionTable(pos_array);
  //a*b+c a b c add mul -> a b mul c add
  //a+b*c a b c mul add -> a b c mul add
  //first get start of first parameter
  n = m_StackSize;

  for(i = priority; i < st.GetSize(); i++ )
  {
    const char* sp = st[i]->m_Syntax;    
	k = MatchOperator(IC, sp, pos_array, i + 1, symbol_first);
	if (k!=0)
	{
		const CMathExpression& rule_equ = st[i]->m_Equation;
		ASSERT(rule_equ.GetSize());

		CMathExpression equ(m_ElementDB); //save current stack
		equ.Copy(*this);
		m_StackSize = n;
		if (!symbol_first) //FIXME
			NextBranch(m_StackSize); //remove first parameter
		ApplyRule(equ, pos_array, rule_equ, false);

#if 0 //def _DEBUG
		CDisplay ds;
		Display(ds);
		TRACE(ds.GetBufferPtr());
#endif
		return true;
	}
	IC.SetPos(init_pos);
	m_StackSize = n;
  }
  
  return false;
}

unsigned CMathExpression::MatchOperator( CParser& IC, const char* sp, unsigned pos_array[], unsigned precedence, bool symbol_first )
{
	unsigned k = 0;
	char c;

  c = *sp;
  if (c == '(' || c == ')') //Parenthesis not used in parsing stage
  {
	  sp++;
	  c = *sp;
  }
  if (CParser::IsWord(c) == symbol_first)
	  return 0;

  if (CParser::IsWord(c))
  {
	  StoreStackPointer(c, pos_array);
	  sp++;
	  k++;
  }

  // try to match prefix operator
  c = *sp;
  while( c )
  {
    if ( c == '(' || c == ')' ) //Parenthesis not used in parsing stage
    {
		sp++;
      c=*sp;
	}
    if( CParser::IsWord( c )  )
    {
      if( c < 'a' )
      {
        precedence = 0;
      }
      GetLevel( IC, precedence ); //Push parameters
      StoreStackPointer( c, pos_array );
      sp++;
	  k++;
    }
    else if( !IC.TryMatchSymbol( sp ) )
    {
      return 0;
    }
	c = *sp;
  }

  return k;
}

bool CMathExpression::ParseElement( CParser& IC )
{
  CElement* e;
  CFunction* f;
  bool element_creation;
  unsigned i=0, n;
  const char* pos;

  pos = IC.GetPos();
 // s = m_StackSize;
  n = m_ElementDB->GetSize();
  e = m_ElementDB->ParseElement( IC );
  
  if( e )
  {
	  if (IC.TryFind('(')) {
		  i = GetLevel(IC);
		  IC.Find(')');
	  }
	element_creation = (m_ElementDB->GetSize() - n) != 0; 

    f = e->GetFunction();
    if ( element_creation && ( i > 0 ) )
    {
      f->SetParameterNb( i );
      e->SetFunct();
#ifdef _DEBUG
      CDisplay ds;
      ds += "SetParamNb   : ";
      e->Display( ds );
      ds +=  ':' ;
      ds += CString( e->GetFunction()->GetParameterNb() );
      TRACE( ds.GetBufferPtr() );
#endif
    }
    else if( ( f->GetParameterNb() == 2 ) && ( GetLastOperator() == CElementDataBase::OP_CONCAT ) )
    {
      m_StackSize--;
      ASSERT( e->IsNumeric() ); // for numeric function, it is allowed to use comma without concatenation op.
      ASSERT( f->GetParameterNb() == i + 1 );
    }
    else
    {
      ASSERT( f->GetParameterNb() == i );
    }

    Push( e );

    //Check if symbol definition for this operator
    if( IC.GetChar() == CParser::m_SymbolDelimiter )
    {
		CMathExpression equ(m_ElementDB);
		equ.PushBranch(*this,m_StackSize);
      m_ElementDB->AssociateSymbol( IC, equ);
      //clear equation to avoid being executed after being parsed.
	  //m_StackSize = s;
      Push( RefToElement( CElementDataBase::OP_NONE ) );
    }
  }

  return ( e != NULL );
}

void CMathExpression::StoreStackPointer( char c, unsigned pos_array[] )
{
  unsigned elem_id;
  elem_id = toupper( c ) - 'A';
  ASSERT( elem_id < CElementDataBase::MAX_EXP );
  pos_array[elem_id]  = m_StackSize;
}
