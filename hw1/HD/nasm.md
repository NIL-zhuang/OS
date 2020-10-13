# 汇编语言设计教程

## 寻址方式&指令系统

### 寄存器

数据寄存器，指针寄存器，变址寄存器统称为通用寄存器

* AX 字乘法、除法、IO，累加器
* BX 存储器指针，基地址寄存器
* CX 串操作or循环控制计数器，计数寄存器
* DX 字乘法、除法、间接IO
* SI 串操作源指针
* DI 串操作目的指针
* BP 存取堆栈指针
* SP 堆栈指针

4个段寄存器实现寻址1M字节物理地址，段值和段内偏移形成20位地址

* CS code segment
* DS data segment
* SS stack segment
* ES extra segment

### 指令系统

* MOV DST, SRC          数据传送
* XCHG OPRD1， OPRD2    交换内容