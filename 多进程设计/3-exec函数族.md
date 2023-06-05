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
◼ int execve(const char *filename, char *const argv[], char *const envp[]);
l(list) 参数地址列表，以空指针结尾
v(vector) 存有各参数地址的指针数组的地址
p(path) 按 PATH 环境变量指定的目录搜索可执行文件
e(environment) 存有环境变量字符串地址的指针数组的地址：也就是自己的环境变量目录
```
一般用`execl`和`execlp`比较多，一个是给一个程序的路径来执行，一个是给一个环境变量的程序名来执行

