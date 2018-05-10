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

//���� 
//�������� 
int J;

//�������� 
int F;

//�������� 
int M;

//ÿ�������Ļ����� 
int *Factory_Machine;

//ÿ�������Ĵ����ٶ�
double *Speed;

//ÿ�������ı�׼����ʱ�� 
double *Process_Time;

//���� 
double Get_LowerBound()
{
	//���ձ�׼�ӹ�ʱ��Ӵ�С����������
	double job_time[J] = {0.0};									//������� - 1	
	double sum_tp = 0.0;									//���й����ܼӹ�ʱ�䣬�Ժ���õ� 
	for(int j = 0; j < J; j++)
	{
		job_time[j] = Process_Time[j];
		sum_tp += job_time[j];
	}
	for (int i = 0; i < J - 1; i++)
    	for (int j = 0; j < J - i - 1; j++)
    		if(job_time[j] < job_time[j + 1])
				Swap<double>(job_time[j],job_time[j + 1]);
				
	//���ռӹ��ٶȴӴ�С����������
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

    //���� 
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
	//������ӹ��ٶ� 
	F = fac;
	for(int f = 0; f < F; f++)
	{
		Speed[f] =  rand() % 101 + 100;
		Speed[f] /= 100;
	} 
	
	//���� 
	M = 0;								//��ʼ�� 
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
	
	//���������봦��ʱ��  
	J = job;
	for(int j = 0; j < J; j++)
	{
		Process_Time[j] =  rand() % (pt_ub * 100 - pt_lb * 100 + 1) + pt_lb * 100;
		Process_Time[j] /= 100.0;
	}
	
	if(fac == 3 && job == 20)			//���������Ϊ3������Ϊ20����Ϊ�˼��ʹ�����ĵ����� 
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

//�� 
class Country_I{
	private:
		bool isCol;													//�Ƿ�Ϊֳ��� 
		double makespan;											//�����ʱ��
		int number;													//��� 
		double* pro_time;											//ÿ�������ļӹ�ʱ��
		
	public:
		int** matrix;
		int** matrix_sep;											//���ָ����ľ��󣨵���ʹ�õڶ��׶�ʱ�� 
		int*  len;													//ÿ�������ж��ٹ��� 
		
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
		virtual void Calculate(string mthd = "LPT");				//����makespan						
		virtual void Display(char show_m = 'y',char show_p = 'n');	//չʾ�����������
};

Country_I::Country_I()
{
	this->makespan = -1;											//���û���� 
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

Country_I::Country_I(int n,string mthd)								//�����ÿ���������伸������				
{	
	this->makespan = -1;											//���û���� 
	len = Build_Array<int>(F);
	for(int f = 0; f < F; f++) 
		len[f] = 0;
	number = n;														//��Ÿ�ֵ 
	matrix = Build_Matrix<int>(F,J);  								//���ٿռ䣨ռ�Ŀռ��е�࣬�������Ż���
	pro_time = Build_Array<double>(F);
	memset(pro_time, 0, sizeof(pro_time));
	matrix_sep = Build_Matrix<int>(F,J);	
	for(int f = 0; f < F; f++)
		for(int j = 0; j < J; j++)
			matrix_sep[f][j] = 0;

	//��ʼ��
	int job_machine = J / M;										//ȷ��ÿ�������������м�������								 
	int job_random = J % M;											//ȷ���ж��ٸ������μ��������
	int assign_temp;												//����f������䵽�Ĺ������� 
	
	//���ձ�׼�ӹ�ʱ��Ӵ�С����������
	int job_time[J] = {0};											//������� - 1								
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
		
	//���ռӹ��ٶȴӴ�С����������
	int factory_speed[F];											//������� - 1								
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
		//���ո��ʷ��乤��
		double total_speed = 0.0;
		for(int f = 0; f < F; f++)
		{
			total_speed += Speed[f];
		}
		double p[F];															//���չ����ٶȲ����ĸ���
		for(int f = 0; f < F; f++)
		{
			p[f] = (f == 0 ? 0.0 : p[f - 1]) + Speed[f] / total_speed;			//�������
		}
		int job_temp = 0;														//��¼�������е�λ��
		for(int j = 0; j < J; j++)
		{ 
			double prob = rand() % 1000 / (double)1000;							//����0��1������� 
			//cout<<"prob is : "<<prob<<endl;
			int factory;
			for(int f = 0; f < F; f++)
				if(prob < p[f])
				{
					factory = f;												//ȷ�����䵽�ĸ����� 	
					break;			
				}
			matrix[factory][len[factory]++] = job_time[job_temp++] + 1;			//�����ʱ���������ı��
		}
		
		//��֤���й������������ڵ��ڻ�����
		int max_index = 0;														//��¼���������Ĺ������� 
		int bad_fac[F], badfac_index = 0;										//��¼���й����������������Ĺ������� 
		for(int f = 0; f < F; f++)
		{
			if(this->len[f] < Factory_Machine[f])
				bad_fac[badfac_index++] = f;
			else if(this->len[f] > this->len[max_index])
				max_index = f;
		} 
		for(int i = 0; i < badfac_index; i++)
		{
			int diff = Factory_Machine[bad_fac[i]] - this->len[bad_fac[i]];		//����������ֵ 
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
		//���չ�����������ȷ��ÿ��������������
		for(int f = 0; f < F; f++)
		{
			len[f] = job_machine * Factory_Machine[f];
		}
		for(int i = 0; i < job_random; i++)
		{
			len[rand() % (F + 1)]++;
		}
		
		//ȷ���Է��乤��
		int job_temp = 0;
		for(int f = 0; f < F; f++)
		{
			int len_temp = 0;
			for(int j = 0; j < len[factory_speed[f]]; j++)
			{
				matrix[factory_speed[f]][len_temp++] = job_time[job_temp++] + 1;		//�����ʱ���������ı�� 
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
	//��ʼ��
	double total_time[F][M] = {0};											//�ܼӹ�ʱ�� = workload 
	for(int f = 0; f < F; f++)
		for(int j = 0; j < J; j++)
			matrix_sep[f][j] = matrix[f][j];
	 
	//���乤��
	for(int f = 0; f < F; f++)
	{ 
		//����ÿ�������ĸ����ռ�
		int **factory = 													//������ÿ�������м������� 
			Build_Matrix<int>(Factory_Machine[f],len[f]);					//������ÿ�������м������� 
		for(int i = 0; i < Factory_Machine[f]; i++)							//��ʼ�� 
			for(int j = 0; j < len[f]; j++)
				factory[i][j] = 0;	

		//��������λ�õĸ����ռ� 
		int *len_temp = Build_Array<int>(len[f]);
		for(int i = 0; i < len[f]; i++)
			len_temp[i] = 0;

		//��ʼ���� 
		bool flag = false;													//������л����������� 
		for(int j = 0; j < len[f]; j++)
		{
			if(mthd == "LPT")
			{
				//LPT
				flag = false;
				for(int m = 0; m < Factory_Machine[f]; m++)
				{
					if(total_time[f][m] == 0)								//������ڻ������� 
					{
						flag = true;										//����л������� 
						factory[m][len_temp[m]++] = matrix_sep[f][j];				//���乤�� 
						total_time[f][m] += Process_Time[matrix_sep[f][j] - 1];	//�ۼ�ʱ�� 
						break;												//ֱ��������һ������ 
					}
				}
				if(flag == false)											//Ҳ�������л�����������
				{
					int min = 0;											//�ܼӹ�ʱ����С�Ļ���������
					for(int m = 1; m < Factory_Machine[f]; m++)
					{
						if(total_time[f][m] < total_time[f][min])
							min = m;
					}
					factory[min][len_temp[min]++] = matrix_sep[f][j];				//���乤��
					total_time[f][min] += Process_Time[matrix_sep[f][j] - 1];		//�ۼ�ʱ��
				}
			}
			else if(mthd == "random")
			{
				//���
				int m = rand() % Factory_Machine[f];
				factory[m][len_temp[m]++] = matrix_sep[f][j];
				total_time[f][m] += Process_Time[matrix_sep[f][j] - 1];
			}
		}

		//�ϳ�һ������
		int a = 0;															//��¼�ºϳɱ����λ�� 
		for(int m = 0; m < Factory_Machine[f]; m++)
		{
			for(int j = 0; j < len_temp[m]; j++)
			{
				matrix_sep[f][a++] = factory[m][j];
			}
			if(Factory_Machine[f] - m > 1)									//��ӷָ���
			{
				matrix_sep[f][a++] = SEP;
			}
		}

		//�ͷſռ�
		Destory_Array<int>(len_temp);
		Destory_Matrix<int>(factory,Factory_Machine[f]);
	}

	//���makespan
	for(int f = 0; f < F; f++)
	{
		for(int m = 0; m < Factory_Machine[f]; m++)
		{
			pro_time[f] = Get_Max<double>(total_time[f],Factory_Machine[f]) / Speed[f];
		}
	}
	this->makespan = Get_Max<double>(pro_time,F);

//	//���ԣ���ӡ��������
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
		int* full_len;													//��len���������ڼ����˷ָ����������ĳ��� 
		
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

Country_II::Country_II(Country_I* c, string mthd)								//��ԭ�з��䷽�������� 
{
	//���Ⱦ����ʼ�� 
	this->SetMakespan(0.0);
	this->SetNumber(c->GetNumber());											//��Ÿ�ֵ
	this->matrix = Build_Matrix<int>(F,J);
	memset(matrix, 0, sizeof(matrix));
	full_len = Build_Array<int>(F);
	len = Build_Array<int>(F);
	
	//�����ɷָ�����λ��,����乤�� 
	for(int f = 0; f < F; f++)
	{
		//��ʼ������
		int num_sep = Factory_Machine[f] - 1;									//����� = �������� - 1
		this->len[f] = c->len[f];
		this->full_len[f] = this->len[f] + num_sep;								//���� = ����� + ������ 
		
		//��random��LPT���ַ�ʽ
		if(mthd == "random")
		{
			//�Ȱѹ�����Ŷ����ƹ��� 
			for(int i = 0; i < c->len[f]; i++)
			{
				this->matrix[f][i] = c->matrix[f][i];
			}
			//Ȼ�����������ָ���
			for(int s = 0; s < num_sep; s++)
			{
				int assert_idx = rand() % this->len[f];
				Insert_Array<int>(this->matrix[f], SEP, assert_idx, this->len[f] + s);
			} 
		} 
		else if(mthd == "LPT")
		{
			//��ʼ�� 
			bool flag = false;												//�л�������Ϊ�� 
			double total_time[F][M] = {0};
			int* machine_ind = Build_Array<int>(Factory_Machine[f]);		//һ��ʼ��ֵΪÿ�������ĳ�ʼλ�� 
			machine_ind[0] = 0;												//��һ��������0��ʼ 
			int gap = len[f] / Factory_Machine[f];							//ÿ�������֮���������� 
			for(int m = 1; m < Factory_Machine[f]; m++)
			{
				machine_ind[m] = machine_ind[m - 1] + gap + 1;				//Ϊÿ��������ʼ��λ��
				this->matrix[f][machine_ind[m] - 1] = SEP;					//����ָ��� 
			}

			//����c�й�������
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
					if(this->matrix[f][machine_ind[ind]] == SEP)			//�����յķ����Ǽ�¼ÿһ���ָ�����λ�� 
					{
						Insert_Array(this->matrix[f], c->matrix[f][j], machine_ind[ind]++, full_len[f]);
						for(int i = ind + 1; i < Factory_Machine[f]; i++)
						{
							machine_ind[i]++;								//������һ�����ڲ�����涼��һ 
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

			//���������0��ɾ��
			for(int j = 0; j < c->len[f]; j++) 							 
			{
				if(this->matrix[f][j] == 0)
					int drop = TakeAway_Array<int>(this->matrix[f], j, full_len[f]);
			}
			
			//��ʱ�������ٵ�
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
	//��ÿ������ÿ�������ϵ��ۼ�ʱ�� 
	double total_time[F][M] = {0};
	for(int f = 0; f < F; f++)
	{
		int time_ind = 0;
		for(int m = 0; m < Factory_Machine[f]; m++)
		{
			if(time_ind >= this->full_len[f])										//����ڷ�Χ��ֱ������ 
				break; 
			else
			{
				while(this->matrix[f][time_ind] != SEP && time_ind < this->full_len[f])
					total_time[f][m] += Process_Time[matrix[f][time_ind++] - 1];
				if(this->matrix[f][time_ind] == SEP)								//����SEP���� 
					time_ind++;
			}
		}
	}
	
	//ȡ������� 
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
