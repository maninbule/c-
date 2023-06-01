# c语言的IO函数与linux的IO函数的区别
1. 在c语言中的IO操作是带有缓冲区的，当缓存区满或者通过flush函数可以将缓冲区的内容一次性写入到磁盘  
2. c语言是调用了linux的write函数来完成写入的，write函数将缓冲区的内容写入到磁盘，linux的write函数是没有缓冲区的  
3. 如果对实时性要求很高的话，只能直接用系统的write函数  
# 文件描述符
1. 系统每打开一个文件，都会产生一个文件描述符，包括重复打开同一个文件  
2. 在linux内核区中有一个PCB(进程控制块)，有一个长度为1024的文件描述符数组，0是标准输入，1是标准输出，2是标准错误，三者都指向当前的终端  
3. 每打开一个文件，就会在0~1023中选择一个最小且没被占用的文件描述符来进行绑定。
# open函数
```c
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>

    // 打开一个已经存在的文件
    int open(const char *pathname, int flags);
        参数：
            - pathname：要打开的文件路径
            - flags：对文件的操作权限设置还有其他的设置
              O_RDONLY,  O_WRONLY,  O_RDWR  这三个设置是互斥的
        返回值：返回一个新的文件描述符，如果调用失败，返回-1

    errno：属于Linux系统函数库，库里面的一个全局变量，记录的是最近的错误号。

    #include <stdio.h>
    void perror(const char *s);作用：打印errno对应的错误描述
        s参数：用户描述，比如hello,最终输出的内容是  hello:xxx(实际的错误描述)
    

    // 创建一个新的文件
    int open(const char *pathname, int flags, mode_t mode);
```
**示例**
```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {

    // 打开一个文件
    int fd = open("a.txt", O_RDONLY);

    if(fd == -1) {
        perror("open");
    }
    // 读写操作

    // 关闭
    close(fd);

    return 0;
}
```
