#include "../../webserv.hpp"

Cgi::Cgi(Request a) : request(a)
{
	// request = a;
	setEnv();
	return ;
}

Cgi &Cgi::operator=( Cgi &obj )
{
	(void)obj;
	return (*this);
}

Cgi::~Cgi()
{
	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
	return ;
}

void Cgi::setEnv()
{
	std::string a;
	if (request.getMethod() == "GET")
		a = request.getQuery();
	if (request.getMethod() == "POST")
		a = request.getPostImput();
	if (a.empty() == 1)
	{
		env = new char*[1];
		env[0] = NULL;
		return ;
	}
	unsigned int i = 0;
	for (unsigned int j = 0; j < a.size(); j++)
	{
		if (a.at(j) == '&')
			i++;
	}
	env = new char*[i + 2];
	env[i + 1] = NULL;
	i = 0;
	unsigned int k = 0;
	for (unsigned int j = 0; j < a.size(); j++)
	{
		if (a.at(j) == '&')
		{
			env[k] = new char[i + 1];
			k++;
			i = 0;
		}
		else
		{
			a.at(j);
			i++;
		}
	}
	env[k] = new char[i + 1];
	i = 0;
	k = 0;
	for (unsigned int j = 0; j < a.size(); j++)
	{
		if (a.at(j) == '&')
		{
			env[k][i] = '\0';
			k++;
			i = 0;
		}
		else
		{
			env[k][i] = a.at(j);
			i++;
		}
	}
	env[k][i] = '\0';
	return ;
}

std::string Cgi::executeCgi()
{
	int dup_in = dup(0), dup_out = dup(1);
	FILE *in = tmpfile(), *out = tmpfile();
	long fd_in = fileno(in), fd_out = fileno(out);
	std::string ret;
	char * const *n = NULL;
	lseek(fd_out, 0, 0);
	int pid = fork();
	if (pid == -1)
	{
		std::cerr << "CGI forking has failed" << std::endl; // ?
		exit(1); // ?
	}
	if (pid == 0)
	{
		dup2(fd_out, 1);
		dup2(fd_in, 0);
		execve("./website/ressources/script.cgi", n, env);
		std::cerr << "execve error" << std::endl; // ?
		exit (1); // ?
	}

	else
	{
		char buff[50] = {0}; // A REMPLACER
		waitpid(-1, NULL, 0);
		lseek(fd_out, 0, 0);
		int readen = 1;
		while (readen > 0)
		{
			memset(buff, 0, 50); // A REMPLACER
			readen = read(fd_out, buff, 50 - 1); // A REMPLACER
			ret += buff;
		}
	}
	dup2(dup_out, 1);
	dup2(dup_in, 0);
	close(fd_out);
	close(fd_in);
	close(dup_out);
	close(dup_in);
	fclose(out);
	fclose(in);
	if (pid == 0)
		exit(EXIT_SUCCESS);
	return (ret);
}
