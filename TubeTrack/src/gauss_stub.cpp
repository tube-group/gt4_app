#include <iostream>

// 告诉链接器，这几个 C 语言符号由我们自己提供，避免 ld 报 undefined reference
extern "C" {
    // 1. 补齐 lo_lseek64
    long long lo_lseek64(void* conn, int fd, long long offset, int whence) {
        std::cerr << "[GaussStub Warning] lo_lseek64 called!" << std::endl;
        return -1; 
    }

    // 2. 补齐 lo_tell64
    long long lo_tell64(void* conn, int fd) {
        return -1;
    }

    // 3. 补齐 lo_truncate64
    int lo_truncate64(void* conn, int fd, long long len) {
        return -1;
    }

    // 4. 补齐 PQconninfo
    void* PQconninfo(void* conn) {
        return nullptr;
    }

    // 5. 补齐 PQencryptPasswordConn
    char* PQencryptPasswordConn(void* conn, const char* passwd, const char* user, const char* algorithm) {
        std::cerr << "[GaussStub Warning] PQencryptPasswordConn called!" << std::endl;
        return nullptr;
    }
}