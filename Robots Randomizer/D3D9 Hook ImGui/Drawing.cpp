#include "Drawing.h"
#include "Hook.h"
#include "ImGui/imgui_internal.h"
#include "MemoryAddresses.h"
#include <thread>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <map>

BOOL Drawing::bInit = FALSE; // Status of the initialization of ImGui.
bool Drawing::bDisplay = true; // Status of the menu display.
bool Drawing::bSetPos = false; // Status to update ImGui window size / position.
ImVec2 Drawing::vWindowPos = { 0, 0 }; // Last ImGui window position.
ImVec2 Drawing::vWindowSize = { 0, 0 }; // Last ImGui window size.

int timer = 10; // Countdown timer
auto lastTime = std::chrono::steady_clock::now(); // Track the last time the timer was updated
ImFont* arialFont = nullptr; // Pointer to store the custom Arial font

// Effect structure to store ID, name, remaining time, and other information
struct Effect {
    int id;                 // Unique ID for the effect
    std::string name;        // Name to display
    float remainingTime;     // Duration in seconds (use float for smooth transition)
    int totalTime;           // The total duration of the effect
    float chance;            // Chance to trigger the effect (in percentage)
    bool applied = false;    // Flag to track if the effect has been applied
};

// List of currently active effects
std::vector<Effect> activeEffects;

// List of possible effects with their IDs and fixed durations
std::vector<Effect> effectList = {
    {1, "Gravity Fucked", 5, 5, 1.0f},
    {2, "Health Fucked", 12, 12, 1.0f},
    {3, "Speedrun Mode", 18, 18, 1.0f},
    {4, "Rodney BigBotton", 14, 14, 20.0f},
    {5, "Colorful", 20, 20, 90.0f}
};

// Helper function to handle Gravity Fucked effect
void ApplyGravityFucked()
{
    HANDLE hProcess = GetGameProcessHandle();
    if (hProcess)
    {
        DWORD_PTR baseAddress = (DWORD_PTR)GetModuleHandle("Robots.exe") + PLAYER_BASE_ADDRESS;
        DWORD_PTR velocityZAddress = ResolvePointer(hProcess, baseAddress, VELOCITY_Z_OFFSETS, sizeof(VELOCITY_Z_OFFSETS) / sizeof(DWORD_PTR));
        if (velocityZAddress)
        {
            DWORD oldProtect;
            if (VirtualProtectEx(hProcess, (LPVOID)velocityZAddress, sizeof(float), PAGE_READWRITE, &oldProtect))
            {
                WriteMemory<float>(hProcess, velocityZAddress, 10.0f); // Set velocity
                VirtualProtectEx(hProcess, (LPVOID)velocityZAddress, sizeof(float), oldProtect, &oldProtect);
            }
        }
        CloseHandle(hProcess);
    }
}

void RevertGravityFucked()
{
    HANDLE hProcess = GetGameProcessHandle();
    if (hProcess)
    {
        DWORD_PTR baseAddress = (DWORD_PTR)GetModuleHandle("Robots.exe") + PLAYER_BASE_ADDRESS;
        DWORD_PTR velocityZAddress = ResolvePointer(hProcess, baseAddress, VELOCITY_Z_OFFSETS, sizeof(VELOCITY_Z_OFFSETS) / sizeof(DWORD_PTR));
        if (velocityZAddress)
        {
            DWORD oldProtect;
            if (VirtualProtectEx(hProcess, (LPVOID)velocityZAddress, sizeof(float), PAGE_READWRITE, &oldProtect))
            {
                WriteMemory<float>(hProcess, velocityZAddress, -50.0f); // Reset velocity to -50.0f
                VirtualProtectEx(hProcess, (LPVOID)velocityZAddress, sizeof(float), oldProtect, &oldProtect);
            }
        }
        CloseHandle(hProcess);
    }
}


// Helper function to handle Health Fucked effect
void ApplyHealthFucked()
{
    HANDLE hProcess = GetGameProcessHandle();
    if (hProcess)
    {
        DWORD_PTR baseAddress = (DWORD_PTR)GetModuleHandle("Robots.exe") + PLAYER_BASE_ADDRESS;
        DWORD_PTR healthAddress = ResolvePointer(hProcess, baseAddress, HEALTH_OFFSETS, sizeof(HEALTH_OFFSETS) / sizeof(DWORD_PTR));
        if (healthAddress)
        {
            const float healthValues[] = { 0.0f, 20.0f, 40.0f, 60.0f, 80.0f, 100.0f };
            float currentHealth = ReadMemory<float>(hProcess, healthAddress);
            int randomIndex = rand() % (sizeof(healthValues) / sizeof(healthValues[0]));
            float newHealth = healthValues[randomIndex];
            DWORD oldProtect;
            if (VirtualProtectEx(hProcess, (LPVOID)healthAddress, sizeof(float), PAGE_READWRITE, &oldProtect))
            {
                WriteMemory<float>(hProcess, healthAddress, newHealth); // Set random health
                VirtualProtectEx(hProcess, (LPVOID)healthAddress, sizeof(float), oldProtect, &oldProtect);
            }
        }
        CloseHandle(hProcess);
    }
}

void RevertHealthFucked()
{
    HANDLE hProcess = GetGameProcessHandle();
    if (hProcess)
    {
        DWORD_PTR baseAddress = (DWORD_PTR)GetModuleHandle("Robots.exe") + PLAYER_BASE_ADDRESS;
        DWORD_PTR healthAddress = ResolvePointer(hProcess, baseAddress, HEALTH_OFFSETS, sizeof(HEALTH_OFFSETS) / sizeof(DWORD_PTR));
        if (healthAddress)
        {
            DWORD oldProtect;
            if (VirtualProtectEx(hProcess, (LPVOID)healthAddress, sizeof(float), PAGE_READWRITE, &oldProtect))
            {
                WriteMemory<float>(hProcess, healthAddress, 100.0f); // Reset health to 100
                VirtualProtectEx(hProcess, (LPVOID)healthAddress, sizeof(float), oldProtect, &oldProtect);
            }
        }
        CloseHandle(hProcess);
    }
}


// Helper function to handle Speedrun Mode effect
void ApplySpeedrunMode()
{
    HANDLE hProcess = GetGameProcessHandle();
    if (hProcess)
    {
        DWORD oldProtect;
        if (VirtualProtectEx(hProcess, (LPVOID)GAME_SPEED_ADDRESS, sizeof(BYTE), PAGE_READWRITE, &oldProtect))
        {
            WriteMemory<BYTE>(hProcess, GAME_SPEED_ADDRESS, 64); // Set speed to 64
            VirtualProtectEx(hProcess, (LPVOID)GAME_SPEED_ADDRESS, sizeof(BYTE), oldProtect, &oldProtect);
        }
        CloseHandle(hProcess);
    }
}

void RevertSpeedrunMode()
{
    HANDLE hProcess = GetGameProcessHandle();
    if (hProcess)
    {
        DWORD oldProtect;
        if (VirtualProtectEx(hProcess, (LPVOID)GAME_SPEED_ADDRESS, sizeof(BYTE), PAGE_READWRITE, &oldProtect))
        {
            WriteMemory<BYTE>(hProcess, GAME_SPEED_ADDRESS, 63); // Reset speed to 63
            VirtualProtectEx(hProcess, (LPVOID)GAME_SPEED_ADDRESS, sizeof(BYTE), oldProtect, &oldProtect);
        }
        CloseHandle(hProcess);
    }
}


// Helper function to handle Rodney BigBotton effect
void ApplyRodneyBigBotton()
{
    HANDLE hProcess = GetGameProcessHandle();
    if (hProcess)
    {
        DWORD_PTR baseAddress = (DWORD_PTR)GetModuleHandle("Robots.exe") + PLAYER_BASE_ADDRESS;
        DWORD_PTR scaleXAddress = ResolvePointer(hProcess, baseAddress, SCALE_X_OFFSETS, sizeof(SCALE_X_OFFSETS) / sizeof(DWORD_PTR));
        DWORD_PTR scaleYAddress = ResolvePointer(hProcess, baseAddress, SCALE_Y_OFFSETS, sizeof(SCALE_Y_OFFSETS) / sizeof(DWORD_PTR));
        DWORD_PTR scaleZAddress = ResolvePointer(hProcess, baseAddress, SCALE_Z_OFFSETS, sizeof(SCALE_Z_OFFSETS) / sizeof(DWORD_PTR));

        if (scaleXAddress && scaleYAddress && scaleZAddress)
        {
            DWORD oldProtect;
            if (VirtualProtectEx(hProcess, (LPVOID)scaleXAddress, sizeof(float), PAGE_READWRITE, &oldProtect))
            {
                WriteMemory<float>(hProcess, scaleXAddress, 3.0f);
                WriteMemory<float>(hProcess, scaleYAddress, 3.0f);
                WriteMemory<float>(hProcess, scaleZAddress, 3.0f);
                VirtualProtectEx(hProcess, (LPVOID)scaleXAddress, sizeof(float), oldProtect, &oldProtect);
            }
        }

        CloseHandle(hProcess);
    }
}

void RevertRodneyBigBotton()
{
    HANDLE hProcess = GetGameProcessHandle();
    if (hProcess)
    {
        DWORD_PTR baseAddress = (DWORD_PTR)GetModuleHandle("Robots.exe") + PLAYER_BASE_ADDRESS;
        DWORD_PTR scaleXAddress = ResolvePointer(hProcess, baseAddress, SCALE_X_OFFSETS, sizeof(SCALE_X_OFFSETS) / sizeof(DWORD_PTR));
        DWORD_PTR scaleYAddress = ResolvePointer(hProcess, baseAddress, SCALE_Y_OFFSETS, sizeof(SCALE_Y_OFFSETS) / sizeof(DWORD_PTR));
        DWORD_PTR scaleZAddress = ResolvePointer(hProcess, baseAddress, SCALE_Z_OFFSETS, sizeof(SCALE_Z_OFFSETS) / sizeof(DWORD_PTR));

        if (scaleXAddress && scaleYAddress && scaleZAddress)
        {
            DWORD oldProtect;
            if (VirtualProtectEx(hProcess, (LPVOID)scaleXAddress, sizeof(float), PAGE_READWRITE, &oldProtect))
            {
                WriteMemory<float>(hProcess, scaleXAddress, 1.0f); // Reset scale to 1.0
                WriteMemory<float>(hProcess, scaleYAddress, 1.0f);
                WriteMemory<float>(hProcess, scaleZAddress, 1.0f);
                VirtualProtectEx(hProcess, (LPVOID)scaleXAddress, sizeof(float), oldProtect, &oldProtect);
            }
        }

        CloseHandle(hProcess);
    }
}


// Map to store timers for each entity (keyed by entity address)
std::map<DWORD_PTR, float> entityTimers;
// Function to generate a random time between 0.2 and 1.0 seconds
float GetRandomTime()
{
    return 0.2f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1.0f - 0.2f)));
}
// Function to generate a random color between 0.0 and 1.0
float GetRandomColor()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}
// Function to apply a random color change to each entity individually
void ApplyColorfulEffect(float elapsedTime)
{
    HANDLE hProcess = GetGameProcessHandle();
    if (hProcess)
    {
        // Get the base address of the entity list
        DWORD_PTR entityListAddress = ReadMemory<DWORD_PTR>(hProcess, ENTITY_LIST_BASE_ADDRESS);

        // Loop through all entities and apply individual random colors with random timers
        while (entityListAddress != 0)
        {
            // Check if this entity already has a timer; if not, initialize it
            if (entityTimers.find(entityListAddress) == entityTimers.end())
            {
                entityTimers[entityListAddress] = GetRandomTime();
            }

            // Decrease the timer for this entity
            entityTimers[entityListAddress] -= elapsedTime;

            // If the timer reaches 0, assign a new random color and reset the timer
            if (entityTimers[entityListAddress] <= 0.0f)
            {
                float randomR = GetRandomColor();
                float randomG = GetRandomColor();
                float randomB = GetRandomColor();

                DWORD_PTR colorRAddress = entityListAddress + COLOR_R_OFFSET;
                DWORD_PTR colorGAddress = entityListAddress + COLOR_G_OFFSET;
                DWORD_PTR colorBAddress = entityListAddress + COLOR_B_OFFSET;

                DWORD oldProtect;
                if (VirtualProtectEx(hProcess, (LPVOID)colorRAddress, sizeof(float), PAGE_READWRITE, &oldProtect))
                {
                    WriteMemory<float>(hProcess, colorRAddress, randomR);
                    WriteMemory<float>(hProcess, colorGAddress, randomG);
                    WriteMemory<float>(hProcess, colorBAddress, randomB);
                    VirtualProtectEx(hProcess, (LPVOID)colorRAddress, sizeof(float), oldProtect, &oldProtect);
                }

                // Reset the timer for this entity
                entityTimers[entityListAddress] = GetRandomTime();
            }

            // Move to the next entity (assuming +0x04 is the next entity)
            entityListAddress = ReadMemory<DWORD_PTR>(hProcess, entityListAddress + 0x04);
        }

        CloseHandle(hProcess);
    }
}

// Revert RGB values back to default (1.0, 1.0, 1.0) when the effect ends
void RevertColorfulEffect()
{
    HANDLE hProcess = GetGameProcessHandle();
    if (hProcess)
    {
        // Get the base address of the entity list
        DWORD_PTR entityListAddress = ReadMemory<DWORD_PTR>(hProcess, ENTITY_LIST_BASE_ADDRESS);

        // Loop through all entities and reset the RGB values to default (1.0, 1.0, 1.0)
        while (entityListAddress != 0)
        {
            DWORD_PTR colorRAddress = entityListAddress + COLOR_R_OFFSET;
            DWORD_PTR colorGAddress = entityListAddress + COLOR_G_OFFSET;
            DWORD_PTR colorBAddress = entityListAddress + COLOR_B_OFFSET;

            // Set RGB values back to 1.0 (default color)
            DWORD oldProtect;
            if (VirtualProtectEx(hProcess, (LPVOID)colorRAddress, sizeof(float), PAGE_READWRITE, &oldProtect))
            {
                WriteMemory<float>(hProcess, colorRAddress, 1.0f);  // Default to 1.0
                WriteMemory<float>(hProcess, colorGAddress, 1.0f);
                WriteMemory<float>(hProcess, colorBAddress, 1.0f);
                VirtualProtectEx(hProcess, (LPVOID)colorRAddress, sizeof(float), oldProtect, &oldProtect);
            }

            // Move to the next entity
            entityListAddress = ReadMemory<DWORD_PTR>(hProcess, entityListAddress + 0x04);
        }

        CloseHandle(hProcess);
    }
}






// Function to randomly select an effect based on the chance
Effect SelectRandomEffectBasedOnChance()
{
    float totalChance = 0.0f;

    // Calculate the total chance sum
    for (const auto& effect : effectList)
    {
        bool alreadyActive = false;
        for (const auto& activeEffect : activeEffects)
        {
            if (activeEffect.id == effect.id) // Check by ID
            {
                alreadyActive = true;
                break;
            }
        }

        // If the effect is not active, add its chance to the total
        if (!alreadyActive)
        {
            totalChance += effect.chance;
        }
    }

    // Roll a random number between 0 and the total chance
    float randomValue = static_cast<float>(rand()) / RAND_MAX * totalChance;

    float cumulativeChance = 0.0f;

    // Select the effect based on the random roll
    for (const auto& effect : effectList)
    {
        bool alreadyActive = false;
        for (const auto& activeEffect : activeEffects)
        {
            if (activeEffect.id == effect.id) // Check by ID
            {
                alreadyActive = true;
                break;
            }
        }

        if (!alreadyActive)
        {
            cumulativeChance += effect.chance;
            if (randomValue <= cumulativeChance)
            {
                return effect;
            }
        }
    }

    // Fallback in case something goes wrong (shouldn't happen)
    return effectList[0];
}

void DrawRadialProgressBar(ImDrawList* drawList, ImVec2 center, float radius, float thickness, float progress, ImU32 color)
{
    const int numSegments = 10;
    const float startAngle = -IM_PI / 2; // Start at the top
    const float endAngle = startAngle + progress * 2 * IM_PI; // End at progress angle

    drawList->PathArcTo(center, radius, startAngle, endAngle, numSegments);
    drawList->PathStroke(color, false, thickness);
}

HRESULT Drawing::hkEndScene(const LPDIRECT3DDEVICE9 D3D9Device)
{
    // Log that hkEndScene was called
    OutputDebugString("hkEndScene called.\n");

    // Check if the Direct3D device is lost
    HRESULT deviceState = D3D9Device->TestCooperativeLevel();
    if (deviceState != D3D_OK)
    {
        if (deviceState == D3DERR_DEVICELOST)
        {
            OutputDebugString("Device is lost (D3DERR_DEVICELOST), skipping rendering.\n");
            // Device is lost, don't render and return early
            return Hook::oEndScene(D3D9Device);
        }
        else if (deviceState == D3DERR_DEVICENOTRESET)
        {
            OutputDebugString("Device is ready to be reset (D3DERR_DEVICENOTRESET).\n");

            // Device is ready to be reset
            D3DPRESENT_PARAMETERS d3dpp = {};  // You may need to fill in the appropriate presentation parameters
            OutputDebugString("Attempting to reset the device...\n");
            HRESULT resetResult = D3D9Device->Reset(&d3dpp);
            if (resetResult == D3D_OK)
            {
                OutputDebugString("Device reset successful. Reinitializing ImGui device objects.\n");

                // After resetting the device, recreate ImGui device objects
                ImGui_ImplDX9_CreateDeviceObjects();
                bInit = TRUE; // Mark ImGui as initialized after the reset
            }
            else
            {
                char msg[256];
                sprintf_s(msg, "Device reset failed with HRESULT: 0x%08X\n", resetResult);
                OutputDebugString(msg);
                // Reset failed, skip rendering
                return Hook::oEndScene(D3D9Device);
            }
        }
    }

    if (!Hook::pDevice)
        Hook::pDevice = D3D9Device;

    // If we haven't initialized ImGui yet, initialize it now
    if (!bInit)
    {
        OutputDebugString("Reinitializing ImGui after device reset or resolution change.\n");
        InitImGui(D3D9Device);
        OutputDebugString("ImGui reinitialized successfully.\n");
        bInit = TRUE;
    }

    if (!bDisplay)
    {
        OutputDebugString("bDisplay is false, skipping rendering.\n");
        return Hook::oEndScene(D3D9Device);
    }

    // Log that we are starting the ImGui rendering
    OutputDebugString("Rendering ImGui.\n");

    // Update the main timer
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - lastTime;
    lastTime = currentTime; // Update the lastTime to currentTime

    static float accumulatedTime = 0.0f;
    accumulatedTime += elapsedTime.count();

    if (accumulatedTime >= 1.0f) // Update every second
    {
        timer--;
        if (timer < 1)
        {
            timer = 10; // Reset the main timer when it reaches 0

            // Check if all effects are already active
            if (activeEffects.size() < effectList.size())
            {
                // Select a random effect based on chance that is not already active
                Effect newEffect = SelectRandomEffectBasedOnChance();
                activeEffects.push_back(newEffect);
            }
        }

        accumulatedTime -= 1.0f;
    }

    // Handle all effects logic (active)
    for (auto& effect : activeEffects)
    {
        switch (effect.id)
        {
        case 1:
            ApplyGravityFucked();
            break;
        case 2:
            ApplyHealthFucked();
            break;
        case 3:
            ApplySpeedrunMode();
            break;
        case 4:
            ApplyRodneyBigBotton();
            break;
        case 5:
            ApplyColorfulEffect(elapsedTime.count());
            break;
        }
    }

    // Smoothly update each active effect's remaining time
    // Handle effect expiration and reset logic
    for (auto it = activeEffects.begin(); it != activeEffects.end(); )
    {
        it->remainingTime -= elapsedTime.count();
        if (it->remainingTime <= 0)
        {
            switch (it->id)
            {
            case 1:
                RevertGravityFucked();
                break;
            case 2:
                RevertHealthFucked();
                break;
            case 3:
                RevertSpeedrunMode();
                break;
            case 4:
                RevertRodneyBigBotton();
                break;
            case 5:
                RevertColorfulEffect();
                break;
            }

            it = activeEffects.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Rest of the hkEndScene rendering logic goes here
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Set the custom Arial font for the timer text and effect display
    if (arialFont)
        ImGui::PushFont(arialFont);

    // Set the background transparency and color for the timer window
    ImGui::SetNextWindowBgAlpha(0.7f); // Set transparency (0.7f is 70% opaque)
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.7f)); // Set background color to a semi-transparent dark gray

    // Display the main timer at the top of the screen
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 25), ImGuiCond_Always, ImVec2(0.5f, 0.0f)); // Centered at the top
    ImGui::Begin("Timer", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::Text("Timer: %d", timer);
    ImGui::End();
    ImGui::PopStyleColor(); // Restore original window background color

    // Display the list of active effects on the left side of the screen
    ImGui::SetNextWindowPos(ImVec2(20, 120), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(260, 120));
    ImGui::Begin("Active Effects", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    for (const auto& effect : activeEffects)
    {
        ImGui::Text("%s", effect.name.c_str());

        // Calculate the progress for the radial timer
        float progress = effect.remainingTime / effect.totalTime;

        // Position for the radial timer
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 center = ImVec2(cursorPos.x + 220, cursorPos.y - 15); // Adjusted position to align the radial timer correctly
        float radius = 10.0f;
        float thickness = 4.0f;

        // Draw the radial timer
        DrawRadialProgressBar(drawList, center, radius, thickness, progress, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));

        // Add spacing after each effect
        ImGui::Dummy(ImVec2(0.0f, 25.0f)); // Adds space between list items
    }

    ImGui::End();
    ImGui::PopStyleColor(); // Restore original window background color

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    OutputDebugString("Finished rendering ImGui.\n");

    // Return the original EndScene function
    return Hook::oEndScene(D3D9Device);
}

/**
    @brief : function that init ImGui for rendering.
    @param pDevice : Current Direct3D9 Device Object given by the hooked function.
**/
void Drawing::InitImGui(const LPDIRECT3DDEVICE9 pDevice)
{
    D3DDEVICE_CREATION_PARAMETERS CP;
    pDevice->GetCreationParameters(&CP);
    Hook::window = CP.hFocusWindow;
    Hook::HookWindow();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    io.Fonts->AddFontDefault();

    // Load the Arial font with a larger size (e.g., 24.0f)
    arialFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\FRADM.TTF", 19.0f);

    // Initialize random seed
    srand(static_cast<unsigned int>(time(0)));

    ImGui::StyleColorsDark();

    // Set global style for rounded corners
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;     // Window corners rounding
    style.ChildRounding = 10.0f;      // Child windows rounding
    style.FrameRounding = 10.0f;      // Frames, buttons rounding
    style.PopupRounding = 10.0f;      // Popup rounding
    style.GrabRounding = 10.0f;       // Slider/scrollbar/grab bar rounding
    style.ScrollbarRounding = 10.0f;  // Scrollbar rounding

    // Hide the mouse cursor
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.MouseDrawCursor = false; // This hides the mouse cursor in ImGui

    // Initialize ImGui for the specific platforms
    ImGui_ImplWin32_Init(Hook::window);
    ImGui_ImplDX9_Init(pDevice);

    bInit = TRUE;
}
