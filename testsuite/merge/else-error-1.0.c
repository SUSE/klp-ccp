#if __has_attribute(ms_abi)
	r += 16;
#else
#error "klp-ccp: a preceeding branch should have been taken"
#endif
