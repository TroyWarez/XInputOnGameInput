// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
typedef DWORD(*XInputSetStateProc)
(
	DWORD         dwUserIndex,  // Index of the gamer associated with the device
    XINPUT_VIBRATION* pState        // Receives the current state
	);
typedef DWORD(*XInputGetCapabilitiesProc)
(
	DWORD                dwUserIndex,   // Index of the gamer associated with the device
	DWORD                dwFlags,       // Input flags that identify the device type
	XINPUT_CAPABILITIES* pCapabilities  // Receives the capabilities
	);
typedef DWORD(*XInputGetStateProc)
(
	DWORD         dwUserIndex,  // Index of the gamer associated with the device
	XINPUT_STATE* pState        // Receives the current state
	);
using namespace GameInput::v1;
IGameInput* gameInput = nullptr;
HRESULT hr = S_OK;
HMODULE dllXinputH = NULL;
XInputSetStateProc XSetState = NULL;
XInputGetStateProc XGetState = NULL;
XInputGetCapabilitiesProc XGetCapabilities = NULL;

//{
// 	// This checks for input from all gamepads simultaneously.
// 	IGameInputReading* reading = nullptr;
// 	if (!gameInput)
// 	{
// 		hr = GameInputCreate(&gameInput);
// 	}
// 	if (SUCCEEDED(hr))
// 	{
//         hr = gameInput->GetCurrentReading(GameInputKindGamepad, nullptr, &reading);
//             if (SUCCEEDED(hr))
//             {
//                 GameInputGamepadState state;
//                 XINPUT_STATE xstate;
//                 reading->GetGamepadState(&state);
// 
//                 if (state.buttons & GameInputGamepadA)
//                 {
//                     xstate.Gamepad.wButtons = XINPUT_GAMEPAD_A;
//                     pState = &xstate;
//                     // Found the user's gamepad.  At this point we can
//                     // get the device that generated this input, and then
//                     // pass that into future calls to the GetCurrentReading
//                     // method to receive input only from that gamepad.
//                 }
// 
//                 reading->Release();
//             }
// 	}
// 
//     return ERROR_SUCCESS;
// }

DWORD WINAPI XInputSetState
(
    _In_ DWORD             dwUserIndex,  // Index of the gamer associated with the device
    _In_ XINPUT_VIBRATION* pVibration    // The vibration information to send to the controller
) WIN_NOEXCEPT
{
    if (XSetState)
    {
        return XSetState(dwUserIndex, pVibration);
    }
    pVibration = NULL;
	return ERROR_NOT_CONNECTED;

}

DWORD WINAPI XInputGetState
(
    _In_  DWORD         dwUserIndex,  // Index of the gamer associated with the device
    _Out_ XINPUT_STATE* pState        // Receives the current state
) WIN_NOEXCEPT
{
    if (XGetState)
    {
		return XGetState(dwUserIndex, pState);
    }
    pState = NULL;
    return ERROR_NOT_CONNECTED;
}

DWORD WINAPI XInputGetCapabilities
(
    _In_  DWORD                dwUserIndex,   // Index of the gamer associated with the device
    _In_  DWORD                dwFlags,       // Input flags that identify the device type
    _Out_ XINPUT_CAPABILITIES* pCapabilities  // Receives the capabilities
) WIN_NOEXCEPT
{
	if (XGetCapabilities)
	{
       return XGetCapabilities(dwUserIndex, dwFlags, pCapabilities);
    }
    pCapabilities = NULL;
return ERROR_NOT_CONNECTED;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
		if (!dllXinputH)
		{
			WCHAR windowsPath[MAX_PATH] = { 0 };
			if (GetWindowsDirectoryW(windowsPath, MAX_PATH))
			{
				std::wstring WindowsPathStr(windowsPath);

				WindowsPathStr = WindowsPathStr + L"\\system32\\XInput9_1_0.dll";
				dllXinputH = LoadLibraryW(WindowsPathStr.c_str());
				if (dllXinputH)
				{
					XSetState = (XInputSetStateProc)GetProcAddress(dllXinputH, "XInputSetState");
					XGetState = (XInputGetStateProc)GetProcAddress(dllXinputH, "XInputGetState");
					XGetCapabilities = (XInputGetCapabilitiesProc)GetProcAddress(dllXinputH, "XInputGetCapabilities");
				}
			}

		}
        break;
    }
    case DLL_PROCESS_DETACH:
    {
        if (dllXinputH)
        {
            FreeLibrary(dllXinputH);
        }
		break;
    }
    }
    return TRUE;
}

