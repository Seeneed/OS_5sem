#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Process {
    int id, arrival, burst, priority;
    int remaining = 0;
    int start = -1, finish = 0, waiting = 0, turnaround = 0;
};

int main() {
    vector<Process> p = {
        {1, 0, 80, 3}, {2, 20, 45, 5}, {3, 30, 60, 2}, {4, 50, 30, 4}, {5, 70, 90, 1},
        {6, 100, 55, 3}, {7, 120, 75, 2}, {8, 150, 40, 5}, {9, 180, 65, 4}, {10, 200, 85, 1}
    };

    const int quantum = 50;
    int time = 0;
    vector<Process> ready, done;

    for (auto& x : p) x.remaining = x.burst;

    while (!p.empty() || !ready.empty()) {
        for (int i = 0; i < p.size();) {
            if (p[i].arrival <= time) {
                ready.push_back(p[i]);
                p.erase(p.begin() + i);
            }
            else i++;
        }

        if (ready.empty()) { time++; continue; }

        sort(ready.begin(), ready.end(), [](const Process& a, const Process& b) {
            return a.priority > b.priority;
            });

        Process cur = ready[0];
        if (cur.start == -1) cur.start = time;

        int run = min(quantum, cur.remaining);
        cur.remaining -= run;
        time += run;

        for (int i = 0; i < p.size();) {
            if (p[i].arrival <= time) {
                ready.push_back(p[i]);
                p.erase(p.begin() + i);
            }
            else i++;
        }

        ready.erase(ready.begin());
        if (cur.remaining > 0) {
            ready.push_back(cur);
        }
        else {
            cur.finish = time;
            cur.turnaround = cur.finish - cur.arrival;
            cur.waiting = cur.turnaround - cur.burst;
            done.push_back(cur);
        }
    }

    double avgW = 0, avgT = 0, totalBurst = 0; 
    int maxFinish = 0;

    for (auto& x : done) {
        avgW += x.waiting;
        avgT += x.turnaround;
        totalBurst += x.burst; 

        if (x.finish > maxFinish) maxFinish = x.finish; 

        cout << "P" << x.id << " wait=" << x.waiting << " turn=" << x.turnaround << endl;
    }

    cout << "Average waiting: " << avgW / done.size() << endl;
    cout << "Average execution: " << totalBurst / done.size() << endl;
    cout << "Average turnaround: " << avgT / done.size() << endl;
    cout << "Throughput: " << (double)done.size() / maxFinish << endl;

    return 0;
}