#define A
#define B
#define C

#define __str(a) #a
#define str(a) __str(|a|)

void pu_f(void)
{
	str(A B C);
}
