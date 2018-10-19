int lwidth;                                                                   
int lheight;                                                                  
void ConvertFor3dDriver (int requirePO2, int maxAspect)       
{                                                     
  int oldw = lwidth, oldh = lheight;                      

  lheight = FindNearestPowerOf2 (lheight);            
  while (lwidth/lheight > maxAspect) lheight += lheight;              
}                                                                         

/* cp-out: warning: [^:]*: line 7, columns 12-31: identifier "FindNearestPowerOf2" not declared */
