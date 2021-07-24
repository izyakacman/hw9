#include "command.h"

#include <chrono>

/**
*	Process command int the static mode
*/
ICommandHandlerPtr StaticCommandHandler::ProcessCommand(Command* cmd, const std::string& s, bool& exit)
{
	exit = true;

	if (s != EndOfFileString)
	{
		exit = false;

		if (s == "{")
		{
			cmd->PrintPool();
			return ICommandHandlerPtr{ new DynamicCommandHandler(count_) };
		}

		cmd->PushPool(s);

		if (cmd->GetPoolSize() == count_)
		{
			cmd->PrintPool();
		}
	}
	else
	{
		cmd->PrintPool();
	}

	return nullptr;
}

/**
*	Process command int the dynamic mode
*/
ICommandHandlerPtr DynamicCommandHandler::ProcessCommand(Command* cmd, const std::string& s, bool& exit)
{
	exit = true;

	if(s == EndOfFileString)
		return nullptr;

	exit = false;

	if (s == "{")
	{
		++openBraceCount_;
	}
	else
	if (s == "}")
	{
		if (openBraceCount_ == 0)
		{
			cmd->PrintPool();
			return ICommandHandlerPtr{ new StaticCommandHandler(count_) };
		}

		--openBraceCount_;
	}
	else
	{
		cmd->PushPool(s);
	}

	return nullptr;
}

Command::Command(size_t count) : count_{ count }
{
	handler_ = ICommandHandlerPtr{ new StaticCommandHandler(count_) };

	writersPool_.push_back(IWriterPtr{ new CoutWriter });
	writersPool_.push_back(IWriterPtr{ new FileWriter });
}

/**
*	Add command int the block
*/
void Command::PushPool(const std::string& s)
{
	if (pool_.size() == 0)
	{
		firstCmdTime_ = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	pool_.push_back(s);
}

/**
*	Output string
*/
void Command::PrintString(const std::string& s) const
{
	for (auto& writerPtr : writersPool_)
	{
		writerPtr->SetTime(firstCmdTime_);
		writerPtr->Print(s);
	}
}

/**
*	Output block of commands
*/
void Command::PrintPool()
{
	CoutWriter coutWriter;

	if (pool_.size())
	{
		std::string s{ "bulk: " };

		for (size_t i = 0; i < pool_.size(); ++i)
		{
			s += pool_[i];

			if (i != pool_.size() - 1)
			{
				s += ", ";
			}
		}
		s += "\n";
		PrintString(s);
		pool_.clear();
	}
}
