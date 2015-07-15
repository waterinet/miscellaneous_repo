#ifndef SRC_LINE_H
#define SRC_LINE_H

#include <list>
#include <string>

class SrcLine
{
public:
	typedef std::list<std::string>::iterator iterator;
	typedef std::list<std::string>::const_iterator const_iterator;
	SrcLine();
	SrcLine(const std::string& str, std::string file, int line);
	~SrcLine();
	const std::list<std::string>& data() const;
	std::string file_name() const;
	int line_number() const; 
private:
	int _line;
	std::string _file;
    std::list<std::string> _data;
};
 #endif
