#include <net/if.h>
#include <stdlib.h>

#include <linux/nl80211.h>
#include <linux/genetlink.h>
#include <linux/genetlink.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/netlink.h>

static int expectedId;


static int is_valid_ie_attr(const struct nlattr *attr)
{
	const uint8_t *pos;
	int len;

	if (!attr)
		return 1;

	pos = nla_data(attr);
	len = nla_len(attr);

	while (len) {
		uint8_t elemlen;

		if (len < 2)
			return 0;
		len -= 2;

		elemlen = pos[1];
		if (elemlen > len)
			return 0;

		len -= elemlen;
		pos += 2 + elemlen;
	}

	return 1;
}
/*
static void nl80211_parse_beacon(struct nlattr *attrs[],struct cfg80211_beacon_data *bcn)
{
	bool haveinfo = false;

	if (!is_valid_ie_attr(attrs[NL80211_ATTR_BEACON_TAIL]) ||
	    !is_valid_ie_attr(attrs[NL80211_ATTR_IE]) ||
	    !is_valid_ie_attr(attrs[NL80211_ATTR_IE_PROBE_RESP]) ||
	    !is_valid_ie_attr(attrs[NL80211_ATTR_IE_ASSOC_RESP]))
		return -EINVAL;

	memset(bcn, 0, sizeof(*bcn));

	if (attrs[NL80211_ATTR_BEACON_HEAD]) {
		bcn->head = nla_data(attrs[NL80211_ATTR_BEACON_HEAD]);
		bcn->head_len = nla_len(attrs[NL80211_ATTR_BEACON_HEAD]);
		if (!bcn->head_len)
			return -EINVAL;
		haveinfo = true;
	}

	if (attrs[NL80211_ATTR_BEACON_TAIL]) {
		bcn->tail = nla_data(attrs[NL80211_ATTR_BEACON_TAIL]);
		bcn->tail_len = nla_len(attrs[NL80211_ATTR_BEACON_TAIL]);
		haveinfo = true;
	}

	if (!haveinfo)
		return -EINVAL;

	if (attrs[NL80211_ATTR_IE]) {
		bcn->beacon_ies = nla_data(attrs[NL80211_ATTR_IE]);
		bcn->beacon_ies_len = nla_len(attrs[NL80211_ATTR_IE]);
	}

	if (attrs[NL80211_ATTR_IE_PROBE_RESP]) {
		bcn->proberesp_ies =
			nla_data(attrs[NL80211_ATTR_IE_PROBE_RESP]);
		bcn->proberesp_ies_len =
			nla_len(attrs[NL80211_ATTR_IE_PROBE_RESP]);
	}

	if (attrs[NL80211_ATTR_IE_ASSOC_RESP]) {
		bcn->assocresp_ies =
			nla_data(attrs[NL80211_ATTR_IE_ASSOC_RESP]);
		bcn->assocresp_ies_len =
			nla_len(attrs[NL80211_ATTR_IE_ASSOC_RESP]);
	}

	if (attrs[NL80211_ATTR_PROBE_RESP]) {
		bcn->probe_resp = nla_data(attrs[NL80211_ATTR_PROBE_RESP]);
		bcn->probe_resp_len = nla_len(attrs[NL80211_ATTR_PROBE_RESP]);
	}

	return 0;
}
*/

void parse_message( struct nlmsghdr * ret_hdr )
{
//	const struct nlmsghdr * ret_hdr = malloc(sizeof(err->msg));
	struct genlmsghdr *gnlh = (struct genlmsghdr*) nlmsg_data(ret_hdr);
	printf("\nTrying to parse data to char*: %s", (char*) nlmsg_data(ret_hdr));
	printf("\nGnlh cmd: %d", gnlh->cmd);

	struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
	nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),genlmsg_attrlen(gnlh, 0), NULL);

//	int type = nla_get_u32(tb_msg[NL80211_ATTR_BEACON_INTERVAL]);
//	printf("finished with parsing!");

        struct nlattr* attributes = genlmsg_attrdata(gnlh, 0);  // points to the beginning of the payload, to the first attribute header.  //0 is length og <USER> head$

	printf("First attribute type: %d",nla_type(attributes));

        int remaining = genlmsg_attrlen(gnlh, 0);               // Returns length of attribute length.
        printf("\nremaning before started: %d", remaining);
	printf("\nNla_ok says: %d\n",nla_ok(attributes, remaining));

        while (nla_ok(attributes, remaining)) {

                /* parse attribute here */	//parse_attribute(attributes);
                printf("Attribute:\n");
                attributes = nla_next(attributes, &remaining);
        };
}

static int error_msg_handler(struct nl_msg *msg, void *arg)
{
	printf("error_msg_handler() called\n");
	return NL_STOP;
}
static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
//	struct nlmsghdr * ret_hdr = malloc(sizeof(err->msg));
	struct nlmsghdr ret_hdr = err->msg;
	struct nlmsghdr *  mem_ret_hdr = &ret_hdr;
        struct genlmsghdr *gnlh = (struct genlmsghdr*) nlmsg_data(mem_ret_hdr);
	struct nlattr* attributes = genlmsg_attrdata(gnlh, 0); 
        int remaining = genlmsg_attrlen(gnlh, 0);               // Returns length of attribute length.

//	ret_hdr = err->msg;

	printf("valid nlms and genlms AFTER sending? %d\n", genlmsg_valid_hdr(mem_ret_hdr, sizeof(struct nlmsghdr)));
	printf("Nla_ok AFTER sending says: %d\n",nla_ok(attributes, remaining));
	printf("\n");

	// Callback for errors.
	printf("\nError_handler() called.\n");
	printf("Error: %d\n", err->error);
	printf("Message length: %d\n",ret_hdr.nlmsg_len);
	printf("Message type: %d\n", ret_hdr.nlmsg_type);
	printf("Message flags: %d\n", ret_hdr.nlmsg_flags);
	printf("Message seq: %d\n", ret_hdr.nlmsg_seq);
	printf("Message pid: %d\n", ret_hdr.nlmsg_pid);
//	printf("length of payload: %d\n", nlmsg_datalen(ret_hdr));

	printf("\nTrying to parse from pointer, nlmsg total length: %d\n", mem_ret_hdr->nlmsg_len);
	printf("Trying to parse from pointer, payload length in nlmsg: %d\n", nlmsg_datalen(mem_ret_hdr));

	printf("Genlmsg total length: %d\n", genlmsg_len(gnlh));
	printf("Genlmsg total length: %d\n", genlmsg_len(gnlh));
	printf("estimated attribute length: %d\n",genlmsg_attrlen(gnlh, 0));
	printf("user datalength: %d\n",genlmsg_user_datalen(gnlh, 0));
	printf("valid nlms and genlms? %d\n", genlmsg_valid_hdr(mem_ret_hdr, sizeof(struct nlmsghdr)));

	parse_message(mem_ret_hdr);

//	tryvalid(mem_ret_hdr);


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
void parse_attribute(const struct nlattr * nla) {
	printf("attribute type: %d\n", nla_type(nla)); //Return type of the attribute
//	printf("length of attribute: %d\n",nla_len(nla)); //Return length of the payload
//	printf("Is flag attribute set: %d\n", nla_get_flag(nla));  //1 is true, 0 is false
//	printf("Attribute payload section: %s\n\n", (char*) nla_data(nla)); //Return pointer to the payload section.
//	printf("Attribute payload section: %s\n\n",(char*) nla_data(nla)); //Return pointer to the payload section.
}
static int valid_handler(struct nl_msg *msg, void *arg)
{
	struct nlmsghdr* header = nlmsg_hdr(msg);	// retrives the actual netlink message
	struct genlmsghdr *gnlh = (struct genlmsghdr*) nlmsg_data(header);

	struct nlattr* attributes = genlmsg_attrdata(gnlh, 0);	// points to the beginning of the payload, to the first attribute header.  //0 is length og <USER> header // returns pointer to beginning of payload. 
	int remaining = genlmsg_attrlen(gnlh, 0); 		// Returns length of attribute length.
	printf("valid nlms and genlms AFTER sending? %d\n", genlmsg_valid_hdr(header, sizeof(struct nlmsghdr)));
	printf("Nla_ok AFTER sending says: %d\n",nla_ok(attributes, remaining));
	printf("\n");
	while (nla_ok(attributes, remaining)) {

	      	/* parse attribute here */
		parse_attribute(attributes);
        	attributes = nla_next(attributes, &remaining);
	};

//	nla_data(header);

	printf("Valid handler finished called()\n");
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
//        nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifIndex);
        NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, ifIndex);

	nla_put_failure:
		nlmsg_free(msg);

	return msg;
}
struct nl_msg * create_beacon_message(struct nl_sock *sk)
{
        expectedId = genl_ctrl_resolve(sk, "nl80211");
        struct nl_msg *msg = nlmsg_alloc();
	int cmd = NL80211_CMD_SET_BEACON;
//	int cmd = NL80211_CMD_GET_INTERFACE;
        int flags = 0;
//	int ifIndex = if_nametoindex("wlan0");
	genlmsg_put(msg, 0, 0, expectedId, 0, flags, cmd, 0);
	nla_put(msg, NL80211_ATTR_BEACON_HEAD, 0, NULL); // error 0
	nla_put(msg, NL80211_ATTR_BEACON_TAIL, 0, NULL); // error 0
	nla_put(msg, NL80211_ATTR_PROBE_RESP, 0, NULL);

	nla_put(msg, NL80211_ATTR_IE_PROBE_RESP, 0, NULL);
	nla_put(msg, NL80211_ATTR_IE_ASSOC_RESP, 0, NULL);
	nla_put(msg, NL80211_ATTR_IE, 0, NULL);

//	nla_put_u32(msg, NL80211_ATTR_BEACON_INTERVAL, 100);

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
void validate_message(struct nl_msg *msg)
{
        struct nlmsghdr* header = nlmsg_hdr(msg);       // retrives the actual netlink message
        struct genlmsghdr *gnlh = (struct genlmsghdr*) nlmsg_data(header);

        struct nlattr* attributes = genlmsg_attrdata(gnlh, 0);  // points to the beginning of the payload, to the first attribute header.  //0 is length og <USER> head$
        int remaining = genlmsg_attrlen(gnlh, 0);               // Returns length of attribute length.
	printf("valid nlms and genlms BEFORE sending? %d\n", genlmsg_valid_hdr(header, sizeof(struct nlmsghdr)));
	printf("Nla_ok BEFORE sending says: %d\n",nla_ok(attributes, remaining));
        while (nla_ok(attributes, remaining)) {

                /* parse attribute here */
                parse_attribute(attributes);
                attributes = nla_next(attributes, &remaining);
        };
}
int main()
{
	struct nl_sock *sk = allocate_socket();
	struct nl_cb *cb = set_callback();
//      struct nl_msg *msg = create_message(sk); //  nl_send_auto
        struct nl_msg *msg = create_beacon_message(sk); //  nl_send_auto

	printf("\n\nTesting validation of message\n");
	validate_message(msg);
	printf("\n\n");

	nl_send_auto_complete(sk, msg);

//	while (1)
//	{
		nl_recvmsgs(sk, cb);
//	}
       return EXIT_SUCCESS;
}
//static int nl80211_set_beacon(struct sk_buff *skb, struct genl_info *info)


//    if (tb_msg[NL80211_ATTR_BEACON_INTERVAL]) {
//        int type = nla_get_u32(tb_msg[NL80211_ATTR_BEACON_INTERVAL]);


   // printf("Netlink received msg payload:%s\n",(char*)nlmsg_data(ret_hdr));



