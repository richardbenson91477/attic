
OBJS=s.o conf.o ball.o goal.o menu.o player.o cam.o pushem.o
INCS=../n1.hpp ball.hpp cam.hpp conf.hpp goal.hpp menu.hpp player.hpp pushem.hpp s.hpp
LIBS=../n1.a ../blaze/blaze.a ../img/img.a ../arc/arc.a ../file/file.a ../timer/timer.a ../dlog/dlog.a ../snd/snd.a ../math/math.a ../win/win.a
LDFLAGS=-lSDL2 -lSDL2_mixer -lGL -lm -lpng

all: lib_check pushem

lib_check:
	+$(MAKE) -C .. n1.a

$(OBJS): $(INCS)

pushem: $(OBJS) $(LIBS)
	$(CXX) -o $@ $^ $(LDFLAGS)

test: all
	cd data; ../pushem

clean:
	rm -f pushem $(OBJS)

