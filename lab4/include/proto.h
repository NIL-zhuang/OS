
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC void out_byte(u16 port, u8 value);
PUBLIC u8 in_byte(u16 port);
PUBLIC void disp_str(char* info);
PUBLIC void disp_color_str(char* info, int color);

/* protect.c */
PUBLIC void init_prot();
PUBLIC u32 seg2phys(u16 seg);

/* klib.c */
PUBLIC void delay(int time);

/* kernel.asm */
void restart();

/* main.c */
// 在这里插入用户函数
void ReaderA();
void ReaderB();
void ReaderC();
void WriterD();
void WriterE();
void NormalF();

PUBLIC void WRITER(char name, int time_slice);  // interface to writer
PUBLIC void READER(char name, int time_slice);  // interface to reader

PUBLIC void WRITER_rf(char name, int time_slice);   // reader first
PUBLIC void READER_rf(char name, int time_slice);
PUBLIC void WRITER_fair(char name, int time_slice); // fair read write
PUBLIC void READER_fair(char name, int time_slice);
PUBLIC void WRITER_wf(char name, int time_slice);   // writer first
PUBLIC void READER_wf(char name, int time_slice);

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void init_clock();

/* keyboard.c */
PUBLIC void init_keyboard();

/* tty.c */
PUBLIC void task_tty();
PUBLIC void in_process(TTY* p_tty, u32 key);

/* console.c */
PUBLIC void out_char(CONSOLE* p_con, char ch);
PUBLIC void scroll_screen(CONSOLE* p_con, int direction);

/* printf.c */
PUBLIC int printf(const char* fmt, ...);

/* vsprintf.c */
PUBLIC int vsprintf(char* buf, const char* fmt, va_list args);

/* 以下是系统调用相关 */

/* 系统调用 - 系统级 */

/* proc.c */
PUBLIC int sys_get_ticks();
PUBLIC int sys_write(char* buf, int len);
/* syscall.asm */
PUBLIC void sys_call(); /* int_handler */

// 系统调用的新增函数
PUBLIC int sys_sleep(int milli_seconds);
PUBLIC int sys_my_display_str(char* str, int color);
PUBLIC int sys_P_process(SEMAPHORE* s);
PUBLIC int sys_V_process(SEMAPHORE* s);

/* 系统调用 - 用户级 */
PUBLIC int get_ticks();
PUBLIC void write(char* buf, int len);

// 用户级的新增函数
PUBLIC int sleep(int milli_seconds);
PUBLIC int my_display_str(char* str, int color);
PUBLIC int P_process(SEMAPHORE* s);
PUBLIC int V_process(SEMAPHORE* s);
