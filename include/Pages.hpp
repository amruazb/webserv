#pragma once

#include <string>
#include <ServerTraits.hpp>

const	std::string	dirList(const std::string& path, const std::string& reqURL);
const	std::string	getErrPage(const std::string& code, const std::string& mssg);