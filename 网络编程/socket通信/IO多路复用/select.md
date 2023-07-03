# select 多路复用
以前使用的是多线程或者多进程来实现多客户端通信，开销比较大
函数介绍在本目录下的pdf文件中

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    //创建一个socket
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        return -1;
    }
    //填写监听信息
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY; //监听所有ip
    //设置端口复用
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    //将socket和监听信息绑定
    int ret = bind(lfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret == -1){
        perror("bind");
        return -1;
    }
    //开始监听
    listen(lfd,8);

    //创建select需要的结构体
    int nfds = lfd;
    fd_set st,st_tmp;
    FD_ZERO(&st);
    FD_SET(lfd,&st); //将监听的socket文件描述符加入进来
    //开始去等待客户端的连接
    while(1){
        st_tmp = st;
        int cnt = select(nfds + 1,&st_tmp,NULL,NULL,NULL);
        if(cnt == -1){
            perror("select");
            exit(-1);
        }else if(cnt == 0){
            continue;
        }else if(cnt > 0){
            //判断有没有新的客户端到达
            if(FD_ISSET(lfd,&st_tmp)){
                struct sockaddr_in addr;
                int  addrlen = sizeof(addr);
                int cfd = accept(lfd,(struct sockaddr *)&addr,&addrlen);
                if(cfd == -1){
                    perror("accept");
                    exit(-1);
                }
                //获取到客户端的socket文件描述符cfd
                FD_SET(cfd,&st);
                if(cfd > nfds) nfds = cfd;
            }
            //判断除监听的socket之外的socket有没有写入
            for(int fd = lfd + 1;fd<=nfds;fd++){
                if(FD_ISSET(fd,&st_tmp)){
                    char buf[1024] = {0};
                    int len = read(fd,buf,sizeof(buf) - 1);
                    if(len == -1){
                        perror("read");
                        continue;
                    }else if(len == 0){
                        //fd断开了连接
                        FD_CLR(fd,&st);
                    }else{
                        printf("服务器收到了来自fd = %d发送过来的数据： %s\n",fd,buf);
                        len = write(fd,buf,strlen(buf) + 1);
                        if(len == -1){
                            perror("write");
                            continue;
                        }
                    }
                }
            }
        }
    }

    return 0;
}
```
