#include "utils/log.hpp"
#if 0 
Log::Log(const char *msg) {
   auto start = std::chrono::system_clock::now();
   auto end = std::chrono::system_clock::now();

   std::chrono::duration<double> elapsed_seconds = end - start;
   std::time_t end_time = std::chrono::system_clock::to_time_t(end);

   std::cout << "finished computation at " << std::ctime(&end_time) << "endtime" << &end_time << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
}

Log::Log(const char *msg) {
   auto start = std::chrono::system_clock::now();
   auto end = std::chrono::system_clock::now();

   std::chrono::duration<double> elapsed_seconds = end - start;
   std::time_t end_time = std::chrono::system_clock::to_time_t(end);

   std::cout << "finished computation at " << std::ctime(&end_time) << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
}

Timer::Timer() { _start = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); }

Timer::~Timer() {
   _end = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   std::cout << "[Timer]: " << _end - _start;
}

#endif
