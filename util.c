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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

bool get_line(char **lineptr, size_t *n, FILE *stream)
{
    if (!*lineptr)
    {
    	*n = 128;
    	*lineptr = malloc(*n);
    }

    bool success = false;
    size_t len = 0;
    while (fgets(*lineptr + len, *n - len, stream))
    {
    	success = true;
    	len += strlen(*lineptr + len);
    	if ((*lineptr)[len - 1] == '\n')
        {
    	    if (len > 1 && (*lineptr)[len - 2] == '\r')	// DOS newline
    		len--;
    	    (*lineptr)[len - 1] = '\0';
    	    break;
    	}
    	if (feof(stream))	// last line in file lacks a newline
    	    break;
    	*n *= 2;
    	*lineptr = realloc(*lineptr, *n);
    }
    return success;
}

char *dup_str(const char *s)
{
    size_t len = strlen(s) + 1;
    char *t = malloc(len);
    memcpy(t, s, len);
    return t;
}
