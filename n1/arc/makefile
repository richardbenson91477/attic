
OBJS=arc.o
INCS=arc.hpp
BINS=arc.a arc.so
LIBS=../blaze/blaze.so ../dlog/dlog.so  ../file/file.so

all: $(BINS)

$(OBJS): $(INCS)

arc.a: $(OBJS)
	ar rs $@ $^

arc.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LIBS)

test: all

clean:
	rm -f $(BINS) $(OBJS)

