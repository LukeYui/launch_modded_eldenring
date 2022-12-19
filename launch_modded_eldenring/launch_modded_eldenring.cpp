#include "launch_modded_eldenring.h"

int main() {
    LaunchModdedEldenRing ModLauncher = LaunchModdedEldenRing();
    ModLauncher.Run();
    return 0;
}

void LaunchModdedEldenRing::Run() {
    using namespace std::chrono_literals;

    if (!CheckConditions()) {
        should_run = false;
        system("PAUSE");
        return;
    };

    if (!LaunchGame()) {
        should_run = false;
        system("PAUSE");
        return;
    };

    if (!InjectDLLMods()) {
        should_run = false;
        system("PAUSE");
        return;
    };

    // Replace with anything else you'd like to do
    // doesn't have to be a dll injection
    /*
    if (!InjectMod("ModName.dll")) {
        should_run = false;
        system("PAUSE");
        return;
    };
    */

    // Can also ust close as the launcher isn't needed anymore
    printf_s("Elden Ring is running...\n");
    WaitForSingleObject(elden_ring_handle, INFINITE);
    CloseHandle(elden_ring_thread);
    CloseHandle(elden_ring_handle);

    return;
};

bool LaunchModdedEldenRing::CheckConditions() {
    if (FileNotFound(L"eldenring.exe")) {
        printf_s("Failed to find: \"eldenring.exe\". Make sure you put the mod files inside your \"ELDEN RING\\Game\\\" folder!\n");
        return false;
    }

    std::string exe_path = GetExePath() + "eldenring.exe";
    if (exe_path.length() > MAX_PATH) {
        printf_s("Cannot load path! Path is too long! %llu\\%d \n\"%s\"\n", exe_path.length(), MAX_PATH, exe_path.c_str());
        return false;
    }

    //Warnings - These do not cause failure.  
    if (!FileNotFound(L"dinput8.dll")) {
        printf_s("Warning: You are launching seamless with the launcher and there is a dinput8.dll.\n"
            "If this dinput8.dll is a mod loader, it is recommended you use the dll mod loader included with this launcher\n"
            "Check the README for details on how to add dll mods to the launcher via \"eldenring_mods.txt\"\n");
    }

    return true;
}

bool LaunchModdedEldenRing::LaunchGame() {
    OFSTRUCT file_struct = {};
    HFILE elden_ring_exe_handle = OpenFile("eldenring.exe", &file_struct, OF_EXIST);
    if (elden_ring_exe_handle == HFILE_ERROR) {
        printf_s("Failed to find \"eldenring.exe\"\n");
        return false;
    };

    if (!SetEnvironmentVariableA("SteamAppId", "1245620")) {
        printf_s("Failed to create appropriate launch environment\n");
        return false;
    };

    STARTUPINFOA startup_info = {};
    PROCESS_INFORMATION process_info = {};
    if (!CreateProcessA("eldenring.exe", nullptr, nullptr, nullptr,
                        false, 0, nullptr, nullptr, &startup_info, &process_info)) {
        printf_s("Failed to launch \"eldenring.exe\"\n");
        return false;
    };

    elden_ring_thread = process_info.hThread;
    elden_ring_handle = process_info.hProcess;

    return true;
}

bool LaunchModdedEldenRing::InjectDLLMods() {
    std::ifstream file("eldenring_mods.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            printf_s("Loading %s...\n", line.c_str());
            if (!InjectMod(line.c_str())) {
                printf_s("Failed to load %s...\n", line.c_str());
                return false;
            };
        }
        file.close();
    }
    return true;
};

bool LaunchModdedEldenRing::InjectMod(const char* mod_name) {
    OFSTRUCT file_struct = {};
    HFILE elden_ring_exe_handle = OpenFile(mod_name, &file_struct, OF_EXIST);
    if (elden_ring_exe_handle == HFILE_ERROR) {
        std::string mod_path = GetExePath() + "eldenring.exe";
        printf_s("Failed to find: (%llu)\n\"%s\"\n", mod_path.length(), mod_path.c_str());
        return false;
    };

    HANDLE elden_ring_handle_open = OpenProcess(PROCESS_ALL_ACCESS, false, GetProcessId(elden_ring_handle));
    if (elden_ring_handle_open == INVALID_HANDLE_VALUE) {
        printf_s("Failed to open process: \"eldenring.exe\"\n");
        return false;
    };

    size_t memory_alloc = strlen(mod_name);
    void* dll_path = VirtualAllocEx(elden_ring_handle_open, 0, memory_alloc, MEM_COMMIT, PAGE_READWRITE);
    if (!dll_path) {
        printf_s("Out of memory\n");
        return false;
    };

    FARPROC load_library_a = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");
    if (!load_library_a) {
        printf_s("Unable to load Kernel32.dll. Cannot call \"LoadLibraryA\" to load dll mods!\n");
        return false;
    };

    WriteProcessMemory(elden_ring_handle, dll_path, mod_name, memory_alloc, nullptr);

    HANDLE dll_load_thread = CreateRemoteThread(elden_ring_handle, 0, 0, (LPTHREAD_START_ROUTINE)load_library_a,
                                                dll_path, 0, 0);
    if (!dll_load_thread) {
        std::string modPath = GetExePath() + mod_name;
        printf_s("Unable to inject dll mod: (%llu)\n\"%s\"\n", modPath.length(), modPath.c_str());
        return false;
    };

    WaitForSingleObject(dll_load_thread, INFINITE);

    VirtualFreeEx(elden_ring_handle, dll_path, 0, MEM_RELEASE);

    CloseHandle(dll_load_thread);

    CloseHandle(elden_ring_handle_open);
    return true;
};

bool LaunchModdedEldenRing::FileNotFound(const std::wstring& name) {
    return INVALID_FILE_ATTRIBUTES == GetFileAttributes(name.c_str()) && GetLastError() == ERROR_FILE_NOT_FOUND;
}

std::string LaunchModdedEldenRing::GetExePath() {
    char module_dir[MAX_PATH + 1];
    HMODULE mod;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)&"", &mod);
    GetModuleFileNameA(mod, module_dir, sizeof(module_dir));
    char* module_file_path_point = strrchr(module_dir, '\\');
    if (!module_file_path_point) {
        MessageBoxA(0, "Failed to parse filepath string", "Item Randomiser Mod - Error", MB_ICONERROR);
        throw std::runtime_error("Failed to parse filepath string");
    };
    *module_file_path_point = '\\\0'; //add slash and null terminator
    return std::string(module_dir);
}

