#pragma once

#include <string>
#include <iostream>

/**
*	Writer interface
*/
class IWriter
{
public:

	IWriter() = default;
	virtual ~IWriter() = default;

	virtual void Print(const std::string& s) = 0; 
	virtual void SetTime(long long time) = 0;
};

/**
*	Write in the std::cout
*/
class CoutWriter : public IWriter
{
public:

	CoutWriter() = default;
	~CoutWriter() = default;

	void Print(const std::string& s) override
	{
		std::cout << s;
	}

	void SetTime(long long) override {}
};

/**
*	Write in the file
*/
class FileWriter : public IWriter
{
public:

	FileWriter() = default;
	~FileWriter() = default;

	void Print(const std::string& s)  override
	{
		fileStream_.open("bulk" + std::to_string(time_) + "_" + std::to_string(postfix++) + ".log");

		fileStream_ << s;

		fileStream_.close();
	}

	void SetTime(long long time) override
	{
		if (time != time_)
		{
			time_ = time;
			postfix = 0;
		}
	}

private:

	std::ofstream fileStream_;
	long long time_ = 0;
	int postfix;
};