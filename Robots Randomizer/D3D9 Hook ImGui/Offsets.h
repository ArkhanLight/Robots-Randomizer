#pragma once

// Base address for "Robots.exe"
constexpr DWORD_PTR BASE_ADDRESS = 0x003BA7C0; // Base address for player-related data

// Array for health-related offsets (these are in the correct order)
constexpr DWORD_PTR HEALTH_OFFSETS[] = { 0x5C, 0x2F8 };

// Array for "Velocity Z" related offsets
constexpr DWORD_PTR VELOCITY_Z_OFFSETS[] = { 0x118, 0x40, 0x124 };

// Function to read memory from a specific address
template <typename T>
T ReadMemory(HANDLE hProcess, DWORD_PTR address)
{
    T buffer;
    ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
    return buffer;
}