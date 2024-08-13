

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

#include <algorithm>
#include <vector>

template < class T > class CVector : public std::vector < T >
{

public:
  void      SetSize( unsigned size )                      { std::vector < T >::resize( size ); }
  void      InsertAt( unsigned index, const T& e )        { std::vector < T >::insert( std::vector < T >::begin() + index, e ); }
  void      InsertAt( unsigned index, const CVector& v  ) { std::vector < T >::insert( std::vector < T >::begin() + index, v.std::vector < T >::begin(), v.std::vector < T >::end() ); }
  void      RemoveAt( unsigned start )                    { std::vector < T >::erase(  std::vector < T >::begin() + start ); }
  void      RemoveAt( unsigned start, unsigned count  )   { std::vector < T >::erase(  std::vector < T >::begin() + start, std::vector < T >::begin() + start + count ); }
  void      Append( const CVector& v  )                   { std::vector < T >::insert( std::vector < T >::end(), v.begin(), v.end() );  }
  void      Append( const T& e )                          { std::vector < T >::push_back( e );}
  void      Push( const T& e )                            { std::vector < T >::push_back( e ); }
  T         Pop()                                         { T e = std::vector < T >::back();  std::vector < T >::pop_back(); return e; }
  void      RemoveAll()                                   { std::vector < T >::clear(); }
  void      DeleteAll()                                   { while( !std::vector < T >::empty() ) { delete std::vector < T >::back(); std::vector < T >::pop_back(); } }
  void      Copy( const CVector& v )                      { std::vector < T >::assign( v.begin(), v.end() ); }
  void      SetAt( unsigned index, const T& e )           { std::vector < T >::at( index ) = e; }
  const T&  GetAt( unsigned index ) const                 { return std::vector < T >::at( index ); }
  const T*  GetData() const                               { return std::vector < T >::data(); }
  T*        GetData()                                     { return std::vector < T >::data(); }
  unsigned  GetSize() const                               { return std::vector < T >::size(); }
  void      CheckSize( unsigned index )                   { if( index >= GetSize() ) SetSize( index + 1 ); }
  bool      Find(const T& e)                              { return std::find( std::vector < T >::begin(), std::vector < T >::end(), e) != std::vector < T >::end();}

};



