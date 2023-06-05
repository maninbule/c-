# exec函数族介绍
对于fork函数是通过复制父进程的内容，但是一般我们不需要子进程和父进程一样  
如果我们想要父进程创建的子进程，能够运行一个我以前写好的程序，就需要用execl函数族来执行

◼ exec 函数族的作用是根据指定的文件名找到可执行文件，并用它来取代调用进程的内容，换句话说，就是在调用进程内部执行一个可执行文件。  

◼ exec 函数族的函数执行成功后不会返回，因为调用进程的实体，包括代码段，数据段和堆栈等都已经被新的内容取代，只留下进程 ID 等一些表面上的信息仍保持原样，
颇有些神似“三十六计”中的“金蝉脱壳”。看上去还是旧的躯壳，却已经注入了新的灵魂。只有调用失败了，它们才会返回 -1，从原程序的调用点接着往下执行。


创建的子进程，通过执行execl函数族，将新的程序的用户区(栈区、堆区、bss区、 数据区、代码区……) 来替换自己的用户区，内核区保持不变  
```c
◼ int execl(const char *path, const char *arg, .../* (char *) NULL */);
◼ int execlp(const char *file, const char *arg, ... /* (char *) NULL */);
◼ int execle(const char *path, const char *arg, .../*, (char *) NULL, char * 
const envp[] */);
◼ int execv(const char *path, char *const argv[]);
◼ int execvp(const char *file, char *const argv[]);
◼ int execvpe(const char *file, char *const argv[], char *const envp[]);
◼ int execve(const char *filename, char *const argv[], char *const envp[]);------------linux的函数，其余的是c语言的，上面的函数都是基于这个函数来写的
l(list) 参数地址列表，以空指针结尾
v(vector) 存有各参数地址的指针数组的地址
p(path) 按 PATH 环境变量指定的目录搜索可执行文件
e(environment) 存有环境变量字符串地址的指针数组的地址：也就是自己的环境变量目录
```
一般用`execl`和`execlp`比较多，一个是给一个程序的路径来执行，一个是给一个环境变量的程序名来执行

```c
    #include <unistd.h>
    int execl(const char *path, const char *arg, ...);
        - 参数：
            - path:需要指定的执行的文件的路径或者名称
                相对路径：a.out 绝对路径：/home/nowcoder/a.out ----------推荐使用绝对路径
                ./a.out hello world

            - arg:是执行可执行文件所需要的参数列表
                第一个参数一般没有什么作用，为了方便，一般写的是执行的程序的名称
                从第二个参数开始往后，就是程序执行所需要的的参数列表。
                参数最后需要以NULL结束（哨兵）

        - 返回值：
            只有当调用失败，才会有返回值，返回-1，并且设置errno
            如果调用成功，没有返回值。
```
## 示例：使用execl调用一个程序
hello.c 被调用的程序
```c
#include <stdio.h>
#include <unistd.h>

int main(){
    printf("hello world: pid = %d ppid = %d\n",getpid(),getppid());
    return 0;
}
```
execl.c 执行调用的程序
```c
#include <unistd.h>
#include <stdio.h>

int main(){
    pid_t pid = fork();
    if(pid > 0){
        printf("father process: pid = %d ppid = %d\n",getpid(),getppid());
        sleep(1);
    }else if(pid == 0){
        printf("child process: pid = %d ppid = %d\n",getpid(),getppid());
        execl("hello","hello",NULL);
    }
    for(int i = 0;i<3;i++){
        printf("%d\n",i);
    }
    return 0;
}
```
**输出**
```
leo@leo-virtual-machine:~/linux/process/exec$ ./execl 
father process: pid = 3983 ppid = 3420
child process: pid = 3984 ppid = 3983
hello world: pid = 3984 ppid = 3983
0
1
2
```
可以看到hello程序和子进程是一样的pid和ppid
