#pragma once
#include <Windows.h>
#include <TlHelp32.h> // For process snapshot functions

// Base address for "Robots.exe"
constexpr DWORD_PTR PLAYER_BASE_ADDRESS = 0x003BA7C0; // Base address for player-related data

// Array for health-related offsets
constexpr DWORD_PTR HEALTH_OFFSETS[] = { 0x5C, 0x2F8 };

// Array for "Velocity Z" related offsets
constexpr DWORD_PTR VELOCITY_Z_OFFSETS[] = { 0x118, 0x40, 0x124 };

// Address where the `fstp` instruction that modifies Z velocity is located
constexpr DWORD_PTR JUMP_VELOCITY_INSTRUCTION_ADDRESS = 0x00446B1E;

// Static address for game speed (Speedrun Mode)
constexpr DWORD_PTR GAME_SPEED_ADDRESS = 0x007AD27B;

// Rodney BigBotton Effect (Scale Change) Offsets
constexpr DWORD_PTR SCALE_X_OFFSETS[] = { 0xF0 };
constexpr DWORD_PTR SCALE_Y_OFFSETS[] = { 0xF8 };
constexpr DWORD_PTR SCALE_Z_OFFSETS[] = { 0xF4 };

// Entity list base address
constexpr DWORD_PTR ENTITY_LIST_BASE_ADDRESS = 0x007BAE3C;

// Offsets for entity RGB
constexpr DWORD_PTR COLOR_R_OFFSET = 0x4C;
constexpr DWORD_PTR COLOR_G_OFFSET = 0x50;
constexpr DWORD_PTR COLOR_B_OFFSET = 0x54;

// Function to read memory from a specific address
template <typename T>
T ReadMemory(HANDLE hProcess, DWORD_PTR address)
{
    T buffer;
    ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
    return buffer;
}

// Function to write memory to a specific address
template <typename T>
BOOL WriteMemory(HANDLE hProcess, DWORD_PTR address, T value)
{
    return WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(T), NULL);
}

// Function to get the process ID of the game (Robots.exe)
DWORD GetProcessID(const wchar_t* processName)
{
    DWORD processID = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe))
        {
            do
            {
                wchar_t wExeFile[MAX_PATH];
                MultiByteToWideChar(CP_ACP, 0, pe.szExeFile, -1, wExeFile, MAX_PATH);
                if (!_wcsicmp(wExeFile, processName))
                {
                    processID = pe.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe));
        }
        CloseHandle(hSnapshot);
    }
    return processID;
}

// Function to get a handle to the game process
HANDLE GetGameProcessHandle()
{
    DWORD processID = GetProcessID(L"Robots.exe");
    if (processID == 0)
    {
        return NULL; // Could not find the process
    }

    // Open the process with read/write access
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
}

// General function to resolve a multi-level pointer
DWORD_PTR ResolvePointer(HANDLE hProcess, DWORD_PTR baseAddress, const DWORD_PTR* offsets, size_t offsetCount)
{
    // Start with the base address
    DWORD_PTR address = ReadMemory<DWORD_PTR>(hProcess, baseAddress);

    // Loop through all offsets except the last one
    for (size_t i = 0; i < offsetCount - 1; ++i)
    {
        address = ReadMemory<DWORD_PTR>(hProcess, address + offsets[i]);
        if (address == 0)
        {
            return 0; // Failed to resolve pointer
        }
    }

    // For the last offset, just add it to the current address
    address += offsets[offsetCount - 1];

    return address;
}
