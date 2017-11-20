/* occ -- find minimum odd cycle covers (Graph Bipartization)   
   Copyright (C) 2006 Falk Hueffner

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

// A string literal containing all whitespace characters.
#define WHITESPACE " \f\r\t\v"

// Return a malloced copy of the string S (like strdup from POSIX).
char *dup_str(const char *s);

/* Similar to GNU getline(3) (but slightly different interface).  Read
   an entire line from STREAM, storing it into *LINEPTR without
   trailing newline. LINEPTR must be NULL or malloced to *N bytes. If
   the buffer is not sufficient, it will be realloced, and *LINEPTR
   and *N will be updated to reflect the new buffer. Returns true on
   success, or false on EOF or error.  */
bool get_line(char **lineptr, size_t *n, FILE *stream);

#endif	// UTIL_H
