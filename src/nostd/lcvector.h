

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

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdlib>
#include "../debug.h"

template <class T>
class CVector
{

private:
  unsigned m_Size;
  T *m_Data;

public:
  void SetSize(unsigned size);
  void InsertAt(unsigned index, const T &e);
  void InsertAt(unsigned index, const CVector &v);
  void RemoveAt(unsigned start, unsigned count = 1);
  void Append(const CVector &v) { InsertAt(m_Size, v); }
  void Append(const T &e) { InsertAt(m_Size, e); }
  void Push(const T &e) { Append(e); }
  T Pop()
  {
    T e = GetAt(m_Size - 1);
    m_Size--;
    return e;
  }
  void RemoveAll() { m_Size = 0; }
  void DeleteAll()
  {
    for (unsigned i = 0; i < GetSize(); i++)
    {
      delete m_Data[i];
    }
    RemoveAll();
  }
  void Copy(const CVector &v)
  {
    RemoveAll();
    Append(v);
  }
  T &GetAt(unsigned index) const
  {
    ASSERT(index < m_Size);
    ASSERT(m_Data != NULL);
    return m_Data[index];
  }
  void SetAt(unsigned index, const T &e)
  {
    ASSERT(index < m_Size);
    ASSERT(m_Data != NULL);
    m_Data[index] = static_cast<T>(e);
  }
  T *GetData() const
  {
    ASSERT(m_Data != NULL);
    return m_Data;
  }
  unsigned GetSize() const { return m_Size; }
  void CheckSize(unsigned index)
  {
    if (index >= GetSize())
      SetSize(index + 1);
  }
  T &operator[](unsigned index) const { return GetAt(index); }
  bool Find(const T &e)
  {
    for (unsigned i = 0; i < GetSize(); i++)
      if (GetAt(i) == e)
        return true;
    return false;
  }

public:
  CVector()
  {
    m_Data = NULL;
    m_Size = 0;
  }
  CVector(const CVector &v)
  {
    m_Data = NULL;
    Copy(v);
  }
  virtual ~CVector()
  {
    if (m_Data)
    {
      free((void *)m_Data);
    }
  }
};

template <class T>
void CVector<T>::SetSize(unsigned size)
{
  if (size)
  {
    m_Data = static_cast<T *>(::realloc(m_Data, size * sizeof(T)));
  }
  else
  {
    ::free(m_Data);
    m_Data = NULL;
  }
  m_Size = size;
}

template <class T>
void CVector<T>::InsertAt(unsigned index, const T &e)
{
  ASSERT(index <= m_Size);
  unsigned old_size = m_Size;
  SetSize(m_Size + 1);
  ::memmove(&(m_Data[index + 1]), &(m_Data[index]), (old_size - index) * sizeof(T));
  m_Data[index] = e;
}

template <class T>
void CVector<T>::InsertAt(unsigned index, const CVector<T> &v)
{
  ASSERT(index <= m_Size);
  unsigned old_size = m_Size;
  SetSize(m_Size + v.m_Size);
  ::memmove(&(m_Data[index + v.m_Size]), &(m_Data[index]), (old_size - index) * sizeof(T));
  ::memcpy(&(m_Data[index]), v.m_Data, v.m_Size * sizeof(T));
}

template <class T>
void CVector<T>::RemoveAt(unsigned start, unsigned count)
{
  ASSERT(m_Data != NULL);
  ASSERT((int)(m_Size - start - count) >= 0);
  ::memmove(&(m_Data[start]), &(m_Data[start + count]), (m_Size - start - count) * sizeof(T));
  m_Size -= count;
}

#endif
