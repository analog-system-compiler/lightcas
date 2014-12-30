/*******************************************************************************/
/*  Copyright (C) 2014 The LightCAS project                                      */
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

#pragma once

#include <cstdlib>
#include "Debug.h"
#include "Display.h"

class CValue 
{
private:
    double m_Value;
public:
    //bool   operator <  ( const CValue& v ) const { return ( m_Value < v.m_Value );  }
    //bool   operator >  ( const CValue& v ) const { return ( m_Value > v.m_Value );  }
    //bool   operator == ( const CValue& v ) const { return ( m_Value == v.m_Value ); }
    void   operator =  ( const CValue& v )       { m_Value = v.m_Value; }
    void   operator =  ( double v )              { m_Value = v;}
    double GetValue( )   const                   { return m_Value;    }
    bool   IsNegative()  const                   { return m_Value<0.; }
    const char *  GetFromString( const char *s1 );
    void   Display( CDisplay& s ) const;
    void   Negate() { m_Value = -m_Value; }
    
    CValue( double v=0. ) { m_Value=v;  }    
};