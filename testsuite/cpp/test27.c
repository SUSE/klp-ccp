#define __a(b) #b
#define a(b) __a(b)
#define c(d) .d

a(c( e))
c( e)
