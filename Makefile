
ALL = plot

default: plot

FLAGS = -W -Wall -Wextra -Werror
FLAGS += -ggdb

BASE_LIBS = -lm
ALLEGRO_LIBS = -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf
LIBS = $(BASE_LIBS) $(ALLEGRO_LIBS)

INC = -I./include

SRCS = src/plot.cc \
	   src/plot_util.cc \
	   plotter.cc

plot: 
	g++ $(FLAGS) $(INC) $(SRCS) $(LIBS) -o plot

clean:
	rm -f $(ALL)
