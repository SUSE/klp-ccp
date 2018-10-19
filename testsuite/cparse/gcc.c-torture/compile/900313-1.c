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

/* cp-out: warning: [^:]*: line 5, columns 2-5: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 5, columns 7-13: identifier "alloca" not declared */
/* cp-out: warning: [^:]*: line 6, columns 2-5: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 6, columns 7-13: identifier "alloca" not declared */
/* cp-out: warning: [^:]*: line 7, columns 2-5: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 7, columns 7-13: identifier "alloca" not declared */
/* cp-out: warning: [^:]*: line 15, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 16, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 17, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 18, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 19, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 20, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 21, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 22, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 23, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 24, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 25, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 26, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 27, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 28, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 29, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 30, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 31, columns 2-8: identifier "foobar" not declared */
/* cp-out: warning: [^:]*: line 42, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 43, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 44, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 45, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 46, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 47, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 48, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 49, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 50, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 51, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 52, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 53, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 54, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 55, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 56, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 57, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 58, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 59, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 60, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 61, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 62, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 63, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 64, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 65, columns 7-10: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 67, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 68, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 69, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 70, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 71, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 72, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 73, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 74, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 75, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 76, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 77, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 78, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 79, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 80, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 81, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 82, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 83, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 84, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 85, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 86, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 87, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 88, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 89, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 90, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 91, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 92, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 93, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 94, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 95, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 96, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 97, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 98, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 100, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 101, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 102, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 103, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 104, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 105, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 106, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 107, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 108, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 109, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 110, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 111, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 112, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 113, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 114, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 115, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 117, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 118, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 119, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 120, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 121, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 122, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 123, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 124, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 125, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 126, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 127, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 128, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 129, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 130, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 131, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 132, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 133, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 134, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 135, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 136, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 137, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 138, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 139, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 140, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 141, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 142, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 143, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 144, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 145, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 146, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 147, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 148, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 150, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 151, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 152, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 153, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 154, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 155, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 156, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 157, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 158, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 159, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 160, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 161, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 162, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 163, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 164, columns 2-8: identifier "boofar" not declared */
/* cp-out: warning: [^:]*: line 165, columns 2-8: identifier "boofar" not declared */
