/*
 * An insertion inbetween two argument references: the array length
 * inserted before the ']' sits between the tail of 'dims' and the
 * head of 'rest'. Both parameters are referenced only once, so
 * either side would be feasible and the insertion's stickiness
 * decides: it goes to the tail of 'dims'. The original definition of
 * ARR gets reused.
 */
#define ARR(name, dims, rest) char name dims rest

ARR(espr_o, [, ] = "123");

void pu_f(void)
{
	char s[sizeof(espr_o)];
}
