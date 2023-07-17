#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

int main(){
    //创建key
    key_t key = ftok("/home/leo/linux/code",123);    
    //获取消息队列
    int msgID = msgget(key,0);
    if(msgID == -1){
        perror("msgID");
        return -1;
    }
    //接收消息
    while(1){
        struct {
            long type;
            char data[1024];
        } msg_info;
        memset(msg_info.data,0,sizeof(msg_info.data));
        ssize_t ret = msgrcv(msgID,&msg_info,sizeof(msg_info.data)-1,0,IPC_NOWAIT);
        if(ret == -1){
            if(errno == EIDRM){
                printf("消息队列已关闭!\n");
                break;
            }else if(errno == ENOMSG){
                printf("没有消息\n");
                sleep(1);
                continue;
            }else{
                perror("msgrcv");
                break;
            }
        }
        printf("%ld > %s",msg_info.type,msg_info.data);
    }   

    return 0;
}