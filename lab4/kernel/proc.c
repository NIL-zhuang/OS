
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule() {
    // printf("schedule");
    PROCESS* p;
    int greatest_ticks = 0;
    // 让所有在休眠的进程等待时间-1
    for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++) {
        if (p->sleep_ticks > 0) p->sleep_ticks--;
    }

    while (!greatest_ticks) {
        // FSCS 找到当前等待最久的那个进程，那个进程不能被阻塞也不能处于睡眠状态
        // 如果有的六个进程都sleep或block了，那就转到tty在那里调整ticks
        for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++) {
            if (p->sleep_ticks > 0 || p->blocked == True) continue;
            if (p->ticks > greatest_ticks) {
                greatest_ticks = p->ticks;
                p_proc_ready = p;
            }
        }
        // printf("%x ", greatest_ticks);

        if (!greatest_ticks) {
            for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++) {
                if (p->sleep_ticks > 0 || p->blocked == True) continue;
                p->ticks = p->priority;
            }
        }
    }
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks() {
    return ticks;
}

PUBLIC int sys_sleep(int milli_seconds) {
    // 设置进程睡眠
    p_proc_ready->sleep_ticks = milli_seconds;
    schedule();
    return 0;
}

PUBLIC int sys_P_process(SEMAPHORE* s) {
    disable_int();
    s->value--;
    printf("\n%x\n", s->value);
    printf("\nsys_P_Process\n");
    if (s->value < 0) {
        p_proc_ready->blocked = True;
        s->process_list[s->tail] = p_proc_ready;
        s->tail = (s->tail + 1) % PROCESS_LIST_SIZE;
        schedule();
    }
    enable_int();
}

PUBLIC int sys_V_process(SEMAPHORE* s) {
    disable_int();
    // printf("\n sys_V_process \n");
    s->value++;
    if (s->value <= 0) {
        // 释放队列头的进程
        PROCESS* proc = s->process_list[s->head];
        proc->blocked = False;
        // 队列数组移动
        s->head = (s->head + 1) % PROCESS_LIST_SIZE;
    }
    enable_int();
}

PUBLIC int sys_my_display_str(char* str, int color) {}

void start_read(char name) {
}

void READER(char name) {
    // 开始读
    P_process(&mutex_count);
    // sth_print();
    if (readerCount == 0) P_process(&writeblock);
    readerCount++;
    V_process(&mutex_count);
    printf("%c Start Reading\n", name);
    // 进行读，对写操作加锁
    printf("%c Is Reading\n", name);
    // 完成读
    P_process(&mutex_count);
    readerCount--;
    if (readerCount == 0) V_process(&writeblock);
    V_process(&mutex_count);
    printf("%c Finish Reading\n", name);
}

void WRITER(char name) {
    P_process(&writeblock);
    printf("%c Start Writing\n", name);
    printf("%c Is Writing\n", name);
    V_process(&writeblock);
    printf("%c Finish Writing\n", name);
}