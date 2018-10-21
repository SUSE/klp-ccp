/* This test used to ICE on IA64.  */
/* { dg-require-effective-target indirect_jumps } */
/* { dg-require-effective-target label_values } */

long stack[100];
int main(int argc,char**argv,char **envp)
{
  long *esp=stack;
  static void* jarray[]={ &&KeyCtrlKV };
 *++esp=(long)&&_loc0;
 goto SetTermStruc;
 _loc0:;
 *++esp=(long)&&_loc1;
 _loc1:;
*++esp=(long)&&_loc35;
 _loc35:;
goto *(*esp--);
*++esp=(long)&&_loc36;
 _loc36:;
*++esp=(long)&&_loc37;
 _loc37:;
KeyCtrlKV:
*++esp=(long)&&_loc66;
_loc66:;
*++esp=(long)&&_loc106;
 _loc106:;
*++esp=(long)&&_loc119;
 _loc119:;
SetTermStruc:
 goto *(*esp--);
}

/* cp-out: warning: [^:]*: line 17, columns 6-7: dereferencing integer at computed goto */
/* cp-out: warning: [^:]*: line 30, columns 7-8: dereferencing integer at computed goto */
