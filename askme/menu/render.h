#pragma once
#include "main.h"

inline bool bTest = false;
inline bool gravity = false;
inline bool spiderman = false;
inline bool fovchanger = false;
inline bool setautomatic = false;
inline float setGravity = 1.2f;
inline float fov = 60.f;

namespace Render {
	void DrawInfo(IDirect3DDevice9* pDevice);
	void DrawVisuals(IDirect3DDevice9* pDevice);
	void DrawMenu(IDirect3DDevice9* pDevice);
}