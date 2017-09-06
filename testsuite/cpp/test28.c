#define a(b,c,d,e,f) f

#define call(id,...) id(__VA_ARGS__)
#define e e,f
call(a, b, c, d, e)

