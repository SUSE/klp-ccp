/* { dg-require-effective-target alloca } */
typedef unsigned long grub_uint64_t;
typedef grub_uint64_t grub_size_t;
grub_cmdline_get (unsigned max_len, int echo_char)
{
  unsigned xpos, ypos, ystart;
  grub_size_t lpos, llen;
  char buf[max_len];
  void cl_print (int pos, int c)
  {
      char *p;
      for (p = buf + pos; *p; p++)
      {
        if (xpos++ > 78)
          grub_putchar ('\n');
        grub_putchar (*p);
      }
 }
 void cl_delete (unsigned len)
 {
   cl_set_pos ();
   cl_print (lpos, ' ');
   grub_memmove ();
   cl_print (lpos, echo_char);
   cl_set_pos ();
 }
 cl_delete (llen);
 grub_size_t n = lpos;
 cl_delete (n);
}

/* cp-out: warning: [^:]*: line 15, column 10: identifier "grub_putchar" not declared */
/* cp-out: warning: [^:]*: line 16, column 8: identifier "grub_putchar" not declared */
/* cp-out: warning: [^:]*: line 21, column 3: identifier "cl_set_pos" not declared */
/* cp-out: warning: [^:]*: line 23, column 3: identifier "grub_memmove" not declared */
/* cp-out: warning: [^:]*: line 25, column 3: identifier "cl_set_pos" not declared */
