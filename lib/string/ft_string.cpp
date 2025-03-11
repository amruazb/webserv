
#include "ft_string.hpp"

namespace ft
{
	string::string() : std::string() {}
	string::string(const char* str) : std::string(str) {}
	string::string(const std::string& str) : std::string(str) {}
	string& string::operator=(const std::string& other)
	{
		std::string::operator=(other);
		return *this;
	}

	string& string::operator= (const char* s)
	{
		std::string::operator=(s);
		return *this;
	}

	string& string::operator= (char c)
	{
		std::string::operator=(c);
		return *this;
	}

	std::vector<ft::string> string::split(char sep)
	{
		std::vector<ft::string> vec;
		std::stringstream ss(*this);
		ft::string buff;

		while (std::getline(ss, buff, sep))
		{
			if (buff.length() == 0)
				continue ;
			vec.push_back(buff);
		}
		return vec;
	}

	string string::rstrip(char c)
	{
		string cpy(*this);

        if (cpy.empty() == true)
            return (cpy);
		while (*(cpy.end() - 1) == c)
        {
			cpy.erase(cpy.end() - 1);
        }
		return (cpy);
	}

	string string::lstrip(char c)
	{
		string cpy(*this);

		size_t spos = 0;
		for (spos = 0; spos < cpy.size() && cpy[spos] == c; ++spos)
			;
		return cpy.substr(spos);
	}

	string string::strip(char c)
	{
		return lstrip(c).rstrip(c);
	}

	string string::replace_all(const string& from, const string& to)
	{
		string cpy(*this);
		size_t	index = 0;
		
		while ((index = cpy.find(from, index)) != std::string::npos)
		{
			cpy.replace(index, from.length(), to);
			index += to.length();
		}
		return (cpy);
	}

	string file_to_string(const string file_name)
	{
		std::stringstream ss;
		std::ifstream file;

		file.exceptions ( std::ifstream::badbit );
		file.open(file_name.c_str());
		if (file.fail())
			throw std::exception();
		ss << file.rdbuf();
		if (ss.bad())
			throw std::exception();
		file.close();
		return (ss.str());
	}
}


