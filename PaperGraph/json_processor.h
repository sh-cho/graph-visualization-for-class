#pragma once
#include "stdafx.h"

class json_processor
{
	//private var
private:
	rapidjson::Document document;

	//constructor, destructor
public:
	json_processor();
	~json_processor();

	//methods
public:
	void read_json(const std::string& json_str);
	bool is_ok();
	int get_citation_count();
};

