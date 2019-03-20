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

/* cp-out: warning: [^:]*: line 4, column 2: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 5, column 6: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 5, column 12: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 6, column 2: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 6, column 7: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 12, column 2: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 13, column 6: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 13, column 13: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 14, column 2: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 14, column 8: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 16, column 2: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 17, column 6: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 17, column 13: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 18, column 2: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 18, column 8: dereferencing pointer to incomplete type */
