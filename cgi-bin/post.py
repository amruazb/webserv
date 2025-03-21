#!/usr/bin/env python

import cgi, cgitb 
# Create an instance of FieldStorage
form = cgi.FieldStorage()
Your_name = form.getvalue('name1')
# getting the data from fields
print ("<html>")
print ("<head>")
print ("<title>First CGI Script</title>")
print ("</head>")
print ("<body>")
print ("<h2>Hi, %s</h2>"
% (Your_name))


print ("</body>")
print ("</html>")

# import sys

# input_string = sys.stdin.read()
# parsed_string = input_string.split("&")
# input_map = {}
# for envvar in parsed_string:
# 	envvar = envvar.replace("+", " ")
# 	envvar = envvar.replace("=", ": ")
# 	input_map[envvar.split(": ")[0]] = envvar.split(": ")[1]
# print(input_map)