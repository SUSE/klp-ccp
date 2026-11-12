/*
 * __has_attribute() knows about the aarch64 specific attributes, but
 * not about those of other architectures.
 */
int pu_f(void)
{
	int r = 0;
#if __has_attribute(unused)
	r += 1;
#endif
#if __has_attribute(aarch64_vector_pcs)
	r += 2;
#endif
#if __has_attribute(__aarch64_vector_pcs__)
	r += 4;
#endif
#if __has_attribute(ms_abi)
	r += 8;
#endif
#if __has_attribute(arm_sve_vector_bits)
	r += 16;
#endif
	return r;
}
