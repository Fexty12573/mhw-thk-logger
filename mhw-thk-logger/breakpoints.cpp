
#include <breakpoints.h>
#include <globals.h>
#include <fmt/format.h>
#include <monster/monster.h>

#include <loader/loader.h>
#include <utility/ghidra_export.h>
using namespace loader;

#include <globals.h>


std::string wide_string_to_string(const std::wstring& wide_string)
{
	if (wide_string.empty())
	{
		return "";
	}

	const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &wide_string.at(0), (int)wide_string.size(), nullptr, 0, nullptr, nullptr);
	if (size_needed <= 0)
	{
		throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
	}

	std::string result(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wide_string.at(0), (int)wide_string.size(), &result.at(0), size_needed, nullptr, nullptr);
	return result;
}

std::vector<std::string> python_split(std::wstring wstr, const char c) {
	std::string str = wide_string_to_string(wstr);
	std::vector<std::string>   result;
	std::stringstream  data(str);

	std::string line;
	while (std::getline(data, line, c))
	{
		result.push_back(line); // Note: You may get a couple of blank lines
		// When multiple underscores are beside each other.
	}
	return result;
}

bp_triplet split_breakpoint_arguments(std::wstring s) {
	auto data = python_split(s, ' ');
	auto len = data.size();
	int thk = 0;
	int node = 0;
	int segment = 0;
	if (len > 3) thk = std::stoi(data[3]);
	if (len > 4) node = std::stoi(data[4]);
	if (len > 5) segment = std::stoi(data[5]);
	bp_triplet result = std::make_tuple(thk, node, segment);
	return result;
}

void add_breakpoint(bp_triplet t)
{
	auto monster = "Global";
	std::set<bp_triplet> target;
	if (g_MonsterFilter != -1) {
		monster = mh::Monster::Names.at((mh::Monster::ID)g_MonsterFilter);
	}
	if (!g_MonsterBreakpoints.contains(g_MonsterFilter)) {
		LOG(INFO) << "Emplacing Element";
		target = std::set<bp_triplet>();
		g_MonsterBreakpoints.emplace(g_MonsterFilter, target);
	}
	else {
		target = g_MonsterBreakpoints[g_MonsterFilter];
	}
	target.insert(t);
	g_MonsterBreakpoints[g_MonsterFilter] = target;
	auto msg = fmt::format("Created {} Breakpoint: THK{:02} Node {} Segment {}", monster, std::get<0>(t), std::get<1>(t), std::get<2>(t));
	MH::Chat::DisplayMessage(msg);
	LOG(INFO) << std::get<0>(t) << "," << std::get<1>(t) << "," << std::get<2>(t);
	for (auto f : target)
		LOG(INFO) << std::get<0>(f) << "," << std::get<1>(f) << "," << std::get<2>(f);
	for (auto f : g_MonsterBreakpoints[g_MonsterFilter])
		LOG(INFO) << std::get<0>(f) << "," << std::get<1>(f) << "," << std::get<2>(f);
	LOG(INFO) << msg;
}

void missing_breakpoint(const char* monster_name, bp_triplet f) {
	auto msg = fmt::format("No {} Breakpoint: THK{:02} Node {} Segment {}", monster_name, std::get<0>(f), std::get<1>(f), std::get<2>(f));
	MH::Chat::DisplayMessage(msg);
}

void del_breakpoint(bp_triplet t)
{
	auto monster = "Global";
	std::set<bp_triplet> target;
	if (g_MonsterFilter != -1) {
		monster = mh::Monster::Names.at((mh::Monster::ID)g_MonsterFilter);
	}
	if (!g_MonsterBreakpoints.contains(g_MonsterFilter)) {
		missing_breakpoint(monster, t);
		return;
	}
	target = g_MonsterBreakpoints[g_MonsterFilter];
	if (!target.contains(t)) {
		missing_breakpoint(monster, t);
		return;
	}
	target.erase(t);
	g_MonsterBreakpoints[g_MonsterFilter] = target;
	auto msg = fmt::format("Deleted {} Breakpoint: THK{:02} Node {} Segment {}", monster, std::get<0>(t), std::get<1>(t), std::get<2>(t));
	MH::Chat::DisplayMessage(msg);
	LOG(INFO) << msg;
}

void show_breakpoint()
{
	auto msg = fmt::format("Listing Breakpoints:");
	MH::Chat::DisplayMessage(msg);
	for (const auto& [mon_id, bps] : g_MonsterBreakpoints) {
		std::string name;
		if (mon_id != -1) name = mh::Monster::Names.at((mh::Monster::ID)mon_id);
		else name = "Global";
		for (auto f : bps) {
			auto name = mh::Monster::Names.at((mh::Monster::ID)g_MonsterFilter);
			auto msg = fmt::format("{} Breakpoint: THK{:02} Node {} Segment {}", name, std::get<0>(f), std::get<1>(f), std::get<2>(f));
			MH::Chat::DisplayMessage(msg);
			LOG(INFO) << msg;
		}
	}

}

void manage_breakpoint(std::wstring s) {
	if (s.starts_with(L"/thk bp lst"))
	{
		show_breakpoint();
	}
	else {
		bp_triplet indices = split_breakpoint_arguments(s);
		if (s.starts_with(L"/thk bp add"))
		{
			add_breakpoint(indices);
		}
		else if (s.starts_with(L"/thk bp del"))
		{
			del_breakpoint(indices);
		}
	}
}

void check_breakpoints(int mon_id, int thk_ix, int node_ix, int segment_ix){
	LOG(INFO) << mon_id << "," << thk_ix << "," << node_ix << "," << segment_ix;
	std::string monster_name = mh::Monster::Names.at((mh::Monster::ID) mon_id);
	if (g_MonsterBreakpoints.contains(mon_id)) {
		std::set<bp_triplet> bps = g_MonsterBreakpoints[mon_id];
		bp_triplet t = std::make_tuple(thk_ix, node_ix, segment_ix);
		if (bps.contains(t)) {
			auto msg = fmt::format("{} Breakpoint Triggered at THK{:02} Node {} Segment {}", monster_name, thk_ix, node_ix, segment_ix);
			MH::Chat::DisplayMessage(msg);
			LOG(INFO) << msg;
			pause();
			return;
		}
	}
	mon_id = -1;
	if (g_MonsterBreakpoints.contains(-1)) {
		std::set<bp_triplet> bps = g_MonsterBreakpoints[mon_id];
		bp_triplet t = std::make_tuple(thk_ix, node_ix, segment_ix);
		if (bps.contains(t)) {
			auto msg = fmt::format("Global Breakpoint Triggered by {} at THK{:02} Node {} Segment {}", monster_name, thk_ix, node_ix, segment_ix);
			MH::Chat::DisplayMessage(msg);
			LOG(INFO) << msg;
			pause();
			return;
		}
	}
}