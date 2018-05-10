/*
	Name: function.h
	Copyright: 2018 Wuhan University of Technology 
					all rights reserved
	Author: LightersWang
	Date: 23/01/18 11:42
	Description: Defination of common function.
*/

#ifndef H_FUNCTION
#define H_FUNCTION
#include <iostream>
using namespace std;
#include <fstream>
#include <time.h>
#include <sstream>

//常用函数
template<class T>
void Swap(T &elem1,T &elem2)
{
	T temp;
	temp = elem1;
	elem1 = elem2;
	elem2 = temp;
}

template<class T>
T Get_Min(T array[],int length)
{
	T min = array[0];
	for(int i = 1; i < length; i++)
	{
		if(array[i] < min)
			min = array[i];
	}
	return min;
} 

template<class T>
int Get_Min_Index(T array[],int length)
{
	int min = 0;
	for(int i = 1; i < length; i++)
	{
		if(array[i] < array[min])
			min = i;
	}
	return min;
} 

template<class T>
T Get_Max(T array[],int length)
{
	T max = array[0];
	for(int i = 1; i < length; i++)
	{
		if(array[i] > max)
			max = array[i];
	}
	return max;
} 

template<class T>
int Get_Max_Index(T array[],int length)
{
	int max = 0;
	for(int i = 1; i < length; i++)
	{
		if(array[i] > array[max])
			max = i;
	}
	return max;
} 

template<class T>
T** Build_Matrix(int row, int col)
{
	T **p = new T* [row];									
	for(int m = 0; m < row; m++)
		p[m] = new T[col];
	return p;
}

template<class T>
void Destory_Matrix(T **p, int row)
{
	for(int i = 0; i < row; i++)
		delete[] p[i];
	delete[] p;
}

template<class T>
void Print_Matrix(T **p, int row, int col)
{
	for(int f = 0; f < row; f++){
		for(int j = 0; j < col; j++){
			cout<<p[f][j]<<"    ";
		}
		cout<<endl;
	}
}

template<class T>
void Output_Matrix(ofstream& file, T **p, int row, int col)
{
	for(int f = 0; f < row; f++){
		for(int j = 0; j < col; j++){
			file<<p[f][j]<<"    ";
		}
		file<<endl;
	}
}

template<class T>
T* Build_Array(int length)
{
	T *p = new T[length];
	return p;
}

template<class T>
void Destory_Array(T *p)
{
	delete[] p;
}

template<class T>
void Print_Array(T r[], int n)
{
	for(int i = 0; i < n; i++)
	{
		cout<<r[i]<<"  ";
	}
	cout<<endl;
}

template<class T>
void Output_Array(ofstream& file, T r[], int n)
{
	for(int i = 0; i < n; i++)
	{
		file<<r[i]<<"  ";
	}
	file<<endl;
}

template<class T>
void Insert_Array(T r[], T elem, int p, int l)
//数组、插入的元素、插入的位置（索引）、数组长度
{
	for(int x = l - 1; x >= p; x--)
		r[x + 1] = r[x];
	r[p] = elem;
} 

template<class T>
T TakeAway_Array(T r[], int p, int l)
//数组、拿走的位置（索引）、数组长度
{
	T temp = r[p];
	for(int x = p; x < l; x++)
		r[x] = r[x + 1];
	return temp;
}

void Generate_2random(int &n1, int &n2, int b1, int b2, int a1 = 0, int a2 = 0)			
{
	if(b1 <= 0 || b2 <= 0)
		throw("Boundray mistake!");
	n1 = rand() % b1 + a1;
	do
	{
		n2 = rand() % b2 + a2;
	}
	while(n1 == n2);
}

int Generate_difrdm(int cpr, int b1, int a1 = 0)			
{
	int n1;
	if(b1 <= 0)
		throw("Boundray mistake!");
	do
	{
		n1 = rand() % b1 + a1;
	}
	while(n1 == cpr);
	return n1;
}

string Get_Time()
{
    time_t timep;
    time (&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );
    return tmp;
}

void int2str(const int &int_temp,string &string_temp)  
{  
	stringstream stream;  
    stream<<int_temp;  
    string_temp=stream.str();   //此处也可以用 stream>>string_temp  
}
#endif
