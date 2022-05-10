#include "../webserv.hpp"

/* ************************************************************************** */
/*  LOG                                                                       */
/* ************************************************************************** */
Log::Log(): v()
{
	return;
}

Log::Log( const Log &obj ): v(obj.v)
{
	return ;
}

Log	&Log::operator=( const Log &obj )
{
	v = obj.v;
	return (*this);
}

Log::~Log()
{
	return ;
}

// return a vector of the history of requests sort from the oldest to the newest
std::vector<Request> Log::getLog() const
{
	return (v);
}

//return the oldest request
Request Log::getFirst() const
{
	return (*v.begin());
}

//return the last request done
Request Log::getLast() const
{
	return (*(v.end() - 1));
}

//add a request to the history
void	Log::add_one(Request newone)
{
	v.push_back(newone);
}

// clear the history
void	Log::clear()
{
	v.clear();
}

// returns the number of requests in the log
size_t	Log::size() const
{
	return (v.size());
}
