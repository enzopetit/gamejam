NAME = tomato_blaster

SRC = $(shell find src -type f -name '*.cpp')
OBJ = $(SRC:.cpp=.o)

CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -Isrc
LDFLAGS =

PKG_CONFIG ?= pkg-config
SFML_PKGS ?= sfml-graphics sfml-audio
HAS_PKG := $(shell $(PKG_CONFIG) --exists $(SFML_PKGS) 2>/dev/null && echo yes)

ifeq ($(HAS_PKG),yes)
	CXXFLAGS += $(shell $(PKG_CONFIG) --cflags $(SFML_PKGS))
	LDFLAGS  += $(shell $(PKG_CONFIG) --libs $(SFML_PKGS))
else
	SFML_PREFIX ?= $(shell brew --prefix sfml 2>/dev/null || echo /usr/local)
	CXXFLAGS += -I$(SFML_PREFIX)/include
	LDFLAGS  += -L$(SFML_PREFIX)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -Wl,-rpath,$(SFML_PREFIX)/lib
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
