#define LOG(fmt, ...) printk(fmt, espr_i, ##__VA_ARGS__)

int printk(const char *fmt, ...);
int espr_i;

void pu_f(void)
{
	LOG("x %d", espr_i);
	LOG("y");
}
/* cp-out: warning: [^:]*: line 8, columns 1-20: function "printk" without definition cannot get externalized */
/* cp-out: warning: [^:]*: line 9, columns 1-9: function "printk" without definition cannot get externalized */
