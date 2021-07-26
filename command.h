#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include "writer.h"

class Command;
constexpr auto EndOfFileString = "eof";

using IWriterPtr = std::unique_ptr<IWriter>;

/**
*	Command interface
*/
class ICommand
{
public:

	explicit ICommand(size_t count);
	virtual ~ICommand() = default;

	void PushPool(const std::string& s);

	size_t GetPoolSize()
	{
		return pool_.size();
	}

	void PrintPool();

	void PrintString(const std::string& s) const;

	virtual bool ProcessCommand(const std::string& cmd) = 0;

protected:

	const size_t count_;

private:
	
	long long firstCmdTime_ = 0;
	std::vector<std::string> pool_;
	std::vector<IWriterPtr> writersPool_;
};

/**
*	Static command
*/
class StaticCommand : public ICommand
{
public:

	explicit StaticCommand(size_t count) : ICommand(count) {}

	bool ProcessCommand(const std::string& cmd) override;
};

/**
*	Dynamic command
*/
class DynamicCommand : public ICommand
{
public:

	DynamicCommand() : ICommand(0) {}

	bool ProcessCommand(const std::string& cmd) override;

private:

	size_t openBraceCount_ = 0;
};
