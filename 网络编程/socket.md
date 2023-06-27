# socket函数
客户端将数据发送给服务器，服务器将接收到的数据原样返回
client.c
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(){
    //创建一个socket
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1){
        perror("socket");
        exit(-1);
    }

    //客户端连接到服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    inet_pton(AF_INET,"192.168.222.131",(void *)&addr.sin_addr.s_addr);
    socklen_t addrlen = sizeof(addr);
    int ret = connect(sockfd,(struct sockaddr*)&addr,addrlen);
    if(ret == -1){
        perror("connect");
        exit(-1);
    }
    //开始读写
    char buf[1024] = {0};
    char buf_read[1024] = {0};
    while(1){
        scanf("%s",buf);
        write(sockfd,buf,strlen(buf));
        memset(buf_read,0,sizeof buf_read);
        int len = read(sockfd,buf_read,1024);
        if(len <= 0){
            printf("服务器断开了连接!\n");
            break;
        }
        printf("server: %s\n",buf_read);
    }
    close(sockfd);
    return 0;
}
```

server.c
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
    //创建一个socket
    int lfd = socket(AF_INET,SOCK_STREAM,0); //ipv4 流式传输 TCP协议
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }
    // 给这个socket绑定ip + port
        //设置socketaddr, 在填入ip 和 port的时候需要转换成网络字节序
    struct sockaddr_in sockin;
    sockin.sin_family =  AF_INET;
    sockin.sin_port = htons(9999);
    sockin.sin_addr.s_addr = INADDR_ANY;
    // int ret = inet_pton(AF_INET,"192.168.222.131",(void *)&sockin.sin_addr.s_addr);
    // if(ret != 1){
    //     printf("inet_pton出现错误!\n");
    //     exit(-1);
    // }
    int ret = bind(lfd,(struct sockaddr *)&sockin,sizeof(sockin));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }
    //用这个socket去设置监听,监听队列长度为8
    ret = listen(lfd,8);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }
    //等待客户端的连接,成功等待会得到一个文件描述符，用于读数据和写数据
    struct sockaddr_in client_socket_addr;
    socklen_t addrlen = sizeof(client_socket_addr);
    int client_fd = accept(lfd,(struct sockaddr *)&client_socket_addr,&addrlen);
    if(client_fd == -1){
        perror("accept");
        exit(-1);
    }
    char buf2[20] = {0};
    printf("客户端 ip: %s 端口:%d\n",inet_ntop(AF_INET,&client_socket_addr.sin_addr,buf2,20),client_socket_addr.sin_port);

    //从客户那里读写数据
    char buf[1024] = {0};
    while(1){
        printf("服务器等待客户端发送数据中....\n");
        memset(buf,0,sizeof buf);
        int len = read(client_fd,buf,1024);
        printf("服务器读取到数据: %s\n",buf);
        printf("将数据进行原样返回给客户端\n\n");
        if(len <= 0){
            printf("客户端断开了连接\n");
            break;
        }
        
        write(client_fd,buf,strlen(buf));
    }
    close(client_fd);
    close(lfd);
    



    return 0;
}
```
