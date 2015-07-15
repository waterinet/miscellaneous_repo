#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

class Preprocessor
{
public:
	typedef std::list<std::string> TargetLine;
	Preprocessor(const std::list<SrcLine>&);
	~Preprocessor();
	void process();
	const std::list<SrcLine>& source_data() const;
	const std::list<TargetLine>& target_data() const;
private:
	const std::list<SrcLine>& _source;
	std::list<TargetLine> _target;
};
#endif
