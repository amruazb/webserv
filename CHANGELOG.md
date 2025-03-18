# DATE 18 March 2025

1. Request Parsing Enhancements
# Implemented Request Type Parsing:(Response.cpp)
    >Added logic to parse the HTTP request type (e.g., GET, POST, PUT, etc.) from the raw request.
    >Moved the request type determination logic into a helper function (setReqType or determineRequestType) for better modularity and maintainability.
    .Updated the parseRequest function to use the helper function and store the request type in the _type member variable.
2. Error Handling Improvements
# Enhanced getRoute Function:
    >Debugged and ensured that the getRoute function correctly throws a 404 ErrorPage exception when no route is found.
3. Improved Error Page Handling:

Implemented setErrPage to handle custom error pages if defined in the server configuration (conf.error_pages).
Added a fallback mechanism to use default error pages if no custom error page is available.
Cached default error pages using getCachedErrPage to improve performance.
Refined Exception Handling in ManageRequest:
Added specific catch blocks for ErrorPage and HttpException to handle HTTP-specific errors.
Ensured that generic exceptions are caught and handled with a 500 Internal Server Error.
4. # Response Processing
Implemented ProcessResponse:
Added logic to normalize URLs (e.g., removing trailing slashes, handling duplicate slashes).
Resolved routes using getRoute and constructed file paths based on the server configuration and route.
Checked if the request type is allowed for the given route using throwIfnotAllowed and threw a 405 ErrorPage if not allowed.
Added support for redirection using the redirect function, which sets a 301 Moved Permanently response if a route specifies a redirect.

5. Debugging and Logging
Added detailed logging throughout the code to trace execution and debug issues:
Logged route resolution in getRoute.
Logged request type parsing in parseRequest.
Logged error handling in ManageRequest and setErrPage.
Logged client connections, data reading, and response sending in run.
Key Functionalities Implemented
Request Parsing:

Extracted request type, URL, and headers from raw HTTP requests.
Error Handling:

Handled 404 Not Found, 405 Method Not Allowed, and 500 Internal Server Error with appropriate error pages.
Response Processing:

Constructed file paths based on routes and server configuration.
Supported redirection with 301 Moved Permanently.
Server Management:

Managed client connections and requests using poll.
Handled graceful shutdown with signal handling.
# Next Steps
Test the Web Server:
    >Test various scenarios, including valid requests, nonexistent routes, unsupported methods, and large payloads.
Optimize Performance:
    >Optimize caching mechanisms for error pages and responses.
Add More Features:
    >Implement support for chunked transfer encoding and POST request body handling.
Refactor and Clean Up:
    >Refactor large functions for better readability and maintainability.
