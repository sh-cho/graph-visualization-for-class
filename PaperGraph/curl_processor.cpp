#include "stdafx.h"
#include "curl_processor.h"


//////////////////////////////////////////////////////////////////
// constructor, destructor
//////////////////////////////////////////////////////////////////
curl_processor::curl_processor()
{
	curl = curl_easy_init();
	if (!curl) {
		throw std::exception("failed to make curl object");
	}
}

curl_processor::~curl_processor()
{
	curl_easy_cleanup(curl);
}


//////////////////////////////////////////////////////////////////
// methods
//////////////////////////////////////////////////////////////////
