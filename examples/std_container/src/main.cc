#include "logger.h"
#include <list>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct movie {
  std::string title;
  int year;
};

std::ostream &operator<<(std::ostream &os, movie const &arg) {
  os << "title = " << arg.title << ", year = " << arg.year;
  return os;
}


int main() {
  std::vector<int> vector_of_int = {1, 2, 3, 4, 5};
  LOGDEBUG("%s", STD_CONTAINER(vector_of_int).c_str());

  std::list<int> list_of_int = {1, 2, 3, 4, 5};
  LOGDEBUG("%s", STD_CONTAINER(list_of_int).c_str());

  std::map<int, std::string> map_of_int_string = {{1, "one"}, {2, "two"}, {3, "three"}};
  LOGDEBUG("%s", STD_CONTAINER(map_of_int_string).c_str());

  std::unordered_map<int, std::string> unordered_map_of_int_string = {{1, "one"}, {2, "two"}, {3, "three"}};
  LOGDEBUG("%s", STD_CONTAINER(unordered_map_of_int_string).c_str());

  std::map<std::string, std::vector<std::string>> map_of_string_vector_string = {
    {"one", {"one", "two", "three"}},
    {"two", {"four", "five", "six"}}};

  LOGDEBUG("%s", STD_CONTAINER(map_of_string_vector_string).c_str());

  std::vector<movie> vector_of_movie = {{"The Shawshank Redemption", 1994},
                                        {"The Godfather", 1972},
                                        {"The Dark Knight", 2008}};

  LOGDEBUG("%s", STD_CONTAINER(vector_of_movie).c_str());

  movie film = {"The Shawshank Redemption", 1994};
  LOGDEBUG("%s", CPP_CLASS(film).c_str());

  return 0;
}
