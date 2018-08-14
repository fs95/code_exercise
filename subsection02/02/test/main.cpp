#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//中文字符，

int strcoll_wrapper(const void *v1, const void *v2)
{
    const char **s1 = (const char **)v1;
    const char **s2 = (const char **)v2;

    assert(v1 != NULL);
    assert(v2 != NULL);

    return strcoll(*s1, *s2);
}

int main(int argc, char *argv[])
{
    const char *names[] = {
            "啊丫丫",
            "有了",
            "有",
            "从发现",
            "吧",
            "的",
            "你"
    };

    for (int i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        puts(names[i]);
    // puts("\n");
    //    char * temp = setlocale(LC_COLLATE, "zh-CN");
    char * temp = setlocale(LC_COLLATE, "zh_CN.UTF-8"); //此设置影响strcoll函数判断
    if (temp != NULL) {
        puts(temp);
    } else {
        puts("设置失败\n");
    }
    printf("%ld", strlen("呵t呵"));
    qsort(names, sizeof(names)/sizeof(names[0]), sizeof(names[0]), strcoll_wrapper);
    for (int i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        puts(names[i]);

    return 0;
}
