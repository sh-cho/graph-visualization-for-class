#include "stdafx.h"
#include "PaperGraphWidget.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
	if (1) {
		_curl_processor.set_url("http://dblp.uni-trier.de/rec/bib1/conf/sbrn/WedemannCD06");
		_curl_processor.perform();
		printf("%s", _curl_processor.get_buffer().c_str());
		
		_bibtex_processor.read(_curl_processor.get_buffer());
		std::string doi;
		_bibtex_processor.get_value("doi", doi);

		std::string address = std::string("http://api.crossref.org/works/")+doi;
		_curl_processor.set_url(address.c_str());
		_curl_processor.perform();

		printf("json: %s\n", _curl_processor.get_buffer().c_str());

		//rapidjson test
		rapidjson::Document d;
		d.Parse(_curl_processor.get_buffer().c_str());

		rapidjson::Value& value_status = d["status"];
		std::string status_str = value_status.GetString();
		printf("status: %s\n", status_str.c_str());

		//if (status_str != "ok") {
		//	printf("status: %s\n", status_str.c_str());
		//	printf("not ok\n");
		//	return 1;
		//}

		rapidjson::Value& value_message = d["message"];
		rapidjson::Value& citation_count = value_message["is-referenced-by-count"];
		int cit_cnt = citation_count.GetInt();
		printf("citation: %d\n", cit_cnt);

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
