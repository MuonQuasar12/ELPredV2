#include "date.h"
#include <string>
#include <sstream>

using namespace std;

string date::getDateStr(){
	stringstream outstream;

	outstream<<day<<"-"<<month<<"-"<<year;

	return outstream.str();
}

int date::getDay()const{
	return day;
}
int date::getMonth()const{
	return month;
}
int date::getYear()const{
	return year;
}

bool date::operator== (const date &date1) const{
	if(year == date1.getYear() && month == date1.getMonth() && day == date1.getDay()) return true;
	else return false;
}

bool date::operator< (const date &date1) const{

	if(year < date1.getYear()) return true;
	else if(month < date1.getMonth()) return true;
	else if(day < date1.getDay()) return true;
	else return false;

}
bool date::operator> (const date &date1) const{

	if(*this<date1 || date1 == *this) return false;
	else return true;

}
bool date::operator>= (const date &date1) const{

	if(*this>date1 || date1 == *this) return true;
	else return false;

}
bool date::operator<= (const date &date1) const{

	if(*this<date1 || date1 == *this) return true;
	else return false;

}

