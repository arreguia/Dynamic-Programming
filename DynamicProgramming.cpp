#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

const bool DEBUG_MODE = true;

bool readFile(string file, int &N, int &M, int &T, int * &keysGiven, int * &ballLocations);
void printInfo(int &N, int &M, int &T, int *keysGiven, int *ballLocations);
int* duplicateArray(int *arr, int size);
void sortArray(int *arr, int size);
int min(int x, int y);

void doBruteForceSolution(int &N, int &M, int &T, int *keysGiven, int *ballLocations);
int solveKeyProblem(string s, int numKeys, int *givenKeys, int numBalls, int *ballLocations);
int doDPSolution(int &N, int &M, int &T, int *keysGiven, int *ballLocations);

int main()
{
	int N, M, T, *keysGiven = NULL, *ballLocations = NULL;
	string file = "mydata.txt";

	//cout << "Enter a file to read from: ";
	//cin >> file;

	bool canRead = readFile(file, N, M, T, keysGiven, ballLocations);
	if (!canRead)
		return 1;

	if (DEBUG_MODE)
		printInfo(N, M, T, keysGiven, ballLocations);

	int *keys = duplicateArray(keysGiven, M), *balls = duplicateArray(ballLocations, T);

	cout << "Dynamic Programming Answer: " << doDPSolution(N, M, T, keysGiven, ballLocations) << endl;

	delete[] keysGiven;
	delete[] ballLocations;

	return 0;
}

bool readFile(string file, int &N, int &M, int &T, int * &keysGiven, int * &ballLocations)
{
	
   	ifstream input;
	input.open(file.c_str());

	if (input.fail())
	{
		cout << "ERROR!" << endl;
		return false;
	}

	input >> N >> M >> T;

	keysGiven = new int[M];
	ballLocations = new int[T];

	for (int i = 0; i < M; i++)
		input >> keysGiven[i];
	for (int i = 0; i < T; i++)
		input >> ballLocations[i];

	return true;
}

void printInfo(int &N, int &M, int &T, int *keysGiven, int *ballLocations)
{
	cout << "N: " << N << endl;
	cout << "M: " << M << endl;
	cout << "T: " << T << endl;

	cout << "KEYS: ";
	for (int i = 0; i < M; i++)
		cout << keysGiven[i] << " ";
	cout << endl;

	cout << "BALL: ";
	for (int i = 0; i < T; i++)
		cout << ballLocations[i] << " ";
	cout << endl;
}

int* duplicateArray(int *arr, int size)
{
	int *newArr = new int[size];
	for (int i = 0; i < size; i++)
		newArr[i] = arr[i];
	return newArr;
}

void sortArray(int *arr, int size)
{
	for (int i = 1; i < size; i++)
	{
		for (int j = i; j > 0; j--)
		{
			if (arr[j] < arr[j - 1])
			{
				int temp = arr[j];
				arr[j] = arr[j - 1];
				arr[j - 1] = temp;
			}
		}
	}
}



int solveKeyProblem(string s, int numKeys, int *givenKeys, int numBalls, int *ballLocations)
{
	if (s.length() == numKeys)
	{
		int *key = new int[numKeys + 2], j = 1, minSum = 0, *pos = new int[numKeys + 2];
		pos[0] = key[0] = 600;
		for (unsigned int i = 0; i < s.length(); i++)
			if (s[i] == '1')
			{
				pos[j] = key[j] = givenKeys[i];
				j++;
			}
		pos[j] = key[j] = 600;
		j = 0;

		for (int i = 0; i < numBalls; i++)
		{
			if (abs(key[j] - ballLocations[i]) < abs(key[j + 1] - ballLocations[i]))
			{
				minSum += abs(pos[j] - ballLocations[i]);
				pos[j] = ballLocations[i];
			}
			else
			{
				minSum += abs(pos[j + 1] - ballLocations[i]) + 1;
				pos[j + 1] = max(ballLocations[i], key[j + 1]);
				while (ballLocations[i + 1] <= key[j + 1])
					i++;
				j++;
			}
		}
		delete[] key;
		delete[] pos;
		return minSum;
	}
	else
	{
		return min(solveKeyProblem(s + "0", numKeys, givenKeys, numBalls, ballLocations),
				   solveKeyProblem(s + "1", numKeys, givenKeys, numBalls, ballLocations));
	}
}

int doDPSolution(int &N, int &M, int &T, int *keysGiven, int *ballLocations)
{
	int *dist = new int[N + 1], *used = new int[N + 1];
	bool *isIn = new bool[N + 1];

	for (int i = 0; i <= N; i++)
	{
		dist[i] = 600;
		used[i] = 0;
		isIn[i] = false;
	}
	for (int i = 0; i < T; i++)
		isIn[ballLocations[i]] = true;

	int minSum = 0;
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j <= M; j++)
		{
			if (i < j && i >= 0)
				continue;
			if (i >= 0 && j == 0)
			{
				int k = keysGiven[i], l = keysGiven[i] + 1, maxVal;

				maxVal = 0;
				for (k; k > 0 && keysGiven[i] == used[k]; k--)
				{
					if (isIn[k])
						maxVal = dist[k];
					dist[k] = 0;
				}
				minSum += maxVal;

				maxVal = 0;
				for (l; l <= N && keysGiven[i] == used[l]; l++)
				{
					if (isIn[l])
						maxVal = dist[l];
					dist[l] = 0;
				}
				minSum += maxVal;

				bool changed = false;
			   for (int m = k; m > 0 && k - m + 1 < dist[m]; m--)
				{
					used[m] = keysGiven[i];
					dist[m] = min(k - m + 1, dist[m]);
					changed = true;
				}

				for (int m = l; m <= N && m - l + 1 < dist[m]; m++)
				{
					used[m] = keysGiven[i];
					dist[m] = min(m - l + 1, dist[m]);
					changed = true;
				}
				if (changed)
					i--;
			}
			else
			{
				for (int k = keysGiven[j]; k > 0; k--)
				{
					if (min(keysGiven[j] - k + 1, dist[k]) == (keysGiven[j] - k + 1))
						used[k] = keysGiven[j];
					dist[k] = min(keysGiven[j] - k + 1, dist[k]);
				}
				for (int k = keysGiven[j]; k <= N; k++)
				{
					if (min(k - keysGiven[j] + 1, dist[k]) == (k - keysGiven[j] + 1))
						used[k] = keysGiven[j];
					dist[k] = min(k - keysGiven[j] + 1, dist[k]);
				}
			}  
		}
	}

	return minSum;
}

int min(int x, int y)
{
	return (x < y ? x : y);
}
