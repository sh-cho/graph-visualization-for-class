#include "stdafx.h"

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
	//copy ptr
	std::string* p_str = (std::string*)userp;
	
	//clear
	if (!p_str->empty())
	p_str->clear();
	
	//write
	p_str->append((char*)contents, size*nmemb);
	return size*nmemb;
}