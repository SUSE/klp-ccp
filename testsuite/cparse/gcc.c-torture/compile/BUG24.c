struct ack {
    char a, b, c;
};

main()
{
   struct ack bad;

   foo(bad);
}

foo(c)
   struct ack c;
{
}

/* cp-out: warning: [^:]*: line 9, columns 3-6: identifier "foo" not declared */
