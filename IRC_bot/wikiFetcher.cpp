#include <stdio.h>
#include <stdlib.h>
#include "utils.hpp"
int wiki_desc(std::string topic, std::string &desc)
{

	FILE *fp;
	char path[1035];
	std::string cmd = "/usr/bin/curl ";
	std::string options = "-s ";
	std::string wiki_url = "'https://en.wikipedia.org/w/api.php?format=json&action=query&prop=extracts&exintro&explaintext&redirects=1&titles=";
	std::string exec = cmd + options + wiki_url + topic + "'";
	std::string response;
	std::vector<std::string> s_response;

	/* Open the command for reading. */
	fp = popen(exec.c_str(), "r");
	if (fp == NULL) {
		printf("Failed to run command\n" );
		exit(1);
	}
	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path), fp) != NULL)
		response += path;
	/* close */
	pclose(fp);

	/* parse the output */
	ssplit(response, "title", s_response);
	if (s_response.size() < 2)
		return 1;
	ssplit(s_response[1], "extract", s_response);
	if (s_response.size() < 4)
		return 1;
	desc = trim(s_response[3], "\",}:");
	return 0;
}
