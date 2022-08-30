#include <windows.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>

class LaunchModdedEldenRing {
public:
	void Run();
	bool LaunchGame();
	bool InjectDLLMods();
	bool InjectMod(const char* mod_name);

	LaunchModdedEldenRing() {
		should_run = true;
		elden_ring_thread = INVALID_HANDLE_VALUE;
		elden_ring_handle = INVALID_HANDLE_VALUE;
	};

private:
	bool should_run;
	HANDLE elden_ring_thread;
	HANDLE elden_ring_handle;
};
