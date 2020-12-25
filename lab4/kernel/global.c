
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

PUBLIC PROCESS proc_table[NR_TASKS + NR_PROCS];

PUBLIC TASK task_table[NR_TASKS] = {
    {task_tty, STACK_SIZE_TTY, "tty"}};

PUBLIC TASK user_proc_table[NR_PROCS] = {
    {ReaderA, STACK_SIZE_READER_A, "ReaderA"},
    {ReaderB, STACK_SIZE_READER_B, "ReaderB"},
    {ReaderC, STACK_SIZE_READER_C, "ReaderC"},
    {WriterD, STACK_SIZE_WRITER_D, "WriterD"},
    {WriterE, STACK_SIZE_WRITER_E, "WriterE"},
    {NormalF, STACK_SIZE_NORMAL_F, "NormalF"}};

PUBLIC char task_stack[STACK_SIZE_TOTAL];

PUBLIC TTY tty_table[NR_CONSOLES];
PUBLIC CONSOLE console_table[NR_CONSOLES];

PUBLIC irq_handler irq_table[NR_IRQ];

// 系统调用表
PUBLIC system_call sys_call_table[NR_SYS_CALL] = {
    sys_get_ticks,
    sys_write,
    sys_sleep,
    sys_my_display_str,
    sys_P_process,
    sys_V_process};

PUBLIC int readerCount = 0;
PUBLIC int writerCount = 0;
PUBLIC int trueReaderCount = 0;
PUBLIC SEMAPHORE writeblock = {1, 0, 0};
PUBLIC SEMAPHORE readerLimit = {READER_SAME_TIME, 0, 0};
PUBLIC SEMAPHORE mutex_readerCount = {1, 0, 0};
PUBLIC SEMAPHORE mutex_writerCount = {1, 0, 0};
PUBLIC SEMAPHORE mutex_reader = {1, 0, 0};
PUBLIC SEMAPHORE write_first = {1, 0, 0};
PUBLIC SEMAPHORE mutex_fair_read = {1, 0, 0};