void
dr106_1(void *pv, int i)
{
	*pv;
	i ? *pv : *pv;
	*pv, *pv;
}

void
dr106_2(const void *pcv, volatile void *pvv, int i)
{
	*pcv;
	i ? *pcv : *pcv;
	*pcv, *pcv;

	*pvv;
	i ? *pvv : *pvv;
	*pvv, *pvv;
}

/* cp-out: warning: [^:]*: line 4, columns 2-4: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 5, columns 6-8: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 5, columns 12-14: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 6, columns 2-4: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 6, columns 7-9: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 12, columns 2-5: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 13, columns 6-9: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 13, columns 13-16: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 14, columns 2-5: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 14, columns 8-11: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 16, columns 2-5: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 17, columns 6-9: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 17, columns 13-16: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 18, columns 2-5: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 18, columns 8-11: dereferencing pointer to incomplete type */
