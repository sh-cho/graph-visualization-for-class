#include "stdafx.h"
#include "PaperGraphWidget.h"
#include "MainWindow.h"

size_t write_callback(void *contents, size_t size,
	size_t nmemb, void *userp) {
	std::string* p_str = (std::string*)userp;
	
	//clear
	if (!p_str->empty())
		p_str->clear();

	//write
	p_str->append((char*)contents, size*nmemb);
	return size*nmemb;
}

int main(int argc, char *argv[])
{
	if (1) {
		CURL *curl;
		CURLcode res;
		std::string read_buffer;

		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, "http://dblp.uni-trier.de/rec/bib/conf/sbrn/WedemannCD06");
			/* example.com is redirected, so we tell libcurl to follow redirection */
			
			//write option
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);
			res = curl_easy_perform(curl);


			printf("%s\n", read_buffer.c_str());

			/* Check for errors */
			if (res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));

			/* always cleanup */
			curl_easy_cleanup(curl);
		}
		return 0;
	}

	QApplication app(argc, argv);
	
	MainWindow m;

	try {
		m.make_graph(PAPER_FILENAME);
	} catch (const std::exception& e) {
		qDebug() << "Error: " << e.what();
		return EXIT_FAILURE;
	}

	m.show();

	return app.exec();
}
