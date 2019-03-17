 /**************************************************************************
 作  者 ：大鱼电子
淘宝地址：https://shop119207236.taobao.com
**************************************************************************/
#include "sys.h"
//////////////////////////全局变量的定义////////////////////////////////////  
float pitch,roll,yaw; 								  			 //欧拉角(姿态角)
short aacx,aacy,aacz;													 //加速度传感器原始数据
short gyrox,gyroy,gyroz;											 //陀螺仪原始数据
float temp; 								  								 //温度


short dateProcess(short A)
{
short B;
if (A > (0xffff /2))
{
 B = A -(0xffff /2);

}
else{
	B = A;

}


return B /169;
}

short dateProcessG(short A)
{
short B;
if (A > (0xffff /2))
{
 B = A -(0xffff /2);

}
else{
	B = A;

}


return B /16;
}


////////////////////////////////////////////////////////////////////////////
int main(void)	
{ 
	
	int i ;
	delay_init();	    	           //=====延时函数初始化	
	NVIC_Configuration();					 //=====中断优先级分组,其中包含了所有的中断优先级的配置,方便管理和一次性修改。
	LED_Init();                    //=====初始化与 LED 连接的IO
	KEY_Init();                    //=====按键初始化
	uart1_init(9600);	             //=====串口1初始化
	TIM3_Init(99,7199);	    		   //=====定时器初始化 100ms中断一次
//		OLED_Init();                   //=====OLED初始化
//	OLED_Clear();									 //=====OLED清屏

	MPU_Init();					    			 //=====初始化MPU6050
	mpu_dmp_init();								 //=====初始化MPU6050的DMP模式	

	OLED_ShowString(0,0,"Pitch:",12);
	OLED_ShowString(0,2,"Roll :",12);
	OLED_ShowString(0,4,"Yaw  :",12);
	OLED_ShowString(0,6,"Temp :",12);
	
  while(1)	
	/*{	
		printf("pitch=%.2f,roll=%.2f,yaw=%.2f,temp=%.2f\n",pitch,roll,yaw,temp/100);
	}	*/	
	{
		/*方便我们查看数据的变化,查看姿态角;如果需要看加速度以及角速度可自行修改***/
		mpu_dmp_get_data(&pitch,&roll,&yaw);			//得到姿态角即欧拉角
		//temp=MPU_Get_Temperature();								//得到温度值
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据

		aacx = dateProcess (aacx);
		aacy = dateProcess (aacy);
		aacz = dateProcess (aacz);

		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据	
		gyrox = dateProcessG(gyrox);
		gyroy = dateProcessG(gyroy);
		gyroz = dateProcessG(gyroz);
		
		
		OLED_Float(0,48,pitch,2);									//显示俯仰角
		OLED_Float(2,48,roll,2);									//显示翻滚角
		OLED_Float(4,48,yaw,2);										//显示航向角
		OLED_Float(6,48,temp/100,2);							//显示温度
		i++;
		if (i ==120)
		{
			i =0;
		  //printf("pitch=%.2f,roll=%.2f,yaw=%.2f\n",pitch,roll,yaw);
			//printf("gyrox=%d,gyroy=%d,gyroz=%d\n",gyrox,gyroy,gyroz);
			printf("aacx=%d,aacy=%d,aacz=%d\n",aacx,aacy,aacz);
		}
		/*也可以使用电脑上位机查看,如果没有购买OLED屏幕的客户打开电脑上位机,使用串口打印到上位机即可查看***/
	} 	
}














