/**
 * <3 skit#2567
 *
 * don't make this into a shitty p2c
 */

#include "../imports.h"
#define STB_IMAGE_IMPLEMENTATION
#include <tchar.h>
#include "drawing.hpp"
#include <future>
#include "../visual/visual.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <io.h>
using namespace ImGui;
bool Drawing::CreateDeviceD3D()
{
    /* setup swap chain */
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = h_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}


auto Drawing::text(ImVec2 v2_pos, std::string str_text, ImVec4 v4_col, bool b_outline, float font_size) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const char* text_end = str_text.c_str() + strlen(str_text.c_str());
    const char* text_display_end = FindRenderedTextEnd(str_text.c_str(), text_end);

    if (font_size == 0.f) font_size = g.FontSize;

    if (b_outline) {
        window->DrawList->AddText(g.Font, font_size, v2_pos - ImVec2(1, 0), GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
        window->DrawList->AddText(g.Font, font_size, v2_pos - ImVec2(0, 1), GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
        window->DrawList->AddText(g.Font, font_size, v2_pos + ImVec2(1, 0), GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
        window->DrawList->AddText(g.Font, font_size, v2_pos + ImVec2(0, 1), GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
    }
    window->DrawList->AddText(g.Font, font_size, v2_pos, GetColorU32(v4_col), str_text.c_str(), NULL, 0.0f, NULL);
}

auto Drawing::ToImVec(float* color) -> ImVec4
{
    return ImVec4(color[0], color[1], color[2], color[3]);
}

auto Drawing::line(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, float f_thickness) -> void
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    window->DrawList->AddLine(v2_pos, v2_size, ImGui::GetColorU32(v4_col), f_thickness);
}

auto Drawing::line(RECT rect, ImVec4 v4_col, float f_thickness) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImRect line_bb(ImVec2(rect.left, rect.top), ImVec2(rect.right, rect.bottom));

    window->DrawList->AddLine(ImVec2(line_bb.Min.x, line_bb.Min.y), ImVec2(line_bb.Max.x, line_bb.Max.y), GetColorU32(v4_col), f_thickness);
}

auto Drawing::rect(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImRect rect_bb(v2_pos, v2_pos + v2_size);

    if (b_outline) {
        window->DrawList->AddRect(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
    }
    if (b_inline) {
        window->DrawList->AddRect(rect_bb.Min + ImVec2(1, 1), rect_bb.Max - ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
    }
    window->DrawList->AddRect(rect_bb.Min, rect_bb.Max, GetColorU32(v4_col), 0.0f, 15, f_thickness);
}

auto Drawing::rect(RECT rect, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImRect rect_bb(ImVec2(rect.left, rect.top), ImVec2(rect.right, rect.bottom));

    if (b_outline) {
        window->DrawList->AddRect(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
    }
    if (b_inline) {
        window->DrawList->AddRect(rect_bb.Min + ImVec2(1, 1), rect_bb.Max - ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
    }
    window->DrawList->AddRect(rect_bb.Min, rect_bb.Max, GetColorU32(v4_col), 0.0f, 15, f_thickness);
}

auto Drawing::filled_rect(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, bool b_outline) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImRect rect_bb(v2_pos, v2_pos + v2_size);

    if (b_outline) {
        window->DrawList->AddRectFilled(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f);
    }
    window->DrawList->AddRectFilled(rect_bb.Min, rect_bb.Max, GetColorU32(v4_col), 0.0f);
}

auto Drawing::filled_rect(RECT rect, ImVec4 v4_col, bool b_outline) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    const ImRect rect_bb(ImVec2(rect.left, rect.top), ImVec2(rect.right, rect.bottom));

    if (b_outline) {
        window->DrawList->AddRectFilled(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f);
    }
    window->DrawList->AddRectFilled(rect_bb.Min, rect_bb.Max, GetColorU32(v4_col), 0.0f);
}

auto Drawing::circle(ImVec2 v2_pos, float f_radius, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    if (b_outline) {
        window->DrawList->AddCircle(v2_pos, f_radius + 1.0f, GetColorU32(ImVec4(0, 0, 0, 1)), (int)f_radius, f_thickness);
    }
    if (b_inline) {
        window->DrawList->AddCircle(v2_pos, f_radius - 1.0f, GetColorU32(ImVec4(0, 0, 0, 1)), (int)f_radius, f_thickness);
    }
    window->DrawList->AddCircle(v2_pos, f_radius, GetColorU32(v4_col), 50, f_thickness);
}

auto Drawing::crosshair(ImVec4 v4_col, float f_thickness, float len, float xoffset, float yoffset) -> void
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    int CenterX = GetSystemMetrics(0) / 2 - 1;//Gets screen X resolution then cutting it in half to ##### the center.
    int CenterY = GetSystemMetrics(1) / 2 - 1;//Gets screen Y resolution then cutting it in half to ##### the center.

    rect(ImVec2(CenterX - (len * 2) + xoffset, CenterY + yoffset), ImVec2(len, 1), ImVec4(1.f, 1.f, 1.f, 1.f), 0.5f, true, true);//Left line
    rect(ImVec2(CenterX + len + xoffset, CenterY + yoffset), ImVec2(len, 1), ImVec4(1.f, 1.f, 1.f, 1.f), 0.5f, true, true);//Right line
    circle(ImVec2(CenterX, CenterY), 1, ImVec4(0.f, 0.f, 0.f, 1.f), 0.5, true, true);
}

Drawing* drawing = new Drawing();