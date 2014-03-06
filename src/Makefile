
CXX=g++
CPPFLAGS += -Wall -g

TARGET			= statsd_client
TARGET_LIB		= lib$(TARGET).a
TARGET_HEADER	= $(TARGET).h
TARGET_SOURCE	= $(TARGET).cpp
TARGET_OBJ		= $(TARGET).o

all: $(TARGET_LIB)

$(TARGET_LIB): $(TARGET_OBJ)
	ar crus $@ $<

$(TARGET_OBJ): $(TARGET_SOURCE) $(TARGET_HEADER)
	$(CXX) -c -o $@ $< $(CPPFLAGS)

clean:
	rm -f $(TARGET_OBJ) $(TARGET_LIB)

