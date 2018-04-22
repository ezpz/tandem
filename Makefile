
default: all

FLAGS = -W -Wall -Wextra -Werror
FLAGS += -ggdb -std=c++11

BASE_LIBS = -lm -lzmq -lpthread
ALLEGRO_LIBS = -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf
LIBS = $(BASE_LIBS) $(ALLEGRO_LIBS)

INC = -I./include 

DIRS = src src/graph src/dataset
SRCS = $(wildcard $(DIRS:=/*.cc))
OBJS = $(patsubst %.cc,%.o,$(SRCS))

refs:
	cscope -b -R 

tandem: tandem.cc $(OBJS)
	g++ $(FLAGS) $(INC) $^ $(LIBS) -o $@

$(OBJS): %.o: %.cc
	g++ $(FLAGS) $(INC) $^ $(LIBS) -c -o $@ 

all: tandem refs

clean:
	rm -f $(OBJS)
	rm -f tandem
	rm -f cscope.out
