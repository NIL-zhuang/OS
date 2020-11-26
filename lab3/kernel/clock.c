
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               clock.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

PRIVATE int clock_count;
/*======================================================================*
                           clock_handler
 *======================================================================*/
PUBLIC void clock_handler(int irq) {
    ticks++;
    clock_count++;
    p_proc_ready->ticks--;
    if (clock_count >= 20000 && STATUS == NORMAL_MODE) {
        cleanConsole(console_table);
        // out_char(console_table, '?');
        clock_count = 0;
    }
    // out_char(console_table, '?');

    if (k_reenter != 0) {
        return;
    }

    if (p_proc_ready->ticks > 0) {
        return;
    }

    schedule();
}

/*======================================================================*
                              milli_delay
 *======================================================================*/
PUBLIC void milli_delay(int milli_sec) {
    int t = get_ticks();
    while (((get_ticks() - t) * 1000 / HZ) < milli_sec) {
    }
}

/*======================================================================*
                           init_clock
 *======================================================================*/
PUBLIC void init_clock() {
    /* 初始化 8253 PIT */
    clock_count = 0;
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
    out_byte(TIMER0, (u8)((TIMER_FREQ / HZ) >> 8));

    put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
    enable_irq(CLOCK_IRQ);                     /* 让8259A可以接收时钟中断 */
}
