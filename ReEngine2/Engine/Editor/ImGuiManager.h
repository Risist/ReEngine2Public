#pragma once
#include <string>
#include <sstream>
#include "ImGuiHelpers.h"

class ImGuiManager
{
public:
	int GetNextId() { return _id++; }
	const char* GetNextId(const char* s)
	{
		return GetIdString(s, GetNextId() );
	}

	const char* GetIdString(const char* s, int id)
	{
		_ss.str(s);
		_ss << "##" << id;
		_ss >> _buffer;
		return _buffer.c_str();
	}

	const char* GetIdString(const char* s, const char* type, int id)
	{
		_ss.str("");
		_ss << s << "##" << type << id;
		_buffer = _ss.str();
		return _buffer.c_str();
	}
	const char* GetIdString(const char* s, const char* type, int id, int id2)
	{
		_ss.str("");
		_ss << s << "##" << type << id << "|" << id2;
		_buffer = _ss.str();
		return _buffer.c_str();
	}
	const char* GetIdString(const char* s, const char* type, int id, int id2, int id3)
	{
		_ss.str("");
		_ss << s << "##" << type << id << "|" << id2 << "|" << id3;
		_buffer = _ss.str();
		return _buffer.c_str();
	}

	void ResetId() { _id = 0; }

private:
	std::stringstream _ss;
	std::string _buffer;
	int _id = 0;
};
extern ImGuiManager imGuiManager;

//#define IMGUI_ID(cstr) imGuiManager.GetNextId((cstr))
#define IMGUI_ID(cstr) imGuiManager.GetIdString((cstr), __FILE__, __LINE__ )