#define __stringify(a) #a
#define stringify(...) __stringify(__VA_ARGS__)
#define c d

__stringify(c)
stringify(c)
