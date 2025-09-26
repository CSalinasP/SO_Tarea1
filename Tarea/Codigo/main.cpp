#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <algorithm>
using namespace std;

#include "funciones.h"

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

        vector<string> pipeLines = parseLine(line, '|');
        vector<vector<string>> commands;

        for (auto &pipe : pipeLines) {
            commands.push_back(parseLine(pipe, ' '));
        }

        if (commands[0][0] == "miprof") {

            if (commands[0].size() < 2) {
                cerr << "Se tiene que usar asi: miprof <ejec|ejecsave> ..." << endl;
                continue;
            }

            if (commands[0][1] == "ejec") {

                if (commands[0].size() > 2 && all_of(commands[0][2].begin(), commands[0][2].end(), ::isdigit)) {
                    int maxtime = stoi(commands[0][2]);
                    commands[0].erase(commands[0].begin(), commands[0].begin() + 3);
                    executeMiprofMaxTime(commands[0], maxtime);

                } else if (commands[0].size() > 2) {
                    commands[0].erase(commands[0].begin(), commands[0].begin() + 2);
                    executeMiprof(commands[0]);
                }

                else {
                    cerr << "Se tiene que usar asi: miprof ejec (opcional)<maxtiempo> <comando> <args>" << endl;
                    continue;
                }

            } else if (commands[0][1] == "ejecsave") {
                if (commands[0].size() > 3) {
                    char* archive = const_cast<char*>(commands[0][2].c_str());
                    commands[0].erase(commands[0].begin(), commands[0].begin() + 3);
                    executeMiprofEjecsave(commands[0], archive);
                }

                else {
                    cerr << "Se tiene que usar asi: miprof ejecsave <archivo> <comando> <args>" << endl;
                    continue;
                }

            } else {
                cerr << "error, use 'ejec' o 'ejecsave'." << endl;
            }
            continue;
        }

        else if (commands.size() == 1) {
            executeNormalCommand(commands[0]);
        } else {
            executePipelineCommand(commands);
        }
    }
    return 0;
}