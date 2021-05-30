#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#ifdef GDLOADER_EXPORT
#define GDLOADER_DLL __declspec(dllexport)
#else
#define GDLOADER_DLL __declspec(dllimport)
#endif

enum MH_STATUS;

namespace GDLoader {
    class Mod {
    protected:
        unsigned int m_id;
        std::string m_name;
        std::unordered_map<void*, void*> m_hooks;
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
        GDLOADER_DLL MH_STATUS addHook(void* addr, void* hook, void** orig);

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
        GDLOADER_DLL void enableHook(void* addr);

        // Enables all created hooks so far
        GDLOADER_DLL void enableAllHooks();

        // Disables a hook by its target address
        //
        // Note that this is broken atm and will
        // end up disabling every hook of the target
        // function instead of just this mod's
        GDLOADER_DLL void disableHook(void* addr);

        // Removes all the hooks in this mod, and if
        // provided, unloads the module as well.
        //
        // Note that this is extremely unsafe and
        // should be used with caution
        GDLOADER_DLL void unload(bool free = false);
    };
    
    // Creates a mod and stores it internally in GDLoader
    GDLOADER_DLL std::shared_ptr<Mod> createMod(const std::string& name);
}