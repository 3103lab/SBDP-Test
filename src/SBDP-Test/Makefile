include ../makefile_common

CFLAGS        += -I../LightTest/include/ltest -I../SBDP/include
LIB_DIR       = 
LDFLAGS       += $(LIB_DIR)

DEST          = ../../bin
PROGRAM       = SBDP-Test
TARGET        = $(DEST)/$(PROGRAM)

OBJS          = DecodeTest.o							\
				EncodeTest.o							\
				main.o									\
				RoundTripTest.o							\
				SocketTest.o

# サフィックスルール
%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f *~ $(TARGET) $(OBJS)

#install:        $(PROGRAM)
#                install -s $(PROGRAM) $(DEST)

