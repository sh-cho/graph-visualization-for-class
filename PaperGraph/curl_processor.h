#pragma once
#include "stdafx.h"

class curl_processor
{
	//private var
private:
	CURL *curl;
	CURLcode res;
	std::string result_buffer;

	//constructor, destructor
public:
	curl_processor();
	~curl_processor();

	//methods
public:
	std::string get_buffer() const { return result_buffer; }
	void set_url(const char *url);
	bool perform();
};

