
BINS=img.so img.a
OBJS=img.o
INCS=img.hpp
LIBS=../dlog/dlog.so ../file/file.so ../arc/arc.so ../blaze/blaze.so -lpng 

all: $(BINS)
	+$(MAKE) -C tools

$(OBJS): $(INCS)

img.a: $(OBJS)
	ar rs $@ $(OBJS)

img.so: $(OBJS)
	$(CXX) -shared -o $@ $(OBJS) $(LIBS)

test: all

clean:
	rm -f $(OBJS) $(BINS)
	+$(MAKE) -C tools clean

