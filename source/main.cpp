//#include "toml.hpp"
#include <cassert>
#include <thread>

int	main(void) {
	std::thread thread;

	thread.hardware_concurrency();

	return (0);
}
