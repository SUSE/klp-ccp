/* GCC < 6.1 doesn't support the x86 named address spaces: __seg_gs
   and __seg_fs are ordinary identifiers and __SEG_GS/__SEG_FS are not
   predefined. */
#ifdef __SEG_GS
#error "__SEG_GS unexpectedly defined"
#endif
#ifdef __SEG_FS
#error "__SEG_FS unexpectedly defined"
#endif

int __seg_gs;
int __seg_fs(int __seg_gs);

int f(void)
{
	return __seg_fs(__seg_gs);
}
