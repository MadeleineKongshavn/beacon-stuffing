#include <net/if.h>
#include <stdlib.h>

#include <linux/nl80211.h>
#include <linux/genetlink.h>
#include <linux/genetlink.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/netlink.h>

static int expectedId;

static int error_msg_handler(struct nl_msg *msg, void *arg)
{
	printf("error_msg_handler() called\n");
	return NL_STOP;
}
static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg) 
{
	printf("error_handler() called\n");
	return NL_STOP;
}
static int ack_handler(struct nl_msg *msg, void *arg)
{
//    	int *ret = arg;
//    	*ret = 0;
//	Callback for NL_CB_ACK.
	printf("ack_handler() called.\n");
	return NL_STOP; // Skip message currently being processed and continue parsing the receive buffer.
}
static int valid_handler(struct nl_msg *msg, void *arg)
{
	const struct nlmsghdr* header = nlmsg_hdr(msg); // retrives the actual netlink message
	struct genlmsghdr *gnlh = (struct genlmsghdr*) nlmsg_data(header);

//	nla_data(header);

	printf("Valid handler called()\n");
	return NL_OK;
}
void cleanup(struct nl_sock *sk, struct nl_msg *msg)
{
	nlmsg_free(msg);
	nl_socket_free(sk);
}
struct nl_sock * allocate_socket()
{
	// need error handling here!
        struct nl_sock *sk = nl_socket_alloc();
	genl_connect(sk);
        return sk;
}
struct nl_msg * create_message(struct nl_sock *sk)
{
        expectedId = genl_ctrl_resolve(sk, "nl80211");
	struct nl_msg *msg = nlmsg_alloc();
	int cmd = NL80211_CMD_GET_INTERFACE;
	int ifIndex = if_nametoindex("wlan0");
        int flags = 0;
	genlmsg_put(msg, 0, 0, expectedId, 0, flags, cmd, 0);
        NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, ifIndex);

	nla_put_failure:
		nlmsg_free(msg);

	return msg;
}
struct nl_cb * set_callback()
{
	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);
	//Set ACK callback.
	nl_cb_set(cb,NL_CB_SEND_ACK,NL_CB_CUSTOM, ack_handler, NULL);
	nl_cb_set(cb,NL_CB_ACK,NL_CB_CUSTOM, ack_handler, NULL);
	//Set VALID callback.
	nl_cb_set(cb,NL_CB_VALID,NL_CB_CUSTOM, valid_handler, NULL);
	//Set ERROR handler.
	nl_cb_set(cb,NL_CB_INVALID,NL_CB_VERBOSE, error_msg_handler, NULL);
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, NULL);
	return cb;
}
int main()
{
	struct nl_sock *sk = allocate_socket();
	struct nl_cb *cb = set_callback();
        struct nl_msg *msg = create_message(sk); //  nl_send_auto

	nl_send_auto_complete(sk, msg);

	while (1)
	{
		nl_recvmsgs(sk, cb);
	}
       return EXIT_SUCCESS;
}
