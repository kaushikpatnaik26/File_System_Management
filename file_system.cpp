#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cstring>
using namespace std;

const int DISK_SIZE = 1024 * 1024;
const int BLOCK_SIZE = 512;
const int BLOCKS = DISK_SIZE / BLOCK_SIZE;

char disk[DISK_SIZE];
bool used[BLOCKS] = {false};

struct File {
    string name;
    int size;
    vector<int> blocks;
};

struct Dir {
    string name;
    Dir* parent;
    map<string, Dir*> subdirs;
    map<string, File> files;
};

Dir* root = new Dir{"root", nullptr};
Dir* current = root;

void mkdir(const string& name) {
    current->subdirs[name] = new Dir{name, current};
    cout << "Directory created\n";
}

void rmd(const string& name) {
    if (!current->subdirs.count(name)) return;
    Dir* d = current->subdirs[name];
    if (!d->files.empty() || !d->subdirs.empty()) {
        cout << "Directory not empty\n";
        return;
    }
    delete d;
    current->subdirs.erase(name);
    cout << "Directory deleted\n";
}

void cd(const string& name) {
    if (name == ".." && current->parent) current = current->parent;
    else if (current->subdirs.count(name)) current = current->subdirs[name];
    else cout << "Directory not found\n";
}

bool allocate(int size, vector<int>& blocks) {
    int needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    for (int i = 0; i < BLOCKS && needed > 0; i++) {
        if (!used[i]) {
            blocks.push_back(i);
            used[i] = true;
            needed--;
        }
    }
    return needed == 0;
}

void create(const string& name, int size) {
    vector<int> blocks;
    if (!allocate(size, blocks)) {
        cout << "Not enough space\n";
        return;
    }
    current->files[name] = {name, size, blocks};
    cout << "File created\n";
}

void write(const string& name, const string& data) {
    if (!current->files.count(name)) return;
    File& f = current->files[name];
    int i = 0;
    for (int b : f.blocks) {
        int off = b * BLOCK_SIZE;
        int len = min((int)data.size() - i, BLOCK_SIZE);
        memcpy(disk + off, data.c_str() + i, len);
        if (len < BLOCK_SIZE) memset(disk + off + len, 0, BLOCK_SIZE - len);
        i += len;
        if (i >= data.size()) break;
    }
}

void read(const string& name) {
    if (!current->files.count(name)) return;
    File& f = current->files[name];
    for (int b : f.blocks) {
        int off = b * BLOCK_SIZE;
        for (int i = 0; i < BLOCK_SIZE && disk[off + i] != '\0'; i++)
            cout << disk[off + i];
    }
    cout << "\n";
}

void del(const string& name) {
    if (!current->files.count(name)) return;
    for (int b : current->files[name].blocks) used[b] = false;
    current->files.erase(name);
    cout << "File deleted\n";
}

void ls() {
    for (auto& d : current->subdirs) cout << d.first << "/\n";
    for (auto& f : current->files) cout << f.first << "\n";
}

void shell() {
    string cmd;
    while (true) {
        cout << current->name << "> ";
        cin >> cmd;

        if (cmd == "mkdir") {
            string name; cin >> name; mkdir(name);
        } else if (cmd == "rmd") {
            string name; cin >> name; rmd(name);
        } else if (cmd == "cd") {
            string name; cin >> name; cd(name);
        } else if (cmd == "create") {
            string name; int size; cin >> name >> size; create(name, size);
        } else if (cmd == "write") {
            string name; cin >> name; cin.ignore();
            string text; getline(cin, text); write(name, text);
        } else if (cmd == "read") {
            string name; cin >> name; read(name);
        } else if (cmd == "del") {
            string name; cin >> name; del(name);
        } else if (cmd == "ls") {
            ls();
        } else if (cmd == "exit") break;
        else cout << "Unknown command\n";
    }
}

int main() {
    cout << "My name is Kaushik and I made this custom file management system! \n";
    cout << "Use these commnads :\n";
    cout << "mkdir: To create a custom directory i.e a folder" << endl;
    cout << "cd: To change directory" << endl;
    cout << "create: To create a file" << endl;
    cout << "write: To write to the file you have created. Allocate memory to your file as well" << endl;
    cout << "read: To reade the contents of the file" << endl;
    cout << "del: To delete the created file" << endl;
    cout << "Ls: to list out all the files created" << endl;
    cout << "Exit: To exit from the File Syste." << endl;
    cout << "Cd.. : Use this to exit from a directory/File" << endl;
    cout << " " << endl;
    shell();
    return 0;
}
