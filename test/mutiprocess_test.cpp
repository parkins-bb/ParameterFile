#include <sys/wait.h> // waitpid()
#include <unistd.h> // fork(), getpid()
#include <iostream>
#include "Parameter.h"

void processFunction(const std::string& filename) {
    Parameter param(filename);
    param.doWork(); // 假设Parameter类有一个操作YAML文件的成员函数
}

int main() {
    pid_t pids[5];
    int i;
    int numProcesses = sizeof(pids) / sizeof(pid_t);

    for (i = 0; i < numProcesses; ++i) {
        pids[i] = fork();

        if (pids[i] < 0) {
            std::cerr << "Error: Failed to fork process " << i << std::endl;
            return -1;
        } else if (pids[i] == 0) {
            // 子进程
            std::cout << "Child process " << i << " started, PID: " << getpid() << std::endl;
            processFunction("path/to/your/yamlfile.yaml");
            exit(0); // 子进程完成任务后退出
        }
        // 父进程继续循环创建下一个子进程
    }

    // 父进程等待所有子进程完成
    for (i = 0; i < numProcesses; ++i) {
        waitpid(pids[i], NULL, 0);
    }

    std::cout << "All child processes have completed." << std::endl;

    return 0;
}

