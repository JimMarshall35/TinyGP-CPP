#pragma once
#include <chrono>
#include <iostream>
/*
timer.h
Jim Marshall 2021
TIMER_START
(code to time)
TIMER_STOP
^
prints out the average time taken for code
to run every time TIMER_SAMPLE_SIZE no of samples
accumulate
*/
#define TIMER_SAMPLE_SIZE 100000
#define TIMER_START(name) std::string timer_name = name; static long int timer_n = 0; static double   timer_accumulated_total = 0; std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
#define TIMER_STOP std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0); timer_accumulated_total += time_span.count(); timer_n++;if (timer_n >= TIMER_SAMPLE_SIZE) { std::cout << timer_name << " done in " << (timer_accumulated_total / timer_n) * 1000 << " ms" << std::endl; timer_n = 0; timer_accumulated_total = 0;}