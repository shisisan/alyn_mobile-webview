#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <sys/syscall.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class ThreadOptimizer {
private:
	std::mutex m_mutex;
	std::vector<pid_t> m_threads;

	static void setThreadAffinityMask(pid_t tid, uint32_t mask)
	{
		syscall(__NR_sched_setaffinity, tid, sizeof(mask), &mask);
	}

	[[noreturn]] void routine()
	{
		while (true) {
			m_mutex.lock();
			for (auto& i : m_threads) {
				uint32_t mask = 0xff;
				setThreadAffinityMask(i, mask);
			}
			m_mutex.unlock();

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

public:
	ThreadOptimizer()
	{
		std::thread(&ThreadOptimizer::routine, this).detach();
	}

	~ThreadOptimizer() = default;

	void pushThread(pid_t tid)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_threads.push_back(tid);
	}
};
