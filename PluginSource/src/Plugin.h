#pragma once
#include "Unity/IUnityGraphics.h"
#include "Unity/IUnityGraphicsD3D11.h"

void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API IsActive();
extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetUavSlotIndex();
extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API IsOpCodeSupported(unsigned int opcode); 