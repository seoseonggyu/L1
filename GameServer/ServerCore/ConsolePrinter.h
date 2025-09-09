#pragma once

#include "ConcuurentQueue.h"
#include "ConsoleLog.h"

struct LogItem 
{
	String		text;
	Color		color{ Color::YELLOW };
};

class ConsolePrinter
{
public:
	ConsolePrinter() {}
	~ConsolePrinter(){}

	void Start()
	{
		bool expected = false;
		if (!running_.compare_exchange_strong(expected, true)) return;
		worker_ = std::thread([this] { this->Run(); });
	}

	void Push(String msg) {
		LogItem log;
		log.text = msg;

		_logs.Push(log);
	}

private:
	void Run() 
	{
		while (true)
		{
			Vector<LogItem> items;
			_logs.PopAll(items);
			for(int32 i = 0; i < items.size(); ++i)
			{
				wcout << items[i].text << endl;
			}

			for (auto& it : items) 
			{
				_consoleLogger.WriteStdOut(it.color, L"%s", it.text.c_str());
			}
		}
	}

private:
	Atomic<bool> running_{ false };
	thread worker_;

	ConcuurentQueue<LogItem> _logs;
	ConsoleLog _consoleLogger;
};

