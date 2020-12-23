
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
    s->value--;
    if (s->value < 0) {
        p_proc_ready->blocked = True;                   // 阻塞自己
        s->process_list[s->cur_size++] = p_proc_ready;  // 移入等待队列
    }
    schedule();
    return 0;
}

PUBLIC int sys_V_process(SEMAPHORE* s) {
    s->value++;
    if (s->value <= 0) {
        // 释放一个等待进程转为就绪，进程继续执行
        s->process_list[0]->blocked = False;
        for (int i = 0; i < PROCESS_LIST_SIZE - 1; i++) {
            s->process_list[i] = s->process_list[i + 1];
        }
    }
    return 0;
}

PUBLIC int sys_my_display_str(char* str, int color) {
    return 0;
}

void READER(char* name) {
    printf(name);
    printf(" Start Reading\n");
    // printf(name);
    // printf(" Is Reading\n");
    // printf(name);
    // printf("Finish Reading");
}

void WRITER(char* name) {
    printf(name);
    printf(" Start Writing\n");
    // printf(name);
    // printf(" Is Writing\n");
    // printf(name);
    // printf("Finish Writing\n");
}