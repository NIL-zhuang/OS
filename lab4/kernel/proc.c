
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
                if (p->ticks > 0) continue;   // this is blocked
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

PUBLIC void READER(char name, int time_slice){
    printf("Reader %c Arrive\n", name);
    if(FAIR_READ) READER_fair(name, time_slice);
    else{
        if(READER_FIRST) READER_rf(name, time_slice);
        else READER_wf(name, time_slice);
    }
}

PUBLIC void WRITER(char name, int time_slice){
    printf("Writer %c Arrive\n", name);
    if(FAIR_READ) WRITER_fair(name, time_slice);
    else{
        if(READER_FIRST) WRITER_rf(name, time_slice);
        else WRITER_wf(name, time_slice);
    }
}

void READER_rf(char name, int time_slice) {
    // 开始读
    P_process(&readerLimit);

    P_process(&mutex_readerCount);
    if (readerCount == 0) P_process(&writeblock);
    readerCount++;
    V_process(&mutex_readerCount);

    // 进行读，对写操作加锁
    trueReaderCount += 1;
    printf("%c Start Reading\n", name);
    printf("%c Is Reading\n", name);
    sleep(time_slice * 900);
    // 完成读
    printf("%c Finish Reading\n", name);
    trueReaderCount -= 1;

    V_process(&readerLimit);
    sleep(time_slice * 100);
    P_process(&mutex_readerCount);
    readerCount--;
    if (readerCount == 0) V_process(&writeblock);
    V_process(&mutex_readerCount);
}

void WRITER_rf(char name, int time_slice) {
    P_process(&writeblock);
    printf("%c Start Writing\n", name);
    printf("%c Is Writing\n", name);
    sleep(time_slice * 1000);
    printf("%c Finish Writing\n", name);
    V_process(&writeblock);
}

void READER_fair(char name, int time_slice) {
    // 开始读
    P_process(&mutex_fair_read);

    P_process(&readerLimit);
    P_process(&mutex_readerCount);
    if (readerCount == 0) P_process(&writeblock);
    V_process(&mutex_fair_read);

    printf("%c Start Reading\n", name);
    readerCount++;
    V_process(&mutex_readerCount);
    // 进行读，对写操作加锁
    sleep(time_slice * 900);
    printf("%c Is Reading\n", name);
    // 完成读
    P_process(&mutex_readerCount);
    readerCount--;
    if (readerCount == 0) V_process(&writeblock);
    V_process(&mutex_readerCount);
    printf("%c Finish Reading\n", name);
    V_process(&readerLimit);
    sleep(time_slice * 100);
}

void WRITER_fair(char name, int time_slice) {
    P_process(&mutex_fair_read);
    P_process(&writeblock);
    V_process(&mutex_fair_read);
    printf("%c Start Writing\n", name);
    printf("%c Is Writing\n", name);
    sleep(time_slice * 900);
    printf("%c Finish Writing\n", name);
    V_process(&writeblock);
    sleep(time_slice * 100);
}

void READER_wf(char name, int time_slice){
    P_process(&readerLimit);

    P_process(&write_first);
    P_process(&mutex_readerCount);
    if (readerCount == 0) P_process(&writeblock);
    printf("%c Start Reading\n", name);
    readerCount++;
    V_process(&mutex_readerCount);
    V_process(&write_first);

    // 进行读，对写操作加锁
    sleep(time_slice * 900);
    printf("%c Is Reading\n", name);

    // 完成读
    P_process(&mutex_readerCount);
    readerCount--;
    if (readerCount == 0) V_process(&writeblock);
    V_process(&mutex_readerCount);
    printf("%c Finish Reading\n", name);

    V_process(&readerLimit);
    sleep(time_slice * 100);

}

void WRITER_wf(char name, int time_slice){
    P_process(&mutex_writerCount);
    writerCount+=1;
    if(writerCount == 1) P_process(&write_first);
    V_process(&mutex_writerCount);
    P_process(&writeblock);
    printf("%c Start Writing\n", name);
    sleep(time_slice * 900);
    printf("%c Is Writing\n", name);

    P_process(&mutex_writerCount);
    writerCount -= 1;
    if(writerCount == 0)V_process(&write_first);
    V_process(&mutex_writerCount);

    printf("%c Finish Writing\n", name);
    V_process(&writeblock);
    sleep(time_slice * 100);

}