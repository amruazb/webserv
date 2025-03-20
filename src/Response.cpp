#include "Response.hpp"

Response::Response()
{
    header = "HTTP/1.1";
    code = "200";
    mssg = "OK";
    content_type = "text/html";
    content_len = 0;
    res_body = "";
    res = "";
    setResponseHeader(code, mssg);
    parseMimes();
}
void	Response::setCgiBody(std::string body)
{
	this->res_body.clear();
	this->res_body = body;
	this->content_len = res_body.length();
	this->header += "Content-Type: " + content_type + "; charset=utf-8" CRLF
					"Content-Length: " + ft::to_string(this->content_len) + CRLF
					CRLF;
}
Response::Response(const Response &src)
{
    if (this == &src)
        return ;
    *this = src;
}

Response& Response::operator=(const Response &src)
{
    if (this == &src)
        return *this;
    header = src.header;
    code = src.code;
    mssg = src.mssg;
    content_type = src.content_type;
    content_len = src.content_len;
    res_body = src.res_body;
    res = src.res;
    return *this;
}
void Response::setResponseHeader(std::string code, std::string mssg)
{
    setCode(code);
    setMssg(mssg);
    setHeader();
}
void Response::setHeader()
{
   header.clear();
   header += "HTTP/1.1 " +code + " "+ mssg + "\r\n";

}
void Response::setCode(std::string code)
{
    this->code = code;
}
void Response::setMssg(std::string msg)
{
    mssg = msg;
}
void Response::appendHeader(const std::string& str)
{
	header += str + "\r\n";
}
Response::~Response()
{
}
std::string Response::getRes()
{
    res.clear();
    res = header + res_body;
    return res;
}
std::string Response::getHeader()
{
    return header;
}
void Response::setErrBody(std::string body, const Request &req)
{
    res_body.clear();
    res_body = body;
    content_len = res_body.length();
    if(req.getReqType() == HEAD)
        res_body.clear();
    header += "Content-Type: " + content_type + "\r\n";
    header += "Content-Length: " + ft::to_string(content_len) + "; charset=utf-8""\r\n";
    // res += "Connection: close\r\n";
    header += "\r\n";
   
}
const std::string dirList(const std::string& path, const std::string& reqURL)
{
    std::string html = "<html>"
                       "<head>"
                       "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
                       "<title>Directory listing</title>"
                       "</head>"
                       "<body>"
                       "<h1>Directory listing</h1>"
                       "<hr>"
                       "<ul>";

    std::cout << "Directory path: " << path << std::endl;                  
    DIR *dirptr = opendir(path.c_str());
    if (dirptr == NULL)
        return html + "<li>COULD NOT OPEN DIRECTORY</li></ul><hr></body></html>";

    struct dirent *dirElement;
    while ((dirElement = readdir(dirptr)) != NULL)
    {
        std::string filename = dirElement->d_name;
        
        // Skip "." and ".." and hidden files
        if (filename[0] == '.')
            continue;

        std::cout << "Filename: " << filename << std::endl;

        std::string fullPath = path + "/" + filename;
        std::string link = reqURL;

        // Ensure request URL has a trailing slash
        if (link.empty() || link[link.length() - 1] != '/')
            link += "/";

        link += filename; // Append filename

        // Check if entry is a directory
        struct stat statbuf;
        if (stat(fullPath.c_str(), &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
        {
            link += "/"; // Append trailing slash for directories
        }

        std::cout << "Full Path: " << fullPath << ", Link: " << link << std::endl;

        html += "<li><a href=\"" + link + "\">" + filename + "</a></li>";
    }
    closedir(dirptr);
    return html + "</ul><hr></body></html>";
}


void Response::setResBody(std::string path, const Request &req,bool autoindex)
{
    std::string body;
    std::string type;
    size_t pos;

    // Handle GET, HEAD, DELETE requests
    if (req.getReqType() != POST && req.getReqType() != PUT) 
    {
        if (is_dir(path.c_str())) 
        {
            std::cout << "is dir" << std::endl;
            std::cout << autoindex << std::endl;
            if (autoindex || req.getReqType() == DELETE) 
            {
                std::cout << "url is" << req.getReqUrl() << std::endl;
                std::cout<< "path is" << path << std::endl;
                body = dirList(path, req.getReqUrl());
            }
            else 
                throw HttpException("404","Not Found");
        } 
        else if (is_file(path.c_str())) 
            body = ft::file_to_string(path);
        else 
            throw HttpException("404"," Not Found");
       std::cout <<"sjhfjsfghgfhjgfhsdfg"<<std::endl; 
    }
    // Determine MIME type
    if ((pos = path.find_last_of('.')) != std::string::npos) 
    {
        type = path.substr(pos);
        std::map<std::string, std::string>::iterator it = this->mimes.find(type);
        if (it != this->mimes.end()) 
            content_type = it->second;     
    }
    // Set response body and content length
    res_body.clear();
    res_body = body;
    content_len = res_body.length();
    // Handle POST and PUT requests
    // if (request.getReqType() == POST || request.getReqType() == PUT) {
    //     if (request.getPutCode() == "201") {
    //         this->setResponseHeader("201", "Created");
    //     }
    //     if (request.getReqType() == POST) {
    //         this->setResponseHeader("200", "OK");
    //     }
    //     this->res_body.clear();
    // }
      // Handle HEAD requests
    if (req.getReqType() == HEAD) 
        this->res_body.clear();
    
    // Handle DELETE requests
    // if (req.getReqType() == DELETE) 
    // {
    //     if (remove(req.getDeleteURL().c_str()) != 0) 
    //         throw std::runtime_error("Failed to delete file: " + req.getDeleteURL());
    // }

    // Build response header
    header += "Content-Type: " + content_type + "; charset=utf-8" "\r\n"
                    "Content-Length: " + ft::to_string(this->content_len) + "\r\n"
                    "\r\n";

}
void	Response::parseMimes()
{
	std::ifstream	mimieFile("mimes.txt");
	std::string		line;
	
	if (mimieFile.fail())
	{
		mimieFile.close();
	}
	getline(mimieFile, line);
	while (!(mimieFile.eof()))
	{
		std::stringstream	str(line);
		std::string			ext, type;
		getline(str, ext, ' ');
		getline(str, type);
		this->mimes[ext] = type;
		getline(mimieFile, line);
	}
	mimieFile.close();
}

int	is_dir(const string path)
{
	struct stat	statbuf;

	if (stat(path.c_str(), &statbuf) != 0)
		return (0);
	return (S_ISDIR(statbuf.st_mode));
}

int is_file(const string path)
{
	struct stat	statbuf;

	if (stat(path.c_str(), &statbuf) != 0)
		return (0);
	return (S_ISREG(statbuf.st_mode));
}
