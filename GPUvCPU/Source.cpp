/*
	Wesley McClintock
	6/14/2014
	This is just a redo of a little demo I made to show the differences between the CPU and GPU
*/
#include <iostream>
#include <amp.h>
#include <amp_math.h>
#include <time.h>
#include <math.h>

clock_t timedCPURoots(float squares[], unsigned size)
{
	clock_t start;
	start = clock();

	for (int i = 0; i < size; i++)
	{
		std::sqrtf(squares[i]);
	}

	return  clock() - start;
}

clock_t timedGPURoots(float squares[], unsigned size)
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


	float *squares;
	try
	{
		squares = new float[sizeOfArray];
	}
	catch (std::bad_alloc)
	{
		std::cout << "Could not allocate the memory (you may want to restart)" << std::endl;
		return 0;
	}

	for (int i = 0; i < sizeOfArray; i++)
	{
		squares[i] = i + 1;
	}
	
	std::cout << "CPU took: " << timedCPURoots(squares, sizeOfArray) 
		<< "ms to calculate the root of " << sizeOfArray << " numbers" << std::endl;
	std::cout << "GPU took: " << timedGPURoots(squares, sizeOfArray)
		<< "ms to calculate the root of " << sizeOfArray << " numbers" << std::endl;

	//delete, delete, delete!
	delete[] squares;

	std::system("pause");
	return 0;
}