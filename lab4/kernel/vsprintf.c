
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                              vsprintf.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "string.h"

/*
 *  为更好地理解此函数的原理，可参考 printf 的注释部分。
 */

/*======================================================================*
                                vsprintf
 *======================================================================*/
int vsprintf(char *buf, const char *fmt, va_list args) {
    char *p;
    char tmp[256];
    va_list p_next_arg = args;

    // 将fmt里的内容拷贝到buf里，如果遇到%则解析%后的内容并重新导入
    for (p = buf; *fmt; fmt++) {
        if (*fmt != '%') {
            *p++ = *fmt;
            continue;
        }

        fmt++;

        switch (*fmt) {
            case 'x':
                itoa(tmp, *((int *)p_next_arg));
                strcpy(p, tmp);
                p_next_arg += 4;
                p += strlen(tmp);
                break;
            case 'c':
                // 解析一个char字符
                *p++ = *((char *)p_next_arg);
                p_next_arg += 4;
                break;
            case 's':
                break;
            default:
                break;
        }
    }

    return (p - buf);
}
