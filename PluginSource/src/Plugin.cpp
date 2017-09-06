#include <d3d11.h>

#include "Plugin.h"
#include "nvapi/nvapi.h"

// --------------------------------------------------------------------------
IUnityGraphics* unityGraphics = nullptr;
IUnityGraphicsD3D11* d3d11 = nullptr;

bool isActive = false;
int uavSlots = 0;

// --------------------------------------------------------------------------
// UnityPluginLoad
extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API 
                                UnityPluginLoad(IUnityInterfaces* interfaces)
{
    unityGraphics = interfaces->Get<IUnityGraphics>();
    d3d11 = interfaces->Get<IUnityGraphicsD3D11>();
    unityGraphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

    // Run OnGraphicsDeviceEvent(initialize) manually on plugin load
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

// --------------------------------------------------------------------------
// UnityPluginUnload
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
    unityGraphics = nullptr;
    d3d11 = nullptr;
    isActive = false;
    uavSlots = 0;
    unityGraphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

// --------------------------------------------------------------------------
// GraphicsDeviceEvent
void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    // Create graphics API device upon initialization
    auto device = d3d11->GetDevice();
    if (device == nullptr)
        return;

    if (eventType == kUnityGfxDeviceEventInitialize) {
        // Use the last UAV slot, based on feature level info:
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ff476876.aspx
        uavSlots = 0;
        auto featureLevel = device->GetFeatureLevel();
        if (featureLevel >= D3D_FEATURE_LEVEL_11_1) {
            uavSlots = 64;
        } else if (featureLevel >= D3D_FEATURE_LEVEL_11_0) {
            uavSlots = 8;
        } else {
            // Although feature level 10_1 has uav slots available,
            // every Nvidia device that supports shader extensions
            // has a feature level of at least 11_0.
            return;
        }
        // Do this once during app initialization.
        NvAPI_Initialize();

        // First, enable compilation of intrinsics. 
        // The second parameter is the UAV slot index that is used in the shader: u#.
        auto NvapiStatus = NvAPI_D3D11_SetNvShaderExtnSlot(device, uavSlots - 1);
        if (NvapiStatus != NVAPI_OK) {
            return;
        }
        isActive = true;
    }

    else if (eventType == kUnityGfxDeviceEventShutdown) {
        // And disable again by telling the driver to use an invalid UAV slot.
        NvAPI_D3D11_SetNvShaderExtnSlot(device, ~0u);
    }
}

// Checks whether the shaders of the current Unity game
// were compiled with Nvidia HLSL Shader Extensions enabled or not.
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API IsActive()
{
    return isActive;
}

// UAV slot index used by Nvidia's driver.
extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetUavSlotIndex()
{
    return uavSlots - 1;
}

// Tests if a Shader Extension opcode is supported on current hardware.
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API 
                                        IsOpCodeSupported(unsigned int opcode)
{
    auto device = d3d11->GetDevice();
    bool supported = false;
    NvAPI_D3D11_IsNvShaderExtnOpCodeSupported(device, opcode, &supported);
    return supported;
}