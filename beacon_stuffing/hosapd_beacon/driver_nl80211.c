static int wpa_driver_nl80211_set_ap(void *priv,
				     struct wpa_driver_ap_params *params)
{
	struct i802_bss *bss = priv;
	struct wpa_driver_nl80211_data *drv = bss->drv;
	struct nl_msg *msg;
	u8 cmd = NL80211_CMD_NEW_BEACON;
	int ret;
	int beacon_set;
	int num_suites;
	int smps_mode;
	u32 suites[10], suite;
	u32 ver;
#ifdef CONFIG_MESH
	struct wpa_driver_mesh_bss_params mesh_params;
#endif /* CONFIG_MESH */

	beacon_set = params->reenable ? 0 : bss->beacon_set;

	wpa_printf(MSG_DEBUG, "nl80211: Set beacon (beacon_set=%d)",
		   beacon_set);
	if (beacon_set)
		cmd = NL80211_CMD_SET_BEACON;

	wpa_hexdump(MSG_DEBUG, "nl80211: Beacon head",
		    params->head, params->head_len);
	wpa_hexdump(MSG_DEBUG, "nl80211: Beacon tail",
		    params->tail, params->tail_len);
	wpa_printf(MSG_DEBUG, "nl80211: ifindex=%d", bss->ifindex);
	wpa_printf(MSG_DEBUG, "nl80211: beacon_int=%d", params->beacon_int);
	wpa_printf(MSG_DEBUG, "nl80211: dtim_period=%d", params->dtim_period);
	wpa_hexdump_ascii(MSG_DEBUG, "nl80211: ssid",
			  params->ssid, params->ssid_len);
	if (!(msg = nl80211_bss_msg(bss, 0, cmd)) ||
	    nla_put(msg, NL80211_ATTR_BEACON_HEAD, params->head_len,
		    params->head) ||
	    nla_put(msg, NL80211_ATTR_BEACON_TAIL, params->tail_len,
		    params->tail) ||
	    nl80211_put_beacon_int(msg, params->beacon_int) ||
	    nl80211_put_dtim_period(msg, params->dtim_period) ||
	    nla_put(msg, NL80211_ATTR_SSID, params->ssid_len, params->ssid))
		goto fail;
	if (params->proberesp && params->proberesp_len) {
		wpa_hexdump(MSG_DEBUG, "nl80211: proberesp (offload)",
			    params->proberesp, params->proberesp_len);
		if (nla_put(msg, NL80211_ATTR_PROBE_RESP, params->proberesp_len,
			    params->proberesp))
			goto fail;
	}
	switch (params->hide_ssid) {
	case NO_SSID_HIDING:
		wpa_printf(MSG_DEBUG, "nl80211: hidden SSID not in use");
		if (nla_put_u32(msg, NL80211_ATTR_HIDDEN_SSID,
				NL80211_HIDDEN_SSID_NOT_IN_USE))
			goto fail;
		break;
	case HIDDEN_SSID_ZERO_LEN:
		wpa_printf(MSG_DEBUG, "nl80211: hidden SSID zero len");
		if (nla_put_u32(msg, NL80211_ATTR_HIDDEN_SSID,
				NL80211_HIDDEN_SSID_ZERO_LEN))
			goto fail;
		break;
	case HIDDEN_SSID_ZERO_CONTENTS:
		wpa_printf(MSG_DEBUG, "nl80211: hidden SSID zero contents");
		if (nla_put_u32(msg, NL80211_ATTR_HIDDEN_SSID,
				NL80211_HIDDEN_SSID_ZERO_CONTENTS))
			goto fail;
		break;
	}
	wpa_printf(MSG_DEBUG, "nl80211: privacy=%d", params->privacy);
	if (params->privacy &&
	    nla_put_flag(msg, NL80211_ATTR_PRIVACY))
		goto fail;
	wpa_printf(MSG_DEBUG, "nl80211: auth_algs=0x%x", params->auth_algs);
	if ((params->auth_algs & (WPA_AUTH_ALG_OPEN | WPA_AUTH_ALG_SHARED)) ==
	    (WPA_AUTH_ALG_OPEN | WPA_AUTH_ALG_SHARED)) {
		/* Leave out the attribute */
	} else if (params->auth_algs & WPA_AUTH_ALG_SHARED) {
		if (nla_put_u32(msg, NL80211_ATTR_AUTH_TYPE,
				NL80211_AUTHTYPE_SHARED_KEY))
			goto fail;
	} else {
		if (nla_put_u32(msg, NL80211_ATTR_AUTH_TYPE,
				NL80211_AUTHTYPE_OPEN_SYSTEM))
			goto fail;
	}

	wpa_printf(MSG_DEBUG, "nl80211: wpa_version=0x%x", params->wpa_version);
	ver = 0;
	if (params->wpa_version & WPA_PROTO_WPA)
		ver |= NL80211_WPA_VERSION_1;
	if (params->wpa_version & WPA_PROTO_RSN)
		ver |= NL80211_WPA_VERSION_2;
	if (ver &&
	    nla_put_u32(msg, NL80211_ATTR_WPA_VERSIONS, ver))
		goto fail;

	wpa_printf(MSG_DEBUG, "nl80211: key_mgmt_suites=0x%x",
		   params->key_mgmt_suites);
	num_suites = 0;
	if (params->key_mgmt_suites & WPA_KEY_MGMT_IEEE8021X)
		suites[num_suites++] = WLAN_AKM_SUITE_8021X;
	if (params->key_mgmt_suites & WPA_KEY_MGMT_PSK)
		suites[num_suites++] = WLAN_AKM_SUITE_PSK;
	if (num_suites &&
	    nla_put(msg, NL80211_ATTR_AKM_SUITES, num_suites * sizeof(u32),
		    suites))
		goto fail;

	if (params->key_mgmt_suites & WPA_KEY_MGMT_IEEE8021X_NO_WPA &&
	    (!params->pairwise_ciphers ||
	     params->pairwise_ciphers & (WPA_CIPHER_WEP104 | WPA_CIPHER_WEP40)) &&
	    (nla_put_u16(msg, NL80211_ATTR_CONTROL_PORT_ETHERTYPE, ETH_P_PAE) ||
	     nla_put_flag(msg, NL80211_ATTR_CONTROL_PORT_NO_ENCRYPT)))
		goto fail;

	wpa_printf(MSG_DEBUG, "nl80211: pairwise_ciphers=0x%x",
		   params->pairwise_ciphers);
	num_suites = wpa_cipher_to_cipher_suites(params->pairwise_ciphers,
						 suites, ARRAY_SIZE(suites));
	if (num_suites &&
	    nla_put(msg, NL80211_ATTR_CIPHER_SUITES_PAIRWISE,
		    num_suites * sizeof(u32), suites))
		goto fail;

	wpa_printf(MSG_DEBUG, "nl80211: group_cipher=0x%x",
		   params->group_cipher);
	suite = wpa_cipher_to_cipher_suite(params->group_cipher);
	if (suite &&
	    nla_put_u32(msg, NL80211_ATTR_CIPHER_SUITE_GROUP, suite))
		goto fail;

	if (params->ht_opmode != -1) {
		switch (params->smps_mode) {
		case HT_CAP_INFO_SMPS_DYNAMIC:
			wpa_printf(MSG_DEBUG, "nl80211: SMPS mode - dynamic");
			smps_mode = NL80211_SMPS_DYNAMIC;
			break;
		case HT_CAP_INFO_SMPS_STATIC:
			wpa_printf(MSG_DEBUG, "nl80211: SMPS mode - static");
			smps_mode = NL80211_SMPS_STATIC;
			break;
		default:
			/* invalid - fallback to smps off */
		case HT_CAP_INFO_SMPS_DISABLED:
			wpa_printf(MSG_DEBUG, "nl80211: SMPS mode - off");
			smps_mode = NL80211_SMPS_OFF;
			break;
		}
		if (nla_put_u32(msg, NL80211_ATTR_SMPS_MODE, smps_mode))
			goto fail;
	}

	if (params->beacon_ies) {
		wpa_hexdump_buf(MSG_DEBUG, "nl80211: beacon_ies",
				params->beacon_ies);
		if (nla_put(msg, NL80211_ATTR_IE,
			    wpabuf_len(params->beacon_ies),
			    wpabuf_head(params->beacon_ies)))
			goto fail;
	}
	if (params->proberesp_ies) {
		wpa_hexdump_buf(MSG_DEBUG, "nl80211: proberesp_ies",
				params->proberesp_ies);
		if (nla_put(msg, NL80211_ATTR_IE_PROBE_RESP,
			    wpabuf_len(params->proberesp_ies),
			    wpabuf_head(params->proberesp_ies)))
			goto fail;
	}
	if (params->assocresp_ies) {
		wpa_hexdump_buf(MSG_DEBUG, "nl80211: assocresp_ies",
				params->assocresp_ies);
		if (nla_put(msg, NL80211_ATTR_IE_ASSOC_RESP,
			    wpabuf_len(params->assocresp_ies),
			    wpabuf_head(params->assocresp_ies)))
			goto fail;
	}

	if (drv->capa.flags & WPA_DRIVER_FLAGS_INACTIVITY_TIMER)  {
		wpa_printf(MSG_DEBUG, "nl80211: ap_max_inactivity=%d",
			   params->ap_max_inactivity);
		if (nla_put_u16(msg, NL80211_ATTR_INACTIVITY_TIMEOUT,
				params->ap_max_inactivity))
			goto fail;
	}

#ifdef CONFIG_P2P
	if (params->p2p_go_ctwindow > 0) {
		if (drv->p2p_go_ctwindow_supported) {
			wpa_printf(MSG_DEBUG, "nl80211: P2P GO ctwindow=%d",
				   params->p2p_go_ctwindow);
			if (nla_put_u8(msg, NL80211_ATTR_P2P_CTWINDOW,
				       params->p2p_go_ctwindow))
				goto fail;
		} else {
			wpa_printf(MSG_INFO,
				   "nl80211: Driver does not support CTWindow configuration - ignore this parameter");
		}
	}
#endif /* CONFIG_P2P */

	if (params->pbss) {
		wpa_printf(MSG_DEBUG, "nl80211: PBSS");
		if (nla_put_flag(msg, NL80211_ATTR_PBSS))
			goto fail;
	}

	ret = send_and_recv_msgs(drv, msg, NULL, NULL);
	if (ret) {
		wpa_printf(MSG_DEBUG, "nl80211: Beacon set failed: %d (%s)",
			   ret, strerror(-ret));
	} else {
		bss->beacon_set = 1;
		nl80211_set_bss(bss, params->cts_protect, params->preamble,
				params->short_slot_time, params->ht_opmode,
				params->isolate, params->basic_rates);
		if (beacon_set && params->freq &&
		    params->freq->bandwidth != bss->bandwidth) {
			wpa_printf(MSG_DEBUG,
				   "nl80211: Update BSS %s bandwidth: %d -> %d",
				   bss->ifname, bss->bandwidth,
				   params->freq->bandwidth);
			ret = nl80211_set_channel(bss, params->freq, 1);
			if (ret) {
				wpa_printf(MSG_DEBUG,
					   "nl80211: Frequency set failed: %d (%s)",
					   ret, strerror(-ret));
			} else {
				wpa_printf(MSG_DEBUG,
					   "nl80211: Frequency set succeeded for ht2040 coex");
				bss->bandwidth = params->freq->bandwidth;
			}
		} else if (!beacon_set && params->freq) {
			/*
			 * cfg80211 updates the driver on frequence change in AP
			 * mode only at the point when beaconing is started, so
			 * set the initial value here.
			 */
			bss->bandwidth = params->freq->bandwidth;
		}
	}

#ifdef CONFIG_MESH
	if (is_mesh_interface(drv->nlmode) && params->ht_opmode != -1) {
		os_memset(&mesh_params, 0, sizeof(mesh_params));
		mesh_params.flags |= WPA_DRIVER_MESH_CONF_FLAG_HT_OP_MODE;
		mesh_params.ht_opmode = params->ht_opmode;
		ret = nl80211_set_mesh_config(priv, &mesh_params);
		if (ret < 0)
			return ret;
	}
#endif /* CONFIG_MESH */

	return ret;
fail:
	nlmsg_free(msg);
	return -ENOBUFS;
}


