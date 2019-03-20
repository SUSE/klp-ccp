unsigned char foo(unsigned long);
main()
{
    unsigned char AChar;
    unsigned long ALong = 0x12345678;

    AChar = foo(ALong);

    printf("AChar = %x\n",(int)AChar);
}
unsigned char
foo( unsigned long TheLong)
{
     return( (unsigned char) (TheLong & 0xff) );
}

/* cp-out: warning: [^:]*: line 9, column 4: identifier "printf" not declared */
