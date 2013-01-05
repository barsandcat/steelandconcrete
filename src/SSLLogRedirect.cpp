#include <pch.h>
#include <SSLLogRedirect.h>


void SSLInfoCallback(const SSL *s, int where, int ret)
{
    const char *str;
    int w;

    w = where& ~SSL_ST_MASK;

    if (w & SSL_ST_CONNECT)
    {
        str = "SSL_connect";
    }
    else if (w & SSL_ST_ACCEPT)
    {
        str = "SSL_accept";
    }
    else
    {
        str = "undefined";
    }

    if (where & SSL_CB_LOOP)
    {
        LOG(INFO) << str << ":" << SSL_state_string_long(s);
    }
    else if (where & SSL_CB_ALERT)
    {
        str = (where & SSL_CB_READ) ? "read" : "write";
        LOG(INFO) << "SSL3 alert " << str << ":" << SSL_alert_type_string_long(ret) << ":" << SSL_alert_desc_string_long(ret);
    }
    else if (where & SSL_CB_EXIT)
    {
        if (ret == 0)
        {
            LOG(INFO) << str << ":failed in " << SSL_state_string_long(s);
        }
        else if (ret < 0)
        {
            LOG(INFO) << str << ":error in " << SSL_state_string_long(s);
        }
    }
}
