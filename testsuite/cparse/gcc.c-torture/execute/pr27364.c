int f(unsigned number_of_digits_to_use)
{
  if (number_of_digits_to_use >1294)
    return 0;
  return (number_of_digits_to_use * 3321928 / 1000000 + 1) /16;
}

int main(void)
{
  if (f(11) != 2)
    __builtin_abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 12, columns 2-6: identifier "exit" not declared */
