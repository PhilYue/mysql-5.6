/* Copyright (c) 2000, 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef MY_DBUG_INCLUDED
#define MY_DBUG_INCLUDED

/**
  @file include/my_dbug.h
*/

#ifdef MY_MSCRT_DEBUG
#include <crtdbg.h>
#endif
#include <stdlib.h>

#include "my_compiler.h"

#if !defined(DBUG_OFF)
#include <assert.h>  // IWYU pragma: keep
#include <stdio.h>
#endif

// Needs to be extern "C" for the time being, since extra/regex/ uses it.
#ifdef  __cplusplus
extern "C" {
#endif
#if !defined(DBUG_OFF)

struct _db_stack_frame_ {
  const char *func;      /* function name of the previous stack frame       */
  const char *file;      /* filename of the function of previous frame      */
  unsigned int level;    /* this nesting level, highest bit enables tracing */
  struct _db_stack_frame_ *prev; /* pointer to the previous frame */
};

struct  CODE_STATE;

extern  int _db_keyword_(struct CODE_STATE *, const char *, int);
extern  int _db_explain_(struct CODE_STATE *cs, char *buf, size_t len);
extern  int _db_explain_init_(char *buf, size_t len);
extern	int _db_is_pushed_(void);
extern  void _db_process_(const char *name);
extern  void _db_push_(const char *control);
extern  void _db_pop_(void);
extern  void _db_set_(const char *control);
extern  void _db_set_init_(const char *control);
extern void _db_enter_(const char *_func_, const char *_file_, unsigned int _line_,
                       struct _db_stack_frame_ *_stack_frame_);
extern  void _db_return_(unsigned int _line_, struct _db_stack_frame_ *_stack_frame_);
extern  void _db_pargs_(unsigned int _line_,const char *keyword);
extern  int _db_enabled_();
extern  void _db_doprnt_(const char *format,...)
  MY_ATTRIBUTE((format(printf, 1, 2)));
extern  void _db_dump_(unsigned int _line_,const char *keyword,
                       const unsigned char *memory, size_t length);
extern  void _db_end_(void);
extern  void _db_lock_file_(void);
extern  void _db_unlock_file_(void);
extern  FILE *_db_fp_(void);
extern  void _db_flush_();
extern  const char* _db_get_func_(void);

#define DBUG_ENTER(a) struct _db_stack_frame_ _db_stack_frame_; \
        _db_enter_ (a,__FILE__,__LINE__,&_db_stack_frame_)
#define DBUG_LEAVE _db_return_ (__LINE__, &_db_stack_frame_)
#define DBUG_RETURN(a1) do {DBUG_LEAVE; return(a1);} while(0)
#define DBUG_VOID_RETURN do {DBUG_LEAVE; return;} while(0)
#define DBUG_EXECUTE(keyword,a1) \
        do {if (_db_keyword_(0, (keyword), 0)) { a1 }} while(0)
#define DBUG_EXECUTE_IF(keyword,a1) \
        do {if (_db_keyword_(0, (keyword), 1)) { a1 }} while(0)
#define DBUG_EVALUATE(keyword,a1,a2) \
        (_db_keyword_(0,(keyword), 0) ? (a1) : (a2))
#define DBUG_EVALUATE_IF(keyword,a1,a2) \
        (_db_keyword_(0,(keyword), 1) ? (a1) : (a2))
#define DBUG_PRINT(keyword,arglist) \
        do \
        {  \
          _db_pargs_(__LINE__,keyword); \
          if (_db_enabled_()) \
          {  \
            _db_doprnt_ arglist; \
          } \
        } while(0)

#define DBUG_PUSH(a1) _db_push_ (a1)
#define DBUG_POP() _db_pop_ ()
#define DBUG_SET(a1) _db_set_ (a1)
#define DBUG_SET_INITIAL(a1) _db_set_init_ (a1)
#define DBUG_PROCESS(a1) _db_process_(a1)
#define DBUG_FILE _db_fp_()
#define DBUG_DUMP(keyword,a1,a2) _db_dump_(__LINE__,keyword,a1,a2)
#define DBUG_END()  _db_end_ ()
#define DBUG_LOCK_FILE _db_lock_file_()
#define DBUG_UNLOCK_FILE _db_unlock_file_()
#define DBUG_ASSERT(A) assert(A)
#define DBUG_EXPLAIN(buf,len) _db_explain_(0, (buf),(len))
#define DBUG_EXPLAIN_INITIAL(buf,len) _db_explain_init_((buf),(len))
#ifndef _WIN32
#define DBUG_ABORT()                    (_db_flush_(), abort())
#else
/*
  Avoid popup with abort/retry/ignore buttons. When BUG#31745 is fixed we can
  call abort() instead of _exit(2) (now it would cause a "test signal" popup).
*/
#include <crtdbg.h>

#define DBUG_ABORT() (_db_flush_(),\
                     (void)_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE),\
                     (void)_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR),\
                     _exit(2))
#endif

/*
  Make the program fail, without creating a core file.
  abort() will send SIGABRT which (most likely) generates core.
  Use SIGKILL instead, which cannot be caught.
  We also pause the current thread, until the signal is actually delivered.
  An alternative would be to use _exit(EXIT_FAILURE),
  but then valgrind would report lots of memory leaks.
 */
#ifdef _WIN32
#define DBUG_SUICIDE() DBUG_ABORT()
#else
extern void _db_suicide_() MY_ATTRIBUTE((noreturn));
extern void _db_flush_gcov_();
#define DBUG_SUICIDE() (_db_flush_(), _db_suicide_())
#endif

#else                                           /* No debugger */

#define DBUG_ENTER(a1)
#define DBUG_LEAVE
#define DBUG_RETURN(a1)                 do { return(a1); } while(0)
#define DBUG_VOID_RETURN                do { return; } while(0)
#define DBUG_EXECUTE(keyword,a1)        do { } while(0)
#define DBUG_EXECUTE_IF(keyword,a1)     do { } while(0)
#define DBUG_EVALUATE(keyword,a1,a2) (a2)
#define DBUG_EVALUATE_IF(keyword,a1,a2) (a2)
#define DBUG_PRINT(keyword,arglist)     do { } while(0)
#define DBUG_PUSH(a1)                   do { } while(0)
#define DBUG_SET(a1)                    do { } while(0)
#define DBUG_SET_INITIAL(a1)            do { } while(0)
#define DBUG_POP()                      do { } while(0)
#define DBUG_PROCESS(a1)                do { } while(0)
#define DBUG_DUMP(keyword,a1,a2)        do { } while(0)
#define DBUG_END()                      do { } while(0)
#define DBUG_ASSERT(A)                  do { } while(0)
#define DBUG_LOCK_FILE                  do { } while(0)
#define DBUG_FILE (stderr)
#define DBUG_UNLOCK_FILE                do { } while(0)
#define DBUG_EXPLAIN(buf,len)
#define DBUG_EXPLAIN_INITIAL(buf,len)
#define DBUG_ABORT()                    do { } while(0)
#define DBUG_SUICIDE()                  do { } while(0)

#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#if !defined(DBUG_OFF)
#include <sstream>
#include <string>

/*
  A C++ interface to the DBUG_PRINT macro.  The DBUG_LOG macro takes two
  arguments.  The first argument is the keyword, as that of the
  DBUG_PRINT.  The 2nd argument 'v' will be passed to a C++ output stream.
  This enables the use of C++ style output stream operator.  In the code, it
  will be used as follows:

  DBUG_LOG("blob", "space: " << space_id);

  Note: DBUG_PRINT() has a limitation of 1024 bytes for a single
  print out.  So, this limitation is there for DBUG_LOG macro also.
*/

#define DBUG_LOG(keyword, v) do { \
	std::ostringstream		sout; \
	sout << v; \
	DBUG_PRINT(keyword, ("%s", sout.str().c_str())); \
} while(0)

void log_prefix(std::ostream& out);
void trace(const std::string& note);
void dump_trace();

#define DBUG_TRACE(x) \
do { \
  std::ostringstream sout; \
  log_prefix(sout); \
  sout << x; \
  trace(sout.str());\
} while(false)

#else /* DBUG_OFF */
#define DBUG_LOG(keyword, v) do {} while(0)
#define DBUG_TRACE(x) do {} while(0)
#endif /* DBUG_OFF */
#endif /* __cplusplus */

#endif /* MY_DBUG_INCLUDED */
