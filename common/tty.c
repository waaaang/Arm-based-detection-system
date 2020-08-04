#include "tty.h"

/*****************************************************************************
 * 函数名：opendev
 *   打开串口设备
 *
 * 参数:
 *    dev     串口设备节点
 *
 * 返回值:
 *    int  文件描述符
 *
 * 注意事项：
 *    无
 *
 ****************************************************************************/
int opendev(char *dev)
{
    int fd = open(dev, O_RDWR);         //  O_NOCTTY   O_NDELAY
    if (-1 == fd) {
        fprintf (stderr, "opendev err: %s\n", strerror(errno));
        exit (EXIT_FAILURE);
    }

    return fd;
}

/*****************************************************************************
 * 函数名：set_port
 *   设置串口通信参数
 *
 * 参数:
 *    fd         串口设备文件描述符
 *    speed      波特率
 *    databits   有效数据位
 *    stopbits   停止位
 *    parity     校验位
 *
 * 返回值:
 *    无
 *
 * 注意事项：
 *    无
 *
 ****************************************************************************/
void set_port(int fd, int speed, int databits, int stopbits, int parity)
{
    set_speed (fd, speed);

    if (!set_parity(fd, databits, stopbits, parity)) {
        fprintf (stderr, "set_parity err: %s\n", strerror(errno));
        exit (EXIT_FAILURE);
    }
}

/*****************************************************************************
 * 函数名：set_speed
 *   设置波特率
 *
 * 参数:
 *    fd         串口设备文件描述符
 *    speed      波特率
 *
 * 返回值:
 *    无
 *
 * 注意事项：
 *    无
 *
 ****************************************************************************/

void set_speed(int fd, int speed)
{
    int i;
    int status;
    struct termios opt;                  //定义了这样一个结构
    /*============================================================
     *波特率设置参数
     *===========================================================*/
    int speed_arr[]= {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300, };

    int name_arr[] = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, };

    tcgetattr(fd, &opt);                 //用来得到机器原端口的默认设置
    for (i = 0; i < sizeof(speed_arr)/sizeof(speed_arr[0]); i++) {
        if (speed == name_arr[i]) {

            tcflush(fd, TCIOFLUSH);      //刷新输入输出缓冲
            cfsetispeed(&opt, speed_arr[i]);	//这里分别设置
            cfsetospeed(&opt, speed_arr[i]);

            //这是立刻把rates设置真正写到串口中去
            status = tcsetattr(fd, TCSANOW, &opt);
            if (status != 0)
                perror("tcsetattr fd1"); //设置错误

            return ;
        }

        tcflush(fd,TCIOFLUSH);           //同上
    }
}

/*****************************************************************************
 * 函数名：set_parity
 *   设置校验位/数据位/停止位
 *
 * 参数:
 *    fd         串口设备文件描述符
 *    databits   有效数据位
 *    stopbits   停止位
 *    parity     校验位
 *
 * 返回值:
 *    无
 *
 * 注意事项：
 *    无
 *
 ****************************************************************************/
int set_parity(int fd, int databits, int stopbits, int parity)
{
    struct termios options;              //定义一个结构
    if (tcgetattr(fd, &options) != 0) {
        perror("SetupSerial 1");
        return 0;
    }

    //修改控制模式，保证程序不会占用串口
    options.c_cflag |= CLOCAL;
    //修改控制模式，使得能够从串口中读取输入数据
    options.c_cflag |= CREAD;
    options.c_cflag &= ~CSIZE;           //这是设置c_cflag选项不按位数据位掩码
    switch (databits){                   /*设置数据位数*/
    case 7:
        options.c_cflag |= CS7;          //设置c_cflag选项数据位为7位
        break;
    case 8:
        options.c_cflag |= CS8;          //设置c_cflag选项数据位为8位
        break;
    default:
        fprintf(stderr,"unsupported data size\n"); //其他的都不支持
        return 0;
    }

    switch (parity){                    //设置奇偶校验，c_cflag和c_iflag有效
    case 'n':
    case 'N':                           //无校验 当然都不选
        options.c_cflag &= ~PARENB;     /* Clear parity enable */
        options.c_iflag &= ~INPCK;      /* Enable parity checking */
        break;
    case 'o':                           //奇校验 其中PARENB校验位有效；PARODD奇校验
    case 'O':
        options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
        options.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    case 'e':
    case 'E':                           //偶校验，奇校验不选就是偶校验了
        options.c_cflag |= PARENB;/* Enable parity */
        options.c_cflag &= ~PARODD;/* 转换为偶效验*/
        options.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    default:
        fprintf(stderr,"Unsupported parity\n");
        return (0);
    }

    /* 设置停止位*/
    switch (stopbits){                  //这是设置停止位数，影响的标志是c_cflag
    case 1:
        options.c_cflag &= ~CSTOPB;     //不指明表示一位停止位
        break;
    case 2:
        options.c_cflag |= CSTOPB;      //指明CSTOPB表示两位，只有两种可能
        break;
    default:
        fprintf(stderr, "unsupported stop bits\n");
        return (0);
    }

    /* Set input parity option */
    if (parity != 'n')                  //这是设置输入是否进行校验
        options.c_iflag |= INPCK;

    options.c_oflag &= ~OPOST;
    options.c_cflag &= ~HUPCL;
    options.c_iflag |= IGNBRK;
    options.c_iflag &= ~(IXON|ICRNL|INPCK);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // 这个地方是用来设置控制字符和超时参数的，一般默认即可。稍微要注意的是c_cc数组的VSTART 和 VSTOP 元素被设定成DC1 和 DC3，代表ASCII 标准的XON和XOFF字符。所以如果在传输这两个字符的时候就传不过去，这时需要把软件流控制屏蔽 options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_cc[VTIME] = 10;          // 设置超时时间
    options.c_cc[VMIN] = 0;

    tcflush(fd,TCIFLUSH);               //刷新和立刻写进去
    if (tcsetattr(fd,TCSANOW,&options) != 0) {
        perror("SetupSerial 3");
        return 0;
    }

    return 1;
}


//int main(int argc, const char *argv[])
//{
//    int fd = opendev("/dev/ttySAC0");
//    set_port(fd, 115200, 8, 1, 'n');

//    while(1)
//    {
//        write(fd, "nihao", 5);
//        sleep(3);
//    }
//    return 0;
//}
//END FILE
