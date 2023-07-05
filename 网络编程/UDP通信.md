# UDP
udp_server.c
```c
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(){
    //创建一个socket用于通信
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }
    //填写要监听的ip 和 端口
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;


    //绑定
    int ret = bind(fd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }
    while (1){
        int arr[100];
        struct sockaddr_in src_addr;
        socklen_t addrlen = sizeof(src_addr);
        ret = recvfrom(fd,arr,sizeof(arr),0,(struct sockaddr *)&src_addr,&addrlen);
        if(ret == -1){
            perror("recvfrom");
            continue;
        }
        printf("收到的数组为： ");
        for(int i = 0;i<=arr[0];i++){
            printf("%d ",arr[i]);
        }
        printf("\n");
        int len = arr[0];
        int sum = 0;
        for(int i = 1;i<=len;i++){
            sum += arr[i];
        }
        arr[0] = sum;
        ret = sendto(fd,arr,sizeof(int),0,(struct sockaddr *)&src_addr,addrlen);
        if(ret == -1){
            perror("sendto");
            continue;
        }
    }
    return 0;
}
```
udp_client.c
```c
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

int main(){
    //创建用于发送数据的socket
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }
    //填写服务器sockaddr信息
    struct sockaddr_in addr;
    addr.sin_family =  AF_INET;
    addr.sin_port = htons(9999);
    socklen_t len = sizeof(addr);
    int ret = inet_pton(AF_INET,"192.168.222.131",&addr.sin_addr.s_addr);
    if(ret == -1){
        perror("inet_pton");
        exit(-1);
    }
    
    while(1){
        int arr[100];
        scanf("%d",&arr[0]);
        for(int i = 1;i<=arr[0];i++){
            scanf("%d",&arr[i]);
        }
        ret = sendto(fd,arr,sizeof(int) * arr[0] + 4,0,(struct sockaddr *)&addr,sizeof(addr));
        if(ret == -1){
            perror("sendto");
            continue;
        }
        ret = recvfrom(fd,arr,sizeof(int),0,(struct sockaddr *)&addr,&len);
        if(ret == -1){
            perror("recvfrom");
            continue;
        }
        printf("server: sum = %d\n",arr[0]);
    }

    return 0;
}
```
