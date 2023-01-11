#define _CRT_SECURE_NO_DEPRECATE
#include <set>
#include <nlohmann/json.hpp>

#include <monster/monster.h>
#include <symbols.h>
#include <globals.h>

#include <file_dialog.h>
#include <fmt/format.h>
#include <loader/loader.h>
#include <utility/ghidra_export.h>
using namespace loader;

using json = nlohmann::json;

inline int32_t getReg(const void* thk, uint32_t regindex)
{
	return *(int32_t*)((byte*)thk + 0x640 + 4 * regindex);
}

inline bool file_check(const std::string& name) {
	if (FILE* file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

std::string get_file_path() {
	return get_file_dialog();
}

void load_symbols() {
	std::string fp = get_file_path();
	if (!file_check(fp)) {
		auto msg = fmt::format("No File Found at {} ", fp);
		MH::Chat::DisplayMessage(msg);
		return;
	}


	std::ifstream f(fp);
	json data = json::parse(f);
	int monster = data["monster"];
	std::map<s32, sym_Thk> thkl;
	for (auto& [thk, thk_data] : data["files"].items()) {
		int thk_ix = std::stoi(thk);
		std::string thk_name = thk_data["name"];
		std::string thk_path = thk_data["path"]["path"];
		std::map<s32, sym_Node> node_map;
		for (auto& node : thk_data["module"]) {
			std::string node_name = node["names"][0];
			int node_line = node["lineno"];
			int node_index = node["index"];
			std::map<s32, std::string> segment_map;
			int i = 0;
			for (auto& segment : node["segments"]) {
				std::string repr = segment["repr"];
				segment_map[i] = repr;
				i++;
			}
			sym_Node c_node = std::make_tuple(node_name, node_line, segment_map);
			node_map[node_index] = c_node;
		}
		sym_Thk thkf = std::make_tuple(thk_name, thk_path, node_map);
		thkl[thk_ix] = thkf;
	}
	g_symbols[monster] = thkl;
	auto msg = fmt::format("Loaded {} ", fp);
	MH::Chat::DisplayMessage(msg);
	LOG(INFO) << fmt::format("Loaded Symbols from {} for {}", fp,monster);
}

void symbol_check(int m_id, int thkId, int nodeIndex, int segmentIndex, void* m, int prevMonster, int prevThk, int prevNode) {
	std::string monster_name = mh::Monster::Names.at((mh::Monster::ID)m_id);
	//LOG(INFO) << fmt::format("{} {} {} - {} {} {}", m_id, thkId, nodeIndex, prevMonster, prevThk, prevNode);
	if (g_symbols.contains(m_id)) {
		if (prevMonster != m_id) {
			LOG(INFO) << fmt::format("Monster {} [{}] at {}", monster_name, m_id, m);
		}
		sym_Thkl thkl = g_symbols[m_id];
		if (thkl.contains(thkId)) {
			sym_Thk thk = thkl[thkId];
			std::string thk_name = std::get<0>(thk);
			std::string thk_path = std::get<1>(thk);
			sym_ThkNode nodes = std::get<2>(thk);
			if (prevThk != thkId or prevMonster != m_id) {
				LOG(INFO) << fmt::format("[THK{:02}] {} at {}",
					thkId, thk_name, thk_path);
			}
			if (nodes.contains(nodeIndex)) {
				sym_Node node = nodes[nodeIndex];
				sym_NodeSegment segments = std::get<2>(node);
				std::string node_name = std::get<0>(node);
				int node_lineno = std::get<1>(node);
				if (prevNode != nodeIndex or prevThk != thkId or prevMonster != m_id)
				{
					LOG(INFO) << fmt::format("\t[Node {:03}] {} at Line {}",
						nodeIndex, node_name, node_lineno);
				}
				if (g_LogSegments)
				{
					if (segments.contains(segmentIndex)) {
						LOG(INFO) << fmt::format("\t\t[{:02}] {}",
							segmentIndex, segments[segmentIndex]);
						return;
					}
				}
			}
		}
	}
	if (g_LogSegments)
	{
		LOG(INFO) << fmt::format("[{} at {}] THK{:02}, Node Index: {}, Segment: {}",
			monster_name, m,
			thkId, nodeIndex, segmentIndex);
	}
	else
	{
		if (prevNode != nodeIndex or prevThk != thkId)
		{
			LOG(INFO) << fmt::format("[{} at {}] THK{:02}, Node Index: {}",
				monster_name, m,
				thkId, nodeIndex);
		}
	}
}

void symbol_check_registers(void* cThinkEm) {
	int i = 0;
	LOG(INFO) << fmt::format("\t[{}] [{}] [{}] [{}] [{}] [{}] [{}] [{}] [{}] [{}] [{}]",
		getReg(cThinkEm, 0 + i), getReg(cThinkEm, 1 + i), getReg(cThinkEm, 2 + i), getReg(cThinkEm, 3 + i),
		getReg(cThinkEm, 4 + i), getReg(cThinkEm, 5 + i), getReg(cThinkEm, 6 + i), getReg(cThinkEm, 7 + i),
		getReg(cThinkEm, 8 + i), getReg(cThinkEm, 9 + i), getReg(cThinkEm, 10 + i));
	i += 11;
	LOG(INFO) << fmt::format("\t[{}] [{}] [{}] [{}] [{}] [{}] [{}] [{}] [{}] [{}] [{}]",
		getReg(cThinkEm, 0 + i), getReg(cThinkEm, 1 + i), getReg(cThinkEm, 2 + i), getReg(cThinkEm, 3 + i),
		getReg(cThinkEm, 4 + i), getReg(cThinkEm, 5 + i), getReg(cThinkEm, 6 + i), getReg(cThinkEm, 7 + i),
		getReg(cThinkEm, 8 + i), getReg(cThinkEm, 9 + i), getReg(cThinkEm, 10 + i));
	i++;
}