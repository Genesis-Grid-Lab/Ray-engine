#include "Core/ImGuiLayer.h"
#include "Core/FA.h"

#include <imgui_impl_raylib.h>

#include "Core/Application.h"

// #include <ImGuizmo.h>

#define FONT_FILE "Data/Fonts/Roboto-Medium.ttf"
#define ICON_FONT "Data/Fonts/fa-solid-900.ttf"
#define REGULAR_FONT_SIZE 17
#define SMALL_FONT_SIZE 15

namespace RE {

    ImGuiContext *g_CTX;

    ImGuiLayer::ImGuiLayer()
        :Layer("ImGuiLayer"){}    

    void ImGuiLayer::OnAttach(){

        // Setup Dear ImGui context
        g_CTX = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplRaylib_Init();

        ImFontConfig fontConfig;
        fontConfig.MergeMode = true;
        fontConfig.PixelSnapH = true;
        static const ImWchar iconRange[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

        // regular font and icon
        io.Fonts->AddFontFromFileTTF(FONT_FILE, REGULAR_FONT_SIZE);
        io.Fonts->AddFontFromFileTTF(ICON_FONT, REGULAR_FONT_SIZE, &fontConfig, iconRange);

        // small font and icon
        io.Fonts->AddFontFromFileTTF(FONT_FILE, SMALL_FONT_SIZE);
        io.Fonts->AddFontFromFileTTF(ICON_FONT, SMALL_FONT_SIZE, &fontConfig, iconRange);
        ImGui_ImplRaylib_BuildFontAtlas();
        // if (!std::filesystem::exists("imgui.ini"))
		// {
		// 	ImGui::LoadIniSettingsFromDisk("Data/ImGui/default.ini");
		// }

        // if the linked ImGui has docking, enable it.
        // this will only be true if you use the docking branch of ImGui.
    #ifdef IMGUI_HAS_DOCK
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    #endif
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    }

    void ImGuiLayer::OnDetach(){

        ImGui_ImplRaylib_Shutdown();
        ImGui::DestroyContext();
    }

    ImGuiContext *ImGuiLayer::GetContext() { return g_CTX; }

    void ImGuiLayer::Begin(){
        ImGui_ImplRaylib_ProcessEvents();

        // Start the Dear ImGui frame
        ImGui_ImplRaylib_NewFrame();
        ImGui::NewFrame();
        // ImGuizmo::BeginFrame();
    }

    void ImGuiLayer::End(){   
        ImGuiIO& io = ImGui::GetIO(); 
        ImGui::Render();
        ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {            
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();            
        }
    }
}