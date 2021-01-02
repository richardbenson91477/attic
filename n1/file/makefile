
OBJS=file.o
INCS=file.hpp
BINS=file.a file.so
LIBS=../dlog/dlog.so

all: $(BINS)

$(OBJS): $(INCS)

file.a: $(OBJS)
	ar rs $@ $^

file.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LIBS)

test: all

clean:
	rm -f $(BINS) $(OBJS)

