typedef struct {
 long l[5];
} t;

f(size)
{
 t event;
 g(&(event.l[2 + size]), (3 - size) * 4);
}

/* cp-out: warning: [^:]*: line 8, columns 1-2: identifier "g" not declared */
