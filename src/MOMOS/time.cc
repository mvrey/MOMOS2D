#include <MOMOS/time.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

namespace MOMOS {
  
	double Time() {
		return glfwGetTime() * 1000.0;
	}


	void Sleep(unsigned int ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}

 }