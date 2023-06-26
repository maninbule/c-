# 点分十进制和整数形式的转换
```c
    #include <arpa/inet.h>
    // p:点分十进制的IP字符串，n:表示network，网络字节序的整数
    int inet_pton(int af, const char *src, void *dst);
        af:地址族： AF_INET  AF_INET6
        src:需要转换的点分十进制的IP字符串
        dst:转换后的结果保存在这个里面

    // 将网络字节序的整数，转换成点分十进制的IP地址字符串
    const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
        af:地址族： AF_INET  AF_INET6
        src: 要转换的ip的整数的地址
        dst: 转换成IP地址字符串保存的地方
        size：第三个参数的大小（数组的大小）
        返回值：返回转换后的数据的地址（字符串），和 dst 是一样的
```

**示例：点分十进制和网络字节序的转换**
```c
#include <stdio.h>
#include <arpa/inet.h>

int main(){
    //客户端ip点分十进制转换成网络字节序
    char *ip = "192.168.0.1";
    char net_ip[4] = "";
    int ret = inet_pton(AF_INET,ip,(void *)net_ip);
    if(ret != 1){
        printf("error");
        return -1;
    }
    unsigned char *p = net_ip;
    printf("192.168.0.1转换成网络字节序是：%d %d %d %d\n",*(p),*(p+1),*(p+2),*(p+3));
    //192.168.0.1转换成网络字节序是：192 168 0 1 (高位在前)

    //将网络字节序转换成客户端ip的点分十进制
    char client_ip[16] = {0};
    const char *rets = inet_ntop(AF_INET,(void *)net_ip,client_ip,16);
    printf("重新转回来后的点分十进制为%s\n",rets);

    return 0;
}
```
