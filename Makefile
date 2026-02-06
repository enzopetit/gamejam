NAME = tomato_blaster

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

SFML_PREFIX = $(shell brew --prefix sfml)

CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -I$(SFML_PREFIX)/include
LDFLAGS = -L$(SFML_PREFIX)/lib -lsfml-graphics -lsfml-window -lsfml-system -Wl,-rpath,$(SFML_PREFIX)/lib

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
