#include <thread>
#include <basetsd.h>
#include "driver.h"
#include "offsets.h"
#include "thread.h"
#include "menu/render.h"
#include <map>
#include <locale>
#include <codecvt>
#include "recoil.h"


std::string get_class_name(uintptr_t object)
{
	auto object_unk = driver::read<uintptr_t>(object);
	if (!object_unk)
		return {};

	return driver::read_string(driver::read<uintptr_t>(object_unk + 0x10), 13);
}

UINT64 TodCycle = 0;


std::wstring read_unicode(const std::uintptr_t address, std::size_t size)
{
	const auto buffer = std::make_unique<wchar_t[]>(size);
	driver::read(address, buffer.get(), size * 2);
	return std::wstring(buffer.get());
}

void SetRA(uintptr_t LocalPlayer) {
	auto Held = driver::read<uintptr_t>(LocalPlayer + 0x88);
	std::cout << "held: " << Held << std::endl;
	UINT64 RecoilProperties = driver::read<uintptr_t>(Held + 0x2C8);
	std::cout << "RecoilProperties: " << RecoilProperties << std::endl;

	driver::write(RecoilProperties + oRecoilMinYaw, 0.f);
	driver::write(RecoilProperties + oRecoilMaxYaw, 0.f);
	driver::write(RecoilProperties + oRecoilMinPitch, 0.f);
	driver::write(RecoilProperties + oRecoilMaxPitch, 0.f);

}

void AlwaysDay(UINT64 TodCycle)
{
	driver::write(TodCycle + 0x10, 12.00f);
}

//void SetRA(uintptr_t LocalPlayer)
//{
//
//	auto Held = driver::read<uintptr_t>(LocalPlayer + 0x98);
//	std::cout << "held" << Held << std::hex << std::endl;
//	UINT64 recoil = driver::read<uintptr_t>(Held + 0x2C0); //public RecoilProperties recoil;
//	std::cout << "recoil" << recoil << std::hex << std::endl;
//
//	driver::write(Held + 0x260, 0.f); //NoiseRadius;
//	driver::write(Held + 0x2D0, -1.f);
//	driver::write(Held + 0x308, -1.f);
//	driver::write(Held + 0x2D8, -1.f);
//	driver::write(Held + 0x304, 0.f); //private float stancePenalty;
//	driver::write(Held + 0x308, 0.f); //private float aimconePenalty;
//	driver::write(Held + 0x2D0, 0.f); //public float aimCone;
//	driver::write(Held + 0x2D4, 0.f); //public float hipAimCone;
//	driver::write(Held + 0x2D8, 0.f); //public float aimconePenaltyPerShot;
//	driver::write(recoil + oRecoilMinYaw, 0.f);
//	driver::write(recoil + oRecoilMaxYaw, 0.f);
//	driver::write(recoil + oRecoilMinPitch, 0.f);
//	driver::write(recoil + oRecoilMaxPitch, 0.f);
//
//
//}

void DoWaterHax(uintptr_t LocalPlayer)
{
	driver::write<float>(LocalPlayer + 0x644, 0.10f);
}

void SpiderAdam(uintptr_t LocalPlayer)
{

	auto BaseMovemento = driver::read<uintptr_t>(LocalPlayer + 0x4D8);
	if (!BaseMovemento)
		return;
	driver::write<float>(BaseMovemento + 0xB8, 0.0f);

}

void FovChanger(float val) {

	auto fovConvar = driver::read<uintptr_t>(game_assembly + 0x30643A8);

	auto m_pGraphicsInstance = driver::read<uintptr_t>(fovConvar + 0xB8);
	driver::write(m_pGraphicsInstance + 0x18, val);

}

void SetGravity(uintptr_t LocalPlayer, float val) {
	auto Movement = driver::read<uintptr_t>(LocalPlayer + 0x4D8);
	driver::write(Movement + 0x74, val);
}

void SetAdminFlag(uintptr_t LocalPlayer)
{
	int flags = driver::read<int>(LocalPlayer + 0x638);//0x638

	flags |= 4;

	driver::write<uintptr_t>(LocalPlayer + 0x638, flags);
}

void changefov(uintptr_t LocalPlayer)
{
	auto base_movement22 = driver::read<uintptr_t>(LocalPlayer + 0x4D8);
	auto modelstate = driver::read<uintptr_t>(LocalPlayer + 0x5C8);
	auto modelstate_flags = driver::read<uintptr_t>(modelstate + 0x24);

	modelstate_flags |= 16;

	driver::write(modelstate + 0x24, modelstate_flags);
	driver::write<float>(base_movement22 + 0x40, 60.f);
}

void thread::features()
{
	char className[1024];

			uintptr_t b_networkable = driver::IL2CPPScan("BaseNetworkable");
			std::cout << "base net: " << b_networkable << std::endl;

			uintptr_t clent = driver::read<uintptr_t>(b_networkable + 0xB8);
			std::cout << "clent: " << clent << std::endl;

			uintptr_t cliententities = driver::read<uintptr_t>(clent);
			std::cout << "cliententities net: " << cliententities << std::endl;

			uintptr_t entrealm = driver::read<uintptr_t>(cliententities + 0x10);
			std::cout << "bentrealm: " << entrealm << std::endl;

			uintptr_t bufflist = driver::read<uintptr_t>(entrealm + 0x28);
			std::cout << "bufflist net: " << bufflist << std::endl;

			uintptr_t objlist = driver::read<uintptr_t>(bufflist + 0x18);
			std::cout << "objlist net: " << objlist << std::endl;

			uint32_t objlsize = driver::read<uint32_t>(bufflist + 0x10);
			std::cout << "objlsize net: " << objlsize << std::endl;

			while (1)
			{


				for (auto i = 0; i < objlsize; i++) {
					std::cout << "1" << std::endl;

					uintptr_t curobject = driver::read<uintptr_t>(objlist + (0x20 + (i * 8)));
					uintptr_t baseobject = driver::read<uintptr_t>(curobject + 0x10);
					uintptr_t object = driver::read<uintptr_t>(baseobject + 0x30);
					WORD tag = driver::read<WORD>(object + 0x54);


					if (tag == 6)
					{

						uintptr_t objClass = driver::read<uintptr_t>(object + 0x30);

						uintptr_t ent = driver::read<uintptr_t>(objClass + 0x18);

						uintptr_t namepointer = driver::read<uintptr_t>(object + 0x60);

						auto p_cName = driver::read<uintptr_t>(object + 0x60);


						const auto classname = get_class_name(curobject);

						if (classname.find("BasePlayer") != std::string::npos)
						{
							if (!i && !local_player) // assign local player
							{
								local_player = object;
								std::cout << "base_player: " << std::hex << local_player << std::endl;

								std::cout << className << std::endl;
								while (true)
								{

									local_player = driver::read<uintptr_t>(ent + 0x28);
									float health = driver::read<float>(local_player + 0x214);
									std::cout << "health: " << health << std::endl;
									if (bTest == true)
									{
										SetAdminFlag(local_player);
									}
									if (spiderman == true)
									{
										SpiderAdam(local_player);
									}
									if (gravity == true) {
										SetGravity(local_player, setGravity);
									}
									if (fovchanger == true) {
										FovChanger(fov);

										auto active_weapon = get_active_weapon(local_player);
										std::cout << "active weapon: " << active_weapon << std::hex << std::endl;
										set_recoil_props(active_weapon);
										set_automatic(active_weapon);
										recoil_changed = false;
									}
									if (setautomatic == false) {
										
									}
								}
							}

						}
						Sleep(1);

					}


				}
			}
		

}