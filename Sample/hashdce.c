#include<stdio.h>

int main()
{
  int a,b,c,d,e,f,g,h,i,j,k,x;
  scanf("%d",&a);
  scanf("%d",&x);
  b=a;
  c=b;
  d=c;
  e=d;
  f=e;
  g=e+10+d;
  h=10+a+b+f;
  
  if(x)
  {
    c=a;
    d=b;
    i=a+b+d;
    j=a+10+d;
    c=10;
    b=20;
  }
  else
  {
    k=a+10+25;
    j=a+10+d;
    c=e;
    d=c;
    k=c;
    b=10;
    c=20;
  }
  printf("%d",j);

  return 0;
}




