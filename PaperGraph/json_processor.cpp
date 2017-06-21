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
	//������ json_processor�� ��ӹ޴� Ŭ������ ������ ������
	//���� ������ ���ؼ� ���� �� Ŭ������ ����
	rapidjson::Value& citation_count = document["message"]["is-referenced-by-count"];
	return citation_count.GetInt();
}