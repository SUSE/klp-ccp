/* Assignments via pointers pointing to global variables were being killed
   by SSA-DCE.  Test contributed by Paul Brook <paul@nowt.org>  */

int glob; 
 
void 
fn2(int ** q) 
{ 
  *q = &glob; 
} 
 
void test() 
{ 
  int *p; 
 
  fn2(&p); 
 
  *p=42; 
} 
 
int main() 
{ 
  test(); 
  if (glob != 42) abort(); 
  exit (0); 
}

/* cp-out: warning: [^:]*: line 24, column 18: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 25, column 2: identifier "exit" not declared */
