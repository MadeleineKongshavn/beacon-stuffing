#include <net/if.h>
#include <linux/nl80211.h>
#include <stdlib.h>
#include </usr/include/libnl3/netlink/netlink.h>

int main() {
    int if_index = if_nametoindex("wlan0"); // Use this wireless interface for scanning.
    struct nl_sock *socket = nl_socket_alloc();

    return EXIT_SUCCESS;
}
