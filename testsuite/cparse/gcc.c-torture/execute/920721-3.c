static inline fu (unsigned short data)
{
  return data;
}
ru(i)
{
   if(fu(i++)!=5)abort();
   if(fu(++i)!=7)abort();
}
static inline fs (signed short data)
{
  return data;
}
rs(i)
{
   if(fs(i++)!=5)abort();
   if(fs(++i)!=7)abort();
}


main()
{
  ru(5);
  rs(5);
  exit(0);
}

/* cp-out: warning: [^:]*: line 7, column 17: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 8, column 17: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, column 17: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 17, column 17: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 25, column 2: identifier "exit" not declared */
