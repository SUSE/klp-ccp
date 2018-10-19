struct s {
  int f;
};

f (w, v0, v1, v2, v3)
     struct s *w;
{
 g (v0 ? 1 : w->f, v1 ? v3 : v2);
}

/* cp-out: warning: [^:]*: line 8, columns 1-2: identifier "g" not declared */
