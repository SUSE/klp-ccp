void g();

f()
{
int x=1;
while(x)
{
x=h();
if(x)
g();
}
}

/* cp-out: warning: [^:]*: line 8, column 2: identifier "h" not declared */
