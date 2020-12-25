
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* EXTERN is defined as extern except in global.c */
#ifdef GLOBAL_VARIABLES_HERE
#undef EXTERN
#define EXTERN
#endif

EXTERN int ticks;

EXTERN int disp_pos;
EXTERN u8 gdt_ptr[6];  // 0~15:Limit  16~47:Base
EXTERN DESCRIPTOR gdt[GDT_SIZE];
EXTERN u8 idt_ptr[6];  // 0~15:Limit  16~47:Base
EXTERN GATE idt[IDT_SIZE];

EXTERN u32 k_reenter;

EXTERN TSS tss;
EXTERN PROCESS* p_proc_ready;

EXTERN int nr_current_console;

extern PROCESS proc_table[];
extern char task_stack[];
extern TASK task_table[];
extern TASK user_proc_table[];
extern irq_handler irq_table[];
extern TTY tty_table[];
extern CONSOLE console_table[];

EXTERN int readerCount;
EXTERN int writerCount;
EXTERN int trueReaderCount;    // real reader when reading, for reader first
EXTERN SEMAPHORE writeblock;
EXTERN SEMAPHORE readerLimit;  // 限制同时读同一本书的人数
EXTERN SEMAPHORE mutex_readerCount;  // protect reader count mutex
EXTERN SEMAPHORE mutex_writerCount;

EXTERN SEMAPHORE mutex_reader;  // protect writer count mutex, can't write together
EXTERN SEMAPHORE write_first;        // use this to block read procs
EXTERN SEMAPHORE mutex_fair_read; // read and write should be fair
// EXTERN SEMAPHORE mutex_isReading;  // protect isReading
