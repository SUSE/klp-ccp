#define __concat(a, b) a ## b
#define concat(c,d) __concat(c,d)
#define e f

__concat(g,e)
concat(g,e)
