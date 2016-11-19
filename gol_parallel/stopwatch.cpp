#include "stopwatch.h"

stopwatch::stopwatch()
	:m_count(0)
{
	
}


stopwatch::~stopwatch()
{
}

std::chrono::high_resolution_clock::time_point stopwatch::now()
{
	return std::chrono::high_resolution_clock::now();
}

int stopwatch::start()
{
	m_start.push_back(std::chrono::high_resolution_clock::now());
	return m_count;
}

int stopwatch::stop()
{
	m_stop.push_back(std::chrono::high_resolution_clock::now());
	m_count++;
	return m_count - 1;
}

double stopwatch::getDuration(int count) const
{
	if (count >= m_stop.size() || count >= m_start.size())
	{
		return 66.6;
	}
	std::chrono::duration<double> d = std::chrono::duration_cast<std::chrono::duration<double>>(m_stop[count] - m_start[count]);
	return d.count();
}

double stopwatch::getFullDuration()
{
	std::chrono::duration<double> d = std::chrono::duration_cast<std::chrono::duration<double>>(m_stop[m_stop.size()-1] - m_start[0]);
	return d.count();
}

std::string stopwatch::getFullDurationString()
{
	return getFormatted(getFullDuration());
}

std::string stopwatch::getDurationString(int count)
{
	double d = getDuration(count);
	return getFormatted(d);
}

std::string stopwatch::getFormatted(double nmbr)
{
	std::stringstream test;	
	std::chrono::duration<double> diff(nmbr);

	auto hours = std::chrono::duration_cast<std::chrono::hours>(diff);
	diff -= hours;

	auto mins = std::chrono::duration_cast<std::chrono::minutes>(diff);
	diff -= mins;

	auto secs = std::chrono::duration_cast<std::chrono::seconds>(diff);
	diff -= secs;

	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

	test << std::setfill('0');
	test << std::setw(2) << hours.count() << ':'
		<< std::setw(2) << mins.count() << ':'
		<< std::setw(2) << secs.count() << '.'
		<< std::setw(3) << millis.count();

	return test.str();
}
