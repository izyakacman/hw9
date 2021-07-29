#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include "writer.h"

class ICommandsProcessor;
constexpr auto EndOfFileString = "eof";

using IWriterPtr = std::unique_ptr<IWriter>;

/**
*	ICommandsProcessor interface
*/
class ICommandsProcessor
{
public:

	explicit ICommandsProcessor(size_t count);
	virtual ~ICommandsProcessor() = default;

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
*	Static commands processor
*/
class StaticCommandsProcessor : public ICommandsProcessor
{
public:

	explicit StaticCommandsProcessor(size_t count) : ICommandsProcessor(count) {}

	bool ProcessCommand(const std::string& cmd) override;
};

/**
*	Dynamic command
*/
class DynamicCommandsProcessor : public ICommandsProcessor
{
public:

	DynamicCommandsProcessor() : ICommandsProcessor(0) {}

	bool ProcessCommand(const std::string& cmd) override;

private:

	size_t openBraceCount_ = 0;
};
