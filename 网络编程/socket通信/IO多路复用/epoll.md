# epoll
用红黑树来保存要监控的文件描述符，将监控到有变化的文件描述符重新放入一个数组进行返回  
优势： 红黑树在监控的时候查询速度很高， 返回的时候返回的都是有变化的文件描述符  

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>


int main(){
    //创建一个socket
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }
    //设置ip 和 端口
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;

    //绑定socket与ip+端口
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
    //创建epoll的红黑树，数字大于0即可，容量无限的
    int epfd = epoll_create(2000);
    if(epfd == -1){
        perror("epoll_create");
        exit(-1);
    }
    //往epoll红黑树加入监听的socket文件描述符
    struct epoll_event ee;
    ee.events = EPOLLIN;
    ee.data.fd = lfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ee);

    //创建接收epoll返回状态发生变化的文件描述符
    struct epoll_event rev_events[2000];
    while(1){
        //cnt是状态发生改变的文件描述符个数，已经存入了rev_events中了
        int cnt = epoll_wait(epfd,rev_events,2000,-1);
        if(cnt == -1){
            perror("epoll_wait");
            exit(-1);
        }
        for(int i = 0;i<cnt;i++){
            int curfd = rev_events[i].data.fd;
            if(curfd == lfd){
                struct sockaddr_in addr;
                socklen_t len = sizeof(addr);
                int cfd = accept(lfd,(struct sockaddr *)&addr,&len);
                if(cfd == -1){
                    perror("accept");
                    continue;
                }
                //将新连接的客户端文件描述符加入到红黑树中
                struct epoll_event ee;
                ee.events = EPOLLIN;
                ee.data.fd = cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ee);
            }else{
                if(rev_events[i].events & EPOLLIN){
                    char buf[1024] = {0};
                    int len = read(curfd,buf,sizeof(buf));
                    if(len == -1){
                        perror("read");
                        continue;
                    }else if(len == 0){
                        //客户端已经断开,需要从红黑树中删除对应的文件描述符
                        epoll_ctl(epfd,EPOLL_CTL_DEL,curfd,NULL);
                        continue;
                    }
                    printf("接收到数据： %s\n",buf);
                    len = write(curfd,buf,strlen(buf) + 1);
                    if(len == -1){
                        perror("write");
                        continue;
                    }
                }
            }
        }
    }

    return 0;
}

```
