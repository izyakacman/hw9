#include "command.h"

#include <chrono>

/**
*	@brief	Process static command
*	@return true - change type of command
* 
*/
bool StaticCommand::ProcessCommand(const std::string& cmd)
{
	if (cmd != EndOfFileString)
	{
		if (cmd == "{")
		{
			PrintPool();
			return true;
		}

		PushPool(cmd);

		if (GetPoolSize() == count_)
		{
			PrintPool();
		}
	}
	else
	{
		PrintPool();
	}

	return false;
}

/**
*	@brief	Process dynamic command
*	@return true - change type of command
*
*/
bool DynamicCommand::ProcessCommand(const std::string& cmd)
{
	if (cmd == EndOfFileString)
		return false;

	if (cmd == "{")
	{
		++openBraceCount_;
	}
	else
		if (cmd == "}")
		{
			if (openBraceCount_ == 0)
			{
				PrintPool();
				return true;
			}

			--openBraceCount_;
		}
		else
		{
			PushPool(cmd);
		}

	return false;
}

ICommand::ICommand(size_t count) : count_{ count }
{
	writersPool_.push_back(IWriterPtr{ new CoutWriter });
	writersPool_.push_back(IWriterPtr{ new FileWriter });
}

/**
*	Add command int the block
*/
void ICommand::PushPool(const std::string& s)
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
void ICommand::PrintString(const std::string& s) const
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
void ICommand::PrintPool()
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
