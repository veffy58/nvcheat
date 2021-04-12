/**
 * <3 skit#2567
 *
 * don't make this into a shitty p2c
 */
#include "../imports.h"
#include "../menu/imgui.h"
#include "../menu/direct3d.h"
#include "../menu/imgui_internal.h"
#include "../menu/drawing.hpp"
#include "visual.hpp"
ImGuiContext& g = *GImGui;

int iterations = 1;

/* call this in the ImGui::Draw Loop <3 */
auto Visuals::DrawPlayers() -> void
{
	globals->players.mutex.lock();
	std::vector<Player> render_players = globals->players.vec;
	globals->players.mutex.unlock();


	/* Loop through the players list */
	for (auto& entry : render_players)
	{
		SleepEx(1, false);

		iterations++;

		/* Skip the LocalPlayer */
		if (entry.local_player) continue;

		/* Get the player object*/
		Player player = (Player)entry;

		if (player.health < 0.1) continue;
		if (!player.bn) continue;

		Vector3 l_origin = (globals->local_player.bones[l_foot] + globals->local_player.bones[r_foot]) / 2;
		Vector3 p_origin = (player.bones[l_foot] + player.bones[r_foot]) / 2;

		/*Calculate the distance*/
		int distance = sqrtf(pow((l_origin.x - p_origin.x), 2) + pow((l_origin.y - p_origin.y), 2) + pow((l_origin.z - p_origin.z), 2));

		/*Make sure they're less than 300m*/
		if (distance > 300)
			continue;

		/* w2s player pos */
		Vector2 screened_origin = utils->W2S(p_origin, &globals->matrix, globals->camera);
		Vector2 screened_top = utils->W2S(player.bones[head] + 0.2, &globals->matrix, globals->camera);

		if (screened_origin.x == 0 && screened_origin.y == 0 || screened_top.x == 0 && screened_top.y == 0)
			continue;

		/* set font scale */
		ImGui::SetWindowFontScale(settings->visuals.font_size / 6);

		/* Calculate how long the player name is and then subtract half the equilibirum */
		std::string player_name = player.GetASCIIName() + "[" + std::to_string(distance) + "m]";

		/* Get Player Height */
		int player_height = (screened_top.y - screened_origin.y);

		/* Get Player Width */
		int player_width = 2.0f * (player_height / 4.0f);

		/* calculate text size */
		ImVec2 text_size = ImGui::CalcTextSize(player_name.c_str(), NULL, false, -1.f);

		/* get left */
		Vector2 screened_left = screened_origin - Vector2(player_width / 2, 0);

		/* Draws Player Name */
		if (settings->visuals.players.name.enabled)
		{
			/* Subtract the equilibirum of pixel length */
			Vector2 mid_feet = screened_origin - Vector2(text_size.x / 2, 0);

			/* ensure it's in bounds. */
			if (screened_origin.x != 0 || screened_origin.y != 0)
			{
				/* Name ESP */
				drawing->text({ mid_feet.x, screened_left.y }, player_name, ImVec4(settings->visuals.players.name.color[0], settings->visuals.players.name.color[1], settings->visuals.players.name.color[2], settings->visuals.font_fading ? (1 - (0.005 * player.Distance(&globals->local_player))) : settings->visuals.players.name.color[3]), settings->visuals.font_outline);
			}
		}

		/* Check for box esp */
		if (settings->visuals.players.box.enabled)
		{
			/* ensure it's in bounds. */
			if (screened_origin.x != 0 || screened_origin.y != 0)
			{
				DrawCornerBox(screened_left.x, screened_left.y, player_width, player_height, ImVec4(settings->visuals.players.box.inline_color[0], settings->visuals.players.box.inline_color[1], settings->visuals.players.box.inline_color[2], settings->visuals.font_fading ? (1 - (0.005 * player.Distance(&globals->local_player))) : settings->visuals.players.box.inline_color[3]), ImVec4(settings->visuals.players.box.outline_color[0], settings->visuals.players.box.outline_color[1], settings->visuals.players.box.outline_color[2], settings->visuals.font_fading ? (1 - (0.005 * player.Distance(&globals->local_player))) : settings->visuals.players.box.outline_color[3]));
			}
		}

		/* Check for skeleton esp */
		if (settings->visuals.players.skeleton.enabled)
		{
			/* This map will store screen position for bones */
			std::map<Bones, Vector2> screen_bones;

			for (auto& bone : player.bones)
			{
				/* Get the Vector2 screen bones position */
				screen_bones[bone.first] = utils->W2S(bone.second, &globals->matrix, globals->camera);
			}

			/* draw skeleton */
			{
				/* hand to forearm */
				drawing->line(ImVec2(screen_bones[r_hand].x, screen_bones[r_hand].y), ImVec2(screen_bones[r_forearm].x, screen_bones[r_forearm].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);
				drawing->line(ImVec2(screen_bones[l_hand].x, screen_bones[l_hand].y), ImVec2(screen_bones[l_forearm].x, screen_bones[l_forearm].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);

				/* forarm to shoulder */
				drawing->line(ImVec2(screen_bones[r_forearm].x, screen_bones[r_forearm].y), ImVec2(screen_bones[r_upperarm].x, screen_bones[r_upperarm].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);
				drawing->line(ImVec2(screen_bones[l_forearm].x, screen_bones[l_forearm].y), ImVec2(screen_bones[l_upperarm].x, screen_bones[l_upperarm].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);

				/* shoulder to spine */
				drawing->line(ImVec2(screen_bones[r_upperarm].x, screen_bones[r_upperarm].y), ImVec2(screen_bones[spine4].x, screen_bones[spine4].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);
				drawing->line(ImVec2(screen_bones[l_upperarm].x, screen_bones[l_upperarm].y), ImVec2(screen_bones[spine4].x, screen_bones[spine4].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);

				/* top spine to neck */
				drawing->line(ImVec2(screen_bones[spine4].x, screen_bones[spine4].y), ImVec2(screen_bones[neck].x, screen_bones[neck].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);

				/* top spine to tailbone */
				drawing->line(ImVec2(screen_bones[spine4].x, screen_bones[spine4].y), ImVec2(screen_bones[spine1].x, screen_bones[spine1].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);

				/* spine1 to hips */
				drawing->line(ImVec2(screen_bones[spine1].x, screen_bones[spine1].y), ImVec2(screen_bones[r_hip].x, screen_bones[r_hip].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);
				drawing->line(ImVec2(screen_bones[spine1].x, screen_bones[spine1].y), ImVec2(screen_bones[l_hip].x, screen_bones[l_hip].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);

				/* spine1 to pelvis + penis */
				drawing->line(ImVec2(screen_bones[spine1].x, screen_bones[spine1].y), ImVec2(screen_bones[pelvis].x, screen_bones[pelvis].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);

				/* hips to knees */
				drawing->line(ImVec2(screen_bones[r_hip].x, screen_bones[r_hip].y), ImVec2(screen_bones[r_knee].x, screen_bones[r_knee].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);
				drawing->line(ImVec2(screen_bones[l_hip].x, screen_bones[l_hip].y), ImVec2(screen_bones[l_knee].x, screen_bones[l_knee].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);

				/* knees to feet */
				drawing->line(ImVec2(screen_bones[r_knee].x, screen_bones[r_knee].y), ImVec2(screen_bones[r_foot].x, screen_bones[r_foot].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);
				drawing->line(ImVec2(screen_bones[l_knee].x, screen_bones[l_knee].y), ImVec2(screen_bones[l_foot].x, screen_bones[l_foot].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);

				/* feet to toes */
				drawing->line(ImVec2(screen_bones[r_foot].x, screen_bones[r_foot].y), ImVec2(screen_bones[r_toe].x, screen_bones[r_toe].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);
				drawing->line(ImVec2(screen_bones[l_foot].x, screen_bones[l_foot].y), ImVec2(screen_bones[l_toe].x, screen_bones[l_toe].y), drawing->ToImVec(settings->visuals.players.skeleton.color), settings->visuals.players.skeleton.thickness);
			}
		}
	}
}

void Visuals::DrawCornerBox(int X, int Y, int W, int H, ImVec4 inline_color, ImVec4 outline_color)
{
	float lineW = (W / 5);
	float lineH = (H / 6);
	float lineT = 1;

	/* outline */
	{
		/* bottom right */
		drawing->line(ImVec2{ X - lineT, Y - lineT }, ImVec2{ X + lineW, Y - lineT }, outline_color, 1.5f);
		drawing->line(ImVec2{ X - lineT, Y - lineT }, ImVec2{ X - lineT, Y + lineH }, outline_color, 1.5f);

		/* top right */
		drawing->line(ImVec2{ X - lineT, Y + H - lineH }, ImVec2{ X - lineT, Y + H + lineT }, outline_color, 1.5f);
		drawing->line(ImVec2{ X - lineT, Y + H + lineT }, ImVec2{ X + lineW, Y + H + lineT }, outline_color, 1.5f);

		/* bottom left */
		drawing->line(ImVec2{ X + W - lineW, Y - lineT }, ImVec2{ X + W + lineT, Y - lineT }, outline_color, 1.5f);
		drawing->line(ImVec2{ X + W + lineT, Y - lineT }, ImVec2{ X + W + lineT, Y + lineH }, outline_color, 1.5f);

		/* top left */
		drawing->line(ImVec2{ X + W + lineT, Y + H - lineH }, ImVec2{ X + W + lineT, Y + H + lineT }, outline_color, 1.5f);
		drawing->line(ImVec2{ X + W - lineW, Y + H + lineT }, ImVec2{ X + W + lineT, Y + H + lineT }, outline_color, 1.5f);
	}

	/* inline */
	{
		/* top left*/
		drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y), static_cast<LONG>(X), static_cast<LONG>(Y + lineH) }, inline_color, 1.f);
		drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y), static_cast<LONG>(X + lineW), static_cast<LONG>(Y) }, inline_color, 1.f);

		/* top right*/
		drawing->line({ static_cast<LONG>(X + W - lineW), static_cast<LONG>(Y), static_cast<LONG>(X + W), static_cast<LONG>(Y) }, inline_color, 1.f);
		drawing->line({ static_cast<LONG>(X + W), static_cast<LONG>(Y), static_cast<LONG>(X + W), static_cast<LONG>(Y + lineH) }, inline_color, 1.f);

		/* bottom left */
		drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y + H - lineH),  static_cast<LONG>(X),  static_cast<LONG>(Y + H) }, inline_color, 1.f);
		drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y + H), static_cast<LONG>(X + lineW), static_cast<LONG>(Y + H) }, inline_color, 1.f);

		/* bottom right */
		drawing->line({ static_cast<LONG>(X + W - lineW),  static_cast<LONG>(Y + H),  static_cast<LONG>(X + W),  static_cast<LONG>(Y + H) }, inline_color, 1.f);
		drawing->line({ static_cast<LONG>(X + W),  static_cast<LONG>(Y + H - lineH),  static_cast<LONG>(X + W),  static_cast<LONG>(Y + H) }, inline_color, 1.f);
	}

}

auto Visuals::Draw() -> void
{
	/* Set ImGui Window Size */
	ImGui::SetNextWindowSize(ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));

	/* Set ImGui Window Pos */
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	/* Begin Drawing */
	ImGui::Begin("main", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_None | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
	{
		DrawPlayers();
	}
	ImGui::End();
}

Visuals* visuals = new Visuals();