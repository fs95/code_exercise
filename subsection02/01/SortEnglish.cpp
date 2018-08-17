#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Comparison function in descending order
bool compare(const string &a, const string &b) {
    return a > b;
}

int main()
{
    vector<string> str_vector;
    ifstream file("input.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            str_vector.push_back(line);
        }
        sort(str_vector.begin(), str_vector.end(), compare);
        for (const string &item : str_vector) {
            cout << item << endl;
        }
        file.close();
    } else {
        cout << "Can't open file!" << endl;
    }
    return 0;
}
