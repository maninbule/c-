#include <stdio.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>

int main(){
    //创建key
    key_t key = ftok("/home/leo/linux/code",123);
    //创建消息队列
    int msgID = msgget(key,IPC_CREAT | IPC_EXCL | 0664);
    if(msgID == -1){
        perror("msgget");
        return -1;
    }
    //发送消息
    while(1){
        struct {
            long type;
            char data[1024];
        } msg_info = {100,""};
        memset(msg_info.data,0,sizeof(msg_info.data));
        fgets(msg_info.data,sizeof(msg_info.data),stdin);
        char *p = msg_info.data;
        if(strcmp(msg_info.data,"exit\n") == 0){
            printf("退出了进程\n");
            break;
        }
        int ret = msgsnd(msgID,&msg_info,strlen(msg_info.data),0);
        if(ret == -1){
            perror("msgsnd");
        }
    }

    //销毁消息队列
    int ret = msgctl(msgID,IPC_RMID,NULL);
    if(ret == -1){
        perror("msgctl");
        return -1;
    }

    return 0;
}

