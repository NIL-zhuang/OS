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
    string name;         // �ļ���       /etc/bin/hello
    string simple_Name;  // �򵥵��ļ���  hello
    int attribute;       // ����     0���ļ��У�1���ļ�
    int start_cluster;   // �״غ�
    int size;            // �ļ���С
};

struct DirEntry {
    char DIR_Name[8];            // �ļ���
    char DIR_Attr[3];            //��չ��
    char attribute;              // 0x10 dict, 0x20 file
    char remain[10];             // 10������λ
    char DIR_WrtTime[2];         // ���һ��д��ʱ��
    char DIR_WrtDate[2];         // ���һ��д������
    unsigned short DIR_FstClus;  // ��ʼ�غ� 2byte
    unsigned int DIR_FileSize;   // �ļ���С 4byte
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
    // �ж�ls���������ǲ����з�lָ��
    for (int i = 0; i < command.option.length(); i++) {
        if (command.option[i] != 'l') {
            printDefault(command.operand + ": inapplicable options --" +
                         command.option + "\n");
            return;
        }
    }
    // �жϴ�ӡ�������Ƿ���Ҫl
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
    /* �ݹ��ӡ�ļ��ײ�����Ϣ�����param��1,��Ҫ��ӡparam����ϸ��Ϣ*/
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
    // ��root�µ������ļ�������FAT��浽fileArray��
    image.open(imageName);
    if (!image) {
        return false;
    }
    File root;
    root.name = "/";
    root.attribute = 0;
    root.size = 0;
    // 1������������8+8��FAT��12��������root
    root.start_cluster = -12;  // ����ɴغ�, root�ȵ���12������
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
    // ��Ӧ��������ʼ��ַ
    int start = (dir.start_cluster + 31) * 512;
    image.seekg(start);  //��ʼ��ַ
                         // ��ȡ32��byte�浽tmpDirEntry��
    image.read((char *)tmpDirPtr, 32);
    for (int i = 15; i >= i; i--) {
        // ������һ�����������е�Ŀ¼��Ŀ
        // һ�����������16����Ŀ
        if (tmpDirEntry.DIR_Name[0] == '\0') {
            break;  // �����˿��ļ�
        }
        if (tmpDirEntry.attribute != 0x10 && tmpDirEntry.attribute != 0x20 &&
            tmpDirEntry.attribute != 0x00) {
            // ���ļ��к���ͨ�ļ�
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
        // �ж϶�ȡ���ļ�����
        if (tmpDirEntry.attribute == 0x10) {
            file.attribute = 0;
        } else {
            file.attribute = 1;
        }
        file.start_cluster = tmpDirEntry.DIR_FstClus;
        file.size = tmpDirEntry.DIR_FileSize;
        fileArray[fileArraySize++] = file;
        if (file.attribute == 0) {
            // ������ļ��о͵ݹ��ȡĿ¼
            readDir(file);
        }
    }
}

string getFileName(DirEntry &dirEntry) {
    /** �����ļ�����ļ������� */
    string fileName;
    int fileNameLen = 0;
    if (dirEntry.attribute == 0x10) {
        // �ļ��е��������ռ8�ֽ�
        fileName = trim(dirEntry.DIR_Name);
    } else if (dirEntry.attribute == 0x20) {
        // �ļ�������8�ֽڵ��ļ�������3�ֽڵ���չ��
        fileName = trim(dirEntry.DIR_Name);
        if (trim(dirEntry.DIR_Attr).length() > 0)
            // Ԥ���������ļ�
            fileName = fileName + "." + dirEntry.DIR_Attr;
    }
    return fileName;
}

int getNextCluster(int cluster) {
    /**
     * �������ڵĴغţ���FAT��õ���һ���غţ����û�оͷ���-1
     * һ����ռ12bit���Խ�FAT12
     * 123456�����������غŷֱ���312��564
     */
    if (cluster == 0) {
        // ��0���ؾͱ�ʾnil��
        return -1;
    }
    int num1, num2;
    char *p1 = (char *)&num1;  // Ϊ��ȡ��2��byte������ô��
    char *p2 = (char *)&num2;
    int start = cluster / 2 * 3;  // �����ֽڵĴص���ʼλ��
    int res = 0;
    if (cluster % 2 == 0) {
        // �������������ֽ�
        image.seekg(512 + start);
        image.read(p1, 1);
        image.read(p2, 1);
        num2 &= 0x0f;  // ȡ�ұ�4λ�͵�ַ
        res = num2 << 8 + num1;
    } else {
        // �����Ҳ������ֽ�
        image.seekg(512 + start + 1);
        image.read(p1, 1);
        image.read(p2, 1);
        num1 &= 0xf0;  // ȡ���
        res = (num1 >> 4) + (num2 << 4);
    }
    if (res >= 0x0ff8) return -1;  // �������һ����
    return res;
}

int fileFile(string path) {
    /**
     * ����path��·���ҵ�file����fileArray���λ��
     * ����ҵ�pos�ͷ��أ����򷵻�-1��ʾ�Ҳ����ļ�
     */
    for (int pos = 0; pos < fileArraySize; pos++) {
        if (fileArray[pos].name == path) return pos;
    }
    return -1;
}
string trim(string s) {
    /** ȥ���ַ���ǰ��Ŀո�����"   ab  "��ת��Ϊ"ab" */
    if (s.empty()) {
        return s;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}