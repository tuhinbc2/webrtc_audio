ALL = agc_test.out

all: $(ALL)

WEBRTC_PATH := ../../../../
CPP_PATH := ../legacy/
BASE_PATH := $(WEBRTC_PATH)base/
SP_PATH := $(WEBRTC_PATH)common_audio/signal_processing/

C++ := g++
C   := gcc

CPPFLAGS := -std=c++11 -Wall -Wextra -DWEBRTC_POSIX

#LDLIBS := \
#	-lgtest \
#	-lpthread \


INCLUDE := -I./$(WEBRTC_PATH)../ -I./../legacy/

SRCS := $(BASE_PATH)checks.cc \
	$(SP_PATH)dot_product_with_scale.cc \
	source.cpp

analog.o: $(CPP_PATH)analog_agc.c
	$(C) $(INCLUDE) -ggdb -c $(CPP_PATH)analog_agc.c -o analog.o

digital.o: $(CPP_PATH)digital_agc.c
	$(C) $(INCLUDE) -ggdb -c $(CPP_PATH)digital_agc.c -o digital.o

spl.o: $(SP_PATH)spl_sqrt.c
	$(C) $(INCLUDE) -ggdb -c $(SP_PATH)spl_sqrt.c -o spl.o

div.o: $(SP_PATH)division_operations.c
	$(C) $(INCLUDE) -ggdb -c $(SP_PATH)division_operations.c -o div.o

samp.o: $(SP_PATH)resample_by_2.c
	$(C) $(INCLUDE) -ggdb -c $(SP_PATH)resample_by_2.c -o samp.o

copy.o: $(SP_PATH)copy_set_operations.c
	$(C) $(INCLUDE) -ggdb -c $(SP_PATH)copy_set_operations.c -o copy.o


agc_test.out: analog.o digital.o spl.o div.o samp.o copy.o source.cpp
	$(C++) -ggdb   $(INCLUDE) analog.o digital.o spl.o div.o samp.o copy.o $(CPPFLAGS) $(SRCS) -o agc_test.out $(LIBS)

clean:
	$(RM) $(ALL)

-include $(OBJS:%.o=%.d)