/* A simple SocketCAN example */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
int soc;
int read_can_port;
int open_port(const char *port)
{
    struct ifreq ifr;
    struct sockaddr_can addr;
    /* open socket */
    soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(soc < 0)
    {
	printf("error!");
        return (-1);
    }
    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, port);
    if (ioctl(soc, SIOCGIFINDEX, &ifr) < 0)
    {
	printf("error!");
        return (-1);
    }
    addr.can_ifindex = ifr.ifr_ifindex;
    fcntl(soc, F_SETFL, O_NONBLOCK);
    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
	printf("binding error!");
        return (-1);
    }

    return 0;
}


/* this is just an example, run in a thread */
void read_port()
{
    struct can_frame frame;
    int recvbytes = 0;
    read_can_port = 1;
    while(read_can_port)
    {
        struct timeval timeout = {1, 0};
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(soc, &readSet);
        if (select((soc + 1), &readSet, NULL, NULL, &timeout) >= 0)
        {
            if (!read_can_port)
            {
		printf("error!");
                break;

            }
            if (FD_ISSET(soc, &readSet))
            {
                recvbytes = read(soc, &frame, sizeof(struct can_frame));
                if(recvbytes)
                { 
                    for (int i = 0; i < frame.can_dlc; i++) 
		    {
                        printf("%02x", frame.data[i]);
                    }
		    printf("\n");
                    
                }
            }
        }
    }
}

void write_port()
{

        struct can_frame frame;	
	frame.can_id = 0x123;
	frame.can_dlc = 8;
	frame.data[0] = 0x12;
	frame.data[1] = 0x34;
	frame.data[2] = 0x56;
	frame.data[3] = 0x78;
	frame.data[4] = 0x91;
	frame.data[5] = 0x01;
	frame.data[6] = 0x11;
	frame.data[7] = 0x21;

	ssize_t nbytes = write(soc, &frame, sizeof(struct can_frame));
	if(nbytes < 1)
	{
		printf("send error!\n");
	}
}
int close_port()
{
	close(soc);
	return 0;
}
int main(void)
{
    int cnt = 0;
    //printf("error!");
    while(cnt < 10 ){
        open_port("can0");
        write_port();
	close_port();
        cnt++;
    }
    //printf("error!");
    return 0;
}
