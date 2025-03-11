

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

namespace ft
{
	/**
	 * std::string overload to apply some of the useful methods.
	*/
	class string : public std::string
	{
		public:
			string();
        	string(const char* str);
        	string(const std::string& str);
			string& operator=(const std::string& other);
			string& operator= (const char* s);
			string& operator= (char c);

			std::vector<string> split(char sep);

			string rstrip(char c);
			string lstrip(char c);
			string strip(char c);

			string replace_all(const string& from, const string& to);

	};

	template <typename T>
	string to_string(T n)
	{
		std::stringstream ss;

		ss << n;
		return (ss.str());
	}

	string file_to_string(const string file_name); // Throws

	template <typename T>
	T from_string(string str) // Throws
	{
		std::stringstream ss(str);

		T n;
		ss >> n;
		if (ss.fail())
			throw std::exception();
		return (n);
	}
}
