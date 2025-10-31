#pragma once

#include "Layer.h"

struct ImGuiContext;

namespace RE {

  class  ImGuiLayer : public Layer {
  public:
    ImGuiLayer();
    ~ImGuiLayer() = default;
    
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    
    void Begin();
    void End();
    ImGuiContext* GetContext();
    
    void BlockEvents(bool block) { m_BlockEvents = block;}
    
    // void SetDarkThemeColors();
  private:
    bool m_BlockEvents = true;
  };
}