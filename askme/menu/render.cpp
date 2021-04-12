#include "render.h"

char buffer[256];
char text[256] = "veffy#4190\nPress INS to show/hide menu";

bool bFirstTime = true;

namespace Render {
	void DrawInfo(IDirect3DDevice9* pDevice) {
		sprintf(buffer, "FPS at %d\n", Globals::iFPS);
		DrawStringOutline(text, 15, 10, 255, 255, 255, 255, pFontUISmall);
		DrawStringOutline(buffer, 15, 35, 197, 66, 244, 255, pFontUISmall);
	}

	void DrawVisuals(IDirect3DDevice9* pDevice) {
		//ESP Loop.. etc
	}

	void DrawMenu(IDirect3DDevice9* pDevice) {
		//ImGui Custom Colors
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.62f, 0.93f, 0.06f, 1.00f);

		//ImGui Frame
		ImGui_ImplDX9_NewFrame();
		static const auto flags =  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus;

		if (bFirstTime)
			ImGui::SetNextWindowSize(ImVec2(400, 300));

		ImGui::Begin("for contact veffy#4190", nullptr, flags);
		
		ImGui::Checkbox("Set Admin", &bTest);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 200);
		ImGui::Checkbox("Spiderman", &spiderman);
		ImGui::Checkbox("Set Gravity", &gravity);
		if (gravity == true) {
			ImGui::SliderFloat("SetGravity", &setGravity, 1.2f, 3.f);
		}
		ImGui::Checkbox("Fov Changer", &fovchanger);
		if (fovchanger == true) {
			ImGui::SliderFloat("Fov", &fov, 60.f, 130.f);
		}

		ImGui::End();

		ImGui::Render();

		bFirstTime = false;
	}
}