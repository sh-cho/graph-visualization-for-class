#include "stdafx.h"
#include "curl_processor.h"


//////////////////////////////////////////////////////////////////
// private func
//////////////////////////////////////////////////////////////////
//size_t curl_processor::write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
//	//copy ptr
//	std::string* p_str = (std::string*)userp;
//
//	//clear
//	if (!p_str->empty())
//	p_str->clear();
//
//	//write
//	p_str->append((char*)contents, size*nmemb);
//	return size*nmemb;
//}


//////////////////////////////////////////////////////////////////
// constructor, destructor
//////////////////////////////////////////////////////////////////
curl_processor::curl_processor() {
	curl = curl_easy_init();
	if (!curl) {
		throw std::exception("failed to make curl object");
	}

	//set curl option
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result_buffer);
}

curl_processor::~curl_processor() {
	curl_easy_cleanup(curl);
}


//////////////////////////////////////////////////////////////////
// methods
//////////////////////////////////////////////////////////////////
void curl_processor::set_url(const char *url) {
	curl_easy_setopt(curl, CURLOPT_URL, url);
}

bool curl_processor::perform() {
	res = curl_easy_perform(curl);
	return (res == CURLE_OK);
}