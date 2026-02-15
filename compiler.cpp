#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int main()
{
    string line;
    string filename;
    cout << "File name: ";
    cin >> filename;

    ifstream in(filename);
    ofstream out("temp.asm");

    if (in.is_open() && out.is_open())
    {
        string command;
        string arg1, arg2, arg3;

        out << "global _start" << endl;
        out << endl;

        while (getline(in, line))
        {
            if (line.empty()) continue;

            istringstream iss(line);
            iss >> command;

            if (command == "f"){
                iss >> arg1 >> arg2 >> arg3;

                if (arg2 == "=") out << "mov " << arg1 << ", " << arg3 << endl;
                else{
                    out << "mov rax, " << "[" << arg1 << "]" << endl;

                    if (arg2 == "+=") out << "add rax, " << arg3 << endl;
                    else if (arg2 == "-=") out << "sub rax, " << arg3 << endl;

                    out << "mov " << "[" << arg1 << "]" << ", rax" << endl;
                }

            }
            else if (command == "mov") {
                iss >> arg1 >> arg2;
                out << "mov " << arg1 << ", " << arg2 << endl;
            }
            else if (command == "movv") {
                iss >> arg1 >> arg2;
                out << "mov " << "[" << arg1 << "]" << ", " << "[" << arg2 << "]" << endl;
            }
            else if (command == "returnv") {
                iss >> arg1;
                out << "mov " << "rdi, " << "[" << arg1 << "]" << endl;
            }
            else if (command == "return") {
                iss >> arg1;
                out << "mov " << "rdi, " << arg1 << endl;
            }
            else if (command == "var") {
                string arg1, arg2, arg3;
                iss >> arg1 >> arg2;
                if (arg2 == "=") {
                    getline(iss, arg3);
                    if (!arg3.empty() && arg3[0] == ' ') arg3 = arg3.substr(1);
                    size_t pos = 0;
                    while ((pos = arg3.find("\\n", pos)) != string::npos) {
                        arg3.replace(pos, 2, "\", 10, \"");
                        pos += 7;
                    }
                    out << arg1 << " db " << arg3 << endl;
                    out << arg1 << "_len equ $ - " << arg1 << endl;
                } else {
                    out << arg1 << " db 0" << endl;
                    out << arg1 << "_len equ 1" << endl;
                }
            }
            else if (command == "printv") {
                iss >> arg1;
                out << "mov rsi, " << arg1 << endl;
                out << "mov rax, 1" << endl;
                out << "mov rdi, 1" << endl;
                out << "mov rdx, " << arg1 << "_len" << endl;
                out << "syscall" << endl;
            }
            else if (command == "printvbyte") {
                iss >> arg1 >> arg2;
                out << "mov rsi, " << arg1 << endl;
                out << "mov rax, 1" << endl;
                out << "mov rdi, 1" << endl;
                out << "mov rdx, " << arg2 << endl;
                out << "syscall" << endl;
            }
            else if (command == "VARS:") {
                out << "section .data" << endl;
            }
            else if (command == "START:") {
                out << "section .text" << endl;
                out << "_start:" << endl;
            }
            else if (command == "end") {
                out << "mov rax, 60" << endl;
                out << "syscall" << endl;
            }
            else cout << "unknown command '" << command << "'" << endl;
            out << endl;
        }

        in.close();
        out.close();
    }
    else
    {
        return 1;
    }

    return 0;
}
