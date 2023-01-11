
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
	auto offset = std::string("/thk bp ").size();
	auto data = python_split(s, ' ');
	auto len = data.size();
	int thk = 0;
	int node = 0;
	int segment = 0;
	if (len >= 3) thk = stoi(data[3]);
	if (len >= 4) node = stoi(data[4]);
	if (len >= 5) segment = stoi(data[5]);
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
		g_MonsterBreakpoints.emplace(g_MonsterFilter, target);
	}
	else {
		target = g_MonsterBreakpoints[g_MonsterFilter];
	}
	target = g_MonsterBreakpoints[g_MonsterFilter];
	target.insert(t);
	auto msg = fmt::format("Created {} Breakpoint: THK{:02} Node {} Segment {}", monster, std::get<0>(t), std::get<1>(t), std::get<2>(t));
	MH::Chat::DisplayMessage(msg);
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
	auto msg = fmt::format("Deleted {} Breakpoint: THK{:02} Node {} Segment {}", monster, std::get<0>(t), std::get<1>(t), std::get<2>(t));
	MH::Chat::DisplayMessage(msg);
	LOG(INFO) << msg;
}

void show_breakpoint()
{
	if (g_MonsterBreakpoints.contains(-1)) {
		for (auto f : g_MonsterBreakpoints[-1]) {
			auto msg = fmt::format("Global Breakpoint: THK{:02} Node {} Segment {}", std::get<0>(f), std::get<1>(f), std::get<2>(f));
			MH::Chat::DisplayMessage(msg);
			LOG(INFO) << msg;
		}
	}
	if (g_MonsterFilter != -1 && g_MonsterBreakpoints.contains(g_MonsterFilter)) {
		for (auto f : g_MonsterBreakpoints[g_MonsterFilter]) {
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