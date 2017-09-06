# define a(b,c) c
#define d(...) a(__VA_ARGS__)

d(e, f)
