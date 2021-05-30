#include "GDLoader.hpp"
#include <MinHook.h>
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include <memory>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <cctype>

EXPORT void zzz_gdloader_stub() {}

std::string& lower_str(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
    return str;
}

namespace GDLoader {
    std::vector<std::shared_ptr<Mod>> g_mods;
    unsigned int g_idCounter = 1; // 0 is reserved for GDLoader itself
    std::unordered_map<void*, void*> g_hookTramps; // used for chaining hooks :D

    Mod::Mod(const std::string& name) {
        m_id = g_idCounter++;
        m_name = name;
        m_module = nullptr;
    }

    MH_STATUS Mod::addHook(void* addr, void* hook, void** orig) {
        if (m_hooks.find(addr) != m_hooks.end()) {
            return MH_ERROR_ALREADY_CREATED;
        }
        auto tramp = g_hookTramps.find(addr);
        auto actual = addr;
        if (tramp != g_hookTramps.end()) {
            addr = tramp->second;
        }
        auto status = MH_CreateHook(addr, hook, orig);
        if (status == MH_OK) {
            m_hooks[actual] = addr;
            g_hookTramps[actual] = *orig;
        }
        return status;
    }

    void Mod::enableHook(void* addr) {
        MH_EnableHook(m_hooks.at(addr));
    }

    void Mod::enableAllHooks() {
        for (const auto& [_, addr] : m_hooks) {
            MH_QueueEnableHook(addr);
        }
        MH_ApplyQueued();
    }

    void Mod::disableHook(void* addr) {
        MH_DisableHook(addr);
        m_hooks.erase(m_hooks.find(addr));
    }

    void Mod::unload(bool free) {
        for (const auto& [_, addr] : m_hooks) {
            MH_RemoveHook(addr);
        }
        const auto it = std::find_if(g_mods.begin(), g_mods.end(), [&](const auto& mod) { return mod->getID() == m_id; });
        if (it != g_mods.end())
            g_mods.erase(it);
        if (free && m_module != nullptr)
            FreeLibrary(reinterpret_cast<HMODULE>(m_module));
    }

    std::shared_ptr<Mod> createMod(const std::string& name) {
        auto mod = std::make_shared<Mod>(name);
        g_mods.push_back(mod);
        return mod;
    }

    std::unordered_set<std::wstring> g_loadLater;

    bool (__thiscall* LoadingLayer_init)(void*, bool);
    bool __fastcall LoadingLayer_init_H(void* self, void*, bool unk) {
        if (!LoadingLayer_init(self, unk))
            return false;

        for (const auto& path : g_loadLater) {
            LoadLibraryW(path.c_str());
        }
        g_loadLater.clear();

        return true;
    }

    void init() {
        MH_Initialize();
        // TODO: maybe make some way to make mods load one at a time
        // although this multihook branch has a mutex and should
        // handle multiple mods loading at the same time fine?
        TCHAR _basePath[MAX_PATH];
        GetModuleFileNameA(NULL, _basePath, MAX_PATH);
        std::filesystem::path basePath = _basePath;
        basePath = basePath.parent_path();

        std::unordered_set<std::string> loadLaterNames;

        if (std::filesystem::exists(basePath / "settings.txt")) {
            std::ifstream file(basePath / "settings.txt");
            std::string line;
            while (std::getline(file, line)) {
                loadLaterNames.insert(lower_str(line));
            }
            file.close();
        }

        auto modsPath = basePath / "mods";
        if (!std::filesystem::exists(modsPath) || !std::filesystem::is_directory(modsPath)) {
            std::filesystem::create_directory(modsPath);
        }
        for (const auto& entry : std::filesystem::directory_iterator(modsPath)) {
            const auto& path = entry.path();
            if (path.extension() == ".dll") {
                auto filename = path.filename().string();
                if (loadLaterNames.find(lower_str(filename)) != loadLaterNames.end()) {
                    g_loadLater.insert(path.wstring());
                } else {
                    LoadLibraryW(path.wstring().c_str());
                }
            }
        }
        const auto addr = reinterpret_cast<char*>(GetModuleHandle(0)) + 0x18c080;
        // create a mod instead of just calling minhook directly
        // because what if some mod uses loadinglayer::init you never know
        auto mod = createMod("GDLoader");
        mod->addHook(
            addr,
            LoadingLayer_init_H,
            &LoadingLayer_init
        );
        // maybe disable the hook after its called once
        // although disabling hooks is broken rn lol
        mod->enableAllHooks();
    }
}

DWORD WINAPI DllThread(void* module) {
    DisableThreadLibraryCalls(reinterpret_cast<HMODULE>(module));
    GDLoader::init();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, DllThread, hModule, 0, 0);
    return TRUE;
}