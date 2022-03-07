#include "Bot.hpp"
#include <fcntl.h>

int main(void)
{
	Bot bota;

	bota.idBot("bota");
	bota.startBot("0.0.0.0", 6667, "123");
	return 0;
}
