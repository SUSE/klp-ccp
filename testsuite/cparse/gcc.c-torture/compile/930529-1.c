/* { dg-require-effective-target untyped_assembly } */

struct r
{
  int d1, d2;
};

struct km
{
  int d;
};

struct f1
{
  char *fn;
  char *fd;
  char *fs;
  char *ic;
  void (*ff) ();
};

int g ();

int y;
struct r *bs;
int bv;

void b ();
char *w ();

struct km **q;
char **mns;
int nm;
struct f1 **z;

f (char *km, char *h)
{
  struct f1 *t;
  int map = midn(km, strlen(km));
  int V;
  int c;
  struct r r;
  struct f1 *cm;

  if (!g(&V, &cm, h, strlen(h)))
    {
      c = (cm - z[V]);
      goto L;
    }

  for (c = 0; c < nm; c++)
    if (!strcmp (h, mns[c]))
      {
	V = -1;
	goto L;
      }

  for (c = 0; c < y; c++)
    {
      if (!memcmp (&bs[c], &r, 8))
	goto L;
    }

  h = w (&r);
  if (!bv)
    {
      bs = g (8);
      t = (struct f1 *)g (20);
    }
  else
    {
      bs = g (bs, y * 8);
      z[bv] = cr (z[bv], (1 + y) * 20);
      t = &z[bv][y - 1];
    }
  bs[y - 1] = r;
  t->fs[0] = sp (y - 1);
  t->fs[1] = 0;
  t->ic = 0;
  t->fd = 0;
  t->fn = cs (h);
  t->ff = b;
 L:
  g (q[map], V, c);
}

/* cp-out: warning: [^:]*: line 39, column 12: identifier "midn" not declared */
/* cp-out: warning: [^:]*: line 39, column 21: identifier "strlen" not declared */
/* cp-out: warning: [^:]*: line 45, column 21: identifier "strlen" not declared */
/* cp-out: warning: [^:]*: line 52, column 9: identifier "strcmp" not declared */
/* cp-out: warning: [^:]*: line 60, column 11: identifier "memcmp" not declared */
/* cp-out: warning: [^:]*: line 73, column 14: identifier "cr" not declared */
/* cp-out: warning: [^:]*: line 77, column 13: identifier "sp" not declared */
/* cp-out: warning: [^:]*: line 81, column 10: identifier "cs" not declared */
/* cp-out: warning: [^:]*: line 67, columns 11-16: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 72, columns 11-24: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 73, columns 14-38: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 81, columns 10-16: assignment to pointer from integer which is not NULL */
