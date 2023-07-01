/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"
#include "main.h"
#include "tx_execution_profile.h"
#include <math.h>
/* Private define ------------------------------------------------------------*/
#define SYS_STAT_THREAD_STACK_SIZE 	2048
#define SYS_STAT_THREAD_PRIO       	20

/* Private variables ---------------------------------------------------------*/
TX_THREAD sys_stat_thread;
__attribute__((aligned(8))) CHAR sys_stat_thread_stack[SYS_STAT_THREAD_STACK_SIZE];
volatile double cpu_usage;	// CPU  π”√¬ 

extern EXECUTION_TIME     _tx_execution_thread_time_total;
extern EXECUTION_TIME     _tx_execution_isr_time_total;
extern EXECUTION_TIME     _tx_execution_idle_time_total;
/* Private function prototypes -----------------------------------------------*/
static void sys_stat_thread_entry(ULONG thread_input)
{
	EXECUTION_TIME total_time, idle_time, delat_to_time, delta_to_idle_time;
	
	idle_time = _tx_execution_idle_time_total;
	total_time = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total;
	while(1)
	{		
		delta_to_idle_time = _tx_execution_idle_time_total - idle_time;
		delat_to_time = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total - total_time;
		cpu_usage = (double)delta_to_idle_time/delat_to_time;
		cpu_usage = 100 - cpu_usage * 100;
		idle_time = _tx_execution_idle_time_total;
		total_time = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total;
		tx_thread_sleep(200);
		
//		tx_kprintf("_tx_execution_idle_time_total: %d\r\n", _tx_execution_idle_time_total);
//		tx_kprintf("_tx_execution_isr_time_total: %d\r\n", _tx_execution_isr_time_total);
//		tx_kprintf("_tx_execution_thread_time_total: %d\r\n", _tx_execution_thread_time_total);
	}
}

UINT sys_stat_thread_init(void)
{
	/* Create button thread.  */
    if (tx_thread_create(&sys_stat_thread, "sys stat", sys_stat_thread_entry, 0, &sys_stat_thread_stack[0],
                         SYS_STAT_THREAD_STACK_SIZE, SYS_STAT_THREAD_PRIO, SYS_STAT_THREAD_PRIO,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
    {
        return TX_THREAD_ERROR;
    }
	
	return TX_SUCCESS;
}
