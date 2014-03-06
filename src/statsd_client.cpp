
#include <math.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "statsd_client.h"

using namespace std;
using namespace statsd;

inline bool fequal(float a, float b)
{
    const float epsilon = 0.0001;
    return ( fabs(a - b) < epsilon );
}

inline bool should_send(float sample_rate)
{
    if ( fequal(sample_rate, 1.0) )
    {
        return true;
    }

    float p = ((float)random() / RAND_MAX);
    return sample_rate > p;
}

StatsdClient::StatsdClient(const string& host, int port, const string& ns)
{
    _ns = ns;
    _host = host;
    _port = port;
    _init = false;
    _sock = -1;
    srandom(time(NULL));
}

StatsdClient::~StatsdClient()
{
    // close socket
    if (_sock >= 0) {
        close(_sock);
        _sock = -1;
    }
}

int StatsdClient::init()
{
    if ( _init ) return 0;

    _sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( _sock == -1 ) {
        snprintf(_errmsg, sizeof(_errmsg), "could not create socket, err=%m");
        return -1;
    }

    memset(&_server, 0, sizeof(_server));
    _server.sin_family = AF_INET;
    _server.sin_port = htons(_port);

    int ret = inet_aton(_host.c_str(), &_server.sin_addr);
    if ( ret == 0 )
    {
        // host must be a domain, get it from internet
        struct addrinfo hints, *result = NULL;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;

        ret = getaddrinfo(_host.c_str(), NULL, &hints, &result);
        if ( ret ) {
            snprintf(_errmsg, sizeof(_errmsg),
                    "getaddrinfo fail, error=%d, msg=%s", ret, gai_strerror(ret) );
            return -2;
        }
        struct sockaddr_in* host_addr = (struct sockaddr_in*)result->ai_addr;
        memcpy(&_server.sin_addr, &host_addr->sin_addr, sizeof(struct in_addr));
        freeaddrinfo(result);
    }

    _init = true;
    return 0;
}

/* will change the original string */
void StatsdClient::cleanup(string& key)
{
    size_t pos = key.find_first_of(":|@");
    while ( pos != string::npos )
    {
        key[pos] = '_';
        pos = key.find_first_of(":|@");
    }
}

int StatsdClient::dec(const string& key, float sample_rate)
{
    return count(key, -1, sample_rate);
}

int StatsdClient::inc(const string& key, float sample_rate)
{
    return count(key, 1, sample_rate);
}

int StatsdClient::count(const string& key, size_t value, float sample_rate)
{
    return send(key, value, "c", sample_rate);
}

int StatsdClient::gauge(const string& key, size_t value, float sample_rate)
{
    return send(key, value, "g", sample_rate);
}

int StatsdClient::timing(const string& key, size_t ms, float sample_rate)
{
    return send(key, ms, "ms", sample_rate);
}

int StatsdClient::send(string key, size_t value, const string &type, float sample_rate)
{
    if (!should_send(sample_rate)) {
        return 0;
    }

    cleanup(key);

    if ( fequal( sample_rate, 1.0 ) )
    {
        snprintf(buf, sizeof(buf), "%s%s:%zd|%s",
                _ns.c_str(), key.c_str(), value, type.c_str());
    }
    else
    {
        snprintf(buf, sizeof(buf), "%s%s:%zd|%s|@%.2f",
                _ns.c_str(), key.c_str(), value, type.c_str(), sample_rate);
    }

    return send(buf);
}

int StatsdClient::send(const string &message)
{
    int ret = init();
    if ( ret )
    {
        return ret;
    }
    ret = sendto(_sock, message.data(), message.size(), 0, (struct sockaddr *) &_server, sizeof(_server));
    if ( ret == -1) {
        snprintf(_errmsg, sizeof(_errmsg),
                "sendto server fail, host=%s:%d, err=%m", _host.c_str(), _port);
        return -1;
    }
    return 0;
}

