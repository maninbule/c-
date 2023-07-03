# poll IO多路复用
为了解决select 只能有1024个文件描述符，poll采用了用数组来存储文件描述符，数组想要多大就有多大  
在时间复杂度上差不多，这个数组虽然是按顺序存储的fd，但是有些fd退出之后，就会产生空隙  
**poll是对select的一次改进**

缺点： 需要进行将数组拷贝到内核态使用，要使用O(n)遍历数组

```c
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>

#define maxn 1024
int main(){
    //创建一个socket
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }
    //填写sockaddr信息
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;
    //填写端口复用
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    //绑定socket与ip + 端口
    int ret = bind(lfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }
    //开始监听
    ret = listen(lfd,8);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }
    //创建pollfd数组
    struct pollfd fds[maxn];
    int nfds = 0;
    for(int i = 0;i<maxn;i++){
        fds[i].fd = -1;
        fds[i].events = POLLIN;// 监听读入
    }
    fds[0].fd = lfd;//填入socket监听的文件描述符
    while(1){
        int cnt = poll(fds,nfds + 1,-1);
        if(cnt == -1){
            perror("poll");
            exit(-1);
        }
        if(fds[0].revents & POLLIN){ //已经出现了POLLIN事件了，说明有新的客户端到达
            struct sockaddr addr;
            socklen_t addrlen = sizeof(addr);
            int cfd = accept(lfd,(struct sockaddr *)&addr,&addrlen);
            if(cfd == -1){
                perror("accept");
                exit(-1);
            }
            int idx = -1;//这里最好放在accept前面，万一没有空闲的位置了，只要没有accept还可以将客户端的请求连接缓存着
            for(int i = 1;i<maxn;i++){
                if(fds[i].fd == -1){
                    fds[i].fd = cfd;
                    fds[i].events = POLLIN;
                    idx = i;
                    break;
                }
            }
            if(idx != -1){
                nfds = nfds > idx ? nfds : idx;
            }
        }
        for(int i = 1;i<=nfds;i++){
            if(fds[i].fd != -1 && fds[i].revents & POLLIN){
                char buf[1024] = {0};
                int len = read(fds[i].fd,buf,sizeof(buf)-1);
                if(len == -1){
                    perror("read");
                    exit(-1);
                }else if(len == 0){
                    //客户端断开了连接
                    fds[i].fd = -1;
                    continue;
                }
                printf("服务器接收到数据: %s\n",buf);
                ret = write(fds[i].fd,buf,strlen(buf) + 1);
                if(ret == -1){
                    perror("write");
                    exit(-1);
                }
            }
        }
    }

    return 0;
}
```
