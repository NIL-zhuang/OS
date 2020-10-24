#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

struct Command {
    // only cat and ls
    string operand;
    string option;
    string path;
};

struct File {
    string name;
};

extern "C" {
void printAsm(const char *str, int color);
}

bool readImage(string);
void repl();
void parseCommand(string);
void printDefault(string);
void printRed(string);

string imageName = "a.img";

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
    string command;
    while (true) {
        cout << ">";
        getline(cin, command);
        if (command == "exit") {
            break;
        }
        parseCommand(command);
    }
}

void parseCommand(string command) {
    stringstream ss;
    string operand;
    string option;
    string tmp;
    string path;
    ss << command;
    ss >> operand;
    if (operand != "cat" || operand != "ls") {
        cout << "command not found: " << operand << endl;
        return;
    }
    while (getline(ss, tmp, ' ')) {
        if (tmp[0] == '-') {
            // solve parameters
            option = tmp.substr(1);
            bool onlyLInOption = true;
            for (char ch : tmp) {
                if (ch != 'l') {
                    onlyLInOption = false;
                    break;
                }
            }
            if (operand == "cat" || !onlyLInOption) {
                // the only option will be ls -l*
                cout << command << ": no appliable options" << endl;
            }
            option = "l";
        } else {
            if (path.length() == 0) {
                path = tmp;
            } else {
                cout << "Too many path params" << endl;
                return;
            }
        }
    }
}

bool readImage(string imageName) {
    ifstream image;
    image.open(imageName);
    if (!image) {
        return false;
    }
    return true;
}

void printDefault(string s) { printAsm(s.c_str(), 0); }
void printRed(string s) { printAsm(s.c_str(), 1); }