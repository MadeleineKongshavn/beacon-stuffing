#include <net/if.h>
#include <stdlib.h>

#include <linux/nl80211.h>
#include <linux/genetlink.h>
#include <linux/genetlink.h>

#include <netlink/genl/genl.h>          //genl_connect(socket) heregenl_connect
#include <netlink/genl/ctrl.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>

static int expectedId;


static int nlCallback(struct nl_msg* msg, void* arg)
{
        //netlink messages all have an identical header on each message sent and received This header is used to store metadata about each
        //netlink message and forms the base infrastructure of every netlink protocol
        struct nlmsghdr * ret_hdr = nlmsg_hdr(msg);
        printf("Message length: %d\n",ret_hdr->nlmsg_len);
        printf("Message type: %d\n", ret_hdr->nlmsg_type);
        printf("Message flags: %d\n", ret_hdr->nlmsg_flags);
        printf("Message seq: %d\n", ret_hdr->nlmsg_seq);
        printf("Message pid: %d\n", ret_hdr->nlmsg_pid);
        return 0;
}

int main() {
	struct nl_sock *socket = nl_socket_alloc();
	genl_connect(socket);
	expectedId = genl_ctrl_resolve(socket, "nl80211");
	nl_socket_modify_cb(socket, NL_CB_VALID, NL_CB_CUSTOM, nlCallback, NULL);

        struct nl_msg *msg = nlmsg_alloc();
	int cmd = NL80211_CMD_SET_BEACON;
        int ifIndex = if_nametoindex("wlan0");
        int flags = 0;
	const char *IE_element = "66.66.66.66";

         //Create the message header.
         genlmsg_put(msg, 0, 0, expectedId, 0, flags, cmd, 0);
	 NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, ifIndex);
         nla_put_string(msg, NL80211_ATTR_IE, IE_element);

//	 NLA_PUT_U32(msg, NL80211_ATTR_BEACON_HEAD , ifIndex);
//	 NLA_PUT_U32(msg, NL80211_ATTR_BEACON_TAIL, ifIndex);
//	 NLA_PUT_U32(msg, NL80211_ATTR_IE, ifIndex);
//	 NLA_PUT_U32(msg, NL80211_ATTR_IE_PROBE_RESP, ifIndex);
//	 NLA_PUT_U32(msg, NL80211_ATTR_IE_ASSOC_RESP, ifIndex);

	//send the messge (this frees it)
	nl_send_auto_complete(socket, msg);
	//block for message to return
        nl_recvmsgs_default(socket);

	 nla_put_failure:
         	nlmsg_free(msg);
	return 0;
}
