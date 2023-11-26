/*
 *
 * Copyright 2016 CUBRID Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

/*
 * memory_cppwrapper.hpp - Overloading operator new, delete
 */

#ifndef _MEMORY_CPPWRAPPER_HPP_
#define _MEMORY_CPPWRAPPER_HPP_

#include "memory_cwrapper.h"
// XXX: for test
#include <stdio.h>

#ifdef SERVER_MODE
inline void *operator new (size_t size, const char *file)
{
  void *p = cub_alloc (size, file);
  fprintf (stdout, "overloaded new\n");
  return p;
}

inline void *operator new[] (size_t size, const char *file)
{
  void *p = cub_alloc (size, file);
  fprintf (stdout, "overloaded new[]\n");
  return p;
}

inline void operator delete (void *ptr) noexcept
{
  cub_free (ptr);
}

inline void operator delete (void *ptr, size_t sz) noexcept
{
  cub_free (ptr);
}

#ifndef SKIP_NEW_OVERLOADING
#define new new(__FILE__)
#endif // SKIP_NEW_OVERLOADING
#endif // SERVER_MODE

#endif // _MEMORY_CPPWRAPPER_HPP_
