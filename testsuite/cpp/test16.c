#define __a(b) #b
#define a(b) __a(b)

#define __c(d,e) d ## e
#define c(d,e) __c(d,e)

#define f g h

#define h

a(c(f,i))
