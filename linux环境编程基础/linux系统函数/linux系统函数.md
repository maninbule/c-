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
# lseek函数
```c
标准C库的函数
    #include <stdio.h>
    int fseek(FILE *stream, long offset, int whence);

    Linux系统函数
    #include <sys/types.h>
    #include <unistd.h>
    off_t lseek(int fd, off_t offset, int whence);
        参数：
            - fd：文件描述符，通过open得到的，通过这个fd操作某个文件
            - offset：偏移量
            - whence:
                SEEK_SET
                    设置文件指针的偏移量
                SEEK_CUR
                    设置偏移量：当前位置 + 第二个参数offset的值
                SEEK_END
                    设置偏移量：文件大小 + 第二个参数offset的值
        返回值：返回文件指针的位置, 如果失败会返回-1，并在errno写入错误码


    作用：
        1.移动文件指针到文件头
        lseek(fd, 0, SEEK_SET);

        2.获取当前文件指针的位置
        lseek(fd, 0, SEEK_CUR);

        3.获取文件长度
        lseek(fd, 0, SEEK_END);

        4.拓展文件的长度，当前文件10b, 110b, 增加了100个字节
        lseek(fd, 100, SEEK_END)
        注意：需要写一次数据，末尾不能全为空，否则无效
 ```
 **示例**
 ```c
 #include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {

    int fd = open("hello.txt", O_RDWR);

    if(fd == -1) {
        perror("open");
        return -1;
    }

    // 扩展文件的长度
    int ret = lseek(fd, 100, SEEK_END);
    if(ret == -1) {
        perror("lseek");
        return -1;
    }

    // 写入一个空数据
    write(fd, " ", 1);

    // 关闭文件
    close(fd);

    return 0;
}
 ```
 # stat 查看文件信息
 在终端输入stat 文件，就可以查看文件的详细信息
 ```c
     #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>

    int stat(const char *pathname, struct stat *statbuf);
        作用：获取一个文件相关的一些信息
        参数:
            - pathname：操作的文件的路径
            - statbuf：结构体变量，传出参数，用于保存获取到的文件的信息
        返回值：
            成功：返回0
            失败：返回-1 设置errno

    int lstat(const char *pathname, struct stat *statbuf);
        l表示link，在这个函数中如果pathname是一个软连接，就直接会返回软链接的信息，比如大小，而不是返回指向文件的大小
        参数:
            - pathname：操作的文件的路径
            - statbuf：结构体变量，传出参数，用于保存获取到的文件的信息
        返回值：
            成功：返回0
            失败：返回-1 设置errno
```
**struct stat结构体定义**
```c
struct stat {
           dev_t     st_dev;         /* ID of device containing file */
           ino_t     st_ino;         /* Inode number */
           mode_t    st_mode;        /* File type and mode */
           nlink_t   st_nlink;       /* Number of hard links */
           uid_t     st_uid;         /* User ID of owner */
           gid_t     st_gid;         /* Group ID of owner */
           dev_t     st_rdev;        /* Device ID (if special file) */
           off_t     st_size;        /* Total size, in bytes */
           blksize_t st_blksize;     /* Block size for filesystem I/O */
           blkcnt_t  st_blocks;      /* Number of 512B blocks allocated */

           /* Since Linux 2.6, the kernel supports nanosecond
              precision for the following timestamp fields.
              For the details before Linux 2.6, see NOTES. */

           struct timespec st_atim;  /* Time of last access */
           struct timespec st_mtim;  /* Time of last modification */
           struct timespec st_ctim;  /* Time of last status change */

       #define st_atime st_atim.tv_sec      /* Backward compatibility */
       #define st_mtime st_mtim.tv_sec
       #define st_ctime st_ctim.tv_sec
};
```
## 示例：输出文件的大小
```c
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>

int main(){
    struct stat statbuf;
    int res = stat("a.txt",&statbuf);
    if(res == 1){
        perror("error = ");
        return -1;
    }
    printf("%ld\n",statbuf.st_size);

    return 0;
}
```
## 示例：实现ls -l 文件名功能
要实现的效果
```
ls -l copy.c
-rw-rw-r-- 1 leo leo 631 6月   1 18:14 copy.c
```
```c
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

int main(int argc,char *argv[]){
    if(argc < 2){
        printf("usage: ./ls filename");
        return -1;
    }
    struct stat statbuf;
    int res = stat(argv[1],&statbuf);
    char type[20] = {0};
    switch(statbuf.st_mode & S_IFMT){ // 前4位表示文件类型，这里是取出前4位
        case S_IFSOCK:
            type[0] = 's';
            break;
        case S_IFLNK:
            type[0] = 'l';
            break;
        case S_IFREG:
            type[0] = '-';
            break;
        case S_IFBLK:
            type[0] = 'b';
            break;
        case S_IFDIR:
            type[0] = 'd';
            break;
        case S_IFCHR:
            type[0] = 'c';
            break;
        case S_IFIFO:
            type[0] = 'p';
            break;
        default:
            type[0] = '?';
    }
    char name[9] = "rwxrwxrwx";
    unsigned int nameno[9] = {S_IRUSR,S_IWUSR,S_IXUSR,S_IRGRP,S_IWGRP,S_IXGRP,S_IROTH,S_IWOTH,S_IXOTH};
    for(int i = 0;i<9;i++){
        if(statbuf.st_mode & nameno[i]){
            type[i + 1] = name[i];
        }else{
            type[i + 1] = '-';
        }
    }
    char *username = getpwuid(statbuf.st_uid)->pw_name;
    char *grpname = getgrgid(statbuf.st_gid)->gr_name;
    time_t t = statbuf.st_atime;
    char *t_str = ctime(&t);
    char time_str[100] = {0};
    strncpy(time_str,t_str,strlen(t_str)-1);

    char out[100] = {0};
    sprintf(out,"%s %lu %s %s %ld %s %s",type,statbuf.st_nlink,username,grpname,statbuf.st_size,time_str,argv[1]);
    printf("%s\n",out);

    return 0;
}
/*
各种宏定义：

取前4位的掩码
S_IFMT     0170000   bit mask for the file type bit field

文件类型
S_IFSOCK   0140000   socket
S_IFLNK    0120000   symbolic link
S_IFREG    0100000   regular file
S_IFBLK    0060000   block device
S_IFDIR    0040000   directory
S_IFCHR    0020000   character device
S_IFIFO    0010000   FIFO
-------------------------------------------------------
各种文件权限的宏定义
S_ISUID     04000   set-user-ID bit
S_ISGID     02000   set-group-ID bit (see below)

S_ISVTX     01000   sticky bit (see below)

S_IRWXU     00700   owner has read, write, and execute permission
S_IRUSR     00400   owner has read permission
S_IWUSR     00200   owner has write permission
S_IXUSR     00100   owner has execute permission

S_IRWXG     00070   group has read, write, and execute permission
S_IRGRP     00040   group has read permission
S_IWGRP     00020   group has write permission
S_IXGRP     00010   group has execute permission

S_IRWXO     00007   others  (not  in group) have read, write, and
                    execute permission
S_IROTH     00004   others have read permission
S_IWOTH     00002   others have write permission
S_IXOTH     00001   others have execute permission

*/
```
