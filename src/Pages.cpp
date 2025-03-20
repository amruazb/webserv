#include "Pages.hpp"
#include <dirent.h>

const	std::string	dirList(const std::string& path, const std::string& reqURL)
{
	std::string html = "<html>"
						"<head>"                                                                                  
						"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"               
						"<title>Directory listing"
						" </title>"
                       "</head>"
                       "<body>"
                       "<h1>Directory listing"
					   " </h1>"
                       "<hr>"
                       "<ul>";
	DIR	*dirptr = opendir(path.c_str());
	if (dirptr == NULL)
		return	html + "<li>COULD NOT OPEN"
                      "DIRECTORY</ li></ ul><hr></ body></ html> ";
	struct dirent *dirElement = readdir(dirptr);
	while (dirElement)
	{
		const	std::string &filename = dirElement->d_name;
		html += "<li><a href=\"" + reqURL + filename +  "\">" + filename + "</a></li>";
		dirElement = readdir(dirptr);
	}
	closedir(dirptr);
	return html + "</ul><hr></body></html>";
}

const	std::string	getErrPage(const std::string& code, const std::string& mssg)
{
	// if (conf.error_pages.find(code) != conf.error_pages.end())
	// 	return (conf.error_pages.find(code)->second);

	std::string	html = "<!DOCTYPE html>"
						"<html lang=\"en\">"
						"<head>"
						"	<meta charset=\"UTF-8\">"
						"	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
						"	<title>" + code + " " + mssg + "</title>"
						""
						"	<style>"
						"		h1 {"
						"			text-align: center;"
						"			margin-top: 40px;"
						"			color: red;"
						"		}"
						"		h3 {"
						"			text-align: center;"
						"		}"
						""
						"	</style>"
						"</head>"
						"<body>"
						"	<h1>" + code + " " + mssg + "</h1>"
						"	<hr>"
						"	<h3>webserv server</h3>"
						"</body>"
						"</html>";

	return (html);
}