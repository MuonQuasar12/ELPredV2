#ifndef DATE_H
#define DATE_H

#include <string>


class date{

private:
	int day;
	int month;
	int year;

public:

	date(){
		day = 0;
		month =0;
		year=0;
	}

	date(int day_, int month_, int year_){
		day = day_;
		month = month_;
		year = year_;
	}
	date(const date &Date){
		day = Date.day;
		month = Date.month;
		year = Date.year;
	}

	std::string getDateStr();
	int getDay() const;
	int getMonth() const;
	int getYear() const;

	bool operator< (const date &date1) const;
	bool operator> (const date &date1) const;
	bool operator>= (const date &date1) const;
	bool operator<= (const date &date1) const;
	bool operator== (const date &date1) const;
};

#endif