#include "Cgi.hpp"

Cgi::Cgi(){

}

Cgi::Cgi(const Cgi &obj)
{
    *this = obj;
}

Cgi &Cgi::operator=(const Cgi &rhs)
{
    if (this == &rhs)
        return (*this);
    return (*this);
}

Cgi::~Cgi(){

}

void Cgi::SetEnv(std::map<std::string, std::string> &envMap, Response &res, Request &req){
    (void) res;
    
    this->envMap = envMap;
    this->envMap["AUTH_TYPE"] = "null";
    this->envMap["CONTENT_LENGTH"] = this->envMap["HTTP_CONTENT_LENGTH"];
    this->envMap["CONTENT_TYPE"] = this->envMap["HTTP_CONTENT_TYPE"];
    this->envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->envMap["PATH_INFO"] = req.getReqUrl();
    this->envMap["PATH_TRANSLATED"] = req.getReqUrl();
    this->envMap["QUERY_STRING"] = req.getQueryString();
    this->envMap["REQUEST_METHOD"] = req.getStrRequestType();
    this->envMap["REQUEST_URI"] = req.getQueryUrl();
    this->envMap["SCRIPT_NAME"] = req.getReqUrl();
    this->envMap["SERVER_NAME"] = "WEBSERV";
    this->envMap["SERVER_PORT"] = "8080";
    this->envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->envMap["SERVER_SOFTWARE"] = "Webserv/1.0";
    this->envMap["REDIRECT_STATUS"] = "200";
}

char **Cgi::GetCharEnv(){

    size_t size =  this->envMap.size();
    char **charenv = new char*[size + 1]; 
    charenv[size] = NULL;

    std::map<std::string, std::string>::iterator b = this->envMap.begin();
    std::string temp;

    for (int i = 0; (size_t)i < this->envMap.size(); i++)
    {
        temp = b->first + "=" + b->second;
        charenv[i] = strdup(temp.c_str());
        b++;
    }
    return (charenv);
}
// add all the headers to the response

void Cgi::HandleCgi(Response &res, Request &req, std::string rooturl, const ServerTraits& conf)
{
    this->scriptpath = rooturl + req.getCgiUrl();
    this->RunCgi(res, req, conf);
	std::cout << "handle cgi" << std::endl;
}

void Cgi::RunCgi(Response &res, Request &req, const ServerTraits& conf)
{

    FILE *parent_input = tmpfile();
    FILE *child_output = tmpfile();

    if (parent_input == NULL || child_output == NULL)
	{
		fclose(parent_input);
		fclose(child_output);
        throw ServerManager::ErrorPage(conf, "500");
	}

    int parent_in_fd = fileno(parent_input);
    int child_out_fd = fileno(child_output);

    if (req.getReqType() == POST)
    {
		std::cout << "POSTBODY" << req.getPostBody() << std::endl;
		if (fputs(req.getPostBody().c_str(), parent_input) == EOF)
		{
		
			fclose(parent_input);
			fclose(child_output);
			close(child_out_fd);
			close(parent_in_fd);
            throw ServerManager::ErrorPage(conf, "500");
	   }
	// std::cout<< "first one"<< std::endl;
       if  (fseek(parent_input, 0, SEEK_SET) == -1)
	   {
		std::cout << "her cgi" << std::endl;
			fclose(parent_input);
			fclose(child_output);
			close(child_out_fd);
			close(parent_in_fd);
            throw ServerManager::ErrorPage(conf, "500");
	   }
    }

    pid_t pid;

    pid = fork();
	// std::cout<< "second one"<< std::endl;
    if (pid == -1)
    {
		fclose(parent_input);
		fclose(child_output);
		close(child_out_fd);
		close(parent_in_fd);
        throw ServerManager::ErrorPage(conf, "500");
    }
	// set file descriptors for child process
	// if execve fails then free and exit child process and return error.
	// std::cout<< "third one"<< std::endl;
	if (pid == 0)
	{
		char *const *env = this->GetCharEnv();
	
    	if (dup2(parent_in_fd, STDIN_FILENO) == -1)
		{
			fclose(parent_input);
			fclose(child_output);
			close(child_out_fd);
			close(parent_in_fd);
            throw ServerManager::ErrorPage(conf, "500");
		}
		// std::cout<< "4 one"<< std::endl;
        if (dup2(child_out_fd, STDOUT_FILENO) == -1)
		{
			fclose(parent_input);
			fclose(child_output);
			close(child_out_fd);
			close(parent_in_fd);
            throw ServerManager::ErrorPage(conf, "500");
		}

		std::cerr << this->scriptpath <<std::endl;
        char *const *argv = NULL;
		std::cout<< "5 one"<< std::endl;
		if (execve(this->scriptpath.c_str(), argv, env) == -1)
		{
			std::cout<< this->scriptpath << std::endl;
			std::cout<< argv << std::endl;
			std::cout<< env << std::endl;
			for (int i = 0; env[i] != NULL; i++)
				free(env[i]);
			delete[] env;

			std::cout << getErrPage("500", "Internal Server Error") << std::endl;

			fclose(parent_input);
			fclose(child_output);
			close(child_out_fd);
			close(parent_in_fd);

			exit(-1);
		}
	}

	// stop child process
	// check for error
	// wait for child to end and then read from std output of the child process via pipe.

	if (pid != 0)
	{
		int     outchild = 0;
        double  time = 0;
        int status = 0;

        clock_t start = clock();
        while (outchild == 0)
        {
            // check for if kill fails
		    outchild = waitpid(pid, &status, WNOHANG);
            clock_t end = clock();
            time = static_cast<double>(end - start) / CLOCKS_PER_SEC;
            // std::cerr << "outchild: " << outchild << "time :" << time << std::endl;
            // std::cout<< "6 one"<< std::endl;
            if (time > 5)
            {
                kill(pid, SIGTERM);
				fclose(parent_input);
				fclose(child_output);
				close(child_out_fd);
				close(parent_in_fd);
                throw ServerManager::ErrorPage(conf, "504");
            }
        }
		// std::cout<< "7 one"<< std::endl;
        if (WIFEXITED(status) == true && WEXITSTATUS(status) == EXIT_FAILURE)
        {
			fclose(parent_input);
			fclose(child_output);
			close(child_out_fd);
			close(parent_in_fd);
            throw ServerManager::ErrorPage(conf, "500");
        }
		char read_buffer[CGI_BUFF];
        std::string head = "";
		std::string body = "";

		memset(read_buffer, 0, CGI_BUFF);
		// std::cout<< "8 one"<< std::endl;
		if (fseek(child_output, 0, SEEK_SET) != 0)
		{
			fclose(parent_input);
			fclose(child_output);
			close(child_out_fd);
			close(parent_in_fd);
            throw ServerManager::ErrorPage(conf, "500");
		}
		while (feof(child_output) == false)
		{
			fread(read_buffer, (CGI_BUFF - 1), 1, child_output);
			body += read_buffer;
			memset(read_buffer, 0, CGI_BUFF);
		}
		// std::cerr << body << std::endl;
        // parse response.
    
        if (body.find("\r\n\r\n") != std::string::npos)
        {
            head = body.substr(0, body.find("\r\n\r\n"));
            body.erase(0, (body.find("\r\n\r\n") + 4));

            res.setResponseHeader("200", "OK");
            res.appendHeader(head);
            res.setCgiBody(body);
        }
        else
        {
            res.setResponseHeader("200", "OK");
            res.setCgiBody(body);
        }
	}

	fclose(parent_input);
	fclose(child_output);
	close(child_out_fd);
	close(parent_in_fd);

    // set response object.
}