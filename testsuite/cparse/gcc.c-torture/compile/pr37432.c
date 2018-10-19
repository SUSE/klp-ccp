static void print_wkb_byte(unsigned char val) {
    print_wkb_bytes((unsigned char *)&val, 1, 1);
}
void InsertMultiPoint(int b) {
    char a = 1;
    if (b) a = 0;
    print_wkb_byte(a);
}

/* cp-out: warning: [^:]*: line 2, columns 4-19: identifier "print_wkb_bytes" not declared */
