#include <Arduino.h>

#ifndef DATETIME_H
#define DATETIME_H
class DateTime
{
    public:
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;

        DateTime(int year, int month, int day, int hour, int minute, float second) {
            this->year = year;
            this->month = month;
            this->day = day;
            this->hour = hour;
            this->minute = minute;
            this->second = second;
        }

        DateTime() {
            this->year = 0;
            this->month = 0;
            this->day = 0;
            this->hour = 0;
            this->minute = 0;
            this->second = 0;
        }

        DateTime(const String dateTime) {
            //check if the string is in the format "yyyy-mm-dd hh:mm:ss"
            //or if it's only the time part "hh:mm:ss"
            if (dateTime.length() >= 10) {
                year = dateTime.substring(0, 4).toInt();
                month = dateTime.substring(5, 7).toInt();
                day = dateTime.substring(8, 10).toInt();
                hour = dateTime.substring(11, 13).toInt();
                minute = dateTime.substring(14, 16).toInt();
                second = dateTime.substring(17, 19).toFloat();
            } else if (dateTime.length() <= 9) {
                year = 0;
                month = 0;
                day = 0;
                hour = dateTime.substring(0, 2).toInt();
                minute = dateTime.substring(3, 5).toInt();
                second = dateTime.substring(6, 8).toFloat();
            }
        }

        //compare only the time part between two DateTime objects
        //say whether the time part of the first DateTime object is less than the time part of the second DateTime object
        bool timeLessThan(const DateTime &other) const {
            return hour < other.hour || (hour == other.hour && minute < other.minute) || (hour == other.hour && minute == other.minute && second < other.second);
        }

        //compare only the time part between two DateTime objects
        //say whether the time part of the first DateTime object is greater than the time part of the second DateTime object
        bool timeGreaterThan(const DateTime &other) const {
            return hour > other.hour || (hour == other.hour && minute > other.minute) || (hour == other.hour && minute == other.minute && second > other.second);
        }

        //compare only the time part between two DateTime objects
        //say whether the time part of the first DateTime object is equal to the time part of the second DateTime object
        bool timeEquals(const DateTime &other) const {
            return hour == other.hour && minute == other.minute && second == other.second;
        }

        String toString() {
            String y = year < 10 ? "000" + String(year) : String(year);
            String m = month < 10 ? "0" + String(month) : String(month);
            String d = day < 10 ? "0" + String(day) : String(day);
            String h = hour < 10 ? "0" + String(hour) : String(hour);
            String min = minute < 10 ? "0" + String(minute) : String(minute);
            String s = second < 10 ? "0" + String(second) : String(second);
            return y + "-" + m + "-" + d + " " + h + ":" + min + ":" + s;
        }

        String toTimeString() {
            String h = hour < 10 ? "0" + String(hour) : String(hour);
            String m = minute < 10 ? "0" + String(minute) : String(minute);
            String s = second < 10 ? "0" + String(second) : String(second);
            return h + ":" + m + ":" + s;
        }

        unsigned long toSeconds() const { 
            return year * 31536000 + month * 2592000 + day * 86400 + hour * 3600 + minute * 60 + second;
        }

        bool operator==(const DateTime &other) const {
            return year == other.year && month == other.month && day == other.day && hour == other.hour && minute == other.minute && second == other.second;
        }

        bool operator!=(const DateTime &other) const {
            return !(*this == other);
        }

        bool operator<(const DateTime &other) const {
            return this->toSeconds() < other.toSeconds();
        }

        bool operator>(const DateTime &other) const {
            return this->toSeconds() > other.toSeconds();
        }

        bool operator<=(const DateTime &other) const {
            return this->toSeconds() <= other.toSeconds();
        }

        bool operator>=(const DateTime &other) const {
            return this->toSeconds() >= other.toSeconds();
        }
};
#endif