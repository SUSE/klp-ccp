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

/* cp-out: warning: [^:]*: line 19, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 25, column 2: identifier "exit" not declared */
