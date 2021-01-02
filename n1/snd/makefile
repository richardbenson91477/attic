
OBJS=snd.o
INCS=snd.hpp
BINS=snd.a snd.so
LIBS=../math/math.so ../dlog/dlog.so ../file/file.so ../arc/arc.so -lSDL2_mixer

all: $(BINS)

$(OBJS): $(INCS)

snd.a: $(OBJS)
	ar rs $@ $^

snd.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LIBS)

test: all

clean:
	rm -f $(BINS) $(OBJS)

