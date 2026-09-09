/**
 * Author: Ford Jones
 * Date: 09/09/2026
 * Title: Sponza Demo
 * Minimum version: Lazarus v0.16.3
 
    A simple, single-file lazarus implementation showcasing the sponza atrium by Frank Meinl and Marco Dobravic for Crytek.
    This version of the scene was downloaded from the Khronos samples here: https://github.com/KhronosGroup/glTF-Sample-Assets/tree/main/Models/Sponza
    The asset was converted to glb format using the glTF vscode extension from here: https://marketplace.visualstudio.com/items?itemName=cesium.gltf-vscode

    Compiling with g++:
        g++ main.cpp -O3 -o sponza -llazarus
    
    Compiling with MSVC:
        cl /EHsc /std:c++17 /Zc:__cplusplus main.cpp /I"C:/Program Files/LazarusEngine/include" /link msvcrt.lib user32.lib gdi32.lib shell32.lib /LIBPATH:"C:/Program Files/LazarusEngine/lib" lazarus.lib /out:sponza.exe /NODEFAULTLIB:libcmt

    This sample program was written with Lazarus v0.16.3. Some older versions may work but haven't been tried.

    ----------------
    Tested Devices:
    ----------------
    MSI GF65
    - OS: Windows 11
    - Arch: amd64
    - CPU: Intel 10th Gen Core i5-10400H 4-Core 2.6 – 4.6GHz
    - GPU: NVIDIA GeForce GTX 1650

    MSI Prestige 15
    - OS: Windows 10
    - Arch: x64
    - CPU: Intel Core i7-10710U @ 1.10GHz
    - GPU: Intel UHD Graphics

    GA-Z97X-SLI
    - OS Windows 10
    - Arch: x64
    - CPU: Intel Core i5 3.5ghz
    - GPU: AMD Radeon RX 590

    Dell Optiplex 9020
    - OS: Windows 10
    - Arch: x64
    - CPU: Intel 4th Gen Core i7 3.4ghz
    - GPU: Nvidia GeForce GT 1030
    
    Asus Zenbook
    - OS: Linux Debian 13.0 (Trixee) | Linux Ubuntu 22.04.5 (Jammy)
    - Graphical session: KDE-plasma (X11, wayland) | Gnome (wayland)
    - Arch: x86_64
    - CPU: Intel i7-10510U (8) @ 4.900GHz
    - GPU: Intel CometLake-U GT2 [UHD Graphics]

    MacBook Pro (2020)
    - OS: Sonoma 14.8.7
    - Arch: Arm64
    - CPU: Apple M1
    - GPU: M1 SoC (System-on-Chip, unified graphics)

    Mac Mini (2014)
    - OS: MacOS Monterey 12.6.3
    - Arch: x86_64
    - CPU: 1.4 GHz Dual-Core Intel Core i5
    - GPU: Intel HD Graphics 5000 1536 MB
*/
#include <lazarus.h>

#ifndef LAZARUS_EXAMPLE_SPONZA
#define LAZARUS_EXAMPLE_SPONZA

float moveX = 0.0f;
float moveZ = 0.0f;
float turnX = 0.0f;
float turnY = 0.0f;

int main()
{
    /* Init and config */
    Lazarus::WindowManager window = Lazarus::WindowManager();
    Lazarus::WindowManager::WindowConfig win_config = {};
    win_config.title = "Lazarus Sponza Demo";
    win_config.fullscreen = true;
    win_config.disableCursor = true;
    win_config.backgroundColor = glm::vec3(0.0f, 0.0f, 1.0f);

    window.create(win_config);
    
    Lazarus::Shader shader_manager = Lazarus::Shader();
    uint32_t shader_program = 0;
    
    shader_manager.compileShaders(shader_program);
    shader_manager.setActiveShader(shader_program);

    Lazarus::Transform transformer = Lazarus::Transform();

    Lazarus::CameraManager *camera_manager = new Lazarus::CameraManager(shader_manager);
    Lazarus::CameraManager::Camera fly_cam = {};
    Lazarus::CameraManager::CameraConfig camera_config = {};
    camera_config.type = Lazarus::CameraManager::CameraType::PERSPECTIVE_FLYING;
    camera_config.clippingDistance = 10000.0f;
    camera_manager->createCamera(fly_cam, camera_config);

    Lazarus::LightManager *light_manager = new LightManager(shader_manager);
    Lazarus::LightManager::Light directional_light = {};
    Lazarus::LightManager::LightConfig light_config = {};
    light_config.type = Lazarus::LightManager::LightType::DIRECTIONAL;
    light_config.direction = glm::vec3(-1.0f, 0.5f, 0.0f);
    light_manager->createLightSource(directional_light, light_config);

    Lazarus::ModelManager *model_manager = new Lazarus::ModelManager(shader_manager);
    Lazarus::ModelManager::Model sponza = {};
    Lazarus::ModelManager::AssetConfig sponza_config = {};
    sponza_config.meshPath = "sponza.glb";
    model_manager->create3DAsset(sponza, sponza_config);
    
    window.eventsInit();
    window.open();

    while(window.isOpen)
    {
        /* Poll events */
        window.monitorEvents();
        window.monitorFPS();
        std::cout << "FPS: " << window.framesPerSecond << std::endl;

        /* Render scene */
        camera_manager->loadCamera(fly_cam);
        light_manager->loadLightSource(directional_light);

        model_manager->loadModel(sponza);
        model_manager->drawModel(sponza);

        /* Simple flycamera implementation */
        for(size_t i = 0; i < window.eventQueue.size(); i++)
        {
            Lazarus::WindowManager::Event event = window.eventQueue[i];
            int32_t code = event.type != Lazarus::WindowManager::EventType::KEY_UP ? event.key : 0;

            switch(code)
            {
                //  Arrow up / move forward
                case 265:
                    moveZ = 0.2f;
                    break;
                //  Arrow down / move back
                case 264:
                    moveZ = -0.2f;
                    break;
                //  Arrow left / move left
                case 263:
                    moveX = -0.2f;
                    break;
                //  Arrow right / move right
                case 262:
                    moveX = 0.2f;
                    break;
                //  W / Look up
                case 87:
                    turnX = -1.0f;
                    break;
                //  A / Look left
                case 65:
                    turnY = -2.0f;
                    break;
                //  S / Look down
                case 83:
                    turnX = 2.0f;
                    break;
                //  D / Look right
                case 68:
                    turnY = 2.0f;
                    break;
                //  Reset
                default:
                    moveX = 0.0f;
	        		moveZ = 0.0f;
                    turnX = 0.0f;
                    turnY = 0.0f;
                    break;
            }

        };

        transformer.rotateCamera(fly_cam, turnX, turnY, 0.0f);
        transformer.translateCamera(fly_cam, moveX, 0.0f, moveZ);

        window.presentNextFrame();
    };

    return 1;
}

#endif