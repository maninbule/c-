# c语言的IO函数与linux的IO函数的区别
1. 在c语言中的IO操作是带有缓冲区的，当缓存区满或者通过flush函数可以将缓冲区的内容一次性写入到磁盘  
2. c语言是调用了linux的write函数来完成写入的，write函数将缓冲区的内容写入到磁盘，linux的write函数是没有缓冲区的  
3. 如果对实时性要求很高的话，只能直接用系统的write函数  
# 文件描述符
1. 系统每打开一个文件，都会产生一个文件描述符，包括重复打开同一个文件  
2. 在linux内核区中有一个PCB(进程控制块)，有一个长度为1024的文件描述符数组，0是标准输入，1是标准输出，2是标准错误，三者都指向当前的终端  
3. 每打开一个文件，就会在0~1023中选择一个最小且没被占用的文件描述符来进行绑定。
# open函数
## 打开文件
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
## 创建新文件
```c
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>

    int open(const char *pathname, int flags, mode_t mode);
        参数：
            - pathname：要创建的文件的路径
            - flags：对文件的操作权限和其他的设置
                - 必选项：O_RDONLY,  O_WRONLY, O_RDWR  这三个之间是互斥的
                - 可选项：O_CREAT 文件不存在，创建新文件
            - mode：八进制的数，表示创建出的新的文件的操作权限，比如：0775
            最终的权限是：mode & ~umask   umask可以自己设置，比如umask 0022(设置只对当前终端有效)
            mode = 0777  umask = 0002
            0777   ->   111111111
        &   0775   ->   111111101
        ----------------------------
                        111111101
        按位与：0和任何数都为0
        umask的作用就是抹去某些权限。

        flags参数是一个int类型的数据，占4个字节，32位。
        flags 32个位，每一位就是一个标志位。
```

**示例**
```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {

    // 创建一个新的文件
    int fd = open("create.txt", O_RDWR | O_CREAT, 0777);

    if(fd == -1) {
        perror("open");
    }

    // 关闭
    close(fd);

    return 0;
}
```
# 文件读写
```c
#include <unistd.h>
    ssize_t read(int fd, void *buf, size_t count);
        参数：
            - fd：文件描述符，open得到的，通过这个文件描述符操作某个文件
            - buf：需要读取数据存放的地方，数组的地址（传出参数）
            - count：指定的数组的大小
        返回值：
            - 成功：
                >0: 返回实际的读取到的字节数
                =0：文件已经读取完了
            - 失败：-1 ，并且设置errno

    #include <unistd.h>
    ssize_t write(int fd, const void *buf, size_t count);
        参数：
            - fd：文件描述符，open得到的，通过这个文件描述符操作某个文件
            - buf：要往磁盘写入的数据，数据
            - count：要写的数据的实际的大小
        返回值：
            成功：实际写入的字节数,如果写入的字节数小于count,可能是因为磁盘已满或者被中断了
            失败：返回-1，并设置errno
```

**示例：文件拷贝**
```c
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>


int main(){
    //打开读入文件
    int srcfd = open("english.txt",O_RDONLY);
    if(srcfd == -1){
        perror("error is ");
    }
    //打开写入文件
    int desfd = open("english_copy.txt",O_WRONLY|O_CREAT,0777);
    if(desfd == -1){
        perror("error is ");
    }
    //读写部分
    char buf[1024];
    int N = 1024;
    int len = 0;
    while((len = read(srcfd,buf,N)) > 0){
        write(desfd,buf,len);
    }
    //关闭文件
    close(srcfd);
    close(desfd);
    return 0;
}
```

