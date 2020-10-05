#include <stdio.h>
#define MAX 100

char add_res[MAX];
char mul_res[MAX];
char a[MAX];
char b[MAX];
int len_a;
int len_b;

void init() {
    for (int i = 0; i < MAX; i++) {
        add_res[i] = '0';
        mul_res[i] = '0';
        a[i] = '0';
        b[i] = '0';
    }
}

void input() {
    scanf("%s", &a);
    scanf("%s", &b);
}

char* add(char* a, char* b, int len_a, int len_b) {
    int a = 0;
    return NULL;
}

char* mul(char* a, char* b, int len_a, int len_b) { return NULL; }

void show_num(char* num) {
    int flag = 0;
    for (int i = 0; i < MAX; i++) {
        if (num[i] == '0' && flag) {
            continue;
        } else {
            flag = 1;
            print('%d', num[i]);
        }
    }
    print("\n");
}

void show() {
    show_num(add_res);
    show_num(mul_res);
}

int main() {
    print("OS_LAB_1 sample big number add & mul\n");
    print("Please input x and y :\n");
    init();
    return 0;
}
