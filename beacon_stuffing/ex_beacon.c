kernel multicast notifications
/*
struct nlmsghdr {
       __u32 nlmsg_len;    /* Length of message including header
       __u16 nlmsg_type;   /* Type of message content
       __u16 nlmsg_flags;  /* Additional flags
       __u32 nlmsg_seq;    /* Sequence number
       __u32 nlmsg_pid;    /* Sender port ID
 };
The Netlink message header is shown below.

   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          Length                             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |            Type              |           Flags              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      Sequence Number                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      Process ID (PID)                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 Netlink messages consist of a byte stream with one or multiple
   Netlink headers and an associated payload.  If the payload is too big
   to fit into a single message it, can be split over multiple Netlink
   messages, collectively called a multipart message. Individual IP services specify more message types, e.g.,
   NETLINK_ROUTE service specifies several types, 
 
#define NLM_F_REQUEST           1

#define NLM_F_MULTI             2

#define NLM_F_ACK               4

#define NLM_F_ECHO              8


Generic Netlink families are defined by the genl_family structure,
 struct genl_family
 {       
       unsigned int            id;
       unsigned int            hdrsize;
       char                    name[GENL_NAMSIZ];
       unsigned int            version;
       unsigned int            maxattr;
       struct nlattr **        attrbuf;
       struct list_head        ops_list;
       struct list_head        family_list;
 };

Add Generic Netlink headers to Netlink message.


void* genlmsg_put

Parameters
msg	Netlink message object
port	Netlink port or NL_AUTO_PORT
seq	Sequence number of message or NL_AUTO_SEQ
family	Numeric family identifier
hdrlen	Length of user header
flags	Additional Netlink message flags (optional)
cmd	Numeric command identifier
version	Interface version

Generic Netlink uses the standard Netlink subsystem as a transport layer which means that the foundation of the Generic Netlink message
 is the standard Netlink message format


When an error occurs an NLMSG_ERROR message is returned to the client with the error code returned by the Generic Netlink operation handler. Clients can also request the NLMSG_ERROR message when no error has occurred by setting the NLM_F_ACK flag on requests.


*/


#include <net/if.h>
#include <stdlib.h>

#include <linux/nl80211.h>
#include <linux/genetlink.h>
#include <linux/genetlink.h>

#include <netlink/genl/genl.h>		//genl_connect(socket) heregenl_connect
#include <netlink/genl/ctrl.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>

static int expectedId;

static int nlCallback(struct nl_msg* msg, void* arg)
{
	printf("\nKom til callback\n");
	//netlink messages all have an identical header on each message sent and received This header is used to store metadata about each
	//netlink message and forms the base infrastructure of every netlink protocol
	struct nlmsghdr * ret_hdr = nlmsg_hdr(msg);
	printf("Message length: %d\n",ret_hdr->nlmsg_len);
	printf("Message type: %d\n", ret_hdr->nlmsg_type);
	printf("Message flags: %d\n", ret_hdr->nlmsg_flags);
	printf("Message seq: %d\n", ret_hdr->nlmsg_seq);
	printf("Message pid: %d\n", ret_hdr->nlmsg_pid);
	return 0;



// Old code!
//    struct nlmsghdr* ret_hdr = nlmsg_hdr(msg);
//    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];

//    if (ret_hdr->nlmsg_type != expectedId)
//    {
        // what is this??
//        return NL_STOP;
//    }

//    struct genlmsghdr *gnlh = (struct genlmsghdr*) nlmsg_data(ret_hdr);

 //   nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
//              genlmsg_attrlen(gnlh, 0), NULL);

//    if (tb_msg[NL80211_ATTR_IFTYPE]) {
//        int type = nla_get_u32(tb_msg[NL80211_ATTR_IFTYPE]);

//        printf("Type: %d", type);
//    }
//    return 0;
}
int main()
{
	int ret;

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
			//Resolves the Generic Netlink family name to the corresponding numeric family identifier. 
			//This function queries the kernel directly, sk: Generic Netlink socket, name: Name of Generic Netlink family
			expectedId = genl_ctrl_resolve(socket, "nl80211");

			//Modify the callback handler associated to the socket
			//handle netlink handle(socket)
			//type which type callback to set
			//kind kind of callback
			//func callback function
			//arg argument to be passwd to callback function
  			nl_socket_modify_cb(socket, NL_CB_VALID, NL_CB_CUSTOM, nlCallback, NULL);

			//Allocate a new  netlink message.
//NLM_F_REQUEST
			struct nl_msg *msg = nlmsg_alloc(void);
//			int cmd = NL80211_CMD_TRIGGER_SCAN;
			int cmd = NL80211_CMD_GET_INTERFACE;
			int ifIndex = if_nametoindex("wlan0");
			int flags = 0;
			const char *IE_element = "IP address here";
		
			//Create the header of this message(setting up the initial message)
			genlmsg_put(msg, 0, 0, expectedId, 0, flags, cmd, 0);
			NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, ifIndex);
//			nla_put_string(msg, NL80211_ATTR_IE, IE_element);

			//send this  messge (this frees it)
    			ret = nl_send_auto_complete(socket, msg);

//			while ( 1 ) {
			    	//block for message to return
	    			nl_recvmsgs_default(socket);
				printf("message returned");
//			}




			nla_put_failure:
			    nlmsg_free(msg);
			//allocate a message
//    			nl_msg* msg = nlmsg_alloc();

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

//The functionality this
// *	command was planned for has been provided with cleaner design with the
// *	option to specify additional IEs in NL80211_CMD_TRIGGER_SCAN,
// *	NL80211_CMD_AUTHENTICATE, NL80211_CMD_ASSOCIATE,
// *	NL80211_CMD_DEAUTHENTICATE, and NL80211_CMD_DISASSOCIATE


// * @NL80211_CMD_TRIGGER_SCAN: trigger a new scan with the given parameters
// *	%NL80211_ATTR_TX_NO_CCK_RATE is used to decide whether to send the
// *	probe requests at CCK rate or not. %NL80211_ATTR_BSSID can be used to
// *	specify a BSSID to scan for; if not included, the wildcard BSSID will
// *	be used.


//* @NL80211_CMD_AUTHENTICATE: authentication request and notification.
// *	This command is used both as a command (request to authenticate) and
// *	as an event on the "mlme" multicast group indicating completion of the
// *	authentication process.
// *	When used as a command, %NL80211_ATTR_IFINDEX is used to identify the
// *	interface. %NL80211_ATTR_MAC is used to specify PeerSTAAddress (and
// *	BSSID in case of station mode). %NL80211_ATTR_SSID is used to specify
// *	the SSID (mainly for association, but is included in authentication
// *	request, too, to help BSS selection. %NL80211_ATTR_WIPHY_FREQ is used
// *	to specify the frequence of the channel in MHz. %NL80211_ATTR_AUTH_TYPE
// *	is used to specify the authentication type. %NL80211_ATTR_IE is used to
// *	define IEs (VendorSpecificInfo, but also including RSN IE and FT IEs)
// *	to be added to the frame.
// *	When used as an event, this reports reception of an Authentication
// *	frame in station and IBSS modes when the local MLME processed the
// *	frame, i.e., it was for the local STA and was received in correct
// *	state. This is similar to MLME-AUTHENTICATE.confirm primitive in the
// *	MLME SAP interface (kernel providing MLME, userspace SME). The
// *	included %NL80211_ATTR_FRAME attribute contains the management frame
// *	(including both the header and frame body, but not FCS). This event is
// *	also used to indicate if the authentication attempt timed out. In that
// *	case the %NL80211_ATTR_FRAME attribute is replaced with a
// *	%NL80211_ATTR_TIMED_OUT flag (and %NL80211_ATTR_MAC to indicate which
// *	pending authentication timed out)

//@NL80211_CMD_ASSOCIATE: association request and notification; like
// *	NL80211_CMD_AUTHENTICATE but for Association and Reassociation
// *	(similar to MLME-ASSOCIATE.request, MLME-REASSOCIATE.request,
// *	MLME-ASSOCIATE.confirm or MLME-REASSOCIATE.confirm primitives). The
// *	%NL80211_ATTR_PREV_BSSID attribute is used to specify whether the
// *	request is for the initial association to an ESS (that attribute not
// *	included) or for reassociation within the ESS (that attribute is
// *	included).


// * @NL80211_CMD_DEAUTHENTICATE: deauthentication request and notification; like
// *	NL80211_CMD_AUTHENTICATE but for Deauthentication frames (similar to
// *	MLME-DEAUTHENTICATION.request and MLME-DEAUTHENTICATE.indication
// *	primitives).

// * @NL80211_CMD_DISASSOCIATE: disassociation request and notification; like
// *	NL80211_CMD_AUTHENTICATE but for Disassociation frames (similar to
// *	MLME-DISASSOCIATE.request and MLME-DISASSOCIATE.indication primitives).
