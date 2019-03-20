void
func_18 ( int t )
{
  unsigned l_889;
  int l_895 = 1;
  for (0; 1; ++l_889)
    {
      int t1 = 0;
      if (func_81 (1))
        {
          int rhs = l_895;
          if (rhs == 0)
            rhs = 1;
          if (1 & (t % rhs))
            t1 = 1;
        }
      func_98 (t1);
      l_895 = 0;
    }
}

/* cp-out: warning: [^:]*: line 9, column 10: identifier "func_81" not declared */
/* cp-out: warning: [^:]*: line 17, column 6: identifier "func_98" not declared */
