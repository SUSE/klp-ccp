struct vc_data {
	unsigned long	space;
	unsigned char   vc_palette[16*3];        
};

struct vc {
	struct vc_data *d;
};

struct vc_data a_con;
struct vc vc_cons[63] = { &a_con };
int default_red[16];
int default_grn[16];
int default_blu[16];

extern void bar(int);

void reset_palette(int currcons)
{
	int j, k;
	for (j=k=0; j<16; j++) {
		(vc_cons[currcons].d->vc_palette) [k++] = default_red[j];
		(vc_cons[currcons].d->vc_palette) [k++] = default_grn[j];
		(vc_cons[currcons].d->vc_palette) [k++] = default_blu[j];
	}
	bar(k);
}

void bar(int k)
{
	if (k != 16*3)
		abort();
}

int main()
{
	reset_palette(0);
	exit(0);
}

/* cp-out: warning: [^:]*: line 32, columns 2-7: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 38, columns 1-5: identifier "exit" not declared */
