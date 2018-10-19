char a[] = "12345";
char b[] = "12345";

void loop (char * pz, char * pzDta)
{
    for (;;) {
        switch (*(pz++) = *(pzDta++)) {
        case 0:
  	    goto loopDone2;

	case '"':
	case '\\':
	    pz[-1]  = '\\';
            *(pz++) = pzDta[-1];
	}
    } loopDone2:;

  if (a - pz != b - pzDta)
    abort ();
}

main()
{
  loop (a, b);
  exit (0);
}

/* cp-out: warning: [^:]*: line 19, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 25, columns 2-6: identifier "exit" not declared */
