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

    //printf("error!");
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

    //printf("error!");
    return 0;
}


/* this is just an example, run in a thread */
void read_port()
{
    struct can_frame frame_rd;
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
                recvbytes = read(soc, &frame_rd, sizeof(struct can_frame));
		//printf("error!");
                if(recvbytes)
                {
		    //printf("data = %02x\n", frame_rd.data[1]);
                    
                    for (int i = 0; i < frame_rd.can_dlc; i++) {
                        printf("%02x ", frame_rd.data[i]);
                    }
		    printf("\n");
                    
                }
            }
        }
    }
}

int close_port()
{
	close(soc);
	return 0;
}
int main(void)
{
    //printf("error!");
    open_port("can0");
    read_port();
    //printf("error!");
    return 0;
}
