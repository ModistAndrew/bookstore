#include <iostream>
#include <sstream>
#include <string>
int main() {
  std::string s("11111ade==awef===aewfaef");
  std::stringstream ss(s);
  std::string a, b;
  std::getline(ss, a, '=');
  ss >> b;
  return 0;
}
