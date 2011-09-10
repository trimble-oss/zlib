/* gzguts.h -- zlib internal header definitions for gz* operations
 * Copyright (C) 2004, 2005, 2010 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#ifdef _LARGEFILE64_SOURCE
#  ifndef _LARGEFILE_SOURCE
#    define _LARGEFILE_SOURCE
#  endif
#  ifdef _FILE_OFFSET_BITS
#    undef _FILE_OFFSET_BITS
#  endif
#endif

#define ZLIB_INTERNAL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "zlib.h"

#ifdef NO_DEFLATE       /* for compatibility with old definition */
#  define NO_GZCOMPRESS
#endif

#ifdef WIN32
#  include <io.h>
#  define vsnprintf _vsnprintf
#endif

#ifndef local
#  define local static
#endif
/* compile with -Dlocal if your debugger can't find static symbols */

/* gz* functions always use library allocation functions */
#ifndef STDC
  extern voidp  malloc OF((uInt size));
  extern void   free   OF((voidpf ptr));
#endif

/* get errno and strerror definition */
#if defined UNDER_CE && defined NO_ERRNO_H
#  define zstrerror(errnum) strwinerror((DWORD)errnum)
#else
#  ifdef STDC
#    include <errno.h>
#    define zstrerror() strerror(errno)
#  else
#    define zstrerror() "stdio error (consult errno)"
#  endif
#endif

/* MVS fdopen() */
#ifdef __MVS__
#  pragma map (fdopen , "\174\174FDOPEN")
   FILE *fdopen(int, const char *);
#endif

#ifdef _LARGEFILE64_SOURCE
#  define z_off64_t off64_t
#else
#  define z_off64_t z_off_t
#endif

/* default i/o buffer size -- double this for output when reading */
#define GZBUFSIZE 8192

/* gzip modes, also provide a little integrity check on the passed structure */
#define GZ_NONE 0
#define GZ_READ 7247
#define GZ_WRITE 31153
#define GZ_APPEND 1     /* mode set to GZ_WRITE after the file is opened */

/* internal gzip file state data structure */
typedef struct {
        /* used for both reading and writing */
    int mode;               /* see gzip modes above */
    int fd;                 /* file descriptor */
    char *path;             /* path or fd for error messages */
    z_off64_t pos;          /* current position in uncompressed data */
    unsigned size;          /* buffer size, zero if not allocated yet */
    unsigned want;          /* requested buffer size, default is GZBUFSIZE */
    unsigned char *in;      /* input buffer */
    unsigned char *out;     /* output buffer (double-sized when reading) */
    unsigned char *next;    /* next output data to deliver or write */
        /* just for reading */
    int how;                /* 0: get header, 1: copy, 2: decompress */
    unsigned have;          /* amount of output data unused */
    z_off64_t start;        /* where the gzip data started, for rewinding */
    z_off64_t raw;          /* where the raw data started, for seeking */
    int eof;                /* true if end of input file reached */
        /* just for writing */
    int level;              /* compression level */
    int strategy;           /* compression strategy */
        /* seek request */
    int seek;               /* true if seek request pending */
    z_off64_t skip;         /* amount to skip (already rewound if backwards) */
        /* error information */
    int err;                /* error code */
    char *msg;              /* error message */
        /* zlib inflate or deflate stream */
    z_stream strm;          /* stream structure in-place (not a pointer) */
} gz_state;
typedef gz_state FAR *gz_statep;

/* shared functions */
ZEXTERN void ZEXPORT gz_error OF((gz_statep, int, char *));