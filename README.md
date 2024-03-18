# Godot setup for Visual Studio

## Create a solution

1. Create a blank Visual Studio solution.
    
    
2. Add a new Makefile project named **game-cpp** with the following configuration settings:
    - All Configurations | All Platforms
        - General
        	- Configuration Type: `Makefile`
    - Debug | x64
        - Debugging
            - Command: `$(SolutionDir)godot\bin\godot.windows.editor.dev.x86_64.exe`
        	- Command Arguments: `--path "$(SolutionDir)game" --editor`     
        - NMake
            - Build Command Line: `scons arch=x86_64 dev_build=yes`
        	- Clean Command Line: `scons --clean arch=x86_64 dev_build=yes`
        	- Output:
    - Release | x64
        - Debugging
            - Command: `$(SolutionDir)godot\bin\godot.windows.template_release.x86_64.exe`
        	- Command Arguments `--path "$(SolutionDir)game"`
        - NMake
            - Build Command Line: `scons arch=x86_64 target=template_release production=yes`
        	- Clean Command Line: `scons --clean arch=x86_64 target=template_release production=yes`
        	- Output:
        
    
3. Add an empty C++ project named **godot-cpp** with the following configuration settings:
    - All Configurations | All Platforms
    	- General
        	- Configuration Type: `Utility`
    	- VC++ Directories
        	- Public Include Directories
            	```
            	$(ProjectDir)include
            	$(ProjectDir)gen\include
            	$(ProjectDir)gdextension
            	```
            
    
4. Open the **Reference Manager** and select the **game-cpp** project. Add a reference to the **godot-cpp** project.

## Get the source

1. Create a git repo in the solution root:
    
    ```sh
    git init
    ```
    
2. Add Godot Engine source as a submodule:
    
    ```sh
    git submodule add --branch 4.2 https://github.com/godotengine/godot.git
    ```
    
3. Add Godot Engine C++ bindings as a submodule:
    
    ```sh
    mv godot-cpp tmp
    git submodule add --branch 4.2 https://github.com/godotengine/godot-cpp.git
    mv tmp\* godot-cpp
    rm tmp
    ```
    

## **Configure Hot Reload**

1. To enable Hot Reload for Godot Editor replace option `/Zi` with `/ZI` in a file `godot\SConstruct`
    
    
2. To enable Hot Reload for the game code replace option `/Zi` with `/ZI` in a file `godot‑cpp\tools\target.py`
    
> [!NOTE]
> The `/ZI` option disables `#pragma optimize` statements, which are used in Godot Engine to fix a compilation bug, present in Visual Studio versions below 16.4

## Configure the Godot Engine project

1. Build Godot Engine in a release configuration
    
    ```sh
    cd godot
    scons arch=x86_64 target=template_release production=yes vsproj=yes
    ```
    
2. Open the newly created `godot\godot.vsproj` project in Visual Studio and copy the following properties from the **Property Pages** into a temporary text file:
    - NMake
        - template_release | All Platforms
            - Build Command Line
            - Clean Command Line
            - Output
            - Preprocessor Definitions
            - Additional Options
    
3. Build Godot Engine in a debug configuration and overwrite the project file:
    
    ```sh
    scons arch=x86_64 target=editor dev_build=yes vsproj=yes
    ```
    
4. Add the `godot\godot.vsproj` project to the root solution.
    
    
5. Open the **Configuration Manager** and make the following changes:
    1. Delete the newly created solution configurations:
        - editor
        - template_debug
        - template_release
    2. Select the **Debug** solution configuration and set the **godot** project context to **editor**, repeat for every platform.
    3. Select the **Release** solution configuration and set the **godot** project context to **template_release**, repeat for every platform.
    
6. Open the **Property Pages** of the **godot** project and set properties of the **template_release** configuration to the values previously copied from the release version of the project file.
    
    
7. To prevent an accidental overwrite of the project file clear the **Rebuild All Command Line** property value for all configurations and platforms.

## Create a game extension

1. Create a file named `SConstruct` and place it in the `game-cpp` directory:
    
    
<table>
<tr>
<th align="left">game-cpp\SConstruct</th>
</tr>
<tr>
<td>

```python
#!/usr/bin/env python

env = SConscript("../godot-cpp/SConstruct")
env.Append(CPPPATH='src')

sources = Glob('src/*.cpp')
library = env.SharedLibrary("../game/bin/libgame{}{}".format(env["suffix"], env["SHLIBSUFFIX"]), source=sources)

Default(library)
```

</td>
</tr>
</table>

2. Create a directory `game-cpp\src` where the game extension source files will be placed.
    
    
3. Create a class named `CustomSprite` and place it in the `game-cpp\src` directory:
    
<table>
<tr>
<th>game-cpp\src\CustomSprite.h</th>
<th>game-cpp\src\CustomSprite.cpp</th>
</tr>
<tr valign="top">
<td>

```cpp
#pragma once
#include <godot_cpp/classes/sprite2d.hpp>

class CustomSprite : public godot::Sprite2D
{
    GDCLASS(CustomSprite, Sprite2D)

public:
    void _process(double delta) override;

protected:
    static void _bind_methods();
    double time_passed = 0.0;
};
```
    
</td>
<td>

```cpp
#include "CustomSprite.h"
using namespace godot;

void CustomSprite::_process(double delta)
{
    time_passed += delta;
    Vector2 new_position = Vector2
    (
        10.0 + 10.0 * sin(time_passed * 2.0),
        10.0 + 10.0 * cos(time_passed * 2.0)
    );
        
    set_position(new_position);
}

void CustomSprite::_bind_methods()
{
}
```

</td>
</tr>
</table>


4. Create a file named `RegisterTypes.cpp` and place it in the `game-cpp\src` directory:
    
    
<table>
<tr>
<th align="left">game-cpp\src\RegisterTypes.cpp</th>
</tr>
<tr>
<td>

```cpp
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
    GDExtensionBool GDE_EXPORT libgame_entry(GDExtensionInterfaceGetProcAddress p_get_proc_address,
        const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
        
        init_obj.register_initializer(initialize);
        init_obj.register_terminator(deinitialize);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}
```

</td>
</tr>
</table>

5. Set solution configuration to **Debug** and platform to **x64** and build the **game-cpp** project.
    
    
6. A directory `game\bin` should now appear in the solution root, containing a module library and its accompanying files.
    
    
7. The Visual Studio IntelliSense should now recognize includes from the **godot‑cpp** project. If not, close Visual Studio and delete a directory named `.vs` to reset the cache.
    
    
8. Create a module configuration file named `libgame.gdextension` and place it in the `game\bin` directory:
    

<table>
<tr>
<th align="left">game\bin\libgame.gdextension</th>
</tr>
<tr>
<td>

```python
[configuration]
entry_symbol = "libgame_entry"
compatibility_minimum = "4.2"

[libraries]
windows.debug.x86_64 = "res://bin/libgame.windows.template_debug.dev.x86_64.dll"
windows.release.x86_64 = "res://bin/libgame.windows.template_release.x86_64.dll"
```

</td>
</tr>
</table>

## Create a Godot project

1. Run the **Debug** configuration of the **game-cpp** project.
    
    
2. Since the Godot project doesn’t yet exist, a Godot **Project Manager** will launch instead of the editor.
    
    
3. Create a new Godot project in the `game` directory. You will see a warning stating that the selected directory is not empty and the existing files will be imported as project resources. Confirm that you wish to continue.
    
    
4. After the Godot project is created, the editor will launch as a separate process. Use **Attach to Process…** in Visual Studio to be able to debug the code and to use hot reload.
    
    
5. In the editor, you should now be able to find a node named `CustomSprite` in the **Create New Node** dialog and place it in the scene.


## References

Introduction to the buildsystem  
https://docs.godotengine.org/en/stable/contributing/development/compiling/introduction_to_the_buildsystem.html

Compiling for Windows  
https://docs.godotengine.org/en/stable/contributing/development/compiling/compiling_for_windows.html

GDExtension C++ example  
https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html

/Z7, /Zi, /ZI (Debug Information Format)  
https://learn.microsoft.com/en-us/cpp/build/reference/z7-zi-zi-debug-information-format