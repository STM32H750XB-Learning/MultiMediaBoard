/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
#include "multi_button.h"

/* Private define ------------------------------------------------------------*/
#define BUTTON_THREAD_STACK_SIZE 	1024
#define BUTTON_THREAD_PRIO       	10

/* Private variables ---------------------------------------------------------*/
TX_THREAD button_thread;
__attribute__((aligned(8))) CHAR button_thread_stack[BUTTON_THREAD_STACK_SIZE];
struct Button usr_button;
/* Private function prototypes -----------------------------------------------*/
static void button_thread_entry(ULONG thread_input);
static uint8_t key_usr_read(uint8_t paras);
static void cb_single_click(void *paras);

/**
  * @brief  Function implementing the tx_app_thread_entry thread.
  * @param  thread_input: Hardcoded to 0.
  * @retval None
  */
static void button_thread_entry(ULONG thread_input)
{
    while(1)
    {
        button_ticks();
        tx_thread_sleep(TICKS_INTERVAL);			// 留给低优先级线程运行的机会
	}
}

static uint8_t key_usr_read(uint8_t paras)
{
    return (uint8_t)KEY_USR_READ();
}

static void cb_single_click(void *paras)
{
	tx_kprintf("usr key single click\r\n");
}

UINT button_thread_init(VOID)
{
    button_init(&usr_button, key_usr_read, 0, 0);
    button_attach(&usr_button, SINGLE_CLICK, cb_single_click);
    button_start(&usr_button);

	/* Create button thread.  */
    if (tx_thread_create(&button_thread, "button", button_thread_entry, 0, &button_thread_stack[0],
                         BUTTON_THREAD_STACK_SIZE, BUTTON_THREAD_PRIO, BUTTON_THREAD_PRIO,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
    {
        return TX_THREAD_ERROR;
    }
	
	return TX_SUCCESS;
}