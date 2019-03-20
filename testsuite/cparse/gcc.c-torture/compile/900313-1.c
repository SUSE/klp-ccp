/* { dg-require-effective-target alloca } */
main ()
{
  char *a;
  foo (alloca (10000));
  foo (alloca (100000));
  foo (alloca ((int) &main));
}

many_par (a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)
{
  char *x;
  int aa, ba, ca, da, ea, fa, ga, ha, ia, ja, ka, la, ma, na, oa, pa;

  aa = bar ();
  ba = bar ();
  ca = bar ();
  da = bar ();
  ea = bar ();
  fa = bar ();
  ga = bar ();
  ha = bar ();
  ia = bar ();
  ja = bar ();
  ka = bar ();
  la = bar ();
  ma = bar ();
  na = bar ();
  oa = bar ();
  pa = bar ();
  foobar (a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, aa, ba, ca,
	  da, ea, fa, ga, ha, ia, ja, ka, la, ma, na, oa, pa);

}

foobar (a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, aa, ba, ca,
	da, ea, fa, ga, ha, ia, ja, ka, la, ma, na, oa, pa)
{
  int ab, bb, cb, db, eb, fb, gb, hb, ib, jb, kb, lb, mb, nb, ob, pb;
  int qb, rb, sb, tb, ub, vb, xb, yb;

  ab = bar ();
  bb = bar ();
  cb = bar ();
  db = bar ();
  eb = bar ();
  fb = bar ();
  gb = bar ();
  hb = bar ();
  ib = bar ();
  jb = bar ();
  kb = bar ();
  lb = bar ();
  mb = bar ();
  nb = bar ();
  ob = bar ();
  pb = bar ();
  qb = bar ();
  rb = bar ();
  sb = bar ();
  tb = bar ();
  ub = bar ();
  vb = bar ();
  xb = bar ();
  yb = bar ();

  boofar (a);
  boofar (b);
  boofar (c);
  boofar (d);
  boofar (e);
  boofar (f);
  boofar (g);
  boofar (h);
  boofar (i);
  boofar (j);
  boofar (k);
  boofar (l);
  boofar (m);
  boofar (n);
  boofar (o);
  boofar (p);
  boofar (aa);
  boofar (ba);
  boofar (ca);
  boofar (da);
  boofar (ea);
  boofar (fa);
  boofar (ga);
  boofar (ha);
  boofar (ia);
  boofar (ja);
  boofar (ka);
  boofar (la);
  boofar (ma);
  boofar (na);
  boofar (oa);
  boofar (pa);

  boofar (ab);
  boofar (bb);
  boofar (cb);
  boofar (db);
  boofar (eb);
  boofar (fb);
  boofar (gb);
  boofar (hb);
  boofar (ib);
  boofar (jb);
  boofar (kb);
  boofar (lb);
  boofar (mb);
  boofar (nb);
  boofar (ob);
  boofar (pb);

  boofar (a);
  boofar (b);
  boofar (c);
  boofar (d);
  boofar (e);
  boofar (f);
  boofar (g);
  boofar (h);
  boofar (i);
  boofar (j);
  boofar (k);
  boofar (l);
  boofar (m);
  boofar (n);
  boofar (o);
  boofar (p);
  boofar (aa);
  boofar (ba);
  boofar (ca);
  boofar (da);
  boofar (ea);
  boofar (fa);
  boofar (ga);
  boofar (ha);
  boofar (ia);
  boofar (ja);
  boofar (ka);
  boofar (la);
  boofar (ma);
  boofar (na);
  boofar (oa);
  boofar (pa);

  boofar (ab);
  boofar (bb);
  boofar (cb);
  boofar (db);
  boofar (eb);
  boofar (fb);
  boofar (gb);
  boofar (hb);
  boofar (ib);
  boofar (jb);
  boofar (kb);
  boofar (lb);
  boofar (mb);
  boofar (nb);
  boofar (ob);
  boofar (pb);

}

test_exit_ignore_stack ()
{
  foobar (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
}

/* cp-out: warning: [^:]*: line 5, column 2: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 5, column 7: identifier "alloca" not declared */
/* cp-out: warning: [^:]*: line 6, column 2: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 6, column 7: identifier "alloca" not declared */
/* cp-out: warning: [^:]*: line 7, column 2: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 7, column 7: identifier "alloca" not declared */
/* cp-out: warning: [^:]*: line 15, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 16, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 17, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 18, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 19, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 20, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 21, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 22, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 23, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 24, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 25, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 26, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 27, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 28, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 29, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 30, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 31, column 2: identifier "foobar" not declared */
/* cp-out: warning: [^:]*: line 42, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 43, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 44, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 45, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 46, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 47, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 48, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 49, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 50, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 51, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 52, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 53, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 54, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 55, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 56, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 57, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 58, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 59, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 60, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 61, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 62, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 63, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 64, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 65, column 7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 67, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 68, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 69, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 70, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 71, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 72, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 73, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 74, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 75, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 76, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 77, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 78, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 79, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 80, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 81, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 82, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 83, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 84, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 85, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 86, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 87, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 88, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 89, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 90, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 91, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 92, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 93, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 94, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 95, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 96, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 97, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 98, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 100, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 101, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 102, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 103, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 104, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 105, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 106, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 107, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 108, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 109, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 110, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 111, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 112, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 113, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 114, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 115, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 117, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 118, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 119, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 120, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 121, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 122, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 123, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 124, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 125, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 126, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 127, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 128, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 129, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 130, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 131, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 132, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 133, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 134, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 135, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 136, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 137, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 138, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 139, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 140, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 141, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 142, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 143, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 144, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 145, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 146, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 147, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 148, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 150, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 151, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 152, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 153, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 154, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 155, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 156, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 157, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 158, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 159, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 160, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 161, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 162, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 163, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 164, column 2: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 165, column 2: identifier "boofar" not declared */
