#include "repch.h"
#include "Core/Application.h"

namespace RE {

  Application* Application::s_Instance = nullptr;

  Application::Application(const std::string& name, const glm::vec2& size){
    s_Instance = this;
    m_Window = CreateScope<Window>(size.x, size.y, name.c_str());

#if RE_DEBUG
    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
#endif
  }

  Application::~Application(){

  }


  void Application::PushLayer(Layer* layer)
  {    
    // E_CORE_INFO("[APPLICATION] Pushing layer");
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
  }

  void Application::PushOverlay(Layer* layer)
  {
    // E_CORE_INFO("[APPLICATION] Pushing overlay");
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
  }

  void Application::PopLayer(Layer* layer) {
    // E_CORE_INFO("[APPLICATION] Pop layer");
    m_LayerStack.PopLayer(layer);
  }

  void Application::Close(){
    // E_CORE_INFO("[APPLICATION] Closing");
    m_Running = false;
  }

  void Application::Run(){
    while(m_Running && !WindowShouldClose()){
      float deltaTime = GetFrameTime();

      BeginDrawing();
      if(!m_Minimized){
	for(Layer* layer : m_LayerStack){
	  layer->OnUpdate(deltaTime);
	}
      }

#if RE_DEBUG    
                 
      m_ImGuiLayer->Begin();
      {
	for(Layer* layer : m_LayerStack)
	  layer->OnImGuiRender();
      }

      m_ImGuiLayer->End();
#endif
      EndDrawing();

      while (!m_LayerActionQueue.empty()) {
	LayerAction action = m_LayerActionQueue.front();
	m_LayerActionQueue.pop();
                
	if (action.Type == LayerActionType::Push) {
	  PushLayer(action.LayerPtr);
	} else if (action.Type == LayerActionType::Pop) {
	  PopLayer(action.LayerPtr);
	  delete action.LayerPtr;
	}
      }
    }
  }
}
