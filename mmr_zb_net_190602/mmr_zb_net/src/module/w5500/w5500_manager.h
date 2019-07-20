
#ifndef __W5500_MANAGER_H__
#define __W5500_MANAGER_H__



#if USED_W5500 == TRUE 

Bool init_w5500(U32 reset_ms);
extern void w5500_task(void);
 void w5500_read_para(void);
//void w5500_read_para(void);



#endif




#endif

