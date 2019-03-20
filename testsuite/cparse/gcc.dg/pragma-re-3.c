
#pragma redefine_extname f1 f
#pragma redefine_extname g1 g

void f() {
  extern int f1();
  f1();
}

void g() {
  g1();
}

int main () {
  f();
  g();
}

/* cp-out: warning: [^:]*: line 11, column 2: identifier "g1" not declared */
