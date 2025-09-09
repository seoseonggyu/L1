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
		if (!_running.compare_exchange_strong(expected, true)) return;
		_worker = thread([this] { this->Run(); });
	}

	void Push(String msg) 
	{
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
			for (auto& it : items) 
			{
				_consoleLogger.WriteStdOut(it.color, L"%ls\n", it.text.c_str());
			}
		}
	}

private:
	Atomic<bool>				_running{ false };
	thread						_worker;

	ConcuurentQueue<LogItem>	_logs;
	ConsoleLog					_consoleLogger;
};

