//#include/net/cfg80211.h
#include <errno.h>
//#include <netlink/errno.h>
//#include <netlink/netlink.h>
//#include <netlink/genl/genl.h>
#include <linux/nl80211.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/ethernet.h>
//#include <net80211/ieee80211_ioctl.h>
#include <stdio.h> //includes print() command
#include <string.h>
#include <stdlib.h> // Includes the used macro codes EXIT_SUCCESS and EXIT_FAILURE for the conventional status value for success and failure, respectively. 

//#include "netlink/netlink.h"
//#include "netlink/genl/genl.h"
//#include "netlink/genl/ctrl.h"
#include <net/if.h>

//copy this from iw
#include <linux/nl80211.h>

int main(int argc, char** argv)
{

//        char strings[] = "HELLO";
//	int l = 15;
        int* k = 4;
//        k = &l;

	printf("%d", k);

///	const char* s = "Hello";
//      const char g = "Hei";
//	const char* s = "Hello";
//    	printf("\t%s\t ", s);
//        const char* g = "hei";
//        g = "s";
//    	printf("\t%s\t ", s);
//    	printf("\t%s\t ", g);


//    	printf("\t.%10s.\n\t.%-10s.\n\t.%*s.\n", s, s, 10, s);


	return EXIT_SUCCESS;
}
