#include <iostream>
#include <map>
#include <string>

using namespace std;

typedef int (*fun_type) (const char* p);


typedef map<string, fun_type> fMap;
typedef pair<string, fun_type> fPair;

int hello(const char* p) {
  cout << p << endl;
  return 0;
}


int main ()
{
  std::map<char,int> mymap;

  // first insert function version (single parameter):
  mymap.insert ( std::pair<char,int>('a',100) );
  mymap.insert ( std::pair<char,int>('z',200) );
  mymap.insert (std::pair<char, int>('a', 150));

  // showing contents:
  std::cout << "mymap contains:\n";
  for (std::map<char,int>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';

  fMap fmap;
  fmap.insert(make_pair("a", hello));

  std::cout << fmap["a"] << " ... " << fmap["b"] << std::endl;

  return 0;
}
