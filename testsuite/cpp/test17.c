#define __a(b) #b
#define a(b) __a(b)
#define c d e
#define e
a(c)
