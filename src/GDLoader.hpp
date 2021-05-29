#pragma once
#include <string>
#include <vector>
#include <memory>

#define EXPORT __declspec(dllexport)

enum MH_STATUS;

namespace GDLoader {
    class Mod {
    protected:
        unsigned int m_id;
        std::string m_name;
        std::vector<void*> m_hooks;
        void* m_module; // HMODULE
    public:
        Mod(const std::string& name);
        const std::string& getName() const { return m_name; }
        auto getID() const { return m_id; }
        void setModule(void* module) { m_module = module; }

        // Creates a hook using the internal mod id
        // Note that this does not enable the hook
        // Parameters:
        //   addr - Target address
        //   hook - Detour function
        //   orig - Pointer to the trampoline function
        EXPORT MH_STATUS addHook(void* addr, void* hook, void** orig);

        template <typename Addr, typename Hook, typename Orig>
        auto addHook(Addr a, Hook b, Orig c) {
            return addHook(reinterpret_cast<void*>(a), reinterpret_cast<void*>(b), reinterpret_cast<void**>(c));
        }

        // void* addHook(void* addr, void* hook) {
        //     void* orig;
        //     addHook(addr, hook, &orig);
        //     return orig;
        // }

        // Enables a hook by its target address
        EXPORT void enableHook(void* addr);
        // Enables all created hooks so far
        EXPORT void enableAllHooks();
        // Disables a hook by its target address
        EXPORT void disableHook(void* addr);

        // Removes all the hooks in this mod, and if
        // provided, unloads the module as well.
        // Note that this is extremely unsafe and
        // should be used with caution
        EXPORT void unload();
    };
    // Creates a mod and stores it internally in GDLoader
    EXPORT std::shared_ptr<Mod> createMod(const std::string& name);
}