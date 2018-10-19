extern int ptbl[4];
extern int ctbl[4];

void doViews(void) {
    int  *c = ctbl, *p = ptbl;
    while (1)
  {
        p++;
        c++;
        if (*p)
        {
            if (c == p)
            {
                if (*c)
                    return;
            }
            else
              return;
        }
    }

    g();
}

/* cp-out: warning: [^:]*: line 22, columns 4-5: identifier "g" not declared */
