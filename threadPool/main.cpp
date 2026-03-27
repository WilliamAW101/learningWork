#include "threadPool.hpp"
#include <stdio.h>

void add(int x, int y) {
    printf("Adding %d\n", x + y);
    for (int i = 0; i < 12; i++) {
        printf("Done Adding\n");
    }
}

void printingStuff() {
    for (int i = 0; i < 10; i++) {
        printf("Testing Printing..\n");
    }
}

int main() {
    ThreadPool pool(5);
    pool.addTask([](){add(10, 33);});
    pool.addTask([](){printingStuff();});
    pool.addTask([]() { printf("task 3\n"); });

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}