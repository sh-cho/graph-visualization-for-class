#ifndef CURL_PROCESSOR_H
#define CURL_PROCESSOR_H

#include <stdafx.h>

class curl_processor
{
	//private var
private:
	CURL *curl;
	CURLcode res;
	std::string result_buffer;

	//private func
private:
	size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

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

#endif // CURL_PROCESSOR_H