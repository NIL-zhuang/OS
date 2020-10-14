#include <stdio.h>
int a_num[100] = {0};
int b_num[100] = {0};
int a_len = 0;
int b_len = 0;
int add_res[100] = {0};
int mul_res[100] = {0};
int a_neg = 0;
int b_neg = 0;
char buffer = '\0';
int add_neg = 0;
int mul_neg = 0;
// 用在打印数字里
int* number_print;
int sign_num_print = 0;
int len_num_print = 0;

// 用在加法里
int add_temp = 0;
int carry = 0;
// 用在减法里
int* suber = NULL;
int* subee = NULL;
int sub_neg = 0;
int sub_res[100] = {0};
int sub_tmp = 0;

void getNumberLength();
void printNumberByChar();
void multiply();
void addHelper();
void add();
void minus();
void smaller();

int main() {
    printf("hello world \n");
    // input a and b
    while (buffer = getchar()) {
        if (buffer == ' ') break;
        if (buffer == '-') {
            a_neg = 1;
            continue;
        }
        a_num[a_len++] = buffer - '0';
    }
    while (buffer = getchar()) {
        if (buffer == '\n') break;
        if (buffer == '-') {
            b_neg = 1;
            continue;
        }
        b_num[b_len++] = buffer - '0';
    }
    // format numbers from 123xxx into 000123
    int total_size = 99;
    int tmp_alen = a_len;
    while (tmp_alen != 0) {
        a_num[total_size] = a_num[tmp_alen - 1];
        tmp_alen--;
        total_size--;
    }
    for (int i = 0; i < 100 - a_len; i++) {
        a_num[i] = 0;
    }
    total_size = 99;
    int tmp_blen = b_len;
    while (tmp_blen != 0) {
        b_num[total_size] = b_num[tmp_blen - 1];
        tmp_blen--;
        total_size--;
    }
    for (int i = 0; i < 100 - b_len; i++) {
        b_num[i] = 0;
    }
    multiply();
    number_print = mul_res;
    sign_num_print = mul_neg;
    printNumberByChar();
    addHelper();
}

void getNumberLength() {
    // finish
    len_num_print = 0;
    for (int i = 0; i < 100; i++) {
        if (number_print[i] != 0) {
            len_num_print = 100 - i;
            return;
        }
    }
    return;
}

void printNumberByChar() {
    getNumberLength();
    if (len_num_print == 0) {
        printf("0\n");
        return;
    }
    if (sign_num_print == 1) printf("-");
    for (int i = len_num_print; i != 0; i--) {
        printf("%d", number_print[100 - i]);
    }
    printf("\n");
    return;
}

void multiply() {
    if (a_neg != b_neg) mul_neg = 1;
    for (int i = 0; i < a_len; i++) {
        for (int j = 0; j < b_len; j++) {
            mul_res[99 - i - j] += a_num[99 - i] * b_num[99 - j];
        }
    }
    for (int i = 99; i > 0; i--) {
        if (mul_res[i] > 10) {
            mul_res[i - 1] += mul_res[i] / 10;
            mul_res[i] %= 10;
        }
    }
}

void addHelper() {
    if (a_neg != b_neg) {
        smaller();
        minus();
        sign_num_print = sub_neg;
        number_print = sub_res;
        printNumberByChar();
        return;
    }
    add();
    sign_num_print = add_neg;
    number_print = add_res;
    printNumberByChar();
    return;
}

void add() {
    add_neg = a_neg;
    for (int i = 99; i > 0; i--) {
        add_temp = a_num[i] + b_num[i] + carry;
        add_res[i] = add_temp % 10;
        carry = add_temp / 10;
    }
}

void minus() {
    for (int i = 99; i > 0; i--) {
        sub_tmp = suber[i] - subee[i];
        if (sub_tmp < 0) {
            sub_tmp += 10;
            suber[i - 1] -= 1;
        }
        sub_res[i] = sub_tmp;
    }
}
void smaller() {
    for (int i = 0; i < 100; i++) {
        if (a_num[i] > b_num[i]) {
            suber = a_num;
            sub_neg = a_neg;
            subee = b_num;
            return;
        }
        if (a_num[i] < b_num[i]) {
            suber = b_num;
            sub_neg = b_neg;
            subee = a_num;
            return;
        }
    }
    suber = a_num;
    subee = b_num;
}