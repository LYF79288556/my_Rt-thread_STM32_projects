/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 �̹߳���
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 

/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"
#include <string.h>


/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* �����߳̿��ƿ� */
static rt_thread_t led1_thread = RT_NULL; //led�߳�
static rt_thread_t key_thread = RT_NULL;  //�����߳�
static rt_thread_t usart2_thread = RT_NULL;//����2�߳�
static rt_thread_t usart3_thread = RT_NULL;//����3�߳�

/* �����ź������ƿ� */
rt_sem_t test_sem2 = RT_NULL;
rt_sem_t test_sem3 = RT_NULL;

/************************* ȫ�ֱ������� ****************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */

/* ��غ궨�� */
extern char Usart2_Rx_Buf[USART2_RBUFF_SIZE];
extern char Usart3_Rx_Buf[USART3_RBUFF_SIZE];

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
static void key_thread_entry(void* parameter);
static void usart2_thread_entry(void* parameter);
static void usart3_thread_entry(void* parameter);
/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
    /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
  rt_kprintf("����һ��[Ұ��]-STM32F103�Ե�-RTT�̹߳���ʵ�飡\n\n");
  rt_kprintf("����K1�����̣߳�����K2�ָ��߳�\n");
//*******************************************************************************//	
 key_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "key",              /* �߳����� */
                      key_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      2,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (key_thread != RT_NULL)
        rt_thread_startup(key_thread);
    else
        return -1;
//*******************************************************************************//	
		led1_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "led1",              /* �߳����� */
                      led1_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (led1_thread != RT_NULL)
        rt_thread_startup(led1_thread);
    else
        return -1;
//*******************************************************************************//	
		
		/* ����һ���ź��� */
	test_sem2 = rt_sem_create("test_sem2",/* ��Ϣ�������� */
                     0,     /* �ź�����ʼֵ��Ĭ����һ���ź��� */
                     RT_IPC_FLAG_FIFO); /* �ź���ģʽ FIFO(0x00)*/
  if (test_sem2 != RT_NULL)
    rt_kprintf("�ź��������ɹ���\n\n");
    
  usart2_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "usart2",              /* �߳����� */
                      usart2_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      4,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (usart2_thread != RT_NULL)
        rt_thread_startup(usart2_thread);
    else
        return -1;
//*******************************************************************************//			
			/* ����һ���ź��� */
	test_sem3 = rt_sem_create("test_sem3",/* ��Ϣ�������� */
                     0,     /* �ź�����ʼֵ��Ĭ����һ���ź��� */
                     RT_IPC_FLAG_FIFO); /* �ź���ģʽ FIFO(0x00)*/
  if (test_sem3 != RT_NULL)
    rt_kprintf("�ź��������ɹ���\n\n");
    
  usart3_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "usart3",              /* �߳����� */
                      usart3_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      5,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (usart3_thread != RT_NULL)
        rt_thread_startup(usart3_thread);
    else
        return -1;	
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void led1_thread_entry(void* parameter)
{	
  
    while (1)
    {
        LED1_ON;
        rt_thread_delay(500);   /* ��ʱ500��tick */
       // rt_kprintf("led1_thread running,LED1_ON\r\n");
        
        LED1_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
        //rt_kprintf("led1_thread running,LED1_OFF\r\n");
    }
}
//*******************************************************************************//	
static void key_thread_entry(void* parameter)
{	
  rt_err_t uwRet = RT_EOK;	
    while (1)
    {
      if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )/* K1 ������ */
      {
        rt_kprintf("����LED1�̣߳�\n");
        uwRet = rt_thread_suspend(led1_thread);/* ����LED1�߳� */
        if(RT_EOK == uwRet)
        {
          rt_kprintf("����LED1�̳߳ɹ���\n");
        }
        else
        {
          rt_kprintf("����LED1�߳�ʧ�ܣ�ʧ�ܴ��룺0x%lx\n",uwRet);
        }
      } 
      if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )/* K1 ������ */
      {
        rt_kprintf("�ָ�LED1�̣߳�\n");
        uwRet = rt_thread_resume(led1_thread);/* �ָ�LED1�̣߳� */
        if(RT_EOK == uwRet)
        {
          rt_kprintf("�ָ�LED1�̳߳ɹ���\n");
        }
        else
        {
          rt_kprintf("�ָ�LED1�߳�ʧ�ܣ�ʧ�ܴ��룺0x%lx\n",uwRet);
        }
      }
      rt_thread_delay(20);
    }
}

//*******************************************************************************//	
static void usart2_thread_entry(void* parameter)
{
  rt_err_t uwRet = RT_EOK;	
    /* ������һ������ѭ�������ܷ��� */
	Usart2_SendString(USART2,"usart2 runing");
  while (1)
  {
		
		uwRet = rt_sem_take(test_sem2,	/* ��ȡ�����жϵ��ź��� */
                        0); 	  /* �ȴ�ʱ�䣺0 */
		
	 NBiot_SendCmd("AT", "OK", 200);
		rt_thread_delay(2000);
    if(RT_EOK == uwRet)
    {
			Usart2_SendString(USART2,"�յ�����");
			Usart2_SendString(USART2,Usart2_Rx_Buf);
      memset(Usart2_Rx_Buf,0,USART2_RBUFF_SIZE);/* ���� */
    }
  }
}
//*******************************************************************************//	
static void usart3_thread_entry(void* parameter)
{
  rt_err_t uwRet = RT_EOK;	
    /* ������һ������ѭ�������ܷ��� */
	Usart2_SendString(USART3,"usart2 runing");
  while (1)
  {
		
		uwRet = rt_sem_take(test_sem3,	/* ��ȡ�����жϵ��ź��� */
                        0); 	  /* �ȴ�ʱ�䣺0 */
		rt_thread_delay(500);
    if(RT_EOK == uwRet)
    {
			Usart3_SendString(USART3,"�յ�����");
			Usart3_SendString(USART3,Usart3_Rx_Buf);
      memset(Usart3_Rx_Buf,0,USART3_RBUFF_SIZE);/* ���� */
    }
  }
}
//*******************************************************************************//	


/********************************END OF FILE****************************/
