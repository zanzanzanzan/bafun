#include<delay.h>

void delay(int j)
{
unsigned char i;
for(;j>1;j--)
   for(i=100;i>1;i--);

}
//增减注释
void delayms(int j)
{
unsigned char i;
for(;j>1;j--)
   for(i=2;i>1;i--);

}