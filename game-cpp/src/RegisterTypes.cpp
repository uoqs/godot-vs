#include "CustomSprite.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>

using namespace godot;


namespace
{
    void initialize(ModuleInitializationLevel p_level)
    {
        if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
        {
            ClassDB::register_class<CustomSprite>();
        }
    }
    
    void deinitialize(ModuleInitializationLevel p_level)
    {
    }
}

extern "C"
{
    GDExtensionBool GDE_EXPORT libgame_entry(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
        
        init_obj.register_initializer(initialize);
        init_obj.register_terminator(deinitialize);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}