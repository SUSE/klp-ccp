typedef void (*entry_func) (void) __attribute__ ((noreturn));
extern entry_func entry_addr;
static void bsd_boot_entry (void)
{
  stop ();
}   
void bsd_boot (void)
{
  entry_addr = (entry_func) bsd_boot_entry;
  (*entry_addr) ();
}

/* cp-out: warning: [^:]*: line 5, column 2: identifier "stop" not declared */
