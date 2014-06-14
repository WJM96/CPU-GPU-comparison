/*
	Wesley McClintock
	6/14/2014
	This is just a redo of a little demo I made to show the differences between the CPU and GPU

	NOTE: C++ amp does not like release mode, it is (probably) a TDR.
	see: http://blogs.msdn.com/b/nativeconcurrency/archive/2012/03/07/handling-tdrs-in-c-amp.aspx
*/
#include <iostream>
#include <amp.h>
#include <amp_math.h>
#include <time.h>
#include <math.h>
#include <fstream>

typedef float dataType;

clock_t timedCPURoots(dataType squares[], unsigned size)
{
	clock_t start;
	start = clock();

	for (int i = 0; i < size; i++)
	{
		std::sqrt(i);
	}

	return  clock() - start;
}

clock_t timedGPURoots(dataType squares[], unsigned size)
{
	concurrency::array_view<float, 1> av(size, squares);

	clock_t start = clock();

	concurrency::parallel_for_each(
		av.extent,
		[=](concurrency::index<1> idx) restrict(amp)
	{
		
		concurrency::fast_math::sqrt(av[idx]);
	}
	);

	return clock() - start;
}


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Takes one argument: how many numbers to calculate the square root of." << std::endl;
		return 0;
	}

	int sizeOfArray = atoi(argv[1]);
	if (sizeOfArray < 1)
	{
		std::cout << "Invalid number!" << std::endl;
		return 0;
	}

	clock_t *gpuTimes;
	clock_t *cpuTimes;
	dataType *squares;

	std::cout << "allocating memory..." << std::endl;
	try
	{
		squares = new dataType[sizeOfArray];
		gpuTimes = new clock_t[sizeOfArray];
		cpuTimes = new clock_t[sizeOfArray];
	}
	catch (std::bad_alloc)
	{
		std::cout << "Could not allocate the memory (you may want to restart)" << std::endl;
		return 0;
	}
	std::cout << "success!" << std::endl;

	for (int i = 0; i < sizeOfArray; i++)
	{
		squares[i] = i + 1;
	}

	for (int i = 0; i < sizeOfArray; i++)
	{
		cpuTimes[i] = 0;//timedCPURoots(squares, i + 1);
		gpuTimes[i] = timedGPURoots(squares, i + 1);
		if (i % 1000 == 0)
		{
			std::cout << i << std::endl;
		}
	}

	std::ofstream ofs("gpuvcpu.bin");
	ofs.write((char*)gpuTimes, sizeof(gpuTimes[0]) * sizeOfArray);
	ofs.write((char*)cpuTimes, sizeof(cpuTimes[0]) * sizeOfArray);
	ofs.close();
	//delete, delete, delete!
	delete[] squares;
	delete[] cpuTimes;
	delete[] gpuTimes;

	std::system("pause");
	return 0;
}