#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

long getDirSize(char const *dirPath, int *large_then1M);
unsigned long getFileSize(char const *filePath);

int main(int argc, char const *argv[])
{

    if (argc < 2) {
        cout << "Error: Please input a directory." << endl;
        exit(1);
    }

    int large_then1M = 0;
    long size = getDirSize(argv[1], &large_then1M);

    cout << "The total size of the $1 directory: " << size << "[byte]" << endl;
    cout << "The number of files larger than 1M: " << large_then1M << endl;

    return 0;
}

long getDirSize(char const *dirPath, int *large_then1M) {

    DIR *dirp = opendir(dirPath);
    if (dirp == NULL) {
        perror("Error");
        return 0;
    } else {
        dirent *entry = NULL;
        long size = 0;

        while (entry = readdir(dirp)) {
            string fileName(entry->d_name);
            if (fileName == "." || fileName == "..") {
                continue;
            } else {
                string subFilePath = string(dirPath) + "/" + string(fileName);
                if (entry->d_type == DT_REG) {
                    long fileSize = getFileSize(subFilePath.c_str());
                    size += fileSize;
                    if (fileSize > 1024 * 1024) {
                        ++(*large_then1M);
                    }
                } else if (entry->d_type == DT_DIR) {
                    size += getDirSize(subFilePath.c_str(), large_then1M);
                }
            }
        }
        return size;
    }
}

// 
unsigned long getFileSize(char const *filePath) {
    struct stat info;  
    stat(filePath, &info);
    return info.st_size;
}
