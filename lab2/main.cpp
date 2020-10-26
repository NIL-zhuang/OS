#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;
// xxd -u -a -g 1 -c 16 -s +0x2600 -l 512 a.img

struct Command {
    // only cat and ls
    string operand;
    string option;
    string path;
};

struct File {
    string name;         // 文件名       /etc/bin/hello
    string simple_Name;  // 简单的文件名  hello
    int attribute;       // 属性     0是文件夹，1是文件
    int start_cluster;   // 首簇号
    int size;            // 文件大小
};

struct DirEntry {
    char DIR_Name[8];            // 文件名
    char DIR_Attr[3];            //扩展名
    char attribute;              // 0x10 dict, 0x20 file
    char remain[10];             // 10个保留位
    char DIR_WrtTime[2];         // 最后一次写入时间
    char DIR_WrtDate[2];         // 最后一次写入日期
    unsigned short DIR_FstClus;  // 开始簇号 2byte
    unsigned int DIR_FileSize;   // 文件大小 4byte
};

extern "C" {
void printAsm(const char *str, int color);
}

bool readImage(string);
void repl();
Command parseCommand(string);
void exec(Command);
void lsExec(Command);
void lsDFS(File &, int);
void catExec(Command);
void catCommand(File &);
void printDefault(string);
void printRed(string);
void readDir(File &);
string getFileName(DirEntry &);
int findFile(string);

string imageName = "a.img";
File fileArray[4096];
int fileArraySize = 0;
ifstream image;

int main() {
    if (readImage(imageName)) {
        printDefault("No such image " + imageName + "\n");
        return 0;
    }
    printDefault("FAT12 image works successfully \n");
    repl();
    return 0;
}

void repl() {
    // an infinite loop to exec commands
    string command;
    while (true) {
        printDefault("> ");
        getline(cin, command);
        if (command == "exit") {
            return;
        }
        Command cmd = parseCommand(command);
        exec(cmd);
    }
}

Command parseCommand(string command) {
    // parse commands into operand, option, path
    stringstream ss;
    string operand;
    string option;
    string tmp;
    string path;
    ss << command;
    ss >> operand;
    while (getline(ss, tmp, ' ')) {
        if (tmp[0] == '-') {
            // solve parameters
            option += tmp.substr(1);
        } else {
            // solve path
            if (path.length() == 0) {
                path = tmp;
            } else {
                printDefault("Too many path params\n");
                return;
            }
        }
    }
    Command cmd = {operand : operand, option : option, path : path};
    return cmd;
}

void exec(Command command) {
    // an interface to execute command
    if (command.operand == "ls") {
        lsExec(command);
    } else if (command.operand == "cat") {
        catExec(command);
    } else {
        printDefault(command.operand + ": command not found\n");
        return;
    }
}

void lsExec(Command command) {
    // 判断ls的命令里是不是有非l指令
    for (int i = 0; i < command.option.length(); i++) {
        if (command.option[i] != 'l') {
            printDefault(command.operand + ": inapplicable options --" +
                         command.option + "\n");
            return;
        }
    }
    // 判断打印的内容是否需要l
    int option = 0;
    if (command.option.length() > 0 && command.option[0] == 'l') option = 1;
    int pos = findFile(command.path);
    if (pos == -1) {
        printDefault(command.operand + ": no access to" + command.path +
                     " : no such file or dir\n");
        return;
    }
    File file = fileArray[pos];
    if (file.attribute == 0) {
        lsDFS(file, option);
    } else {
        printDefault(file.simple_Name + "\n");
        return;
    }
}

void lsDFS(File &file, int param) {
    /* 递归打印文件底部的信息，如果param是1,就要打印param的详细信息*/
    // TODO: lsDFS
}

void catExec(Command command) {
    if (command.option.length() > 0) {
        printDefault("cat: inapplicable options -- " + command.option + "\n");
        return;
    }
    int pos = findFile(command.path);
    File file = fileArray[pos];
    if (file.attribute == 0) {
        // is a dir
        printDefault("cat: " + command.path + ":is a dir\n");
        return;
    }
}

void catCommand(File &file) {
    // TODO: catCommand
}

bool readImage(string imageName) {
    // read the img file, start from its root part
    // 将root下的所有文件都利用FAT表存到fileArray里
    image.open(imageName);
    if (!image) {
        return false;
    }
    File root;
    root.name = "/";
    root.attribute = 0;
    root.size = 0;
    // 1个引导扇区，8+8个FAT，12个扇区的root
    root.start_cluster = -12;  // 换算成簇号, root先当他12个扇区
    fileArray[fileArraySize++] = root;
    readDir(root);
    image.close();
    return true;
}

void printDefault(string s) { printAsm(s.c_str(), 0); }
void printRed(string s) { printAsm(s.c_str(), 1); }
void readDir(File &dir) {
    DirEntry tmpDirEntry;
    DirEntry *tmpDirPtr = &tmpDirEntry;
    // 对应扇区的起始地址
    int start = (dir.start_cluster + 31) * 512;
    image.seekg(start);  //起始地址
                         // 读取32个byte存到tmpDirEntry里
    image.read((char *)tmpDirPtr, 32);
    for (int i = 15; i >= i; i--) {
        // 遍历第一个扇区里所有的目录条目
        // 一个扇区里最多16个条目
        if (tmpDirEntry.DIR_Name[0] == '\0') {
            break;  // 读到了空文件
        }
        if (tmpDirEntry.attribute != 0x10 && tmpDirEntry.attribute != 0x20 &&
            tmpDirEntry.attribute != 0x00) {
            // 非文件夹和普通文件
            continue;
        }
        File file;
        // getFileName
        file.simple_Name = getFileName(tmpDirEntry);
        if (tmpDirEntry.DIR_Name == "/") {
            file.name = "/" + file.simple_Name;
        } else {
            file.name = dir.name + "/" + file.simple_Name;
        }
        // 判断读取的文件类型
        if (tmpDirEntry.attribute == 0x10) {
            file.attribute = 0;
        } else {
            file.attribute = 1;
        }
        file.start_cluster = tmpDirEntry.DIR_FstClus;
        file.size = tmpDirEntry.DIR_FileSize;
        fileArray[fileArraySize++] = file;
        if (file.attribute == 0) {
            // 如果是文件夹就递归读取目录
            readDir(file);
        }
    }
}

string getFileName(DirEntry &dirEntry) {
    /** 解析文件获得文件的名称 */
    string fileName;
    int fileNameLen = 0;
    if (dirEntry.attribute == 0x10) {
        // 文件夹的名字最多占8字节
        fileName = trim(dirEntry.DIR_Name);
    } else if (dirEntry.attribute == 0x20) {
        // 文件，除了8字节的文件名还有3字节的扩展名
        fileName = trim(dirEntry.DIR_Name);
        if (trim(dirEntry.DIR_Attr).length() > 0)
            // 预防无类型文件
            fileName = fileName + "." + dirEntry.DIR_Attr;
    }
    return fileName;
}

int getNextCluster(int cluster) {
    /**
     * 根据现在的簇号，查FAT表得到下一个簇号，如果没有就返回-1
     * 一个簇占12bit所以叫FAT12
     * 123456里面有两个簇号分别是312和564
     */
    if (cluster == 0) {
        // 第0个簇就表示nil了
        return -1;
    }
    int num1, num2;
    char *p1 = (char *)&num1;  // 为了取这2个byte我容易么我
    char *p2 = (char *)&num2;
    int start = cluster / 2 * 3;  // 三个字节的簇的起始位置
    int res = 0;
    if (cluster % 2 == 0) {
        // 处理左侧的两个字节
        image.seekg(512 + start);
        image.read(p1, 1);
        image.read(p2, 1);
        num2 &= 0x0f;  // 取右边4位低地址
        res = num2 << 8 + num1;
    } else {
        // 处理右侧两个字节
        image.seekg(512 + start + 1);
        image.read(p1, 1);
        image.read(p2, 1);
        num1 &= 0xf0;  // 取左边
        res = (num1 >> 4) + (num2 << 4);
    }
    if (res >= 0x0ff8) return -1;  // 超过最后一个簇
    return res;
}

int fileFile(string path) {
    /**
     * 根据path的路径找到file所在fileArray里的位置
     * 如果找到pos就返回，否则返回-1表示找不到文件
     */
    for (int pos = 0; pos < fileArraySize; pos++) {
        if (fileArray[pos].name == path) return pos;
    }
    return -1;
}
string trim(string s) {
    /** 去除字符串前后的空格，例如"   ab  "被转化为"ab" */
    if (s.empty()) {
        return s;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}