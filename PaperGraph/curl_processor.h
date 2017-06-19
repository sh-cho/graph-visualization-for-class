#ifndef CURL_PROCESSOR_H
#define CURL_PROCESSOR_H

#include <stdafx.h>

class curl_processor
{
	//private var
private:
	CURL *curl;
	CURLcode res;

	//constructor, destructor
public:
	curl_processor();
	~curl_processor();

	//method
public:

};

#endif // CURL_PROCESSOR_H