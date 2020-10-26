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
    int depth;           // 文件所在深度
};

struct DirEntry {
    char DIR_Name[8];            // 文件名
    char DIR_Attr[3];            // 扩展名
    char attribute;              // 0x10 dir, 0x20 file
    char remain[10];             // 10个保留位
    char DIR_WrtTime[2];         // 最后一次写入时间
    char DIR_WrtDate[2];         // 最后一次写入日期
    unsigned short DIR_FstClus;  // 开始簇号 2byte
    unsigned int DIR_FileSize;   // 文件大小 4byte
};

// extern "C" {
// void printAsm(const char *str, int color);
// }

bool readImage(string);
void repl();
Command parseCommand(string);
void exec(Command);
void lsExec(Command);
void lsDFS(int, int);
void catExec(Command);
void catCommand(int);
void printDefault(string);
void printRed(string);
void readDir(File &);
string getFileName(DirEntry &);
int findFile(string);
int subDirNum(int);
int subFileNum(int);
string space_trim(string);
int getNextCluster(int);

string imageName = "a.img";
File fileArray[4096];
int fileArraySize = 0;
ifstream image;

int main() {
    printDefault("Start reading image\n");
    if (!readImage(imageName)) {
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
            printDefault("bye\n");
            return;
        }
        Command cmd = parseCommand(command);
        if (cmd.operand == "error") continue;
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
                Command error = {operand : "fault"};
                return error;
            }
        }
    }
    if (path.length() == 0) path = "/";  // ls默认为/
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
        lsDFS(pos, option);
    } else {
        printDefault(file.simple_Name + "\n");
        return;
    }
}

void lsDFS(int pos, int param) {
    /* 递归打印文件底部的信息，如果param是1,就要打印param的详细信息*/
    File &file = fileArray[pos];
    if (file.depth == 0) {
        printDefault(file.name);
    } else {
        printDefault(file.name + "/");
    }
    if (param == 1) {
        printDefault(" " + to_string(subDirNum(pos)));
        printDefault(" " + to_string(subFileNum(pos)) + "\n");
    }
    if (file.depth > 0) {
        printRed(". ");
        if (param == 1) printDefault("\n");
        printRed(".. ");
        if (param == 1) printDefault("\n");
    }
    // 打印子文件和子文件夹
    for (int i = pos + 1; i < fileArraySize; i++) {
        File &cur = fileArray[i];
        if (cur.depth == file.depth) break;  // 深搜到头了
        if (cur.depth == file.depth + 1) {
            if (cur.attribute == 0) {
                // 文件夹
                printRed(cur.simple_Name + "  ");
                if (param == 1) {
                    printDefault(to_string(subDirNum(i)) + "  ");
                    printDefault(to_string(subFileNum(i)) + "\n");
                }
            } else {
                // 文件
                printDefault(cur.simple_Name + "  ");
                if (param == 1) {
                    printDefault(to_string(cur.size) + "\n");
                }
            }
        }
    }
    printDefault("\n");
    // 递归打印子文件夹里的东西
    for (int i = pos + 1; i < fileArraySize; i++) {
        File &cur = fileArray[i];
        if (cur.depth == file.depth) break;
        if (cur.depth == file.depth + 1 && cur.attribute == 0) {
            lsDFS(i, param);
        }
    }
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

void catCommand(int pos) {
    File &file = fileArray[pos];
    char text[9192];
    if (file.name.length() < 4 ||
        file.name.substr(file.name.length() - 4, 4) != ".txt") {
        printDefault("You can only read txt files\n");
        return;
    }
    int cluster = file.start_cluster;
    int cluster_count = 0;
    while (cluster != -1) {
        int address = (cluster + 31) * 512;
        image.seekg(address);
        image.read((char *)text + cluster_count * 512, 512);
        cluster = getNextCluster(cluster);
        cluster_count++;
    }
    printDefault(text);
    printDefault("\n");
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
    root.simple_Name = "/";
    root.attribute = 0;
    root.size = 0;
    // 1个引导扇区，8+8个FAT，12个扇区的root
    root.start_cluster = -12;  // 换算成簇号, root先当他12个扇区
    root.depth = 0;
    fileArray[fileArraySize++] = root;
    readDir(root);
    image.close();
    return true;
}

void printDefault(string s) {
    cout << s;
    // printAsm(s.c_str(), 0);
}
void printRed(string s) {
    cout << s;
    // printAsm(s.c_str(), 1);
}
void readDir(File &dir) {
    DirEntry tmpDirEntry;
    DirEntry *tmpDirPtr = &tmpDirEntry;
    // 对应扇区的起始地址，如果是root要-12
    int start = (dir.start_cluster + 31) * 512;
    for (int i = 0; i < 16; i++) {
        // 遍历第一个扇区里所有的目录条目
        // 一个扇区里最多16个条目
        image.seekg(start + i * 32);
        image.read((char *)tmpDirPtr, 32);  // 读取32个byte存到tmpDirEntry里
        if (tmpDirEntry.DIR_Name[0] == '\0') {
            break;  // 读到了空文件，读取结束
        }
        if (tmpDirEntry.DIR_Name[0] == '.') continue;  // 针对指向本地的目录
        if (tmpDirEntry.attribute != 0x10 && tmpDirEntry.attribute != 0x20 &&
            tmpDirEntry.attribute != 0x00) {
            // 非文件夹和普通文件
            continue;
        }
        File file;
        // getFileName
        file.simple_Name = getFileName(tmpDirEntry);
        if (dir.name == "/") {
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
        file.depth = dir.depth + 1;
        fileArray[fileArraySize++] = file;
        if (file.attribute == 0) {
            // 如果是文件夹就递归读取目录
            readDir(file);
        }
    }
}

string getFileName(DirEntry &dirEntry) {
    /** 解析文件获得文件的名称 */
    string fileName = dirEntry.DIR_Name;
    string attr = dirEntry.DIR_Attr;
    int fileNameLen = 0;
    if (dirEntry.attribute == 0x10) {
        // 文件夹的名字最多占8字节
        fileName = space_trim(fileName);
    } else if (dirEntry.attribute == 0x20) {
        // 文件，除了8字节的文件名还有3字节的扩展名
        fileName = space_trim(fileName);
        if (space_trim(attr).length() > 0)
            // 预防无类型文件
            fileName = fileName + "." + attr;
    }
    return fileName;
}

int subDirNum(int pos) {
    int num = 0;
    File &dir = fileArray[pos];
    for (int i = pos + 1; i < fileArraySize; i++) {
        File &cur = fileArray[i];
        if (cur.depth == dir.depth) {
            return num;
        }
        if (cur.depth == dir.depth + 1 && cur.attribute == 0) {
            num++;
        }
    }
    return num;
}

int subFileNum(int pos) {
    int num = 0;
    File &dir = fileArray[pos];
    for (int i = pos + 1; i < fileArraySize; i++) {
        File &cur = fileArray[i];
        if (cur.depth == dir.depth) {
            break;
        }
        if (cur.depth == dir.depth + 1 && cur.attribute == 1) {
            num++;
        }
    }
    return num;
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

int findFile(string path) {
    /**
     * 根据path的路径找到file所在fileArray里的位置
     * 如果找到pos就返回，否则返回-1表示找不到文件
     */
    for (int pos = 0; pos < fileArraySize; pos++) {
        if (fileArray[pos].name == path) return pos;
    }
    return -1;
}
string space_trim(string s) {
    /** 去除字符串前后的空格，例如"   ab  "被转化为"ab" */
    char chs[20];
    int size = 0;
    for (int i = 0; i < 8 && s[i] != ' '; i++) {
        chs[size++] = s[i];
    }
    chs[size] = '\0';
    return string(chs);
}