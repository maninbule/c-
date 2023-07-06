# socket本地通信
ipc_server.c
```c
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>

int main(){
    int ret;
    if(access("server.sock",F_OK) == 0){
        ret = unlink("server.sock");
        if(ret == -1){
            perror("unlink");
            exit(-1);
        }
    }
    //创建一个用于监听的socket
    int lfd = socket(AF_LOCAL,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }
    //填写socket需要的信息
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path,"server.sock");
    //绑定到socket
    ret = bind(lfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }
    //监听
    ret = listen(lfd,10);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }
    //等待客户端的连接
    struct sockaddr_un addr2;
    socklen_t addrlen;
    int cfd = accept(lfd,(struct sockaddr *)&addr2,&addrlen);
    if(cfd == -1){
        perror("cfd");
        exit(-1);
    }
    while(1){
       
        int arr[100];
        int len = read(cfd,arr,sizeof(arr));
        if(len == -1){
            perror("read");
            break;
        }else if(len == 0){
            printf("客户端已经断开连接\n");
            break;
        }else{
            int sum = 0;
            for(int i = 1;i<=arr[0];i++){
                sum += arr[i];
            }
            printf("接受到数组：");
            for(int i = 0;i<=arr[0];i++){
                printf("%d ",arr[i]);
            }
            printf("\n");
            ret = write(cfd,&sum,sizeof(sum));
            printf("已经将计算结果：%d 发送给客户端\n",sum);
            if(ret == -1){
                perror("write");
                exit(-1);
            }
        }
    }
    close(cfd);
    close(lfd);

    return 0;
}
```
ipc_client.c
```c
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>

int main(){
    int ret;
    if(access("client.sock",F_OK) == 0){
        ret = unlink("client.sock");
        if(ret == -1){
            perror("unlink");
            exit(-1);
        }
    }
    if(ret == -1){
        perror("unlink");
        exit(-1);
    }
    //创建一个用于通信的socket
    int cfd = socket(AF_LOCAL,SOCK_STREAM,0);
    if(cfd == -1){
        perror("socket");
        exit(-1);
    }
    //填写自己的通信信息
    struct sockaddr_un addr1;
    addr1.sun_family = AF_LOCAL;
    strcpy(addr1.sun_path,"client.sock");
    //绑定
    ret = bind(cfd,(struct sockaddr*)&addr1,sizeof(addr1));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }
    //填写服务器socket信息
    struct sockaddr_un addr2;
    addr2.sun_family = AF_LOCAL;
    strcpy(addr2.sun_path,"server.sock");
    ret = connect(cfd,(struct sockaddr *)&addr2,sizeof(addr2));
    if(ret == -1){
        perror("connect");
        exit(-1);
    }
    while(1){
        int arr[100];
        scanf("%d",&arr[0]);
        for(int i = 1;i<=arr[0];i++){
            scanf("%d",&arr[i]);
        }
        ret = write(cfd,arr,sizeof(int) * (arr[0] + 1));
        printf("已经将数据发送给了服务器\n");
        if(ret == -1){
            perror("write");
            exit(-1);
        }
        int sum;
        int len = read(cfd,&sum,sizeof(sum));
        if(len == -1){
            perror("read");
            exit(-1);
        }else if(len == 0){
            printf("服务器断开了连接\n");
            exit(-1);
        }else{
            printf("server: %d\n",sum);
        }
    }
    close(cfd);

    return 0;
}
```
