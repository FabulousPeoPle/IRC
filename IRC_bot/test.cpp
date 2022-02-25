#include <string>
#include  <iostream>
#include <chrono>

using namespace std;

using sysclock_t = std::chrono::system_clock;

std::string CurrentDate()
{
    std::time_t now = sysclock_t::to_time_t(sysclock_t::now());

    char buf[22] = { 0 };
    std::strftime(buf, sizeof(buf), "%T %Z %d/%m/%y", std::localtime(&now));
    
    return std::string(buf);
}


int main()
{
    string msg = "!bota abbaha";

    if (msg.compare(0, 6, "!bota ") == 0)
        cout << "done" << endl;
    if (msg.compare(6,6, "abbah") == 0)
        cout << "done" << endl;

    cout << CurrentDate() << endl;
    return 0;
}