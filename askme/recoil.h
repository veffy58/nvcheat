#include <cstdint>
#include "driver.h"
#include <string>
#include <locale>
#include <codecvt>
#include <map>
#pragma warning(disable : 4996)

static std::string to_string(const std::wstring& wstr)
{
	std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t > converter;
	return converter.to_bytes(wstr);
}

extern std::string current_weapon;
extern std::map <std::string, std::vector<int>> recoil_map;
extern std::mutex recoil_mutex;
extern bool recoil_changed;
extern bool auto_pistol;
extern bool auto_pistol_changed;
extern bool allow_recoil;
extern int yaw_min;
extern int yaw_max;
extern int pitch_min;
extern int pitch_max;

std::string current_weapon = "";
std::map < std::string, std::vector<int> > recoil_map;
std::mutex recoil_mutex;
bool recoil_changed = true;

bool auto_pistol = false;
bool auto_pistol_changed = true;
bool allow_recoil = false;
int yaw_min = 0;
int yaw_max = 0;
int pitch_min = 0;
int pitch_max = 0;

uintptr_t get_active_item(uintptr_t _local_player)
{
	return  driver::read<uintptr_t>(_local_player + 0x5B0);
}

int get_item_id(uintptr_t item)
{
	return  driver::read<uintptr_t>(item + 0x18);
}

std::wstring get_item_name(uintptr_t item)
{
	auto unk = driver::read<uintptr_t>(item + 0x20);
	unk = driver::read<uintptr_t>(unk + 0x20);
	return driver::read_wstring(unk + 0x14, 14);
}

bool get_recoil_properties(uintptr_t weapon, std::string name)
{
	auto base_projectile = driver::read<uintptr_t>(weapon + 0x98);
	if (!base_projectile)
		return false;

	auto recoil_prop = driver::read<uintptr_t>(base_projectile + 0x2C0);
	if (!recoil_prop)
		return false;

	int yaw_min = driver::read<float>(recoil_prop + 0x18);
	int yaw_max = driver::read<float>(recoil_prop + 0x1C);

	int pitch_min = driver::read<float>(recoil_prop + 0x20);
	int pitch_max = driver::read<float>(recoil_prop + 0x24);

	yaw_min = yaw_min; yaw_max = yaw_max; pitch_min = pitch_min; pitch_max = pitch_max;

	std::lock_guard guard(recoil_mutex);
	recoil_map[name] = { yaw_min, yaw_max, pitch_min, pitch_max };
}

uintptr_t get_active_weapon(uintptr_t _local_player)
{
	if (!_local_player)
		return false;

	auto inventory = driver::read<uintptr_t>(_local_player + 0x648);
	//std::cout << "inventory: " << inventory << std::hex << std::endl;
	if (!inventory)
		return {};

	auto contianer_belt = driver::read<uintptr_t>(inventory + 0x28);
	//std::cout << "contianer_belt: " << contianer_belt << std::hex << std::endl;

	auto contents = driver::read<uintptr_t>(contianer_belt + 0x38);
	//std::cout << "contents: " << contents << std::hex << std::endl;

	auto size = driver::read<int>(contents + 0x18);
	std::cout << "size: " << size << std::hex << std::endl;

	contents = driver::read<uintptr_t>(contents + 0x10);
	//std::cout << "contents: " << contents << std::hex << std::endl;

	try {
		for (int i = 0; i < size; i++)
		{
			static std::vector<std::wstring>recorded{};
			auto item = driver::read<uintptr_t>(contents + (0x20 + (i * 0x8)));
			std::cout << "active item: " << get_active_item(_local_player) << std::hex << std::endl;
			std::cout << "itemid: " << get_item_id(item) << std::hex << std::endl;
			//alt taraftaki get item id hesaplama yanlýþ düzeltilicek
			//if (get_item_id(item) == get_active_item(_local_player))
			//{
				static const auto weps = { L"shotgun", L"pistol", L"rifle", L"smg" };
				const auto item_name = get_item_name(item);
				for (auto wep : weps)
				{
					if (item_name.find(wep) != std::string::npos)
					{
						current_weapon = to_string(item_name);

						// check if we've iterated over this weapon already
						try {
							if (std::find(recorded.begin(), recorded.end(), item_name) == recorded.end())
							{
								get_recoil_properties(item, current_weapon);
								recorded.push_back(item_name);
							}
						}
						catch (const std::exception& exc) {
							std::cout << exc.what() << std::endl;
						}

						return item;
					}
				}
			//}
		}
	}
	catch (const std::exception& exc) {
		std::cout << exc.what() << std::endl;
	}

	return {};
}

bool set_automatic(uintptr_t weapon)
{
	auto base_projectile = driver::read<uintptr_t>(weapon + 0x98);
	if (!base_projectile)
		return false;

	return driver::write<bool>(base_projectile + 0x270, auto_pistol);
}

bool set_recoil_props(uintptr_t weapon)
{
	auto base_projectile = driver::read<uintptr_t>(weapon + 0x98);
	std::cout << "base projectile: " << base_projectile << std::hex << std::endl;
	if (!base_projectile)
		return false;

	auto recoil_prop = driver::read<uintptr_t>(base_projectile + 0x2C8);
	std::cout << "recoilprop: " << recoil_prop << std::hex << std::endl;
	if (!recoil_prop)
		return false;

	driver::write<float>(recoil_prop + 0x18, int(yaw_min));
	driver::write<float>(recoil_prop + 0x1C, int(yaw_max));

	driver::write<float>(recoil_prop + 0x20, int(pitch_min));
	driver::write<float>(recoil_prop + 0x24, int(pitch_max));
	return true;
}
