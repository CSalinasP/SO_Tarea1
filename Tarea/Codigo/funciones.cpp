#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <fstream>
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
            perror("Error al ejecutar el comando.");
        }
        exit(EXIT_FAILURE);

    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);

    } else {
        perror("Error en fork.");
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
                perror("Error al ejecutar comando en pipe.");
                exit(EXIT_FAILURE);
            }

        } else {
            wait(nullptr);
            close(pipefd[1]);
            in_fd = pipefd[0];
        }
    }
}

void executeMiprof(vector<string> command) {
    struct timeval start, end;
    struct rusage usage;
    gettimeofday(&start, nullptr);

    pid_t pid = fork();

    if (pid == 0) {
        cout << "Comando usado: " << endl;

        for (const auto& arg : command) {
            cout << arg << " ";
        }

        cout << endl;
        cout << "Respuesta: " << endl;

        vector<char*> args_c;
        for (auto& arg : command){
            args_c.push_back(const_cast<char*>(arg.c_str()));   
        }
        args_c.push_back(nullptr);

        if (execvp(args_c[0], args_c.data()) == -1) {
            perror("Error al ejecutar el comando.");
        }
        exit(EXIT_FAILURE);
    } 

    else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);

        gettimeofday(&end, nullptr);
        getrusage(RUSAGE_CHILDREN, &usage);

        double real_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        double user_time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
        double sys_time = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
        long max_rss = usage.ru_maxrss; // in kilobytes

        cout << "Tiempo real: " << real_time << " segundos" << endl;
        cout << "Tiempo de usuario: " << user_time << " segundos" << endl;
        cout << "Tiempo de sistema: " << sys_time << " segundos" << endl;
        cout << "Máximo resident set size: " << max_rss << " KB" << endl;

    } else {
        perror("Error en fork.");
    }
}

void executeMiprofMaxTime(vector<string> command, int maxTime) {
    struct timeval start, end;
    struct rusage usage;
    gettimeofday(&start, nullptr);
    bool exceeded = true;

    pid_t pid = fork();

    if (pid == 0) {
        cout << "Comando usado: " << endl;

        for (const auto& arg : command) {
            cout << arg << " ";
        }

        cout << endl;
        cout << "Respuesta: " << endl;

        alarm(maxTime);

        vector<char*> args_c;
        for (auto& arg : command){
            args_c.push_back(const_cast<char*>(arg.c_str()));   
        }
        args_c.push_back(nullptr);

        if (execvp(args_c[0], args_c.data()) == -1) {
            perror("Error al ejecutar el comando.");
        }
        exit(EXIT_FAILURE);
    } 
    else if (pid > 0) {
        int status;
        int currentTime = 0;

        while (currentTime < maxTime) {
            pid_t pid_status = waitpid(pid, &status, WNOHANG);

            if (pid_status == pid) {
                exceeded = false;
                break;
            }

            sleep(1);
            currentTime++;
        }

        if (exceeded){
            kill(pid, SIGKILL);
            cout << "Proceso terminado por exceder el tiempo máximo." << endl;
            waitpid(pid, &status, 0);
        }

        else {
            waitpid(pid, &status, 0);
        }

        gettimeofday(&end, nullptr);
        getrusage(RUSAGE_CHILDREN, &usage);

        double real_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        double user_time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
        double sys_time = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
        long max_rss = usage.ru_maxrss; 

        cout << "Tiempo real: " << real_time << " segundos" << endl;
        cout << "Tiempo de usuario: " << user_time << " segundos" << endl;
        cout << "Tiempo de sistema: " << sys_time << " segundos" << endl;
        cout << "Máximo resident set size: " << max_rss << " KB" << endl;

    } else {
        perror("Error en fork.");
    }
}

void executeMiprofEjecsave(vector<string> command, char* archive){
    struct timeval start, end;
    struct rusage usage;
    gettimeofday(&start, nullptr);

    pid_t pid = fork();

    if (pid == 0) {
        int fd = open(archive, O_WRONLY | O_CREAT | O_APPEND, 0644);

        if (fd == -1) {
            perror("Error al abrir el archivo.");
            exit(EXIT_FAILURE);
        }

        string message = "Comando usado: \n";
        for (int i = 0; i < command.size(); ++i) {
            message += command[i];
            if (i + 1 < command.size()){
                message += " ";
            }
        }

        message += "\nRespuesta: \n";
        write(fd, message.c_str(), message.size());
        fflush(stdout);
        dup2(fd, 1);
        close(fd);

        vector<char*> args_c;

        for (auto& arg : command) {
            args_c.push_back(const_cast<char*>(arg.c_str()));
        }
        args_c.push_back(nullptr);

        if (execvp(args_c[0], args_c.data()) == -1) {
            perror("Error al ejecutar el comando.");
        }
        exit(EXIT_FAILURE);

    } 
    else if (pid > 0) {
        waitpid(pid, nullptr, 0);

        gettimeofday(&end, nullptr);
        getrusage(RUSAGE_CHILDREN, &usage);

        double real_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        double user_time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
        double sys_time = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
        long max_rss = usage.ru_maxrss; 

        ofstream fd(archive, ios::app);
        if (!fd) {
            cerr << "Error al abrir el archivo." << endl;
            return;
        }
        fd << "Tiempo real: " << real_time << " segundos" << endl;
        fd << "Tiempo de usuario: " << user_time << " segundos" << endl;
        fd << "Tiempo de sistema: " << sys_time << " segundos" << endl;
        fd << "Máximo resident set size: " << max_rss << " KB" << endl;
        fd << endl;
        fd << endl;
        fd.close();
        cout << "Se ha guardado la información en el archivo exitosamente." << endl;
    }

    else {
        perror("Error en fork.");
    }
}
