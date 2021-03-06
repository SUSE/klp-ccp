/* Fold used to create a GT_EXPR of pointer vs. integer types,
   which caused us to ICE in VRP.  */

/* { dg-do compile } */
/* { dg-options "-Os -w" } */

unsigned int dsdblm_GetBlockAddress();
void error_LocalAssert(void);
int dsdblm_CreateBlock(unsigned int address)
{
   address = dsdblm_GetBlockAddress();
   if (address >= (void*)0x00020000)
     error_LocalAssert();
   return address;
}

/* cp-out: warning: [^:]*: line 12, column 7: pointer comparison against non-NULL integer */
