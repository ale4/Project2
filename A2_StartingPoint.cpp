#include <iostream>
#include <thread>
#include <cmath>
#include <algorithm>


using namespace std;

void merge(int* array, unsigned int left, unsigned int mid, unsigned int right);

void sequential_merge_sort(int* array, unsigned int left, unsigned int right) {
	if (left < right) {
		unsigned int mid = (left + right) / 2;
		sequential_merge_sort(array, left, mid);
		sequential_merge_sort(array, mid + 1, right);
		merge(array, left, mid, right);
	}
}

void merge(int* array, unsigned int left, unsigned int mid, unsigned int right) 
{
	unsigned int num_left = mid - left + 1; // number of elements in left subarray
	unsigned int num_right = right - mid; // number of elements in right subarray

	// copy data into temporary left and right subarrays to be merged
	int* array_left = new int[num_left];
	int* array_right = new int[num_right];
	copy(&array[left], &array[mid + 1], array_left);
	copy(&array[mid + 1], &array[right + 1], array_right);

	// initialize indices for array_left, array_right, and input subarrays
	unsigned int index_left = 0;    // index to get elements from array_left
	unsigned int index_right = 0;    // index to get elements from array_right
	unsigned int index_insert = left; // index to insert elements into input array

	// merge temporary subarrays into original input array
	while ((index_left < num_left) || (index_right < num_right)) {
		if ((index_left < num_left) && (index_right < num_right)) {
			if (array_left[index_left] <= array_right[index_right]) {
				array[index_insert] = array_left[index_left];
				index_left++;
			}
			else {
				array[index_insert] = array_right[index_right];
				index_right++;
			}
		}
		// copy any remain elements of array_left into array
		else if (index_left < num_left) {
			array[index_insert] = array_left[index_left];
			index_left += 1;
		}
		// copy any remain elements of array_right into array
		else if (index_right < num_right) {
			array[index_insert] = array_right[index_right];
			index_right += 1;
		}
		index_insert++;
	}
	//deallocate to prevent memory leak from occuring
	delete[] array_left;
	delete[] array_right;
}
/* separate a computation into smaller pieces that can run independently,
   then execute those pieces concurrently on multiple cores
*/

void Parallel_Merge_Sort(int* array, unsigned int left, unsigned int right, unsigned int depth, int cutoff)
{
	if (left >= right) {
		return;
	}

	unsigned int length = right - left + 1;

	// Use the existing sequential algorithm for smaller tasks.
	if (depth == 0 || length <= cutoff) {
		sequential_merge_sort(array, left, right);
		return;
	}

	unsigned int mid = (left + right) / 2;

	// sort the left half 
	std::thread worker([&] { Parallel_Merge_Sort(array, left, mid, depth - 1, cutoff); });

	// sort the right half
	Parallel_Merge_Sort(array, mid + 1, right, depth - 1, cutoff);

	// both halves must finish before merging
	worker.join();

	// merge the two sorted halves using the existing merge function
	merge(array, left, mid, right);
}


int main() {
	const int NUM_EVAL_RUNS = 100;
	const int N = 10;

	int* original_array = new int[N];
	int* result = new int[N];
	cout << "original array :";
	int i;
	for (i = 0; i < N; i++) {
		original_array[i] = rand();
		cout << "[" << original_array[i] << "] ";
	}
	cout << endl;

	copy(&original_array[0], &original_array[N], result);
	sequential_merge_sort(result, 0, N-1); 

	//Execute the sort NUM_EVAL_RUNS to get an average of execution time
	for (int i = 0; i < NUM_EVAL_RUNS; i++) {
		copy(&original_array[0], &original_array[N], result);
		sequential_merge_sort(result, 0, N-1);
	}
	cout << "sequential sort :";
	//Comment this out when dealing with very large arrays for timing calculations
	//Uncomment this for the N=10 screenshot that needs to be taken
	for (int i = 0; i < N; i++) {
		cout << "[" << result[i] << "] ";
	}
	cout << endl;
	//Execute the parallel sort NUM_EVAL_RUNS to get an average of execution time
	for (int i = 0; i < NUM_EVAL_RUNS; i++) {
		copy(&original_array[0], &original_array[N], result);
		Parallel_Merge_Sort(result, 0, N-1, depth, 10);
	}

	//Comment this out when dealing with very large arrays for timing calculations
	cout << "Parallel sort :";
	for (i = 0; i < N; i++) 
	{
		cout << "[" << result[i] << "] ";
	}
	cout << endl;
}