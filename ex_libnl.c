#include <net/if.h>
#include <linux/nl80211.h>
#include <stdlib.h>

#include </usr/include/libnl3/netlink/netlink.h> //#include "netlink/netlink.h"
#include </usr/include/libnl3/netlink/genl/genl.h> //#include "netlink/genl/genl.h"
#include </usr/include/libnl3/netlink/genl/ctrl.h> //#include "netlink/genl/ctrl.h"

static int expectedId;

int main() {
    int if_index = if_nametoindex("wlan0"); // Use this wireless interface for scanning.
    struct nl_sock *socket = nl_socket_alloc();

    //connect to generic netlink
//    genl_connect(socket);


    //find the nl80211 driver ID
//    expectedId = genl_ctrl_resolve(socket, "nl80211");




    return EXIT_SUCCESS;
}
