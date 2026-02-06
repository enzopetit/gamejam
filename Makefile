NAME = tomato_blaster

SRC = $(shell find src -type f -name '*.cpp')
OBJ = $(SRC:.cpp=.o)

CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -Isrc
LDFLAGS =

PKG_CONFIG ?= pkg-config
SFML_PKG ?= sfml-graphics
HAS_PKG := $(shell command -v $(PKG_CONFIG) >/dev/null 2>&1 && echo yes)

ifeq ($(HAS_PKG),yes)
	CXXFLAGS += $(shell $(PKG_CONFIG) --cflags $(SFML_PKG))
	LDFLAGS  += $(shell $(PKG_CONFIG) --libs $(SFML_PKG))
else
	SFML_PREFIX ?= /usr/local
	CXXFLAGS += -I$(SFML_PREFIX)/include
	LDFLAGS  += -L$(SFML_PREFIX)/lib -lsfml-graphics -lsfml-window -lsfml-system -Wl,-rpath,$(SFML_PREFIX)/lib
endif

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
