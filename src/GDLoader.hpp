#pragma once
#include <string>
#include <vector>
#include <memory>

#define EXPORT __declspec(dllexport)

namespace GDLoader {
    class Mod {
    protected:
        int m_id;
        std::string m_name;
        std::vector<void*> m_hooks;
        void* m_module; // HMODULE
    public:
        Mod(const std::string& name);
        const std::string& getName() const { return m_name; }
        EXPORT void addHook(void* addr, void* hook, void** orig);
        template <typename Addr, typename Hook, typename Orig>
        auto addHook(Addr a, Hook b, Orig c) {
            return addHook(reinterpret_cast<void*>(a), reinterpret_cast<void*>(b), reinterpret_cast<void**>(c));
        }
        EXPORT void* addHook(void* addr, void* hook);
        EXPORT void enableHook(void* addr);
        EXPORT void enableAllHooks();
        EXPORT void disableHook(void* addr);
    };
    EXPORT std::shared_ptr<Mod> createMod(const std::string& name);
}