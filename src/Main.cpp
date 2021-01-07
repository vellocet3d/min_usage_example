#include <memory>


#include "vel/App.h"
#include "scenes/Asylum.h"


int main()
{
	// Create a configuration
	auto conf = vel::Config();
	conf.LOGIC_TICK = 120.0;
	conf.CURSOR_HIDDEN = true;
	conf.USE_IMGUI = false;

	// Bootstrap an App
	vel::App::init(conf);

	// Load the initial Scene
	vel::App::get().setScene(new Asylum());

	// Execute the program, passing control to the App execute loop
	vel::App::get().execute();

	return 0;
}

