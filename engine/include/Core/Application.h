#pragma once

#include "Config.h"
#include "Window.h"
#include "LayerStack.h"
#include "ImGuiLayer.h"

int main(int argc, char** argv);

namespace RE {

enum class LayerActionType {
    Push,
    Pop
};

struct LayerAction {
    LayerActionType Type;
    Layer* LayerPtr;
};

class Application {
public:
 Application(const std::string& name = "Application", const glm::vec2& size = glm::vec2(100));
 virtual ~Application();

 void PushLayer(Layer* layer);
 void PushOverlay(Layer* layer);
 void PopLayer(Layer* layer);
 ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer;}

 void Close();

 static Application& Get() { return *s_Instance; }

 void QueueLayerAction(LayerActionType type, Layer* layer) {
    m_LayerActionQueue.push({ type, layer });
 }

private:
 void Run();

private:
 Scope<Window> m_Window;
 ImGuiLayer* m_ImGuiLayer;
 bool m_Running = true;
 bool m_Minimized = false;
 LayerStack m_LayerStack;
 std::queue<LayerAction> m_LayerActionQueue;        
private:
    friend int ::main(int argc, char** argv);
    static Application *s_Instance;
};
}