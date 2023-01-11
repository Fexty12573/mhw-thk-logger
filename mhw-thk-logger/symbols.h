#pragma once

using sym_NodeSegment = std::map<int, std::string>;
using sym_Node = std::tuple<std::string, int, sym_NodeSegment>;
using sym_ThkNode = std::map<int, sym_Node>;
using sym_Thk = std::tuple<std::string, std::string, sym_ThkNode>;
using sym_Thkl = std::map<int, sym_Thk>;
using sym_MonThkl = std::map<int, sym_Thkl>;
static sym_MonThkl g_symbols;

std::string get_file_path();
void load_symbols();
void symbol_check(int m_id, int thkId, int nodeIndex, int segmentIndex, void* m, int prevMonster, int prevThk, int prevNode);
void symbol_check_registers(void* cThinkEm);