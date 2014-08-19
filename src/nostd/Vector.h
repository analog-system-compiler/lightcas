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

#ifndef	_VECTOR_H_
#define _VECTOR_H_

#include <cstdlib>
#include "Def.h"

template < class T > class CVector
{

private:
  unsigned	m_Size;
  T*			m_Data;

public:
  void	SetSize( unsigned size );
  void	InsertAt( unsigned index, const T e );
  void	InsertAt( unsigned index, const CVector& v  );
  void	RemoveAt( unsigned start, unsigned count = 1 );
  void	Append( const T* data_ptr, unsigned size );
  void	SetValue( const T e );
  unsigned	GetIndexOf( const T e ) const;

  /* in-line functions */
  /*void	Append( const CVector& v ) // deprecated
  {
      Append( v.m_Data, v.m_Size );
  }*/
  T*	GetData() const
  {
    ASSERT( m_Data != NULL );
    return m_Data;
  }
  void	Add( const T e )
  {
    InsertAt( m_Size , e );
  }
  void	Add( const CVector& v ) // deprecated
  {
    Append( v.m_Data, v.m_Size );
  }
  void	RemoveAll()
  {
    m_Size = 0;
  }
  void	DeleteAll()
  {
    for( unsigned i = 0; i < GetSize(); i++ )
    {
      delete m_Data[i];
    }
    RemoveAll();
  }
  void	Copy( const CVector& v )
  {
    m_Size = 0;
    Add( v );
  }
  void	Copy( const T* data_ptr, unsigned size )
  {
    m_Size = 0;
    Append( data_ptr, size );
  }
  T&	operator[] ( unsigned index ) const
  {
    ASSERT( index < m_Size );
    ASSERT( m_Data != NULL );
    return m_Data[ index ];
  }
  T		GetAt( unsigned index ) const
  {
    ASSERT( index < m_Size );
    ASSERT( m_Data != NULL );
    return m_Data[ index ];
  }
  void	SetAt( unsigned index, const T e )
  {
    ASSERT( index < m_Size );
    ASSERT( m_Data != NULL );
    m_Data[ index ] = static_cast<T>( e );
  }
  unsigned	GetSize() const
  {
    return m_Size;
  }

public:
  CVector()
  {
    m_Data = NULL;
    m_Size = 0;
  }
  CVector( const CVector& v )
  {
    m_Data = NULL;
    Copy( v );
  }
  virtual ~CVector()
  {
    if( m_Data ) free( ( void* )m_Data );
  }

};

template < class T > void CVector< T >::SetSize( unsigned size )
{
  if( size ) 
	m_Data = static_cast<T*>( realloc( m_Data, size * sizeof( T ) ) );
  else
  {
    free( m_Data );
    m_Data = NULL;
  }
  m_Size = size;
}

template < class T > void CVector< T >::InsertAt( unsigned index, const T e )
{
  ASSERT( index <= m_Size );
  unsigned old_size = m_Size;
  SetSize( m_Size + 1 );
  memmove( &( m_Data[index + 1] ), &( m_Data[index] ), ( old_size - index ) * sizeof( T ) );
  m_Data[ index ] = static_cast<T>( e );
}

template < class T > void CVector< T >::InsertAt( unsigned index, const CVector& v )
{
  ASSERT( index <= m_Size );
  unsigned old_size = m_Size;
  SetSize( m_Size + v.m_Size );
  memmove( &( m_Data[index + v.m_Size] ), &( m_Data[index] ), ( old_size - index  ) * sizeof( T ) );
  memcpy( &( m_Data[index] ), v.m_Data, v.m_Size * sizeof( T ) );
}

template < class T > void CVector< T >::RemoveAt( unsigned start, unsigned count )
{
  ASSERT( m_Data != NULL );
  ASSERT( ( int )( m_Size - start - count ) >= 0 );
  memmove( &( m_Data[start] ), &( m_Data[start + count] ), ( m_Size - start - count ) * sizeof( T ) );
  m_Size -= count;
}

template < class T > void CVector< T >::Append( const T* data_ptr, unsigned size )
{
  unsigned old_size = m_Size;
  SetSize( m_Size + size );
  memcpy( &( m_Data[old_size] ), data_ptr, size * sizeof( T ) );
}

template < class T > void CVector< T >::SetValue( const T e )
{
  for( unsigned i = 0; i < m_Size; i++ )
    m_Data[ i ] = e;
}

template < class T > unsigned CVector< T >::GetIndexOf( const T e ) const
{
  unsigned i;

  for( i = 0; i < m_Size; i++ )
    if( e == m_Data[ i ] ) break;

  return i;
}

#endif
