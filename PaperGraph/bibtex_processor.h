#pragma once
#include "stdafx.h"

class bibtex_processor
{
	//private var
private:
	bibtex::BibTeXEntry entry;

	//constructor, destructor
public:
	bibtex_processor();
	~bibtex_processor();

	//methods
public:
	void read(const std::string& bibtex_string);
	bool get_value(const std::string& field_name, std::string& return_val);
};

