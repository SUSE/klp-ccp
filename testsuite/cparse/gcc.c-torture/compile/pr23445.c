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

/* cp-out: warning: [^:]*: line 8, columns 22-30: identifier "__brelse" not declared */
