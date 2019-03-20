/* { dg-do compile } */
/* { dg-options "-O2 -fcompare-debug -w" } */
/* { dg-xfail-if "" { powerpc-ibm-aix* } } */

typedef struct
{
  int default_screen;
} *_XPrivint;
typedef int FcBool;
typedef struct
{
  struct _XftintInfo *next;
} XftintInfo;
int _XftintInfo;

void _XftintInfoGet (int * dpy, FcBool createIfNecessary)
{
  int event_base;
  &event_base
   && dpy[createIfNecessary] != 0;
}

void _XftCloseint (int dpy, XftintInfo * info,XftintInfo  * *prev)
{
  _XftintInfoGet (dpy, 0);
  for (prev = &_XftintInfo; info = *prev; prev = &(*prev)->next)
    ;
}

/* cp-out: warning: [^:]*: line 25, column 18: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 26, columns 14-26: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 26, columns 49-63: incompatible pointer types in assignment */
