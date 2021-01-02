
BINS=ray_test1
LIBS=../ray.a ../../dlog/dlog.a ../../timer/timer.a ../../win/win.a ../../draw/draw.a ../../math/math.a
LDLIBS=-lSDL2 -lGL

all: $(BINS)

$(BINS): $(LIBS)

test: all
	./ray_test1 300

clean:
	rm -f $(BINS) *.o

