
CXX=g++
CPPFLAGS += -Wall -g

STATSD_INC = -I ../src/
STATSD_LIB = -L ../src/ -lstatsd_client
TARGETS=test_client system_monitor

all: $(TARGETS)

test_client: test_client.cpp
	$(CXX) -o $@ $< $(CPPFLAGS) $(STATSD_INC) $(STATSD_LIB)

system_monitor: system_monitor.cpp
	$(CXX) -o $@ $< $(CPPFLAGS) $(STATSD_INC) $(STATSD_LIB)

clean:
	rm -f $(TARGETS)

