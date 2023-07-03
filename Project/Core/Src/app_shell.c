/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
#include "shell.h"
#include "usart.h"

/* Private define ------------------------------------------------------------*/
#define SHELL_THREAD_STACK_SIZE 	4096
#define SHELL_THREAD_PRIO       	31
#define SHELL_BUF_MAX_SIZE			1024
#define PRINT_BUF_MAX_SIZE			1024

/* Private typedef -----------------------------------------------------------*/
typedef struct shell_rx_s
{
    uint8_t buf[2][SHELL_BUF_MAX_SIZE];	// 双缓冲
    uint16_t size;
    uint16_t pos;

    bool is_buf;	// false-buf[0], true-buf[1]
} shell_rx_t;

/* Private variables ---------------------------------------------------------*/
TX_THREAD shell_thread;
__attribute__((aligned(8))) CHAR shell_stack[SHELL_THREAD_STACK_SIZE];
TX_MUTEX shell_lock;
TX_MUTEX tx_kprintf_lock;
TX_SEMAPHORE shell_rx_sem;

Shell shell;
char shellBuffer[SHELL_BUF_MAX_SIZE];

char printf_buf[PRINT_BUF_MAX_SIZE + 1];			// 一次最多打印

shell_rx_t shell_rx =
{
    .buf = {0},
    .size = 0,
    .pos = 0,
    .is_buf = false,
};

extern volatile double cpu_usage;
extern DMA_HandleTypeDef hdma_uart4_rx;
/* Private function prototypes -----------------------------------------------*/
static void shell_task(ULONG thread_input);
static signed short shellRead(char *data, unsigned short len);
static signed short shellWrite(char *data, unsigned short len);
static int lock(Shell *shell);
static int unlock(Shell *shell);
static void shell_thread_entry(ULONG thread_input);

static void shell_task(ULONG thread_input)
{
    shellTask(&shell);
}

static signed short shellRead(char *data, unsigned short len)
{
    int is_buf = shell_rx.is_buf;

    tx_semaphore_get(&shell_rx_sem, TX_WAIT_FOREVER);

    for(int i = 0; i < len; i++)
        *data++ = shell_rx.buf[is_buf][shell_rx.pos++];
    if(shell_rx.pos < shell_rx.size)
    {
        tx_semaphore_put(&shell_rx_sem);
        return len;
    }
    else
    {
        memset(shell_rx.buf, 0, shell_rx.pos);
        shell_rx.pos = 0;
        shell_rx.size = 0;
        return len;
    }
}

static signed short shellWrite(char *data, unsigned short len)
{
    if(HAL_OK == HAL_UART_Transmit(&huart4, (uint8_t *)data, len, 1000))
        return len;
    return 0;
}

static int lock(Shell *shell)
{
    return tx_mutex_get(&shell_lock, TX_WAIT_FOREVER);
}

static int unlock(Shell *shell)
{
    return tx_mutex_put(&shell_lock);
}

/**
  * @brief  list thread info
  * @param  None
  * @retval None
  */
static void ps(void)
{
    TX_THREAD      *thread = &shell_thread;
    UCHAR stat;
    uint8_t *ptr;
    uint8_t maxlen = TX_NAME_MAX;

    /* Print Sysinfo */
    tx_kprintf("\r\n");
    object_split(maxlen, "=");
    tx_kprintf("==============================================================================\r\n");
    tx_kprintf("cpu usage = %5.2f%%\r\n", cpu_usage);
    tx_kprintf("task  exec time	= %.9fs\r\n", (double)_tx_execution_thread_time_total / SystemCoreClock);
    tx_kprintf("idle  exec time	= %.9fs\r\n", (double)_tx_execution_idle_time_total / SystemCoreClock);
    tx_kprintf("isr   exec time	= %.9fs\r\n", (double)_tx_execution_isr_time_total / SystemCoreClock);
    tx_kprintf("total exec time = %.9fs\r\n", (double)(_tx_execution_thread_time_total + \
													   _tx_execution_idle_time_total + \
													   _tx_execution_isr_time_total) / SystemCoreClock);

    /* Print Title */
    object_split(maxlen, "=");
    tx_kprintf("==============================================================================\r\n");
    tx_kprintf("%-*s pri  status        sp     stack size  cur used  max used   cur tick  err\r\n", maxlen, "thread");
    object_split(maxlen, "-");
    tx_kprintf(" ---  --------- ---------- ----------  --------  --------  ---------- ---\r\n");

    /* Traversing Thread Control List */
    while (thread != (TX_THREAD *)0)
    {
        tx_kprintf("%-*.*s %3d ", maxlen, maxlen, thread->tx_thread_name, thread->tx_thread_priority);

        stat = thread->tx_thread_state;
        if(stat == TX_READY)				tx_kprintf(" ready    ");
        else if(stat == TX_COMPLETED)		tx_kprintf(" complete ");
        else if(stat == TX_TERMINATED)		tx_kprintf(" terminate");
        else if(stat == TX_SUSPENDED)		tx_kprintf(" suspend  ");
        else if(stat == TX_SLEEP)			tx_kprintf(" sleep    ");

        ptr = (uint8_t *)thread->tx_thread_stack_start;
        while(*ptr == 0xEF) ++ptr;			// ThreadX Fill Stack With 0xEF

        tx_kprintf(" 0x%08x   %8d       %02d%%       %02d%%",
                   (ULONG)thread->tx_thread_stack_ptr,
                   thread->tx_thread_stack_size,
                   ((ULONG)thread->tx_thread_stack_end - (ULONG)thread->tx_thread_stack_ptr) * 100 / thread->tx_thread_stack_size,
                   ((ULONG)thread->tx_thread_stack_end - (ULONG)ptr) * 100 / thread->tx_thread_stack_size);
        if(thread->tx_thread_time_slice == 0)	tx_kprintf("  Preemptive");
        else	tx_kprintf("  0x%08x", thread->tx_thread_time_slice);
        tx_kprintf(" %03d\r\n", 000);

        thread = thread->tx_thread_created_next;
        if(thread == &shell_thread) break;
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    ps, ps, list all thread);

UINT shell_thread_init(VOID)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart4, shell_rx.buf[0], SHELL_BUF_MAX_SIZE);
    __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);

    shell.read = shellRead;
    shell.write = shellWrite;
    shell.lock = lock;
    shell.unlock = unlock;
    shellInit(&shell, shellBuffer, 1024);

    /* Create shell lock.  */
    if(TX_SUCCESS != tx_mutex_create(&shell_lock, "shell lock", TX_INHERIT))
        return TX_THREAD_ERROR;

    /* Create shell rx sem.  */
    if(TX_SUCCESS != tx_semaphore_create(&shell_rx_sem, "shellrx", 0))
        return TX_THREAD_ERROR;

    /* Create print lock.  */
    if(TX_SUCCESS != tx_mutex_create(&tx_kprintf_lock, "tx kprint lock", TX_INHERIT))
        return TX_THREAD_ERROR;

    /* Create shell thread.  */
    if(TX_SUCCESS != tx_thread_create(&shell_thread, "shell", shell_task, 0,
                                      &shell_stack[0], SHELL_THREAD_STACK_SIZE,
                                      SHELL_THREAD_PRIO, SHELL_THREAD_PRIO, TX_NO_TIME_SLICE, TX_AUTO_START))
        return TX_MUTEX_ERROR;

    return TX_SUCCESS;
}

/**
  * @brief  Thread-Safe Printf
  * @param  used as printf
  * @retval None
  */
void tx_kprintf(const char *fmt, ...)
{
    size_t len = 0;

    va_list args;

    va_start(args, fmt);
    len = vsnprintf(printf_buf, PRINT_BUF_MAX_SIZE, fmt, args);
    va_end(args);

    if (len > PRINT_BUF_MAX_SIZE)	len = PRINT_BUF_MAX_SIZE;

    tx_mutex_get(&tx_kprintf_lock, TX_WAIT_FOREVER);
    printf("%s", printf_buf);
    tx_mutex_put(&tx_kprintf_lock);
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart == &huart4)
    {
        shell_rx.pos = 0;
        shell_rx.size = SHELL_BUF_MAX_SIZE - __HAL_DMA_GET_COUNTER(&hdma_uart4_rx);
        if(shell_rx.is_buf == false)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&huart4, (uint8_t *)shell_rx.buf[1], SHELL_BUF_MAX_SIZE);
            shell_rx.is_buf = true;
        }
        else if(shell_rx.is_buf == true)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&huart4, (uint8_t *)shell_rx.buf[0], SHELL_BUF_MAX_SIZE);
            shell_rx.is_buf = false;
        }
        tx_semaphore_put(&shell_rx_sem);
    }
}