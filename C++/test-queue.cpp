/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Test program for c++ snippest to try to out some features.
 *
 */

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <iterator>
#include <queue>
#include <forward_list>

using namespace std;

int main(int argc, char *argv[])
{
    list<int> l = {1, 2, 3};
    queue<int, list<int>> q(l);

    while (! q.empty())
    {
        cout << q.front() << endl;
        q.pop();
    }

    return 0;
}
