 struct buffer_head {
   char *b_data;
 };
   void asfs_deletebnode( struct buffer_head *bhsec)  {
     if (bhsec == 0)   {
     void *bnc2 = (void *) bhsec->b_data;
     if (bnc2)       return;
     if (bhsec)       __brelse(bhsec);
   }
 }

/* cp-out: warning: [^:]*: line 8, column 22: identifier "__brelse" not declared */
