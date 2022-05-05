#include <cstdlib>
#include <iostream>
#include <queue>

using namespace std;

class process {
public:
	int pno = 0;
	int initial_arrivaltime = 0;
	int arrival_time = 0;
	int burst_time_left = 0;
	int burst_time = 0;
	int temp_burst_time = 0;
	int completion_time = 0;
	int turnaround_time = 0;
	int waiting_time = 0;
	int response_time = 0;
	int priority = 0;

	void process_init(){
		initial_arrivaltime = arrival_time;
		burst_time_left = burst_time;
	}

	void put_completiontime(int time){
		completion_time = time;
		put_turnaroundtime();
		put_waitingtime();
	}

	void put_turnaroundtime(){
		turnaround_time = completion_time - initial_arrivaltime;
	}

	void put_waitingtime(){
		waiting_time = turnaround_time - burst_time;
	}

	void put_responsetime(int time){
		response_time = time - initial_arrivaltime;
	}

	//overloading < wrt arrival_time since we want minimum arrival time to be popped first
	//built in priority queues pop highest value first
	//initially, ready queue is arranged according to arrival time with the process arriving first,being at the front of the queue
	friend bool operator<(const process& p1, const process& p2){
		return p1.arrival_time > p2.arrival_time;
	}
};

priority_queue<process> initialise(){
	priority_queue<process> ready_queue;
	process p;
	int n;
	cout << "Enter number of processes: ";
	cin >> n;
	for(int i=1;i<=n;i++){
		p.pno = i;
		printf("Enter the arrival time for processs p%d :", p.pno);
		cin >> p.arrival_time;
		printf("Enter the burst time                   :");
		cin >> p.burst_time;
		printf("Enter the priority                     :");
		cin >> p.priority;
		printf("\n");
		p.process_init();
		ready_queue.push(p);
	}
	return ready_queue;
}

//function to return the max priority among processes which arrive at a time <= elapsed time
int get_maxpriority(priority_queue<process> prio_q,	int elapsed_time){
	int max = 100;//initialised to a high value
		while (!prio_q.empty() && prio_q.top().arrival_time <= elapsed_time){
			if (prio_q.top().priority < max)
				max = prio_q.top().priority;
			prio_q.pop();
		}
	return max;
}

//function to return the index of process with max priority which arrives at a time <= elapsed time
int get_maxpriority_index(priority_queue<process> prio_q, int elapsed_time){
	int max = 100;
	int index = 0;
	int i=0;
		while (!prio_q.empty() && prio_q.top().arrival_time <= elapsed_time){
			if (prio_q.top().priority < max){
				max = prio_q.top().priority;
				index = i;
			}
			prio_q.pop();
			i++;
		}
	return index; //because last increment is to come out of while loop and not to be considered
}

//function to pop the process at index passed as parameter
process pop_process(priority_queue<process>* original_q, int index){
	priority_queue<process> shrink_q;
	int i;
	process p;

	switch (index){
		case 0:
			p = (*original_q).top();
			(*original_q).pop();
			break;

		default:
			for (i = 0; i < index; i++){
				shrink_q.push((*original_q).top()); //pushing the original queue processes uptill index to a new queue
				(*original_q).pop();
			}
			p = (*original_q).top();
			(*original_q).pop();	//popping the process with specified index

			while (!(*original_q).empty()){ 	
				//pushing remaining processes back to the new queue
				shrink_q.push((*original_q).top());
				(*original_q).pop();
			}
			(*original_q) = shrink_q;
			
			break;
	}
	return p;
}

//core function: that schedules processes based on their priority
priority_queue<process> priority_scheduler(priority_queue<process> ready_queue, queue<process>* gantt){
	int timer = 0;
	int temp;
	process p;
	priority_queue<process> completion_queue;

	while (!ready_queue.empty()){	
		//calculating idle time, if the arrival time of process at front of queue is > timer
		while (timer < ready_queue.top().arrival_time){
			p.temp_burst_time++;
			timer++;
		}

		//pushing idle time on to gantt chart
		if (p.temp_burst_time > 0){
			p.pno = -1;
			p.completion_time = timer;
			(*gantt).push(p);
		}

		p = pop_process(&ready_queue, get_maxpriority_index(ready_queue, timer));

		if (p.arrival_time == p.initial_arrivaltime) //checking if it is the first time that the process is getting CPU, if yes setting response time
			p.put_responsetime(timer);
		temp = get_maxpriority(ready_queue, timer);


		//the same popped process continues getting CPU UNTILL it finishes execution OR 
		//(ready queue is not empty,process at front of ready queue has arrival_time < timer and  priority > current process priority)
		while (p.burst_time_left > 0 && (ready_queue.empty() || timer < ready_queue.top().arrival_time|| p.priority <= get_maxpriority(ready_queue, timer))){
			p.temp_burst_time++;
			p.burst_time_left--;
			timer++;
		}

		if (p.burst_time_left == 0){
			p.arrival_time = p.initial_arrivaltime;		//arrival time updated back to initial arrival time
			p.put_completiontime(timer);
			(*gantt).push(p);
			p.temp_burst_time = 0;
			completion_queue.push(p);
		}

		else{
			p.arrival_time = timer;			//new arrival time for p after it is pre emted by a higher priority process 
			p.completion_time = timer;		//this is updated, to show how much fraction is completed in gantt chart 
			(*gantt).push(p);
			p.temp_burst_time = 0;
			ready_queue.push(p);
		}
	}
	return completion_queue;
}

double return_Tot_completionT(priority_queue<process> pq){
	double total = 0;
	while (!pq.empty()){
		total += pq.top().completion_time;
		pq.pop();
	}
	return total;
}

double return_Tot_turnaroundT(priority_queue<process> pq){
	double total = 0;
	while (!pq.empty()) 
	{
		total += pq.top().turnaround_time;
		pq.pop();
	}
	return total;
}

double return_Tot_waitingtime(priority_queue<process> pq){
	double total = 0;
	while (!pq.empty()) 
	{
		total += pq.top().waiting_time;
		pq.pop();
	}
	return total;
}

double return_Tot_responsetime(priority_queue<process> pq){
	double total = 0;
	while (!pq.empty()) 
	{
		total += pq.top().response_time;
		pq.pop();
	}
	return total;
}

void process_table(priority_queue<process> pq){
	int i = 0, gap, num = pq.size();
	priority_queue<process> tempq = pq;
	double time;
	cout << "+-------------+--------------";
	cout << "+------------+-----------------";
	cout << "+-----------------+--------------+---------------+";
	cout << endl;
	cout << "| Process No. | Arrival Time ";
	cout << "| Burst Time | Completion Time ";
	cout << "| Turnaround Time | Waiting Time | Response Time |";
	cout << endl;
	cout << "+-------------+--------------";
	cout << "+------------+-----------------";
	cout << "+-----------------+--------------+---------------+";
	cout << endl;
	
	while (!pq.empty()){
		gap = to_string(pq.top().pno).length();
		cout << '|' << string(6 - gap / 2 - gap % 2, ' ') << pq.top().pno << string(7 - gap / 2, ' ');
		
		gap = to_string(pq.top().initial_arrivaltime).length();
		cout << '|' << string(7 - gap / 2 - gap % 2, ' ') << pq.top().initial_arrivaltime << string(7 - gap / 2, ' ');
		
		gap = to_string(pq.top().burst_time).length();
		cout << '|' << string(6 - gap / 2 - gap % 2, ' ') << pq.top().burst_time<< string(6 - gap / 2, ' ');
		
		gap = to_string(pq.top().completion_time).length();
		cout << '|' << string(8 - gap / 2 - gap % 2, ' ') << pq.top().completion_time << string(9 - gap / 2, ' ');
		
		gap = to_string(pq.top().turnaround_time).length();
		cout << '|' << string(8 - gap / 2 - gap % 2, ' ') << pq.top().turnaround_time << string(9 - gap / 2, ' ');
		
		gap = to_string(pq.top().waiting_time).length();
		cout << '|' << string(7 - gap / 2 - gap % 2, ' ') << pq.top().waiting_time << string(7 - gap / 2, ' ');
		
		gap = to_string(pq.top().response_time).length();
		cout << '|' << string(7 - gap / 2 - gap % 2, ' ') << pq.top().response_time << string(8 - gap / 2, ' ');
		
		cout << "|\n";
		pq.pop();
	}

	cout << "+-------------+--------------";
	cout << "+------------+-----------------";
	cout << "+-----------------+--------------+---------------+";
	cout << endl;
	
	time = return_Tot_completionT(tempq);
	cout << "\nTotal completion time \t: " << time << endl;
	cout << "Average completion time : " << time / num << endl;
	
	time = return_Tot_turnaroundT(tempq);
	cout << "\nTotal turnaround time \t: " << time << endl;
	cout << "Average turnaround time : " << time / num << endl;
	
	time = return_Tot_waitingtime(tempq);
	cout << "\nTotal waiting time \t: " << time << endl;
	cout << "Average waiting time \t: " << time / num << endl;
	
	time = return_Tot_responsetime(tempq);
	cout << "\nTotal response time \t: " << time << endl;
	cout << "Average response time \t: " << time / num << endl;
}

void ganttchart(queue<process> gantt){
	int temp, prev = 0;
	queue<process> newGantt = gantt;

	cout << "\n\nGantt Chart : \n\n+";
	
	while (!newGantt.empty()){
		cout << string(to_string(newGantt.front().pno).length() + (newGantt.front().pno != -1) + 2 * newGantt.front().temp_burst_time, '-') << "+";
		newGantt.pop();
	}

	cout << "\n|";
	newGantt = gantt;
	while (!newGantt.empty()){
		cout << string(newGantt.front().temp_burst_time, ' ');
		if (newGantt.front().pno == -1)
			cout << "--" << string(newGantt.front().temp_burst_time, ' ') << '|';
		else
			cout << "P" << newGantt.front().pno	<< string(newGantt.front().temp_burst_time, ' ') << '|';
		newGantt.pop();
	}

	newGantt = gantt;
	cout << "\n+";
	while (!newGantt.empty()){
		cout << string(to_string(newGantt.front().pno).length() + (newGantt.front().pno != -1) + 2 * newGantt.front().temp_burst_time, '-') << "+";
		newGantt.pop();
	}

	newGantt = gantt;
	cout << "\n0";
	while (!newGantt.empty()){
		temp = to_string(newGantt.front().completion_time).length();
		cout << string(to_string(newGantt.front().pno).length() + (newGantt.front().pno != -1) + 2 * newGantt.front().temp_burst_time- temp / 2 - prev, ' ') << newGantt.front().completion_time;
		prev = temp / 2 - temp % 2 == 0;
		newGantt.pop();
	}
	cout << "\n\n";
}

int main(){
	priority_queue<process> ready_queue, completion_queue;
	queue<process> gantt;
	ready_queue = initialise();
	completion_queue = priority_scheduler(ready_queue, &gantt);
	process_table(completion_queue);
	ganttchart(gantt);
	
	printf(">>Press 'q' to return to MENU...\n");
    while(cin.get() != 'q');
	return 0;
}
