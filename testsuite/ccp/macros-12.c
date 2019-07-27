#define STATIC_VOID static void
#define EXTERN extern
#define VOID void

#define G() espr_g

#define A()
#define B()

EXTERN VOID G()(void)
{}

void pu_f(void)
{
	A()G()B()();
}
