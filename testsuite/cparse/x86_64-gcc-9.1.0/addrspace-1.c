/* The x86 named address space qualifiers __seg_gs and __seg_fs, as
   supported by GCC >= 6.1, in the various type qualifier positions. */
#ifndef __SEG_GS
#error "__SEG_GS not defined"
#endif
#ifndef __SEG_FS
#error "__SEG_FS not defined"
#endif

/* Declaration specifiers. */
extern __seg_gs int gs_obj;
__seg_fs const int fs_const_obj;
int __seg_gs volatile gs_volatile_obj;

/* Pointers to address space qualified objects and address space
   qualified pointers. */
__seg_gs int *p_gs_int;
int __seg_gs *p_gs_int2;
int * __seg_gs gs_p_int;
__seg_gs int * __seg_fs fs_p_gs_int;

/* Typedefs and typeof. */
typedef __seg_gs int gs_int;
gs_int *p_gs_int3;
const gs_int *p_const_gs_int;
typeof(gs_obj) *p_gs_int4;
__seg_gs typeof(gs_obj) *p_gs_int5;

/* Arrays, struct members and function parameters. */
__seg_gs int gs_arr[4];
struct s {
	__seg_gs int *m;
	int __seg_fs *n;
};
void f(__seg_gs int *a, int __seg_fs b[static __seg_gs 2]);

/* Compatible redeclarations. */
extern __seg_gs int gs_obj;
extern gs_int gs_obj;
extern __seg_gs int *p_gs_int;

int g(__seg_gs int *a, __seg_fs int *b)
{
	__seg_gs int *c = a;
	__seg_gs int * const d = c;
	const __seg_gs int *e = a;
	/* Explicit conversions between address spaces are permitted on
	   x86, recent GCC versions warn about them though. */
	int *flat = (int *)a;
	__seg_fs int *h = (__seg_fs int *)flat;
	return *c + *d + *e + *flat + *h + *b + gs_arr[1] + *p_gs_int3;
}
