#include "stdafx.h"
#include "json_processor.h"

//////////////////////////////////////////////////////////////////
// constructor, destructor
//////////////////////////////////////////////////////////////////
json_processor::json_processor() {
}

json_processor::~json_processor() {
	//document.Clear();
}


//////////////////////////////////////////////////////////////////
// methods
//////////////////////////////////////////////////////////////////
void json_processor::read_json(const std::string& json_str) {
	document.Parse(json_str.c_str());
}

bool json_processor::is_ok() {
	rapidjson::Value& value_status = document["status"];
	std::string status_str = value_status.GetString();
	return (status_str == "ok");
}

int json_processor::get_citation_count() {
	//원래는 json_processor를 상속받는 클래스를 만들어야 하지만
	//빠른 구현을 위해서 대충 이 클래스에 구현
	rapidjson::Value& citation_count = document["message"]["is-referenced-by-count"];
	return citation_count.GetInt();
}