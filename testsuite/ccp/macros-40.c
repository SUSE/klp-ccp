#define WHERE espr_i, __LINE__

int espr_i;

void pu_f(void)
{
	int x = (WHERE);
}
