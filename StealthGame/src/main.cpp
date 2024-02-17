#define ENABLEMEMORYCOUNTER
#include "Utils/MemoryCounter.h"
#include "App.h"

int main()
{
	try
	{
		App app;
		return app.Exec();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}