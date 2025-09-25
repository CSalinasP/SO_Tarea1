#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
using namespace std;

#include "funciones.h"

vector<string> parseLine(const string &line, char delimiter) {
    vector<string> tokens;
    stringstream ss(line);
    string token;
    while (getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
       }
    }
    return tokens;
}

void executeNormalCommand(vector<string> &args) {
    vector<char*> args_c;
    for (auto &arg : args) {
        args_c.push_back(const_cast<char*>(arg.c_str()));
    }
    args_c.push_back(nullptr);

    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args_c[0], args_c.data()) == -1) {
            perror("Error al ejecutar el comando");
        }
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("Error en fork");
    }
}

void executePipelineCommand(vector<vector<string>> &commands) {
    int n = commands.size();
    int pipefd[2], in_fd = 0;

    for (int i = 0; i < n; i++) {
        pipe(pipefd);
        pid_t pid = fork();

        if (pid == 0) {
            if (i > 0) {
                dup2(in_fd, 0);
                close(in_fd);
            }
            if (i < n - 1) {
                dup2(pipefd[1], 1);
            }
            close(pipefd[0]);
            close(pipefd[1]);

            vector<char*> args_c;
            for (auto &arg : commands[i]) {
                args_c.push_back(const_cast<char*>(arg.c_str()));
            }
            args_c.push_back(nullptr);

            if (execvp(args_c[0], args_c.data()) == -1) {
                perror("Error al ejecutar comando en pipe");
                exit(EXIT_FAILURE);
            }
        } else {
            wait(nullptr);
            close(pipefd[1]);
            in_fd = pipefd[0];
        }
    }
}
