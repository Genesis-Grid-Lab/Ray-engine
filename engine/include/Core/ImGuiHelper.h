#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include "Config.h"
// ------------------------
// Small UI helpers
// ------------------------
static bool DrawVec3Control(const char* label, glm::vec3& values,
                            float resetValue = 0.0f, float columnWidth = 100.0f)
{
  bool changed = false;

  ImGui::PushID(label);
  ImGui::PushID(&values);                  // <— unique per field instance

  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, columnWidth);
  ImGui::TextUnformatted(label);
  ImGui::NextColumn();

  ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

  const float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
  const ImVec2 btn = { lineHeight + 3.0f, lineHeight };

  auto axis = [&](const char* axisText, float& v, const ImVec4& col, const char* dragID)
    {
      ImGui::PushStyleColor(ImGuiCol_Button, col);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ col.x + 0.1f, col.y + 0.1f, col.z + 0.1f, col.w });
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);
      if (ImGui::Button(axisText, btn)) { v = resetValue; changed = true; }
      ImGui::PopStyleColor(3);

      ImGui::SameLine();
      changed |= ImGui::DragFloat(dragID, &v, 0.1f);   // <— unique per axis
      ImGui::PopItemWidth();
      ImGui::SameLine();
    };

  axis("X", values.x, ImVec4{0.8f, 0.1f, 0.15f, 1.0f}, "##X");
  axis("Y", values.y, ImVec4{0.2f, 0.7f, 0.2f, 1.0f},  "##Y");
  axis("Z", values.z, ImVec4{0.1f, 0.25f, 0.8f, 1.0f}, "##Z");

  ImGui::PopStyleVar();
  ImGui::Columns(1);

  ImGui::PopID(); // &values
  ImGui::PopID(); // label
  return changed;
}

static bool DrawVec3Control(const char* label, Vector3& values,
                            float resetValue = 0.0f, float columnWidth = 100.0f)
{
  bool changed = false;

  ImGui::PushID(label);
  ImGui::PushID(&values);                  // <— unique per field instance

  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, columnWidth);
  ImGui::TextUnformatted(label);
  ImGui::NextColumn();

  ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

  const float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
  const ImVec2 btn = { lineHeight + 3.0f, lineHeight };

  auto axis = [&](const char* axisText, float& v, const ImVec4& col, const char* dragID)
    {
      ImGui::PushStyleColor(ImGuiCol_Button, col);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ col.x + 0.1f, col.y + 0.1f, col.z + 0.1f, col.w });
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);
      if (ImGui::Button(axisText, btn)) { v = resetValue; changed = true; }
      ImGui::PopStyleColor(3);

      ImGui::SameLine();
      changed |= ImGui::DragFloat(dragID, &v, 0.1f);   // <— unique per axis
      ImGui::PopItemWidth();
      ImGui::SameLine();
    };

  axis("X", values.x, ImVec4{0.8f, 0.1f, 0.15f, 1.0f}, "##X");
  axis("Y", values.y, ImVec4{0.2f, 0.7f, 0.2f, 1.0f},  "##Y");
  axis("Z", values.z, ImVec4{0.1f, 0.25f, 0.8f, 1.0f}, "##Z");

  ImGui::PopStyleVar();
  ImGui::Columns(1);

  ImGui::PopID(); // &values
  ImGui::PopID(); // label
  return changed;
}
