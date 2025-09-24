#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
using namespace std;

#include "Funciones.h"

int main() {
    string line;

    while (true) {
        cout << "mishell$ ";
        getline(cin, line);

        if (line.empty()) {
            continue;
        }

        if (line == "exit") {
            break;
        }

        vector<string> pipeParts = parseLine(line, '|');
        vector<vector<string>> commands;

        for (auto &part : pipeParts) {
            commands.push_back(parseLine(part, ' '));
        }

        if (commands.size() == 1) {
            executeNormalCommand(commands[0]);
        } else {
            executePipelineCommand(commands);
        }
    }

    return 0;
}
