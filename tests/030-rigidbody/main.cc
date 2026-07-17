#include <memory>
#include "a3d/Application.h"
#include "App.h"

int main(int argc, char* argv[]) {
	return a3d::Application::Run(std::make_unique<test::rigidbody::App>(argc, argv));
}
