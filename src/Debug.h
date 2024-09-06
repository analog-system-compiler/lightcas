

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

#pragma once

extern int( *PUTS )( const char* );

#ifdef _DEBUG

#define DEBUG_LEVEL 1

#ifdef __GNUC__

#include <assert.h>
#include <math.h>
#define ASSERT(expr)    assert(expr)
#define ASSERT_NAN( x ) assert( !isnan( x ) )
#define IS_NAN( x )     isnan( x )

#else // defined( __GNUC__ )

#include <crtdbg.h>
#define ASSERT(expr)    _ASSERTE(expr)
#define ASSERT_NAN( x ) _ASSERTE( !_isnan( x ) )
#define IS_NAN( x )     _isnan( x )

#endif // defined( __GNUC__ )

#else // defined( _DEBUG )

#define DEBUG_LEVEL 0
#define TRACE( s, ... )
#define ASSERT(s)
#define ASSERT_NAN( x )
#define IS_NAN( x ) 0

#endif // defined( _DEBUG )



