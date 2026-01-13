#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

struct Process {
    int id;
    int arrival;
    int burst;
    int remaining;
    int waiting;
    int turnaround;
    int finish;
};

int main() {
    vector<Process> p = {
        {1, 0, 80}, {2, 20, 45}, {3, 30, 60}, {4, 50, 30}, {5, 70, 90},
        {6, 100, 55}, {7, 120, 75}, {8, 150, 40}, {9, 180, 65}, {10, 200, 85}
    };

    int quantum = 50;

    queue<int> q;
    int time = 0;
    int completed = 0;
    int n = p.size();

    for (auto& x : p) x.remaining = x.burst;

    while (completed < n) {
        bool added = false;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival <= time && p[i].remaining > 0) {
                bool inQueue = false;
                queue<int> temp = q;
                while (!temp.empty()) {
                    if (temp.front() == i) { inQueue = true; break; }
                    temp.pop();
                }
                if (!inQueue) { q.push(i); added = true; }
            }
        }

        if (q.empty()) { time++; continue; }

        int i = q.front();
        q.pop();

        int exec = min(quantum, p[i].remaining);
        time += exec;
        p[i].remaining -= exec;

        for (int j = 0; j < n; j++) {
            if (p[j].arrival <= time && p[j].remaining > 0) {
                bool inQueue = false;
                queue<int> temp = q;
                while (!temp.empty()) {
                    if (temp.front() == j) { inQueue = true; break; }
                    temp.pop();
                }
                if (!inQueue && j != i) q.push(j);
            }
        }

        if (p[i].remaining > 0) q.push(i);
        else {
            p[i].finish = time;
            p[i].turnaround = p[i].finish - p[i].arrival;
            p[i].waiting = p[i].turnaround - p[i].burst;
            completed++;
        }
    }

    double avgW = 0, avgT = 0, totalBurst = 0; 
    int maxFinish = 0;

    cout << "\nResults:\n";
    for (auto& x : p) {
        avgW += x.waiting;
        avgT += x.turnaround;
        totalBurst += x.burst; 
        if (x.finish > maxFinish) maxFinish = x.finish;

        cout << "P" << x.id
            << "  Wait=" << x.waiting
            << "  Turnaround=" << x.turnaround << endl;
    }

    cout << "\nAverage waiting: " << avgW / n;
    cout << "\nAverage execution: " << totalBurst / n;
    cout << "\nAverage turnaround: " << avgT / n;
    cout << "\nThroughput: " << (double)n / maxFinish << endl;

    return 0;
}