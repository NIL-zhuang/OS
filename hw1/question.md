1. 请简述 80x86 系列的发展历史
   1. 从Intel 4004发展出了Intel 8008处理器
   2. 1978年推出了16位微处理器8086，20位地址线
   3. 1982年发布80286，主频提高到12MHz
   4. 1985年发布80386，处理器32位，地址线32位
   5. 1989年发布80486，1993年发布586并改名奔腾
2. 说明小端和大端的区别，并说明 80x86 系列采用了哪种方式
   1. 高地址存放的是高尾端(大端)还是低尾端（小端）
   2. 小端存储
3. 8086 有哪五类寄存器，请分别举例说明其作用？
   1. 数据寄存器
   2. 指针寄存器
   3. 变址寄存器
   4. 控制寄存器
   5. 段寄存器
4. 什么是寻址？立即寻址和直接寻址的区别是什么？
   1. 找到操作数的地址以取出操作数
   2. 立即寻址直接给出了操作数，没有寻址
   3. 直接寻址给出了操作数的地址，用[]取数
5. 请举例说明寄存器间接寻址、寄存器相对寻址、基址加变址寻种方式的区别
   1. 寄存器间接： 操作数有效地址在寄存器里
   2. 寄存器相对：MOV AX [SI + 3]
   3. 基址+变址： MOV AX [BX + DI]
6. 请分别简述 MOV 指令和 LEA 指令的用法和作用？
7. 请说出主程序与子程序之间至少三种参数传递方式
   1. 寄存器传参
   2. 约定的地址传参
   3. 堆栈传参
8. 如何处理输入和输出，代码中哪里体现出来？
9.  有哪些段寄存器
10. 通过什么寄存器保存前一次的运算结果，在代码中哪里体现出
11. 解释 boot.asm 文件中，org 0700h 的作用
    1.  BIOS发现了引导扇区，由于这个512bit的软盘以0xAA55结束，是一个引导扇区。
    2.  org 0700h表示操作系统将其加载到内存地址0000:7c00处，然后跳转到7c00由引导代码控制
12. boot.bin 应该放在软盘的哪一个扇区？为什么？
    1.  第一个扇区
    2.  BIOS开机的时候从软盘的第一个扇区开始读入OS，直到0xaa55结束
13. loader 的作用有哪些？
    1.  从实模式跳入保护模式，访问1M以上的内存
    2.  启动内存分页
    3.  从kernal.bin中读取内核放入内存，然后跳到内核开始执行
14. 解释 NASM 语言中 [ ] 的作用
    1.  在nasm中，任何不被`[]`括起来的标签or变量是地址。访问标签中的内容必须用`[]`
15. 解释语句 times 510-($-$$) db 0，为什么是 510? $ 和 $$ 分别表示什么
    1.  `$` 表示程序当前位置的地址，也就是0x7c09
    2.  `$$` 表示程序被编译后的开始地址，也就是0x7c00
    3.  表示在这个程序后填充0，知道达到510字节，然后加上结束标志位0xAA55正好是512字节，满一个扇区
16. 解释配置文件 bochsrc 文件中各参数的含义
```txt
megs:32
display_library: sdl
floppya: 1_44=a.img, status=inserted
boot: floppy
```

* megs：how much memory the emulated machine willl have，This option sets the 'guest' and 'host' memory parameters to the same value
* display_library: the code that displays the Bochs VGA screen，使用sdl运行VGA屏幕
* floppya: 指定哪个文件作为软盘映像, first drive
* boot: this defines the boot sequence