{
	unsigned char i;
	UCHAR4 temp;
	unsigned short m_vel,m_material_value_temp;
	//

	if (set_date >= 90)
	{
		set_date =0;
	}


	set_date++;
	switch (set_date)
	{
		case 
		
		
	}
   
	if ( set_date % 6 =0 )
	//if ( 1)
	{
	
		{
			uart_tx_buf[0] = 0xaa; 

			//加速温度显示
			if (set_date/2%3 ==0)
			{
				uart_tx_buf[1] = ADDR_XSB_5;
				
				uart_tx_buf[2] = temp_data[2];	//936
				uart_tx_buf[3] = temp_data[3]; 


				uart_tx_buf[4] = temp_data[4];	//937
				uart_tx_buf[5] = temp_data[5];	

				uart_tx_buf[6] = temp_data[6];	//938				
				uart_tx_buf[7] = temp_data[7]; 


				uart_tx_buf[8] = 0; 
			}else if (set_date/2%3 ==1)
			{
				uart_tx_buf[1] = ADDR_XSB_6;
				
				uart_tx_buf[2] = temp_data[8];	 //939
				uart_tx_buf[3] = temp_data[9]; 


				uart_tx_buf[4] = temp_data[10];  //940
				uart_tx_buf[5] = temp_data[11];  

				uart_tx_buf[6] = temp_data[12];  //941 
				uart_tx_buf[7] = temp_data[13]; 


				uart_tx_buf[8] = 0; 
			}else
			{
				uart_tx_buf[1] = ADDR_XSB_7;
				
				uart_tx_buf[2] = temp_data[14];   //942
				uart_tx_buf[3] = temp_data[15]; 


				uart_tx_buf[4] = temp_data[16];  //943
				uart_tx_buf[5] = temp_data[17];  

				uart_tx_buf[6] = temp_data[18];  //944 
				uart_tx_buf[7] = temp_data[19]; 


				uart_tx_buf[8] = 0; 

			}
			
		}
		
		uart_tx_buf[9] = 0;
		for(i=0; i<9; i++)
		{
			uart_tx_buf[9] += uart_tx_buf[i];//sum
		}

		//开始发送数据
		b485en = 1;//发送使能
		delay_us(100);
		UDR0 = uart_tx_buf[0];
		uart_tx_addr = 1;
	}
	else //7if (set_date > 1)
	{
		canshu_peizhi++;

		uart_tx_buf[0] = 0xaa; 

		switch(xianshi_qiehua % 6)
		{
			case 0: 
			{
				uart_tx_buf[1] = ADDR_XSB_1;
				
				uart_tx_buf[2] = m_rPara.para.max_vel / 256;  //600
				uart_tx_buf[3] = m_rPara.para.max_vel % 256;


				uart_tx_buf[4] = m_rPara.para.min_vel / 256;  //601
				uart_tx_buf[5] = m_rPara.para.min_vel  % 256;

				uart_tx_buf[6] = m_rPara.para.dw_level_pulse;	//620
				
				uart_tx_buf[7] = m_rPara.para.rollin_delay;  //602


				uart_tx_buf[8] = m_rPara.para.material_full; //603	   no use
				break;
			}
			case 1:
			{
				uart_tx_buf[1] = ADDR_XSB_2_2;
				
				uart_tx_buf[2] = m_rPara.para.is_remote;	 //604

				uart_tx_buf[3] = m_rPara.para.mmotor_cur1; //605

				uart_tx_buf[4] = m_rPara.para.mm_cur_overflow; //606

				uart_tx_buf[5] = m_rPara.para.mm_control;	 //607

				uart_tx_buf[6] = m_rPara.para.cur1_full;	 //608
				
				/*
				uart_tx_buf[15] = m_rPara.para.material_port; //609    no use

				uart_tx_buf[16] = m_rPara.para.manual_vel / 256;//610  no use
				uart_tx_buf[17] = m_rPara.para.manual_vel  % 256;

				uart_tx_buf[18] = m_rPara.para.limited_hours / 256; //611  no use
				uart_tx_buf[7] = m_rPara.para.limited_hours  % 256;

				uart_tx_buf[20] = m_rPara.para.all_stop_id; 		//612	no use
				*/

				uart_tx_buf[7] = m_rPara.para.material_start;	  //613
				
				uart_tx_buf[8] = m_rPara.para.material_stop;	//614
				 break;

			}
			case 2:
			{

				uart_tx_buf[1] = ADDR_XSB_3;
				
				uart_tx_buf[2] = m_rPara.para.current1_modify;	 //615

				/*
				uart_tx_buf[24] = m_rPara.para.para_seted_flag / 256; //616   no use
				uart_tx_buf[25] = m_rPara.para.para_seted_flag	% 256;

				uart_tx_buf[26] = m_rPara.para.addr_modbus;   //617   no use


				uart_tx_buf[27] = m_rPara.para.baud_modbus;   //618   no use
				*/
				uart_tx_buf[3] = m_rPara.para.mb_level_pulse;  //619  



				uart_tx_buf[4] = m_rPara.para.mb_div;	//621

				uart_tx_buf[5] = m_rPara.para.current2_modify; //622

				/*
				uart_tx_buf[6] = m_rPara.para.used_hours / 256;  //623	 no use
				uart_tx_buf[7] = m_rPara.para.used_hours  % 256;	
				*/
				uart_tx_buf[6] = m_rPara.para.mmotor_cur2;	//949


				uart_tx_buf[7] = m_rPara.para.cur2_full;   //625	
				uart_tx_buf[8] = m_rPara.para.material_full;
				 break;

			}

			case 3: 
			{
				uart_tx_buf[1] = ADDR_XSB_4;
				
				uart_tx_buf[2] = m_rPara.para.w5500_net_para.ip_para.ip_addr[0];  //630
				uart_tx_buf[3] = m_rPara.para.w5500_net_para.ip_para.ip_addr[1];  //631


				uart_tx_buf[4] = m_rPara.para.w5500_net_para.ip_para.ip_addr[2];  //632
				uart_tx_buf[5] = m_rPara.para.w5500_net_para.ip_para.ip_addr[3];  //633

				uart_tx_buf[6] = m_rPara.para.addr_modbus;	 //634
				
				uart_tx_buf[7] = temp_data[0];	//935


				uart_tx_buf[8] = temp_data[1]; //	 no use
				break;
			}
			default :
			{
				{

			//
			//控制位转换为显示位
			/*
			bit_led_auto = bit_auto;//自动
			bit_led_manual = bit_manual;//手动
			bit_led_mmotor = bit_mmotor;//主电机启动
			bit_led_remote = bit_remote;//远控
			//bit_led_over_i = bit_over_i|mb_is_invalid|bit_qr_ql_error|bit_comm_error;//告警
			bit_led_over_i = bit_over_i;//告警
			//
			bit_led_cssd = bit_setmode|bit_monitor_mode|bit_alarm_mode;//参数设定，监控模式，告警模式
			bit_led_xl = bit_xieliao;//卸料
			bit_led_dw = bit_l_mtl;
			bit_led_gw = bit_h_mtl;
			bit_led_up = bit_current_1st;
			bit_led_down = bit_current_2nd;
			bit_led_dl = bit_duliao;
			//
			*/
			if(1==bit_xieliao)
			{
				m_vel=m_rPara.para.max_vel>>1;
			}
			else if(0==bit_fmotor)
			{
				m_vel=0;
			}
			else if(1==bit_auto)
			{
				m_vel=auto_vel;
			}
			else if(1==bit_manual)
			{
				m_vel=m_rPara.para.manual_vel;
			}
			else
			{
				m_vel=0;
			}	
			//
			uart_tx_buf[0] = 0xaa;

			if (set_date/2%2)
			{
			
				uart_tx_buf[1] = ADDR_XSB;

				if (bit_mmotor == 0)
				{
					uart_tx_buf[5] =0;
					uart_tx_buf[6] = 0;
				}
				else
				{
					uart_tx_buf[5] = m_cur_cal >> 8;
					uart_tx_buf[6] = m_cur_cal	& 0xff;

				}
			
			}
			else
			{

#if __D2_USED__  == TRUE
				uart_tx_buf[1] = ADDR_XSB_8;

				if (bit_mmotor == 0)
				{
					uart_tx_buf[5] =0;
					uart_tx_buf[6] = 0;
				}
				else
				{
					uart_tx_buf[5] = m_cur_cal2 >> 8;
					uart_tx_buf[6] = m_cur_cal2  & 0xff;

				}
#else
				uart_tx_buf[1] = ADDR_XSB;

				if (bit_mmotor == 0)
				{
					uart_tx_buf[5] =0;
					uart_tx_buf[6] = 0;
				}
				else
				{
					uart_tx_buf[5] = m_cur_cal >> 8;
					uart_tx_buf[6] = m_cur_cal	& 0xff;

				}
												
#endif
			}

			uart_tx_buf[2] =	bit_mmotor << 7 |		//磨辊
								bit_auto << 6 | 		//自动
								bit_manual << 5 |		//手动
								bit_fmotor << 4 |		//喂料辊
								li_he_zha << 3 |		//合闸
								bit_remote<< 2 |		//远控
								bit_duliao << 1 |		//堵料	  
								bit_lock << 0;			//解锁




			

	//m_vel = 100;
			uart_tx_buf[3] = m_vel >> 8;
			uart_tx_buf[4] = m_vel	& 0xff;

	//m_cur_cal = 88;


			m_material_value_temp = m_material_value>> 4;



			uart_tx_buf[7] = m_material_value_temp >> 8;
			uart_tx_buf[8] = m_material_value_temp &0xff;



			//磨辊

			//自动

			if (bit_auto ==1)
			{

				if (mb_is_invalid == 1 && auto_vel > 0)
				{
					uart_tx_buf[1] |= 1 << 6;
				}
			}

			//手动

			if (bit_manual ==1)
			{

				if (mb_is_invalid == 1)
				{
					uart_tx_buf[1] |= 1 << 7;
				}
			}


		}
			}

		}
		xianshi_qiehua ++;



		uart_tx_buf[9] = 0;
		for(i=0; i<9; i++)
		{
			uart_tx_buf[9] += uart_tx_buf[i];//sum
		}
		
		b485en = 1;//发送使能
		delay_us(100);
		UDR0 = uart_tx_buf[0];
		uart_tx_addr = 1;
	}
	/*else
	{

		if (temp1_flag_count > 50 )
		{



			temp1_flag_count  = 0;
			uart_tx_buf[0] = 0x55;
			uart_tx_buf[1] = 'A';
			uart_tx_buf[2] = 'A';
			uart_tx_buf[3] = 'A';
			uart_tx_buf[4] = 'A';

			send_uart1_to_computer(uart_tx_buf,5);

		}
	}*/

/*
	TRACE_STRING("usart  =");
	
	send_ulong_ascii(set_date ,TRUE);
	if (set_date < 16)
	{
	for(i=0; i<7; i++)
		{
	send_ulong_ascii(uart_tx_buf[i] ,TRUE);
		}
		}
*/

}