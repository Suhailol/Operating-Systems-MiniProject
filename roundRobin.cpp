#include <bits/stdc++.h>
#include <iostream>
using namespace std;
#define border "+---------------------------------------------------------------+ \n"

typedef struct processDetails{
    int pid;
    int arrivalTime;
    int burstTime;
    int completionTime;
    int turnAroundTime;
    int waitingTime;
    int executionTime;
    int remainingTime;
    int preemptedTimes;
    int responseTime;
    int startTime;
} process;

bool sortProcessByArrivalTime(process p1, process p2);
bool sortProcessByPID(process p1, process p2);
vector<process> getProcessList(int &nProcess, int &timeQuantum);
void checkArrival(const int &currentTime, const vector<process> &pList, queue<process> &pQueue, int id, vector<bool> &visited, const bool verbose);
vector<process> getProcessList(int &nProcess, int &timeQuantum);
void execute(int &currentTime, const int &timeQuantum, int &completed, int &sumTurnAroundTime, int &sumWaitingTime, int &sumResponseTime, const int &nProcess, vector<process> &pList, queue<process> &pQueue, vector<bool> &visited, vector<process> &finalResult, int &flag, const bool verbose);
void printProcessDetails(vector<process> &result, int nProcess);
void printGantt(vector<string> &gantt, vector<int> &time);
void printLegend();

bool sortProcessByArrivalTime(process p1, process p2){
    if(p1.arrivalTime < p2.arrivalTime)
        return true;
    else
        return false;
}

bool sortProcessByPID(process p1, process p2){
    if(p1.pid < p2.pid)
        return true;
    else
        return false;
}

vector<process> getProcessList(int &nProcess, int &timeQuantum){
    printf(">> Number of Processes\t: ");
    nProcess=0;
    cin >> nProcess;
    
    printf(">> Time Quantum (in ms)\t: ");
    timeQuantum=0;
    cin >> timeQuantum;

    printf("\n");

    if(nProcess <= 0){
        printf(">> No Processes in memory. Exiting...\n");
        exit(1);
    }

    if(timeQuantum <= 0){
        printf(">> Time Quantum Invalid. Exiting...\n");
        exit(1);
    }
    
    vector<process> pList;
    for(int i=1; i<nProcess+1; i++){
        process p;
        p.pid = i;
        printf("PID: %d", p.pid);
        printf("\tArrival Time\t: ");
        cin >> p.arrivalTime;
        printf("\tBurst Time\t: ");
        cin >> p.burstTime;

        if(p.arrivalTime < 0 || p.burstTime <= 0){
            printf(">>Invalid Process parameters. Exiting...");
            exit(1);
        }

        p.remainingTime = p.burstTime;
        p.executionTime = 0;
        p.waitingTime = 0;
        p.turnAroundTime = 0;
        p.completionTime = 0;
        p.preemptedTimes = 0;

        pList.push_back(p);
    }
    printf("\n");

    sort(pList.begin(), pList.end(), sortProcessByArrivalTime);
    return pList;
}

void checkArrival(const int &currentTime, const vector<process> &pList, queue<process> &pQueue, int id, vector<bool> &visited, const bool verbose){
    for(auto iterator = pList.begin(); iterator != pList.end(); iterator++){
        if((currentTime == iterator->arrivalTime) && (visited[iterator->pid] == false)){
            if(verbose){
                printf("| %d\t| P%d\t\t| Arrival\t\t| %d\t\t|\n", iterator->arrivalTime, iterator->pid, iterator->burstTime);
            }
            pQueue.push(*iterator);
            visited[iterator->pid] = true;
        }
    }
}

void execute(int &currentTime, const int &timeQuantum, int &completed, int &sumTurnAroundTime, int &sumWaitingTime, int &sumResponseTime, const int &nProcess, vector<process> &pList, queue<process> &pQueue, vector<bool> &visited, vector<process> &finalResult, vector<string> &gantt, vector<int> &time, int &flag, const bool verbose){
    if(!pQueue.empty()){
        if(flag ==1)
            time.push_back(currentTime);

        flag = 0;
        process p = pQueue.front();
        pQueue.pop();
        visited[p.pid] = true;

        if(verbose){
            printf("| %d\t| P%d\t\t| Execute\t\t| %d\t\t|\n", currentTime, p.pid, p.remainingTime);
        }

        gantt.push_back("  P" + to_string(p.pid) + "\t|\0");

        if(p.burstTime == p.remainingTime){
            p.startTime = max(currentTime, p.arrivalTime);
        }

        if(p.remainingTime > timeQuantum){
            int endTime = currentTime + timeQuantum;
            p.remainingTime -= timeQuantum;

            while(currentTime <= endTime){
                checkArrival(currentTime, pList, pQueue, p.pid, visited, verbose);
                currentTime++;
            }
            currentTime--;
            if(verbose){
                printf("| %d\t| P%d\t\t| Preempt\t\t| %d\t\t|\n", currentTime, p.pid, p.remainingTime);
                printf("+------------------------CONTEXT  SWITCH------------------------+\n");
            }

            p.preemptedTimes++;
            time.push_back(currentTime);
            pQueue.push(p);
        }
        else{
            int endTime = currentTime + p.remainingTime;
            p.remainingTime = 0;

            while(currentTime <= endTime){
                checkArrival(currentTime, pList, pQueue, p.pid, visited, verbose);
                currentTime++;
            }

            currentTime--;
            if(verbose){
                printf("| %d\t| P%d\t\t| Complete\t\t| %d\t\t|\n", currentTime, p.pid, p.remainingTime);
                printf("+--------------------------CPU RELEASE--------------------------+\n");
            }
            completed++;

            p.completionTime = currentTime;
            p.turnAroundTime = currentTime - p.arrivalTime;
            p.waitingTime = p.turnAroundTime - p.burstTime;
            p.responseTime = p.startTime - p.arrivalTime;

            sumTurnAroundTime += p.turnAroundTime;
            sumWaitingTime += p.waitingTime;
            sumResponseTime += p.responseTime;

            time.push_back(currentTime);
            finalResult.push_back(p);
        }
    }

    else{
        if(flag == 0){
            if(verbose){
                printf("| %d\t| NULL \t\t| QUEUE EMPTY\t\t|\t\t|\n", currentTime);
                printf("%s", border);
            }
            gantt.push_back("  NULL\t|\0");
            flag = 1;
        }
        currentTime++;
    }
}

void printProcessDetails(vector<process> &result, int nProcess){
    printf("\n\nPROCESS DETAILS:\n");
    printf("%s", border);
    printf("| PID\t| AT\t| BT\t| ST\t| CT\t| TAT\t| WT\t| RT\t|\n");
    printf("%s", border);
    for (int i = 0; i < nProcess; i++){
        process p = result[i];
        printf("| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t|\n", p.pid, p.arrivalTime, p.burstTime, p.startTime, p.completionTime, p.turnAroundTime, p.waitingTime, p.responseTime);
    }
    printf("%s", border);
    printf("\n");
}

void printGantt(vector<string> &gantt, vector<int> &time){
    printf("\nGANTT CHART:\n");
    for(int i=0; i<gantt.size(); i++){
        cout << gantt[i];
    }
    printf("\n");

    for(int i=0; i<gantt.size(); i++){
        cout << time[i] << "\t";
    }
    printf("\n");
}

void printLegend(){
    printf("\n%s", border);
    printf("|\t\t\t     LEGEND: \t\t\t\t|\n");
    printf("| PID\t= Process Id\t\t| AT = Arrival Time\t\t|\n");
    printf("| BT\t= Burst Time\t\t| ST = Start Time\t\t|\n");
    printf("| CT\t= Completion Time\t| TAT = Turn Around Time\t|\n");
    printf("| WT\t= Wait Time\t\t| RT = Response Time\t\t|\n");
    printf("%s", border);
}

int main(int argc, char *argv[]){
    int nProcess = 0, timeQuantum = 0;
    char enable;
    vector<process> pList;
    pList = getProcessList(nProcess, timeQuantum);

    bool verbose = false;
    printf("\nEnable verbose mode? (y/n) :\t");
    getchar();
    enable = getchar();

    if(enable=='y')
        verbose = true;

    vector<bool> visited(nProcess, false);
    int currentTime = 0;
    int completed = 0;
    int sumTurnAroundTime = 0;
    int sumWaitingTime = 0;
    int sumResponseTime = 0;
    int flag = 0;

    queue<process> pQueue;
    vector<process> finalResult;
    vector<string> gantt;
    vector<int> time;

    gantt.push_back("|");
    time.push_back(0);

    if(verbose){
        printf("\n%s", border);
        printf("| Time\t| Process\t| Status\t\t| Remaining BT\t|\n");
        printf("%s", border);
    }

    while(completed != nProcess){
        checkArrival(currentTime, pList, pQueue, -1, visited, verbose);
        execute(currentTime, timeQuantum, completed, sumTurnAroundTime, sumWaitingTime, sumResponseTime, nProcess, pList, pQueue, visited, finalResult, gantt, time, flag, verbose); 
    }

    sort(finalResult.begin(), finalResult.end(), sortProcessByPID);
    
    printProcessDetails(finalResult, nProcess);    

    printGantt(gantt, time);
    printf("\n");
    printf("Average TurnAround Time : %.2f\n", (float)sumTurnAroundTime/nProcess);
    printf("Average Response   Time : %.2f\n", (float)sumResponseTime/nProcess);
    printf("Average Waiting    Time : %.2f\n\n", (float)sumWaitingTime/nProcess);

    printf(">>Press 'q' to return to MENU...\n");
    while(cin.get() != 'q');
}
