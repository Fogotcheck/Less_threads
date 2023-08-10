#include "include/headers/main.h"
uint8_t buf[1024];
char comPortName[] = "\\\\.\\COM24";
int main(void)
{
    int ret = 0;
    serialParam_t comPortParam;
    comPortParam.bufRX = buf;
    comPortParam.bufRXSize = sizeof(buf);
    comPortParam.portName = comPortName;
    comPortParam.bufTX = buf;
    comPortParam.bufTXSize = sizeof(buf);
    comPortParam.rxHandler = NULL;
    pthread_t thSerialID;
    pthread_attr_t thSerialAtt;
    pthread_attr_init(&thSerialAtt);
    pthread_create(&thSerialID, &thSerialAtt, SerialThread, &comPortParam);

    char myExit = 0;
    while (1)
    {
        scanf("%c", &myExit);
        if (myExit == 'y')
        {
            ret = pthread_cancel(thSerialID);
            if (ret == 0)
            {
                break;
            }
        }
    }
    ret = pthread_join(thSerialID, NULL);
    printf("Hello world\n");
    return 0;
}
