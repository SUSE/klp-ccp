/*
 * Like macros-69.c, but the second parameter's argument expands to
 * nothing and its only reference is consumed by SPARSE, which doesn't
 * reference the corresponding parameter either: with the placeholder
 * empty token trimmed from the argument's source, the reference is a
 * "don't care" next to the argument's (empty) range. The __rcu
 * argument stays as is and doesn't get expanded.
 */
#define SPARSE(p, space)
#define __rcu
#define CHECK(p, space) ({ typeof(*p) *q__ = p; SPARSE(p, space); q__; })

int *espr_a;

int pu_f(void)
{
	return *CHECK(espr_a, __rcu);
}
