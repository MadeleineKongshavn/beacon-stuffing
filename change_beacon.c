#include <net/if.h>
#include <stdlib.h>

#include <linux/nl80211.h>
#include <linux/genetlink.h>
#include <linux/genetlink.h>

#include <netlink/genl/genl.h>          //genl_connect(socket) heregenl_connect
#include <netlink/genl/ctrl.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>

#include <netlink/handlers.h>
#include <netlink/msg.h>

static int expectedId;

/*
*An acknowledgment is a NLMSG_ERROR packet with  the
*       error  field  set  to 0. The user has to handle generating
*       acks for received messages himself. The  kernel  tries  to
*       send an NLMSG_ERROR message for every failed packet.
*/
static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg) {
    // Callback for errors.
    printf("error_handler() called.\n");
    int error = err->error;
    printf("Error: %d\n", error);

    const struct nlmsghdr * ret_hdr = malloc(sizeof(err->msg));
    printf("Message length: %d\n",ret_hdr->nlmsg_len);
    printf("Message type: %d\n", ret_hdr->nlmsg_type);
    printf("Message flags: %d\n", ret_hdr->nlmsg_flags);
    printf("Message seq: %d\n", ret_hdr->nlmsg_seq);
    printf("Message pid: %d\n", ret_hdr->nlmsg_pid);
    printf("length of payload: %d\n", nlmsg_datalen(ret_hdr));
    printf("Netlink received msg payload:%s\n",(char*)nlmsg_data(ret_hdr));

    printf("Message noop: %d \t message error: %d \t message done: %d", NLMSG_NOOP, NLMSG_ERROR, NLMSG_DONE);

    if ( ret_hdr->nlmsg_type  == NLMSG_NOOP)
    {
        printf("Message is noop");
    }
    if ( ret_hdr->nlmsg_type == NLMSG_ERROR )
    {
        printf("Message is noop");
    }
    if (  ret_hdr->nlmsg_type  ==  NLMSG_DONE )
    {
        printf("Message is noop");
    }


//NLM_F_MULTI 
//NLMSG_DONE


//    strncpy(nlmsg_data(ret_hdr), ret_hdr, nlmsg_datalen(ret_hdr));


// struct nlmsgerr * ret_hdr = nlmsg_hdr(err);



//    int *ret = arg;
//    *ret = err->error;
    return NL_STOP;
}
static int finish_handler(struct nl_msg *msg, void *arg) {
    // Callback for NL_CB_FINISH.
    printf("Finish_handler() called.\n");
//    int *ret = arg;
//    *ret = 0;
    return NL_SKIP;
}

static int ack_handler(struct nl_msg *msg, void *arg) {
    // Callback for NL_CB_ACK.
    printf("ack_handler() called.\n");
//    int *ret = arg;
//    *ret = 0;
    return NL_STOP;
}
static int no_seq_check(struct nl_msg *msg, void *arg) {
    // Callback for NL_CB_SEQ_CHECK.
    printf("no_seq_check() called.\n");

    return NL_OK;
}
static int family_handler(struct nl_msg *msg, void *arg) {
    // Callback for NL_CB_VALID within nl_get_multicast_id(). From http://sourcecodebrowser.com/iw/0.9.14/genl_8c.html.
    printf("family_handler() called.\n");
    struct nlmsghdr * ret_hdr = nlmsg_hdr(msg);
    printf("Message length: %d\n",ret_hdr->nlmsg_len);
    printf("Message type: %d\n", ret_hdr->nlmsg_type);
    printf("Message flags: %d\n", ret_hdr->nlmsg_flags);
    printf("Message seq: %d\n", ret_hdr->nlmsg_seq);
    printf("Message pid: %d\n", ret_hdr->nlmsg_pid);
    printf("length of payload: %d\n", nlmsg_datalen(ret_hdr));

    printf("Netlink received msg payload:%s\n",(char*)nlmsg_data(ret_hdr));

    printf("Message noop: %d \t message error: %d \t message done: %d", NLMSG_NOOP, NLMSG_ERROR, NLMSG_DONE);


    if ( ret_hdr->nlmsg_type  == NLMSG_NOOP)
    {
        printf("Message is noop");
    }
    if ( ret_hdr->nlmsg_type == NLMSG_ERROR )
    {
        printf("Message is noop");
    }
    if (  ret_hdr->nlmsg_type  ==  NLMSG_DONE )
    {
        printf("Message is noop");
    }



    return NL_SKIP;
}



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
//	nl_socket_modify_cb(socket, NL_CB_VALID, NL_CB_CUSTOM, nlCallback, NULL);

        struct nl_msg *msg = nlmsg_alloc();
	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);


//	int cmd = NL80211_CMD_SET_BEACON;
//	int attr = NL80211_ATTR_BEACON_INTERVAL
	int cmd = NL80211_CMD_GET_INTERFACE;
        int ifIndex = if_nametoindex("wlan0");
        int flags = 0;
//	const char *IE_element = "66.66.66.66";

         //Create the message header.
         genlmsg_put(msg, 0, 0, expectedId, 0, flags, cmd, 0);
	 NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, ifIndex);
// genlmsg_end(skb, msg_head);


//         nla_put_string(msg, NL80211_ATTR_IE, IE_element);

//	 NLA_PUT_U32(msg, NL80211_ATTR_BEACON_HEAD , ifIndex);
//	 NLA_PUT_U32(msg, NL80211_ATTR_BEACON_TAIL, ifIndex);
//	 NLA_PUT_U32(msg, NL80211_ATTR_IE, ifIndex);
//	 NLA_PUT_U32(msg, NL80211_ATTR_IE_PROBE_RESP, ifIndex);
//	 NLA_PUT_U32(msg, NL80211_ATTR_IE_ASSOC_RESP, ifIndex);

	//send the messge (this frees it)
	nl_send_auto_complete(socket, msg);

	//Sets up callback handlers manually, nl_socket_modify_cb does this automaticallt(?)
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, NULL);		//int  nl_cb_err (struct nl_cb *cb, enum nl_cb_kind kind, nl_recvmsg_err_cb_t func, void *arg)
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, NULL);	//nl_cb_set (struct nl_cb *cb, enum nl_cb_type type, enum nl_cb_kind kind, nl_recvmsg_msg_cb_t func, void *arg)
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, family_handler, NULL);

	//block for message to return
//      nl_recvmsgs_default(socket);
	nl_recvmsgs(socket, cb);

	 nla_put_failure:
         	nlmsg_free(msg);
	return 0;
}




//    int mcid = nl_get_multicast_id(socket, "nl80211", "scan");
//    nl_socket_add_membership(socket, mcid);  // Without this, callback_trigger() won't be called.

