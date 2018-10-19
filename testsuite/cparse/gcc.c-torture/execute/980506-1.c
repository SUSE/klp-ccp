struct decision
{
  char enforce_mode;             
  struct decision *next;         
};


static void
clear_modes (p)
     register struct decision *p;
{
  goto blah;

foo:
  p->enforce_mode = 0;
blah:
  if (p)
    goto foo;
}

main()
{
  struct decision *p = 0;
  clear_modes (p);
  exit (0);
}

/* cp-out: warning: [^:]*: line 25, columns 2-6: identifier "exit" not declared */
