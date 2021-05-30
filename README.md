# GDLoader
###### (name is subject to change)

A mod loader that hopefully resolves hook conflicts by chaining hooks.
This works by having a single instance of the loader, instead of each mod statically linking minhook and causing a big mess! D:

## TODO
- disabling hooks properly
- improve settings.txt
- maybe add a priority system
- add some way for mods to interact with other mods

## Example

```cpp
#include <GDLoader.hpp>

// [...] inside your dll function

// each mod can provide their own name, although
// internally this is not used for anything
auto mod = GDLoader::createMod("awesome-mod");
mod->addHook(base + 0x123, hook, &orig); // use the templated overload
mod->enableAllHooks();
```