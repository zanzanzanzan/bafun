 /**************************************************************************
 ��  �� ���������
�Ա���ַ��https://shop119207236.taobao.com
**************************************************************************/
#include "sys.h"
//////////////////////////ȫ�ֱ����Ķ���////////////////////////////////////  
float pitch,roll,yaw; 								  			 //ŷ����(��̬��)
short aacx,aacy,aacz;													 //���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;											 //������ԭʼ����
float temp; 								  								 //�¶�


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
	delay_init();	    	           //=====��ʱ������ʼ��	
	NVIC_Configuration();					 //=====�ж����ȼ�����,���а��������е��ж����ȼ�������,��������һ�����޸ġ�
	LED_Init();                    //=====��ʼ���� LED ���ӵ�IO
	KEY_Init();                    //=====������ʼ��
	uart1_init(9600);	             //=====����1��ʼ��
	TIM3_Init(99,7199);	    		   //=====��ʱ����ʼ�� 100ms�ж�һ��
//		OLED_Init();                   //=====OLED��ʼ��
//	OLED_Clear();									 //=====OLED����

	MPU_Init();					    			 //=====��ʼ��MPU6050
	mpu_dmp_init();								 //=====��ʼ��MPU6050��DMPģʽ	

	OLED_ShowString(0,0,"Pitch:",12);
	OLED_ShowString(0,2,"Roll :",12);
	OLED_ShowString(0,4,"Yaw  :",12);
	OLED_ShowString(0,6,"Temp :",12);
	
  while(1)	
	/*{	
		printf("pitch=%.2f,roll=%.2f,yaw=%.2f,temp=%.2f\n",pitch,roll,yaw,temp/100);
	}	*/	
	{
		/*�������ǲ鿴���ݵı仯,�鿴��̬��;�����Ҫ�����ٶ��Լ����ٶȿ������޸�***/
		mpu_dmp_get_data(&pitch,&roll,&yaw);			//�õ���̬�Ǽ�ŷ����
		//temp=MPU_Get_Temperature();								//�õ��¶�ֵ
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������

		aacx = dateProcess (aacx);
		aacy = dateProcess (aacy);
		aacz = dateProcess (aacz);

		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������	
		gyrox = dateProcessG(gyrox);
		gyroy = dateProcessG(gyroy);
		gyroz = dateProcessG(gyroz);
		
		
		OLED_Float(0,48,pitch,2);									//��ʾ������
		OLED_Float(2,48,roll,2);									//��ʾ������
		OLED_Float(4,48,yaw,2);										//��ʾ�����
		OLED_Float(6,48,temp/100,2);							//��ʾ�¶�
		i++;
		if (i ==120)
		{
			i =0;
		  //printf("pitch=%.2f,roll=%.2f,yaw=%.2f\n",pitch,roll,yaw);
			//printf("gyrox=%d,gyroy=%d,gyroz=%d\n",gyrox,gyroy,gyroz);
			printf("aacx=%d,aacy=%d,aacz=%d\n",aacx,aacy,aacz);
		}
		/*Ҳ����ʹ�õ�����λ���鿴,���û�й���OLED��Ļ�Ŀͻ��򿪵�����λ��,ʹ�ô��ڴ�ӡ����λ�����ɲ鿴***/
	} 	
}














