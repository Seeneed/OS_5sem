#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Process {
    int id;
    int arrival;
    int burst;
    int priority;
    int start;
    int finish;
    int waiting;
    int turnaround;
};

int main() {
    vector<Process> p = {
        {1, 0, 80, 3}, {2, 20, 45, 5}, {3, 30, 60, 2}, {4, 50, 30, 4}, {5, 70, 90, 1},
        {6, 100, 55, 3}, {7, 120, 75, 2}, {8, 150, 40, 5}, {9, 180, 65, 4}, {10, 200, 85, 1}
    };

    int time = 0;
    vector<Process> done;
    vector<Process> ready;

    while (!p.empty() || !ready.empty()) {
        for (auto it = p.begin(); it != p.end();) {
            if (it->arrival <= time) {
                ready.push_back(*it);
                it = p.erase(it);
            }
            else ++it;
        }
        if (ready.empty()) {
            time++;
            continue;
        }

        sort(ready.begin(), ready.end(), [](auto& a, auto& b) { return a.burst < b.burst; });

        Process cur = ready.front();
        ready.erase(ready.begin());

        cur.start = time;
        time += cur.burst;
        cur.finish = time;
        cur.turnaround = cur.finish - cur.arrival;
        cur.waiting = cur.start - cur.arrival;
        done.push_back(cur);
    }

    double avgWait = 0, avgTurn = 0, totalBurst = 0;

    for (auto& x : done) {
        avgWait += x.waiting;
        avgTurn += x.turnaround;
        totalBurst += x.burst; 

        cout << "P" << x.id << " start=" << x.start << " finish=" << x.finish
            << " wait=" << x.waiting << " turn=" << x.turnaround << endl;
    }

    cout << "------------------------------------------------" << endl;
    cout << "Average waiting: " << avgWait / done.size() << endl;
    cout << "Average execution: " << totalBurst / done.size() << endl;
    cout << "Average turnaround: " << avgTurn / done.size() << endl;
    cout << "Throughput: " << (double)done.size() / done.back().finish << endl;

    return 0;
}