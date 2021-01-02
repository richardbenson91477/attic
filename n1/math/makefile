
BINS=math.a math.so
OBJS=math.o
INCS=math.hpp

all: $(BINS)

$(OBJS): $(INCS)

math.a: $(OBJS)
	ar rs $@ $^

math.so: $(OBJS)
	$(CXX) -shared -o $@ $(OBJS)

test: all

clean:
	rm -f $(BINS) $(OBJS)

