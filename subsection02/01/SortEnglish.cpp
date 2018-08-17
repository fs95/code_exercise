#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Comparison function in descending order
bool Compare(const string &a, const string &b) {
    return a > b;
}

int main()
{
    vector<string> strVector;
    ifstream file("input.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            strVector.push_back(line);
        }
        sort(strVector.begin(), strVector.end(), Compare);
        for (const string &item : strVector) {
            cout << item << endl;
        }
        file.close();
    } else {
        cout << "Can't open file!" << endl;
    }
    return 0;
}
