#include <iostream>
#include "priority_queue.hpp"
using namespace std;

int main()
{
    sjtu::priority_queue<int> que;
    for (int i = 1; i <= 10; i++)
    {
        int x = rand();
        cout << x << " ";
        que.push(x);
    }
    cout << "end" << endl;
    while (que.size() >= 1)
    {
        cout << que.top() << endl;
        que.pop();
        cout << que.size() << endl;
    }
    return 0;
}

