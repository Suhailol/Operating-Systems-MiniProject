#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
using namespace std;

int main(){
    int ch;
    pid_t pid;
    while(1){
        system("clear");
        printf("\n----------- MENU -----------\n");
        printf("1. Round Robin Scheduler\n2. Priority Scheduler\n3. Exit\n\n>> ");
        scanf("%d", &ch);
        switch(ch){
            case 1:
                pid = fork();
                if(pid == -1){
                    printf("Fork Error\n");
                    exit(EXIT_FAILURE);
                }
                else if(pid==0){
                    system("clear");
                    printf("\n----------- ROUND ROBIN SCHEDULER -----------\n");
                    execlp("./roundRobin", "roundRobin", "-v", NULL);
                }
                else{
                    wait(NULL);
                }
                break;

            case 2:
                pid = fork();
                if(pid == -1){
                    printf("Fork Error\n");
                    exit(EXIT_FAILURE);
                }
                else if(pid==0){
                    system("clear");
                    printf("\n----------- PRIORITY SCHEDULER -----------\n");
                    execlp("./priority", "priority", NULL);
                }
                else{
                    wait(NULL);
                }

                break;

            case 3:
                exit(1);
                break;
            default:
                printf("Invalid choice\n");
        }
    }
    return EXIT_SUCCESS;
}