
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
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

PUBLIC void init_tasks(){
    readerCount = 0;
    writerCount = 0;
    trueReaderCount = 0;
}

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main() {
    disp_str("-----\"kernel_main\" begins-----\n");
    TASK* p_task = task_table;
    PROCESS* p_proc = proc_table;
    char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16 selector_ldt = SELECTOR_LDT_FIRST;
    int i;
    u8 privilege;
    u8 rpl;
    int eflags;
    for (i = 0; i < NR_TASKS + NR_PROCS; i++) {
        if (i < NR_TASKS) { /* 任务 */
            p_task = task_table + i;
            privilege = PRIVILEGE_TASK;
            rpl = RPL_TASK;
            eflags = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
        } else {             /* 用户进程 */
            p_task = user_proc_table + (i - NR_TASKS);
            privilege = PRIVILEGE_USER;
            rpl = RPL_USER;
            eflags = 0x202; /* IF=1, bit 2 is always 1 */
        }

        strcpy(p_proc->p_name, p_task->name);  // name of the process
        p_proc->pid = i;                       // pid

        p_proc->ldt_sel = selector_ldt;

        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
               sizeof(DESCRIPTOR));
        p_proc->ldts[0].attr1 = DA_C | privilege << 5;
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
               sizeof(DESCRIPTOR));
        p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

        p_proc->regs.eip = (u32)p_task->initial_eip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = eflags;

        p_proc->nr_tty = 0;  // 把tty都默认设为0

        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
        selector_ldt += 1 << 3;
    }

    for (int i = 0; i < 7; i++) {
        proc_table[i].ticks = proc_table[i].priority = 1;
        proc_table[i].blocked = proc_table[i].sleep_ticks = 0;
    }

    k_reenter = 0;
    ticks = 0;

    p_proc_ready = proc_table;
    init_tasks();
    init_clock();
    init_keyboard();

    restart();
    while (1) {
    }
}

void ReaderA() {
    while (True) {
        // printf("A is reading\n");
        READER('A', 2);
        // printf("finish read A\n");
        milli_delay(10000);
        // sleep(2000);
    }
}

void ReaderB() {
    while (True) {
        // printf("B is reading\n");
        READER('B', 3);
        milli_delay(10000);
        // sleep(3000);
    }
}

void ReaderC() {
    while (True) {
        // printf("C was reading\n");
        READER('C', 3);
        milli_delay(10000);
        // sleep(3000);
    }
}

void WriterD() {
    while (True) {
        // printf("D is writing\n");
        WRITER('D', 3);
        milli_delay(10000);
        // sleep(3000);
    }
}

void WriterE() {
    while (True) {
        // printf("E is writing\n");
        WRITER('E', 4);
        milli_delay(10000);
        // sleep(4000);
    }
}

void NormalF() {
    while (True) {
        if (readerCount > 0) {
            if(READER_FIRST) printf("%x Process Is Reading\n", trueReaderCount);
            else printf("%x Process Is Reading\n", readerCount);
        } else {
            printf("Is Writing\n");
        }
        // milli_delay(10000);
        sleep(1 * 1000);
    }
}