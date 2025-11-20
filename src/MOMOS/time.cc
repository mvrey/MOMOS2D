#include <MOMOS/time.h>
#include <GLFW/glfw3.h>

namespace MOMOS {
  
	double Time() {
		return glfwGetTime() * 1000.0;
	}


	void Sleep(unsigned int ms) {

	}

 }