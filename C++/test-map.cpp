/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Test program for c++ snippest to try to out some features.
 *
 */

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <functional>
#include <map>

using namespace std;

int main(int argc, char *argv[])
{
    map<int, string> nmap = { { 0, "Zero" }, { 1, "One" }, { 2, "Two" } };

    cout << nmap[0] << endl;

    return 0;
}
