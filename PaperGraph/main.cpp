#include "stdafx.h"
#include "PaperGraphWidget.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	if (1) {
		CURL *curl;
		CURLcode res;

		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, "http://dblp.uni-trier.de/rec/bib/conf/sbrn/WedemannCD06");
			/* example.com is redirected, so we tell libcurl to follow redirection */
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);
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
