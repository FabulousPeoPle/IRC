
#include <iostream>
#include <vector>
#include <map>

int main (void)
{
	std::map<std::string, int> m;
	m["first"] = 1;
	m["second"] = 2;
	m["third"] = 3;
	m["forth"] = 4;
	m["fifth"] = 5;
	m["sexth"] = 6;
	m["seventh"] = 7;

	std::map<std::string, int>::iterator it_m = m.begin();
	std::map<std::string, int>::iterator end_m = m.end();
	std::vector<std::string> v;
	while (it_m != end_m)
	{
		v.push_back(it_m->first);
		std::cout << it_m->first << std::endl;
		it_m++;
	}
	return (0);
}