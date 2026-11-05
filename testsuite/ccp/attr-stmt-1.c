/*
 * Attribute statements, i.e. null statements with attributes like
 * __attribute__((fallthrough));, are valid wherever a statement is,
 * in particular right after a case label, where a declaration would
 * not be allowed.
 */
#define fallthrough __attribute__((__fallthrough__))

int pu_f(int x)
{
	int r = 0;

	switch (x) {
	case 0:
		__attribute__((__fallthrough__));
	case 1:
		fallthrough;
	default:
		r++;
		fallthrough;
	case 2:
		if (r > 1)
			return r;
		else
			__attribute__((__fallthrough__));
	case 3:
		r++;
		__attribute__(()) __attribute__((__fallthrough__));
	case 4:
		break;
	}

	return r;
}
