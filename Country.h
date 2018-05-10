/*
	Name: Country_I.h
	Copyright: 2018 Wuhan University of Technology 
					all rights reserved 
	Author: LightersWang
	Date: 19/01/18 17:04
	Description: Class defination of Country. 
*/

#ifndef H_COUNTRY
#define H_COUNTRY

#include <iostream>
using namespace std;
#include <string> 
#include <memory.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "functions.h"
#define SEP 1000				//SEPerate

//变量 
//工件总数 
int J;

//工厂总数 
int F;

//机器总数 
int M;

//每个工厂的机器数 
int *Factory_Machine;

//每个工厂的处理速度
double *Speed;

//每个工件的标准处理时间 
double *Process_Time;

//函数 
double Get_LowerBound()
{
	//按照标准加工时间从大到小给工件排序
	double job_time[J] = {0.0};									//工件编号 - 1	
	double sum_tp = 0.0;									//所有工件总加工时间，以后会用到 
	for(int j = 0; j < J; j++)
	{
		job_time[j] = Process_Time[j];
		sum_tp += job_time[j];
	}
	for (int i = 0; i < J - 1; i++)
    	for (int j = 0; j < J - i - 1; j++)
    		if(job_time[j] < job_time[j + 1])
				Swap<double>(job_time[j],job_time[j + 1]);
				
	//按照加工速度从大到小给工厂排序
	double factory_speed[M];	
	int fs_ind = 0;															
	for(int f = 0; f < F; f++)
	{
		for(int m = 0; m < Factory_Machine[f]; m++)
			factory_speed[fs_ind++] = Speed[f];
	}
	for (int i = 0; i < M - 1; i++)
    	for (int j = 0; j < M - i - 1; j++)
    		if(factory_speed[j] < factory_speed[j + 1])
    			Swap<double>(factory_speed[j],factory_speed[j + 1]);

    //计算 
	double lb[M];
	for(int m = 1; m <= M; m++)
	{
		double sum_p,sum_v;
		sum_p = sum_v = 0.0;
		for(int i = 0; i < m; i++)
		{
			if(m == M)
				sum_p = sum_tp;
			else
				sum_p += job_time[i];
			sum_v += factory_speed[i];
		}
		lb[m - 1] = sum_p / sum_v;
	}
	return Get_Max<double>(lb, M);
} 

void Generate_Example(int fac, int machine_lb, int machine_ub, int job, int pt_lb, int pt_ub)
/*
Input:
@fac        : amount of factories
@machine_lb : low bound of the amount of machine at each factory
@machine_ub : up bound of the amount of machine at each factory
@job        : amount of all the jobs
@pt_lb      : low bound of the job processing time 
@pt_ub      : up bound of the job processing time
*/ 
{
	Speed           = Build_Array<double>(fac);
	Process_Time    = Build_Array<double>(job);
	Factory_Machine = Build_Array<int>(fac);
	//工厂与加工速度 
	F = fac;
	for(int f = 0; f < F; f++)
	{
		Speed[f] =  rand() % 101 + 100;
		Speed[f] /= 100;
	} 
	
	//机器 
	M = 0;								//初始化 
	for(int f = 0; f < F; f++)
	{
		if(machine_lb == machine_ub)
		{
			Factory_Machine[f] = machine_lb;
		}
		else
		{
			Factory_Machine[f] = rand() % (machine_ub - machine_lb + 1) + machine_lb;
		}
		M += Factory_Machine[f];
	}
	
	//工件总数与处理时间  
	J = job;
	for(int j = 0; j < J; j++)
	{
		Process_Time[j] =  rand() % (pt_ub * 100 - pt_lb * 100 + 1) + pt_lb * 100;
		Process_Time[j] /= 100.0;
	}
	
	if(fac == 3 && job == 20)			//如果工厂数为3工件数为20，则为了简便使用论文的算例 
	{
		J = job;
		F = fac;
		M = 9;
		int temp[] = {2,3,4};
		for(int i = 0; i < fac; i++)
			Factory_Machine[i] = temp[i];
		double temp1[] = {1.34,1.69,1.06};
		for(int i = 0; i < fac; i++)
			Speed[i] = temp1[i];
		double temp2[] = {59.53,57.67,60.79,63.91,64.19,62.80,65.65,67.23,66.83,65.95,
				  54.23,68.16,50.26,59.23,58.46,55.20,57.73,52.37,51.24,58.17};
		for(int i = 0; i < job; i++)
			Process_Time[i] = temp2[i];
	}
}

void Output_example_ICA(ofstream& example)
{
	example<<"F : "<<F<<endl;
	example<<"J : "<<J<<endl;
	example<<"M : "<<M<<endl;
	Output_Array<double>(example, Speed, F);
	Output_Array<double>(example, Process_Time, J);
	Output_Array<int>(example, Factory_Machine, F);
}

//类 
class Country_I{
	private:
		bool isCol;													//是否为殖民地 
		double makespan;											//最大处理时间
		int number;													//编号 
		double* pro_time;											//每个工厂的加工时间
		
	public:
		int** matrix;
		int** matrix_sep;											//带分隔符的矩阵（当不使用第二阶段时） 
		int*  len;													//每个工厂有多少工件 
		
		//START & END
		Country_I();
		Country_I(int n, string mthd = "prob");								
		~Country_I();
		
		//GETTER
		virtual double GetMakespan();
		int GetNumber(); 
		double* GetProTime();
		bool isColony();							
		bool isImperialist(); 
		
		//SETTER
		void SetAttribute(string a);
		void SetNumber(int n);
		void SetLen(int* len_temp);
		void SetProTime(double* pt);
		void SetMakespan(double ms);
		
		//FUNCTION
		void Display_sep();
		Country_I operator =(Country_I c1);
		virtual void Calculate(string mthd = "LPT");				//计算makespan						
		virtual void Display(char show_m = 'y',char show_p = 'n');	//展示整个分配情况
};

Country_I::Country_I()
{
	this->makespan = -1;											//标记没计算 
	len = Build_Array<int>(F);
	memset(len, 0, sizeof(len));
	pro_time = Build_Array<double>(F);
	memset(pro_time, 0, sizeof(pro_time));	
	matrix = Build_Matrix<int>(F,J);	
	for(int f = 0; f < F; f++)
		for(int j = 0; j < J; j++)
			matrix[f][j] = 0;
	matrix_sep = Build_Matrix<int>(F,J);	
	for(int f = 0; f < F; f++)
		for(int j = 0; j < J; j++)
			matrix_sep[f][j] = 0;
}

Country_I::Country_I(int n,string mthd)								//解决了每个工厂分配几个工件				
{	
	this->makespan = -1;											//标记没计算 
	len = Build_Array<int>(F);
	for(int f = 0; f < F; f++) 
		len[f] = 0;
	number = n;														//编号赋值 
	matrix = Build_Matrix<int>(F,J);  								//开辟空间（占的空间有点多，可以再优化）
	pro_time = Build_Array<double>(F);
	memset(pro_time, 0, sizeof(pro_time));
	matrix_sep = Build_Matrix<int>(F,J);	
	for(int f = 0; f < F; f++)
		for(int j = 0; j < J; j++)
			matrix_sep[f][j] = 0;

	//初始化
	int job_machine = J / M;										//确定每个机器上最少有几个工件								 
	int job_random = J % M;											//确定有多少个工件参加随机分配
	int assign_temp;												//工厂f随机分配到的工件个数 
	
	//按照标准加工时间从大到小给工件排序
	int job_time[J] = {0};											//工件编号 - 1								
	for(int j = 0; j < J; j++)
	{
		job_time[j] = j;
	} 
	for (int i = 0; i < J - 1; i++)
    	for (int j = 0; j < J - i - 1; j++)
    	{
    		if(Process_Time[job_time[j]] < Process_Time[job_time[j + 1]])
    		{
				Swap<int>(job_time[j],job_time[j + 1]);
			}
		}
		
	//按照加工速度从大到小给工厂排序
	int factory_speed[F];											//工厂编号 - 1								
	for(int f = 0; f < F; f++)
	{
		factory_speed[f] = f;
	}
	for (int i = 0; i < F - 1; i++)
    	for (int j = 0; j < F - i - 1; j++)
    	{
    		if(Speed[factory_speed[j]] < Speed[factory_speed[j + 1]])
    		{
    			Swap<int>(factory_speed[j],factory_speed[j + 1]);
			}
		}

	if(mthd == "prob")
	{
		//按照概率分配工件
		double total_speed = 0.0;
		for(int f = 0; f < F; f++)
		{
			total_speed += Speed[f];
		}
		double p[F];															//按照工厂速度产生的概率
		for(int f = 0; f < F; f++)
		{
			p[f] = (f == 0 ? 0.0 : p[f - 1]) + Speed[f] / total_speed;			//计算概率
		}
		int job_temp = 0;														//记录工件序列的位置
		for(int j = 0; j < J; j++)
		{ 
			double prob = rand() % 1000 / (double)1000;							//产生0到1的随机数 
			//cout<<"prob is : "<<prob<<endl;
			int factory;
			for(int f = 0; f < F; f++)
				if(prob < p[f])
				{
					factory = f;												//确定分配到哪个工厂 	
					break;			
				}
			matrix[factory][len[factory]++] = job_time[job_temp++] + 1;			//分配的时候是真正的编号
		}
		
		//保证所有工厂工件数大于等于机器数
		int max_index = 0;														//记录工件数最多的工厂索引 
		int bad_fac[F], badfac_index = 0;										//记录所有工件数不到机器数的工厂索引 
		for(int f = 0; f < F; f++)
		{
			if(this->len[f] < Factory_Machine[f])
				bad_fac[badfac_index++] = f;
			else if(this->len[f] > this->len[max_index])
				max_index = f;
		} 
		for(int i = 0; i < badfac_index; i++)
		{
			int diff = Factory_Machine[bad_fac[i]] - this->len[bad_fac[i]];		//工件个数差值 
			for(int k = 0; k < diff; k++)
			{
				int elem_temp = 
					TakeAway_Array<int>(matrix[max_index], rand() % len[max_index], len[max_index]--);
				Insert_Array<int>(matrix[bad_fac[i]], elem_temp, len[bad_fac[i]], len[bad_fac[i]]);
				len[bad_fac[i]]++;
			}
		}
	}
	else if(mthd == "cert")
	{		
		//按照工厂机器个数确定每个工厂工件个数
		for(int f = 0; f < F; f++)
		{
			len[f] = job_machine * Factory_Machine[f];
		}
		for(int i = 0; i < job_random; i++)
		{
			len[rand() % (F + 1)]++;
		}
		
		//确定性分配工件
		int job_temp = 0;
		for(int f = 0; f < F; f++)
		{
			int len_temp = 0;
			for(int j = 0; j < len[factory_speed[f]]; j++)
			{
				matrix[factory_speed[f]][len_temp++] = job_time[job_temp++] + 1;		//分配的时候是真正的编号 
			}
		}
	}
}

Country_I::~Country_I()
{
	delete[] pro_time;
	for(int i = 0; i < F; i++)
		delete[] matrix[i];
	delete[] matrix;
}

double Country_I::GetMakespan()
{
	return this->makespan;
}

int Country_I::GetNumber()
{
	return this->number;
}

double* Country_I::GetProTime()
{
	return this->pro_time;
}

bool Country_I::isColony()
{
	return this->isCol;
}

bool Country_I::isImperialist()
{
	return !(this->isCol);
}

void Country_I::SetAttribute(string a)
{
	if(a == "Col" || a == "col")
		isCol = true;
	else if(a == "Imp" || a == "imp")
		isCol = false;
	else
		cout<<"Invalid Country_I Attribute!"<<endl;
}

void Country_I::SetNumber(int n)
{
	this->number = n;
}

Country_I Country_I::operator =(Country_I c1)
{
	this->matrix = c1.matrix;
	this->isCol = c1.isCol;
	this->makespan = c1.makespan;
	for(int f = 0; f < F; f++)
		this->len[f] = c1.len[f];
	return *this;
}

void Country_I::Calculate(string mthd)
{
	//初始化
	double total_time[F][M] = {0};											//总加工时间 = workload 
	for(int f = 0; f < F; f++)
		for(int j = 0; j < J; j++)
			matrix_sep[f][j] = matrix[f][j];
	 
	//分配工件
	for(int f = 0; f < F; f++)
	{ 
		//建立每个工厂的辅助空间
		int **factory = 													//行数：每个工厂有几个机器 
			Build_Matrix<int>(Factory_Machine[f],len[f]);					//列数：每个工厂有几个工件 
		for(int i = 0; i < Factory_Machine[f]; i++)							//初始化 
			for(int j = 0; j < len[f]; j++)
				factory[i][j] = 0;	

		//建立分配位置的辅助空间 
		int *len_temp = Build_Array<int>(len[f]);
		for(int i = 0; i < len[f]; i++)
			len_temp[i] = 0;

		//开始分配 
		bool flag = false;													//标记所有机器都不空闲 
		for(int j = 0; j < len[f]; j++)
		{
			if(mthd == "LPT")
			{
				//LPT
				flag = false;
				for(int m = 0; m < Factory_Machine[f]; m++)
				{
					if(total_time[f][m] == 0)								//如果存在机器空闲 
					{
						flag = true;										//标记有机器空闲 
						factory[m][len_temp[m]++] = matrix_sep[f][j];				//分配工件 
						total_time[f][m] += Process_Time[matrix_sep[f][j] - 1];	//累加时间 
						break;												//直接跳到下一个工件 
					}
				}
				if(flag == false)											//也就是所有机器都不空闲
				{
					int min = 0;											//总加工时间最小的机器的索引
					for(int m = 1; m < Factory_Machine[f]; m++)
					{
						if(total_time[f][m] < total_time[f][min])
							min = m;
					}
					factory[min][len_temp[min]++] = matrix_sep[f][j];				//分配工件
					total_time[f][min] += Process_Time[matrix_sep[f][j] - 1];		//累加时间
				}
			}
			else if(mthd == "random")
			{
				//随机
				int m = rand() % Factory_Machine[f];
				factory[m][len_temp[m]++] = matrix_sep[f][j];
				total_time[f][m] += Process_Time[matrix_sep[f][j] - 1];
			}
		}

		//合成一个编码
		int a = 0;															//记录新合成编码的位置 
		for(int m = 0; m < Factory_Machine[f]; m++)
		{
			for(int j = 0; j < len_temp[m]; j++)
			{
				matrix_sep[f][a++] = factory[m][j];
			}
			if(Factory_Machine[f] - m > 1)									//添加分隔符
			{
				matrix_sep[f][a++] = SEP;
			}
		}

		//释放空间
		Destory_Array<int>(len_temp);
		Destory_Matrix<int>(factory,Factory_Machine[f]);
	}

	//求解makespan
	for(int f = 0; f < F; f++)
	{
		for(int m = 0; m < Factory_Machine[f]; m++)
		{
			pro_time[f] = Get_Max<double>(total_time[f],Factory_Machine[f]) / Speed[f];
		}
	}
	this->makespan = Get_Max<double>(pro_time,F);

//	//测试：打印整个调度
//	for(int f = 0; f < F; f++)
//	{
//		printf("Factory %d : ",f + 1);
//		for(int j = 0; j < len[f] + Factory_Machine[f] - 1; j++)
//		{
//			if(temp[f][j] == SEP)
//				cout<<"*    ";
//			else
//				cout<<temp[f][j]<<"    ";
//		}
//		cout<<endl;
//	}
//	cout<<endl;
}

void Country_I::Display(char show_m, char show_p)
{
	printf("Country %d : \n",number);
	for(int f = 0; f < F; f++)
	{
		printf("Factory %d : ",f + 1);
		for(int j = 0; j < len[f]; j++)
		{
			cout<<matrix[f][j];
			if(matrix[f][j] < 10)
			{
				cout<<"    ";
			}
			else
			{
				cout<<"   ";
			}
		}
		cout<<endl;
	}
	if(show_m == 'y')
	{
		this->Calculate();
		cout<<"Makespan is "<<this->GetMakespan()<<endl;
	}
	if(show_p == 'y')
	{
		cout<<"Prossess time are : ";
		Print_Array<double>(this->pro_time, F);
		cout<<endl;
	}
}

void Country_I::Display_sep()
{
	printf("Country %d : \n",this->GetNumber());
	for(int f = 0; f < F; f++)
	{
		printf("Factory %d : ",f + 1);
		for(int j = 0; j < len[f] + Factory_Machine[f] - 1; j++)
		{
			if(matrix_sep[f][j] == SEP)
				cout<<"*    ";
			else
				cout<<matrix_sep[f][j]<<"    ";
		}
		cout<<endl;
	}
}

void Country_I::SetLen(int* len_temp)
{
	for(int i = 0; i < F; i++)
	{
		this->len[i] = len_temp[i];
	}
}

void Country_I::SetProTime(double* pt)
{
	this->pro_time = pt;
}

void Country_I::SetMakespan(double ms)
{
	this->makespan = ms;
}

class Country_II : public Country_I{
	public:
		int* full_len;													//和len的区别在于加上了分隔符，真正的长度 
		
		//START & END
		Country_II();
		Country_II(Country_I* c, string mthd = "LPT");
		~Country_II();
		
		//FUNCTION
		virtual void Calculate();
		virtual void Display(char show_m = 'y',char show_p = 'n');
		
		//SETTER
		void SetFullLen(int full_temp[]);
};

Country_II::Country_II()
{
	this->SetMakespan(0.0);
	this->matrix = Build_Matrix<int>(F,J);
	memset(matrix, 0, sizeof(matrix)); 
	len = Build_Array<int>(F);
	memset(len, 0, sizeof(len));
	full_len = Build_Array<int>(F);
	memset(full_len, 0, sizeof(full_len));
}

Country_II::Country_II(Country_I* c, string mthd)								//在原有分配方案基础上 
{
	//调度矩阵初始化 
	this->SetMakespan(0.0);
	this->SetNumber(c->GetNumber());											//编号赋值
	this->matrix = Build_Matrix<int>(F,J);
	memset(matrix, 0, sizeof(matrix));
	full_len = Build_Array<int>(F);
	len = Build_Array<int>(F);
	
	//先生成分隔符的位置,后分配工件 
	for(int f = 0; f < F; f++)
	{
		//初始化长度
		int num_sep = Factory_Machine[f] - 1;									//间隔符 = 机器个数 - 1
		this->len[f] = c->len[f];
		this->full_len[f] = this->len[f] + num_sep;								//长度 = 间隔符 + 工件数 
		
		//分random和LPT两种方式
		if(mthd == "random")
		{
			//先把工件编号都复制过来 
			for(int i = 0; i < c->len[f]; i++)
			{
				this->matrix[f][i] = c->matrix[f][i];
			}
			//然后再随机插入分隔符
			for(int s = 0; s < num_sep; s++)
			{
				int assert_idx = rand() % this->len[f];
				Insert_Array<int>(this->matrix[f], SEP, assert_idx, this->len[f] + s);
			} 
		} 
		else if(mthd == "LPT")
		{
			//初始化 
			bool flag = false;												//有机器空闲为真 
			double total_time[F][M] = {0};
			int* machine_ind = Build_Array<int>(Factory_Machine[f]);		//一开始的值为每个机器的初始位置 
			machine_ind[0] = 0;												//第一个机器从0开始 
			int gap = len[f] / Factory_Machine[f];							//每个间隔符之间的理想距离 
			for(int m = 1; m < Factory_Machine[f]; m++)
			{
				machine_ind[m] = machine_ind[m - 1] + gap + 1;				//为每个机器起始的位置
				this->matrix[f][machine_ind[m] - 1] = SEP;					//插入分隔符 
			}

			//分配c中工件矩阵
			for(int j = 0; j < c->len[f]; j++)							 
			{
				flag = false;
				for(int m = 0; m < Factory_Machine[f]; m++)
				{
					if(total_time[f][m] == 0)
					{
						flag = true;
						this->matrix[f][machine_ind[m]++] = c->matrix[f][j];
						total_time[f][m] += Process_Time[c->matrix[f][j] - 1];
						break; 
					}
				}
				if(flag == false)
				{
					int ind = Get_Min_Index<double>(total_time[f], Factory_Machine[f]);
					if(this->matrix[f][machine_ind[ind]] == SEP)			//更保险的方法是记录每一个分隔符的位置 
					{
						Insert_Array(this->matrix[f], c->matrix[f][j], machine_ind[ind]++, full_len[f]);
						for(int i = ind + 1; i < Factory_Machine[f]; i++)
						{
							machine_ind[i]++;								//插入了一个，在插入后面都加一 
						} 
						total_time[f][ind] += Process_Time[c->matrix[f][j] - 1];
					} 
					else
					{
						this->matrix[f][machine_ind[ind]++] = c->matrix[f][j];
						total_time[f][ind] += Process_Time[c->matrix[f][j] - 1];
					} 
				}
			}

			//最后检查有无0并删除
			for(int j = 0; j < c->len[f]; j++) 							 
			{
				if(this->matrix[f][j] == 0)
					int drop = TakeAway_Array<int>(this->matrix[f], j, full_len[f]);
			}
			
			//临时矩阵销毁掉
			Destory_Array<int>(machine_ind);							 
		}
	}
}

Country_II::~Country_II()
{
	for(int i = 0; i < F; i++)
		delete[] matrix[i];
	delete[] matrix;
} 

void Country_II::Calculate()
{
	//算每个工厂每个机器上的累加时间 
	double total_time[F][M] = {0};
	for(int f = 0; f < F; f++)
	{
		int time_ind = 0;
		for(int m = 0; m < Factory_Machine[f]; m++)
		{
			if(time_ind >= this->full_len[f])										//如果在范围外直接跳过 
				break; 
			else
			{
				while(this->matrix[f][time_ind] != SEP && time_ind < this->full_len[f])
					total_time[f][m] += Process_Time[matrix[f][time_ind++] - 1];
				if(this->matrix[f][time_ind] == SEP)								//遇到SEP跳过 
					time_ind++;
			}
		}
	}
	
	//取两次最大 
	double* temp_pt = Build_Array<double>(F);
	for(int f = 0; f < F; f++)
	{
		temp_pt[f] = Get_Max<double>(total_time[f],Factory_Machine[f]) / Speed[f];
	}
	this->SetProTime(temp_pt);
	this->SetMakespan(Get_Max<double>(temp_pt,F));
}

void Country_II::Display(char show_m, char show_p)
{
	printf("Country %d : \n",this->GetNumber());
	for(int f = 0; f < F; f++)
	{
		printf("Factory %d : ",f + 1);
		for(int j = 0; j < full_len[f]; j++)
		{
			if(matrix[f][j] == SEP)
				cout<<"*    ";
			else
				cout<<matrix[f][j]<<"    ";
		}
		cout<<endl;
	}
	if(show_m == 'y')
	{
		this->Calculate();
		cout<<"Makespan is "<<this->GetMakespan()<<endl;
	}
	if(show_p == 'y')
	{
		cout<<"Prossess time are : ";
		Print_Array<double>(this->GetProTime(), F);
		cout<<endl;
	}
}
void Country_II::SetFullLen(int full_temp[])
{
	for(int f = 0; f < F; f++)
	{
		this->full_len[f] = full_temp[f];
	}
}
#endif
