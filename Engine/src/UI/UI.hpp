#pragma once

#include "Rendering/Texture.hpp"
#include "UIFonts.hpp"
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <imgui/imgui_internal.h>

namespace Pit::UI {
#define IMGUI_LEFT_LABEL_ENABLE true
#if IMGUI_LEFT_LABEL_ENABLE
#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##", __VA_ARGS__))
#else
#define IMGUI_LEFT_LABEL(func, label, ...)
#endif

	inline static void Vec2(const String& label, glm::vec2& values, float resetValue = 0, float columnWidth = 100) {
		ImFont* extraBoldFont = Fonts::Get(Fonts::ExtraBold);

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .8f, .1f, .15f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ .9f, .2f, .2f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ .8f, .1f, .15f, 1 });
		ImGui::PushFont(extraBoldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, .1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .2f, .7f, .2f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ .3f, .8f, .3f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ .2f, .7f, .2f, 1 });
		ImGui::PushFont(extraBoldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, .1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	inline static void Vec3(const String& label, glm::vec3& values, float resetValue = 0, float columnWidth = 100) {
		ImFont* extraBoldFont = Fonts::Get(Fonts::ExtraBold);

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .8f, .1f, .15f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ .9f, .2f, .2f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ .8f, .1f, .15f, 1 });
		ImGui::PushFont(extraBoldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, .1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .2f, .7f, .2f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ .3f, .8f, .3f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ .2f, .7f, .2f, 1 });
		ImGui::PushFont(extraBoldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, .1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .1f, .25f, .8f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ .2f, .35f, .9f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ .1f, .25f, .8f, 1 });
		ImGui::PushFont(extraBoldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, .1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	inline static void Vec4(const String& label, glm::vec4& values, float resetValue = 0, float columnWidth = 100) {
		ImFont* extraBoldFont = Fonts::Get(Fonts::ExtraBold);

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2;
		ImVec2 buttonSize = { lineHeight + 3, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .8f, .1f, .15f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ .9f, .2f, .2f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ .8f, .1f, .15f, 1 });
		ImGui::PushFont(extraBoldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, .1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .2f, .7f, .2f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ .3f, .8f, .3f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ .2f, .7f, .2f, 1 });
		ImGui::PushFont(extraBoldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, .1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(extraBoldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, .1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ .1f, .25f, .8f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ .2f, .35f, .9f, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ .1f, .25f, .8f, 1 });
		ImGui::PushFont(extraBoldFont);
		if (ImGui::Button("W", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##W", &values.w, .1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	inline static bool DragFloat(const char* label, float* v, float v_speed = 1, float v_min = 0, float v_max = 0, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return IMGUI_LEFT_LABEL(ImGui::DragFloat, label, v, v_speed, v_min, v_max, format, flags);
	}

	inline static bool ImageButton(const Pit::Rendering::Texture* texture, const ImVec2& size, const bool flipX = false, const bool flipY = false, const int& framePadding = -1, const ImVec4& bgColor = { 0,0,0,0 }, const ImVec4& tintColor = { 1,1,1,1 }) {
		ImVec2 uv1;
		ImVec2 uv2;
		if (flipX && flipY) {
			uv1 = { 1, 1 };
			uv2 = { 0, 0 };
		}
		else if (flipX && !flipY) {
			uv1 = { 1, 0 };
			uv2 = { 0, 1 };
		}
		else if (!flipX && flipY) {
			uv1 = { 0, 1 };
			uv2 = { 1, 0 };
		}
		else if (!flipX && !flipY) {
			uv1 = { 0, 0 };
			uv2 = { 1, 1 };
		}
		return ImGui::ImageButton((ImTextureID)Cast<uint64>(texture->GetRendererID()), size, uv1, uv2, framePadding, bgColor, tintColor);
	}
}