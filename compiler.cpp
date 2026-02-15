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
        string arg0, arg1, arg2, arg3, arg4, arg5;

        out << "global _start" << endl;
        out << endl;

        while (getline(in, line))
        {
            if (line.empty()) continue;

            istringstream iss(line);
            iss >> command;
            if (command == "//") continue;
            else if (command == ";") continue;


            else if (command == "asm:"){
                string asm_line;
                getline(iss, asm_line);
                size_t start = asm_line.find_first_not_of(" \t");
                if (start != string::npos) {
                    out << asm_line.substr(start) << endl;
                }
            }
            else if (command == "f") {
                iss >> arg1 >> arg2 >> arg3;

                auto isConstant = [](const string& s) {
                    if (s.empty()) return false;
                    return isdigit(s[0]) || (s[0] == '-' && s.size() > 1 && isdigit(s[1]));
                };

                if (arg2 == "=") {
                    if (isConstant(arg3)) {
                        out << "mov qword [" << arg1 << "], " << arg3 << endl;
                    } else {
                        out << "mov rax, [" << arg3 << "]" << endl;
                        out << "mov [" << arg1 << "], rax" << endl;
                    }
                }
                else {
                    out << "mov rax, [" << arg1 << "]" << endl;

                    if (arg2 == "+=") {
                        if (isConstant(arg3)) out << "add rax, " << arg3 << endl;
                        else out << "add rax, [" << arg3 << "]" << endl;
                    }
                    else if (arg2 == "-=") {
                        if (isConstant(arg3)) out << "sub rax, " << arg3 << endl;
                        else out << "sub rax, [" << arg3 << "]" << endl;
                    }
                    else if (arg2 == "*=") {
                        if (isConstant(arg3)) {
                            out << "imul rax, " << arg3 << endl;
                        } else {
                            out << "imul rax, [" << arg3 << "]" << endl;
                        }
                    }
                    else if (arg2 == "/=" || arg2 == "%=") {
                        out << "xor rdx, rdx" << endl;
                        if (isConstant(arg3)) {
                            out << "mov rbx, " << arg3 << endl;
                            out << "div rbx" << endl;
                        } else {
                            out << "div qword [" << arg3 << "]" << endl;
                        }
                        if (arg2 == "%=") out << "mov rax, rdx" << endl;
                    }
                    else if (arg2 == "tonum") {
                        out << "sub rax, '0'" << endl;
                    }
                    else if (arg2 == "tochar") {
                        out << "add rax, '0'" << endl;
                    }
                    out << "mov [" << arg1 << "], rax" << endl;
                }
            }

            else if (command == "if") {
                iss >> arg0 >> arg1 >> arg2 >> arg3 >> arg4 >> arg5;
                out << "cmp "<< arg0 << " " << arg1 << ", " << arg3 << endl;
                if (arg2 == "=="){
                    if(arg4 == "=>"){
                        out << "je " << arg5 << endl;
                    } else cout << "warning: missing operator '=>' in 'if'" << endl;
                } else if (arg2 == "!="){
                    if(arg4 == "=>"){
                        out << "jne " << arg5 << endl;
                    } else cout << "warning: missing operator '=>' in 'if'" << endl;
                } else if (arg2 == "<"){
                    if(arg4 == "=>"){
                        out << "jb " << arg5 << endl;
                    } else cout << "warning: missing operator '=>' in 'if'" << endl;
                } else if (arg2 == ">"){
                    if(arg4 == "=>"){
                        out << "ja " << arg5 << endl;
                    } else cout << "warning: missing operator '=>' in 'if'" << endl;
                } else if (arg2 == "<="){
                    if(arg4 == "=>"){
                        out << "jbe " << arg5 << endl;
                    } else cout << "warning: missing operator '=>' in 'if'" << endl;
                } else if (arg2 == ">="){
                    if(arg4 == "=>"){
                        out << "jae " << arg5 << endl;
                    } else cout << "warning: missing operator '=>' in 'if'" << endl;
                }
            }
            else if (command == "jump") {
                iss >> arg1;
                out << "jmp " << arg1 << endl;
            }
            else if (command == "call") {
                iss >> arg1;
                out << "call " << arg1 << endl;
            }
            else if (command == "return") {
                iss >> arg1;
                out << "mov " << "rdi, " << arg1 << endl;
            }
            else if (command == "var:") {
                string type, name, value, len_spec;
                iss >> type >> name;

                string next_token;
                iss >> next_token;

                if (next_token == "=") {
                    getline(iss, value);
                    if (!value.empty() && value[0] == ' ') value = value.substr(1);

                    size_t pos = 0;
                    while ((pos = value.find("\\n", pos)) != string::npos) {
                        value.replace(pos, 2, "\", 10, \"");
                        pos += 7;
                    }
                    pos = 0;
                    while ((pos = value.find("\\t", pos)) != string::npos) {
                        value.replace(pos, 2, "\", 9, \"");
                        pos += 7;
                    }
                    pos = 0;
                    while ((pos = value.find("\\r", pos)) != string::npos) {
                        value.replace(pos, 2, "\", 13, \"");
                        pos += 7;
                    }
                    pos = 0;
                    while ((pos = value.find("\\\\", pos)) != string::npos) {
                        value.replace(pos, 2, "\", 92, \"");
                        pos += 7;
                    }
                    pos = 0;
                    while ((pos = value.find("\\\"", pos)) != string::npos) {
                        value.replace(pos, 2, "\", 34, \"");
                        pos += 7;
                    }

                    if (type == "1b") out << name << " db " << value << endl;
                    else if (type == "2b") out << name << " dw " << value << endl;
                    else if (type == "4b") out << name << " dd " << value << endl;
                    else if (type == "8b") out << name << " dq " << value << endl;
                    else cout << "warning: unknown type '" << type << "'" << endl;

                    if (value.find('"') != string::npos) {
                        out << name << "_len equ $ - " << name << endl;
                    }
                }
                else {
                    string len_str = next_token;
                    if (len_str.empty() || !isdigit(len_str[0])) {
                        len_str = "1";
                    }

                    if (type == "1b") out << name << " db 0" << endl;
                    else if (type == "2b") out << name << " dw 0" << endl;
                    else if (type == "4b") out << name << " dd 0" << endl;
                    else if (type == "8b") out << name << " dq 0" << endl;
                    else cout << "warning: unknown type '" << type << "'" << endl;


                    out << name << "_len equ " << len_str << endl;
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
            else if (command == "readv") {
                iss >> arg1;
                out << "mov rsi, " << arg1 << endl;
                out << "mov rax, 0" << endl;
                out << "mov rdi, 0" << endl;
                out << "mov rdx, 100" << endl;
                out << "syscall" << endl;
            }
            else if (command == "readvbyte") {
                iss >> arg1 >> arg2;
                out << "mov rsi, " << arg1 << endl;
                out << "mov rax, 0" << endl;
                out << "mov rdi, 0" << endl;
                out << "mov rdx, " << arg2 << endl;
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
            else if (command == "POINT") {
                iss >> arg1;
                out << arg1 << ":" << endl;
            }
            else if (command == "FUNC") {
                iss >> arg1;
                out << arg1 << ":" << endl;
            }
            else if (command == "term") {
                iss >> arg1;
                out << "ret" << endl;
            }
            else if (command == "syscall") {
                iss >> arg1;
                out << "syscall" << endl;
            }
            else if (command == "VARS:") {
                out << "section .data" << endl;
            }
            else if (command == "BSS:") {
                out << "section .bss" << endl;
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
