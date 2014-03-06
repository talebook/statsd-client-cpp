
#include <iostream>
#include <unistd.h>
#include "statsd_client.h"
using namespace statsd;

int main(void)
{
    std::cout << "running..." << std::endl;
    
    StatsdClient client("127.0.0.1", 8125);
    StatsdClient client2("127.0.0.1", 8125, "myproject.abx.");

    client.count("count1", 123, 1.0);
    client.count("count2", 125, 1.0);
    client.gauge("speed", 10);
    client2.timing("request", 2400);
    sleep(1);
    client.inc("count1", 1.0);
    client2.dec("count2", 1.0);
    int i;
    for(i=0; i<10; i++) {
        client2.count("count3", i, 0.8);
    }

    std::cout << "done" << std::endl;
    return 0;
}
