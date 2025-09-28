// compile:
// g++ -o utxo utxo.cpp -no-pie -fno-stack-protector -std=c++17 -static

#include <iostream>
#include <string>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <cstdint>
#include <algorithm>

using namespace std;

void print_menu();

void alarm_handler(int) {
    cout << "TIME OUT\n";
    _exit(-1);
}

void initialize() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    signal(SIGALRM, alarm_handler);
    alarm(60);
}

struct Menu {
    void (*fp)();
    Menu(void (*f)()) : fp(f) {}
    void call() { if (fp) fp(); }
};

extern "C" void reveal_flag() {
    char* cmd = "/bin/sh";
    system(cmd);
}

int read_int(const char *prompt) {
    int x;
    cout << prompt;
    if (!(cin >> x)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return 0;
    }
    return x;
}

void copy(int *source, size_t source_size, int *target, size_t target_size) {
    memcpy(target, source, source_size * sizeof(int));
}

//trigger..
void view_buffers(int *a, size_t asz, int *b, size_t bsz, Menu* menu_ptr, void (*original_fp)()) {
    cout << "UTXO1 (amounts): [";
    for (size_t i = 0; i < asz; ++i) { cout << a[i] << (i + 1 != asz ? ", " : ""); }
    cout << "]\n";
    cout << "UTXO2 (txids): [";
    for (size_t i = 0; i < bsz; ++i) { cout << b[i] << (i + 1 != bsz ? ", " : ""); }
    cout << "]\n";

    uintptr_t utxo2_addr = reinterpret_cast<uintptr_t>(b);
    uintptr_t menu_addr = reinterpret_cast<uintptr_t>(menu_ptr);
    ptrdiff_t offset = menu_addr - utxo2_addr;

    cout << "Address of utxo2 buffer : " << (void*)utxo2_addr << "\n";
    cout << "Address of menu object  : " << (void*)menu_addr << "\n";
    cout << "Overwrite index in utxo1: " << (offset / sizeof(int)) << "\n";
    cout << "Menu function pointer info: " << (void*)menu_ptr->fp << "\n";

    menu_ptr->call();
}

void print_menu() {
    cout << "\nUTXO System Menu\n";
    cout << "1. Update UTXO amounts\n";
    cout << "2. Update UTXO ids\n";
    cout << "3. Copy UTXO amounts\n";
    cout << "4. Copy UTXO ids\n";
    cout << "5. View UTXOs & Trigger\n";
    cout << "6. Exit\n";
    cout << "[*] Select menu: ";
}


int main() {
    initialize();

    size_t a_size = 16;
    size_t b_size = 4;

    int *utxo1 = (int*)malloc(a_size * sizeof(int));
    int *utxo2 = (int*)malloc(b_size * sizeof(int));
    Menu *menu = (Menu*)malloc(sizeof(Menu));
    
    new (menu) Menu(print_menu);

    for (size_t i = 0; i < a_size; ++i) utxo1[i] = 100 + (int)i;
    for (size_t i = 0; i < b_size; ++i) utxo2[i] = 200 + (int)i;

    while (true) {
        print_menu();
        int sel = read_int("");
        if (sel == 1) {
            cout << "Update utxo1 amounts:\n";
            for (size_t i = 0; i < a_size; ++i) {
                utxo1[i] = read_int(("Amount[" + to_string(i) + "]: ").c_str());
            }
        } else if (sel == 2) {
            cout << "Update utxo2 txids:\n";
             for (size_t i = 0; i < b_size; ++i) {
                utxo2[i] = read_int(("TxID[" + to_string(i) + "]: ").c_str());
            }
        } else if (sel == 3) {
            copy(utxo1, a_size, utxo2, b_size);
        } else if (sel == 4) {
            size_t to_copy = min(a_size, b_size);
            memcpy(utxo1, utxo2, to_copy * sizeof(int));
        } else if (sel == 5) {
            view_buffers(utxo1, a_size, utxo2, b_size, menu, print_menu);
        } else if (sel == 6) {
            cout << "Bye\n";
            break;
        } else {
            cout << "Invalid\n";
        }
    }

    free(utxo1);
    free(utxo2);
    menu->~Menu();
    free(menu);
    
    return 0;
}