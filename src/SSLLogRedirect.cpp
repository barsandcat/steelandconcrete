#include <pch.h>
#include <SSLLogRedirect.h>


void SSLInfoCallback(const SSL *s, int where, int ret)
{
	std::stringstream stateMsg;
	stateMsg << " [" << SSL_state_string_long(s) << ", " << std::hex << where << ", " << std::dec << ret << "]";
	switch(where)
	{
	case SSL_CB_READ_ALERT:
		LOG(INFO) << "SSL_CB_READ_ALERT, " << SSL_alert_type_string_long(ret) << ", " << SSL_alert_desc_string_long(ret) << stateMsg.str();
		break;
	case SSL_CB_WRITE_ALERT:
		LOG(INFO) << "SSL_CB_WRITE_ALERT, " << SSL_alert_type_string_long(ret) << ", " << SSL_alert_desc_string_long(ret) << stateMsg.str();
		break;
	case SSL_CB_ACCEPT_LOOP:
		LOG(INFO) << "SSL_CB_ACCEPT_LOOP" << stateMsg.str();
		break;
	case SSL_CB_ACCEPT_EXIT:
		LOG(INFO) << "SSL_CB_ACCEPT_EXIT" << stateMsg.str();
		break;
	case SSL_CB_CONNECT_LOOP:
		LOG(INFO) << "SSL_CB_CONNECT_LOOP" << stateMsg.str();
		break;
	case SSL_CB_CONNECT_EXIT:
		LOG(INFO) << "SSL_CB_CONNECT_EXIT" << stateMsg.str();
		break;
	case SSL_CB_HANDSHAKE_START:
		LOG(INFO) << "SSL_CB_HANDSHAKE_START" << stateMsg.str();
		break;
	case SSL_CB_HANDSHAKE_DONE:
		LOG(INFO) << "SSL_CB_HANDSHAKE_DONE" << stateMsg.str();
		break;
	default:
		LOG(INFO) << stateMsg.str();
	}
}
