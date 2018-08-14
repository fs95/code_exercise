//#include <iostream>
//#include <fstream>
//#include <string>
//#include <vector>
//#include <algorithm>
//#include <locale>
//
//int main()
//{
//    vector<string> str_vector;
//    ifstream file("../../input.txt");
//    if (file.is_open()) {
//        string line;
//        while (getline(file, line)) {
//            str_vector.push_back(line);
//        }
//        sort(str_vector.begin(), str_vector.end(), compare);
//        for (const auto &item : str_vector) {
//            cout << item << endl;
//        }
//    } else {
//        cout << "Can't open file!" << endl;
//    }
//    return 0;
//}

#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winnt.h>

int strcoll_wrapper(const void *v1, const void *v2)
{
    const char **s1 = (const char **)v1;
    const char **s2 = (const char **)v2;

    assert(v1 != NULL);
    assert(v2 != NULL);

    return strcoll(*s1, *s2);
}

int main(void)
{
    int i = 0;
    const char *names[] = {
            "赵",
            "钱",
            "李",
            "孙",
    };

    for (i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        puts(names[i]);

    setlocale(LC_COLLATE, SORT_CHINESE_PRCP);
//    setlocale(LC_ALL, );

    qsort(names, 4, sizeof(names[0]), strcoll_wrapper);

    for (i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        puts(names[i]);

    return 0;
}
