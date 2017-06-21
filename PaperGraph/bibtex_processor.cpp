#include "stdafx.h"
#include "bibtex_processor.h"

//////////////////////////////////////////////////////////////////
// constructor, destructor
//////////////////////////////////////////////////////////////////
bibtex_processor::bibtex_processor() {
}

bibtex_processor::~bibtex_processor() {
}


//////////////////////////////////////////////////////////////////
// methods
//////////////////////////////////////////////////////////////////
void bibtex_processor::read(const std::string& bibtex_string) {
	bibtex::read(bibtex_string, entry);
}

bool bibtex_processor::get_value(const std::string& field_name, std::string& return_val) {
	if (entry.fields.empty()) return false;

	bool found = false;
	for (auto& field: entry.fields) {
		if (field.first == field_name) {
			found = true;
			return_val = field.second.front();
			break;
		}
	}
	return found;
}