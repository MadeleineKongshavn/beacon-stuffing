#include <net/if.h>
#include <linux/nl80211.h>
#include <linux/genetlink.h>
#include <stdlib.h>
#include <netlink/genl/genl.h>		//genl_connect(socket) heregenl_connect
#include <netlink/genl/ctrl.h>
#include <linux/genetlink.h>

static int expectedId;

static int nlCallback(struct nl_msg* msg, void* arg)
{
    struct nlmsghdr* ret_hdr = nlmsg_hdr(msg);
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];

    if (ret_hdr->nlmsg_type != expectedId)
    {
        // what is this??
        return NL_STOP;
    }

    struct genlmsghdr *gnlh = (struct genlmsghdr*) nlmsg_data(ret_hdr);

    nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
              genlmsg_attrlen(gnlh, 0), NULL);

    if (tb_msg[NL80211_ATTR_IFTYPE]) {
        int type = nla_get_u32(tb_msg[NL80211_ATTR_IFTYPE]);

        printf("Type: %d", type);
    }
    return 0;
}
int main()
{
	//int if_index = if_nametoindex("wlan0"); // Use this wireless interface for scanning.
   	//Allocate a new netlink socket, Retuns: Newly allocated netlink socket or NULL.
	struct nl_sock *socket = nl_socket_alloc();
	if (socket != NULL)
	{
	    	//This function expects a struct nl_socket object previously allocated 
		//via nl_socket_alloc(). It calls nl_connect() to create the local socket file
		// descriptor and binds the socket to the NETLINK_GENERIC Netlink protocol.
		//0 on success or a negative error code.
		if (genl_connect(socket) == 0)
		{
			 expectedId = genl_ctrl_resolve(socket, "nl80211");
  			 nl_socket_modify_cb(socket, NL_CB_VALID, NL_CB_CUSTOM, nlCallback, NULL);


		}
	}


    	//find the nl80211 driver ID
    	//expectedId = genl_ctrl_resolve(socket, "nl80211");


	//nl_socket_free(socket)





    printf("\n\nHei, test finished\n\n");
    return EXIT_SUCCESS;
}



//#include </usr/include/libnl3/netlink/netlink.h> //#include "netlink/netlink.h"
//#include </usr/include/libnl3/netlink/genl/genl.h> //#include "netlink/genl/genl.h"
//#include </usr/include/libnl3/netlink/genl/ctrl.h> //#include "netlink/genl/ctrl.h"

