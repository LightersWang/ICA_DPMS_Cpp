/*
	Name: ICA.h
	Copyright: 2018 Wuhan University of Technology 
					all rights reserved
	Author: LightersWang
	Date: 23/01/18 15:29
	Description: Completment of every procedures in ICA.
*/

#ifndef H_ICA
#define H_ICA
#include <iostream>
using namespace std;
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "Empire.h"

//�˿����� 
const int N_pop = 100;

//�۹����� 
const int N_imp = 10;

//ֳ������� 
const int N_col = N_pop - N_imp;

const double P1_rev = 0.3;

typedef struct
{
	int row;									//ԭ����������� 
	int col;									//ԭ����������� 
	int elem;									//ԭ�����Ԫ������ 
	bool come;									//����Ƿ񵽷ù�������������� 
} Table; 	

class ICA_I{
	private:
		int         len_e;							//���ȣ����е۹������� 
		long long   crowd_idx;						//�ۻ����Ҹ������ñ��)
		Country_I*  GOAT;							//Greatest Of All Time
		Country_I*  crowd[10000 * N_pop]; 			//���ҳ� 

	public:
		Empire_I*   egroup[N_imp];
		
		//START & END
		ICA_I();
		~ICA_I() {}
		
		//GETTER
		int  GetLen();
		
		virtual Country_I* GetGOAT();
		virtual Empire_I* GetBestEmp();
		
		//SETTER
		void SetLen(int n);
		void SetLen(char n); 
		
		//FUNCTION
		void Display(char goat = 'Y', char all = 'N');
		virtual void FindGOAT();
		virtual void DisplayGOAT(char show_m = 'y',char show_p = 'n');
		
		//OPERATOR
		int  TabuSearch(int (&tabu)[N_col], int& len_t);
		int* IdxRevCol(Empire_I* emp, int amount_rev);
		int* NumberColonies(const double power[]);
		virtual void Swap_1(Country_I* c);
		virtual void Insert_1_JSP(Country_I* c);
		virtual void Insert_1_rdm(Country_I* c);
		virtual void Insert_1_ptime(Country_I* c);
		virtual void Assimilation_Op(Empire_I* emp);
		virtual void CrossOver(Country_I* P1, Country_I* P2, Country_I* OS);
		
		//ALGORITHM STEPS	
		virtual void Assimilation_FirstO();						//������ͬ����������ͬһ���۹��� 
		virtual void Assimilation_SecondO();					//����ͬ��������������������֮�� 
		virtual void Revulotion();
		virtual void Competition();
		virtual void EliminateEmpire();
};

ICA_I::ICA_I()
{	
	//˽�б�����ʼ�� 
	this->crowd_idx = 1;
	this->len_e = 0;
	for(int e = 0; e < N_imp; e++)
	{
		egroup[e] = new Empire_I(e + 1, 0);
		len_e++;
	}
	GOAT = new Country_I;
	GOAT->SetNumber(10000);
	
	//����N_pop������
	double makespan_temp[N_pop];							//��¼makespan�����ں��ڱ�׼�� 
	for(int c = 0; c < N_pop; c++)
	{
		crowd[c] = new Country_I(crowd_idx++);
		crowd[c]->Calculate();
		makespan_temp[c] = crowd[c]->GetMakespan();
	}

	//����makespan��С�����˳���crowd�ڵĹ��ҽ������� 
	for (int i = 0; i < N_pop - 1; i++)
	{
		for (int j = 0; j < N_pop - i - 1; j++)
    	{
    		if(crowd[j]->GetMakespan() > crowd[j + 1]->GetMakespan())
    		{
    			Swap<Country_I*>(crowd[j], crowd[j + 1]);
    			Swap<double>(makespan_temp[j], makespan_temp[j + 1]);
			}
		}
	}

//	//���ԣ�makespan 
//	Print_Array<double>(makespan_temp,N_pop);

	//��׼�����й��ҵ�makespan����������������makespan�ĺ� 
	double sum_imp = 0.0;
	for(int i = 0; i < N_pop; i++)
	{
		makespan_temp[i] -= makespan_temp[N_pop - 1];
		if(i < N_imp)
		{
			sum_imp += makespan_temp[i];	
		}
	}
	
//	//���ԣ���׼��makespan���������ܺ� 
//	Print_Array<double>(makespan_temp,N_pop); 
//	cout<<"sum of imps is "<<sum_imp<<endl;

	//ȷ���������� 
	for(int c = 0; c < N_pop; c++)
		crowd[c]->SetAttribute(c < N_imp ? "Imp" : "Col");
	
	//�������������� 
	double power[N_imp];
	for(int i = 0; i < N_imp; i++)
	{
		power[i] = fabs(makespan_temp[i] / sum_imp);
	}
	int* NC = this->NumberColonies(power); 
	int sum_nc = 0; 

	//���ԣ���������� 
//	Print_Array<int>(NC,N_imp);
//	for(int i = 0; i < N_imp; i++)
//	{
//		sum_nc += NC[i];
//	}
//	cout<<endl<<"Total nc are "<<sum_nc<<" (should be "<<N_col<<")"<<endl;
	
	//����������С������������ֳ��� 				
	int tabu[N_col] = {0};										//��ǰѡ����ֳ��ض��ŵ������棬�Ժ�ѡ 
	int len_t  = 0;												//��¼�����б��ĵڼ������ĳɶ��л�򵥺ܶ� 
	for(int c = 0; c < N_imp; c++)
	{
		egroup[c]->AddCountry(crowd[c]);
		sum_nc += NC[c];
		for(int i = 0; i < NC[c]; i++)
		{
			int col_i = TabuSearch(tabu, len_t);
			//���ԣ��鿴������� 
//			cout<<endl<<"Empire "<<egroup[c]->GetNumber_E()<<endl; 
//			crowd[col_i]->Display();
			egroup[c]->AddCountry(crowd[col_i]);				//��ӵ�ǰֳ��ص���Ӧ�۹��� 
		}
	}
	
	//�ҳ�ɽ��
	this->FindGOAT();
}

int ICA_I::GetLen()
{
	return len_e; 
}

void ICA_I::SetLen(int n)
{
	this->len_e = n;
}

void ICA_I::SetLen(char n)
{
	if(n == '+')
	{
		this->len_e++;
	}
	else if(n == '-')
	{
		this->len_e--;
	}
}

Empire_I* ICA_I::GetBestEmp()
{
	Empire_I* best = egroup[0];
	for(int i = 1; i < this->GetLen(); i++)
	{
		if(egroup[i]->GetTotalpower() < best->GetTotalpower())
			best = egroup[i];
	}
	return best;
}

void ICA_I::FindGOAT()
{
	GOAT = egroup[0]->GetBestCountry();
	for(int i = 1; i < this->GetLen(); i++)
	{
		if(egroup[i]->GetBestCountry()->GetMakespan() < GOAT->GetMakespan() )
		{
			GOAT = egroup[i]->GetBestCountry();
		}
	}
}

Country_I* ICA_I::GetGOAT()
{
	return this->GOAT;
}

void ICA_I::DisplayGOAT(char show_m, char show_p)
{
	cout<<"Here's the GOAT! "<<endl;
	GOAT->Display(show_m,show_p);
	cout<<endl;
}

void ICA_I::Display(char goat, char all)
{
	cout<<"Here are all the imperialists and their colonies:"<<endl;
	for(int e = 0; e < this->len_e; e++)
	{
		cout<<"Empire "<<egroup[e]->GetNumber_E()<<" : "<<endl;
		cout<<"Total countries : "<<egroup[e]->GetCountries()<<endl;
		cout<<"Total power : "<<egroup[e]->GetTotalpower()<<endl;
		cout<<"Imperialist : "<<egroup[e]->GetBestCountry()->GetNumber()<<endl;
		cout<<"Imperialist's makespan : "<<egroup[e]->GetBestCountry()->GetMakespan()<<endl;
		cout<<"Colony : ";
		for(int c = 0; c < egroup[e]->GetCountries(); c++)
		{
			if(c == egroup[e]->GetBestIdx())
				continue;
			else
			{
				cout<<egroup[e]->empire[c]->GetNumber();
				if(c < egroup[e]->GetCountries() - 1)
					cout<<", ";
				else
					cout<<endl;
			}
		}
		cout<<endl;
	}
	if(all == 'Y' || all == 'y')			//���й��ҵĹ����ֲ�ȫ����ʾ 
	{
		for(int e = 0; e < len_e; e++)
		{
			egroup[e]->Display();
			cout<<endl;
		}
	}
	if(goat == 'y' || goat == 'Y')			//��ʾ��õ�һ����	
	{
		this->DisplayGOAT();
	}
}

int ICA_I::TabuSearch(int (&tabu)[N_col], int& len_t)
{
	bool flag = false;									//������ǣ�������ɵ��ڽ����б��о���ȷ���������� 
	int col_i;											//�����ѡȡ��ֳ��ص����� 
	do
	{
		flag  = false; 
		col_i = rand() % (N_pop - N_imp) + N_imp;		//10~99
		for(int u = 0; u < len_t; u++)
		{
			if(col_i == tabu[u])
			{
				flag = true;
				break;
			}
		}	
	}
	while(flag);
	tabu[len_t++] = col_i;								//��ӵ�ǰֳ��ر�ŵ������б� 
	return col_i;
}

int* ICA_I::IdxRevCol(Empire_I* emp, int amount_rev)
{
	int  len_t = 0;
	int* number_rev = Build_Array<int>(amount_rev);
	for(int i = 0; i < amount_rev; i++)
	{
		bool flag = false;									//������ǣ�������ɵ��ڽ����б��о���ȷ���������� 
		int col_i;											//�����ѡȡ��ֳ��ض�Ӧ���ڵ۹������е��±� 
		do
		{
			flag  = false;
			col_i = rand() % emp->GetCountries();
			for(int u = 0; u < len_t; u++)
			{
				if(col_i == number_rev[u] || col_i == emp->GetBestIdx())
				{
					flag = true;
					break;
				}
			}
		}
		while(flag);
		number_rev[len_t++] = col_i;								//��ӵ�ǰֳ��ر�ŵ������б�
	}
	return number_rev;
}

int* ICA_I::NumberColonies(const double power[])
{
	int* NC = Build_Array<int>(N_imp);
	int zeros[N_imp], len_z = 0;									//��¼Ϊ0������ 
	int sum_nc = 0;
	
	//��ʼ���� 
	for(int e = 0; e < N_imp; e++)
	{
		NC[e] = round(power[e] * (double)N_col);
		sum_nc += NC[e];
		if(NC[e] == 0)
		{
			zeros[len_z++] = e;
		}
	}
	
	//ȥ��Ϊ0�� 
	int ind;
	for(int i = 0; i < len_z; i++)
	{
		NC[zeros[i]] = 1;										//����Ϊ0�Ĺ���������һ��
		do
		{
			ind = rand() % N_imp;
		}
		while(NC[ind] < 1);
		NC[ind]--;												//Ȼ���������һ����������ֳ�����Ŀ��һ�� 
	}
	
	//�������
	int diff = sum_nc - N_col;
	if(diff > 0)
	{
		for(int i = N_imp - 1; i >= N_imp - diff; i--)			//����0�ʹ����ٵ��Ǽ������Ҽ��٣������ԣ� 
			NC[i]--;
	}
	else if(diff < 0)
	{
		for(int i = 0; i < abs(diff); i++)						//С��0�ʹ������Ǽ����������ӣ������ԣ� 
			NC[i]++;
	}
	
	return NC;
}

void ICA_I::CrossOver(Country_I* P1, Country_I* P2, Country_I* OS)
{
	//ȷ������λ�õı߽磺�ҵ�һ��������̵� 
	int bound = Get_Min<int>(P1->len,F);
	
	//������������λ��
	int cross_1, cross_2, width;
	Generate_2random(cross_1, cross_2, bound, bound);	
	if(cross_1 > cross_2)										//��֤cross_1��ԶС��cross_2 
	{	
		Swap<int>(cross_1,cross_2);
	}
	width = cross_2 - cross_1 + 1;								//��������Ŀ�ȣ�cross_1��cross_2��

	//��P1���������ⲿ���Ƶ��Ӵ��� 
	OS->SetLen(P1->len);
	for(int f = 0; f < F; f++)
	{
		for(int j = 0; j < OS->len[f]; j++)
		{
			OS->matrix[f][j] = (cross_1 <= j && j <= cross_2 ? 0 : P1->matrix[f][j]);
		}
	}

	//��P2����������һ�ű���
	Table p2[J];												//��¼p2��Ԫ�ص�λ�ú��Ƿ���ʹ�
	int p2_front = 0;											//��¼p2�����ĸ�λ��
	for(int f = 0; f < F; f++)
	{
		for(int j = 0; j < P2->len[f]; j++)
		{
			p2[p2_front].row = f;
			p2[p2_front].col = j;
			p2[p2_front].elem = P2->matrix[f][j];
			p2[p2_front].come = false;							//��¼��û�з��ʹ�
			p2_front++;
		}
	}

	//����ظ����� 
	for(int f = 0; f < F; f++)
	{
		for(int j = 0; j < OS->len[f]; j++)
		{
			if(cross_1 <= j && j <= cross_2) 			//����ڽ������ڲ����������ѭ��
				continue;
			else										//�������p2���в�
			{
				for(int i = 0; i < J; i++)
				{
					if(p2[i].come == false && OS->matrix[f][j] == p2[i].elem)
					{
						p2[i].come = true;
					}
				}
			}
		}
	}
	
	//�ҳ�ʣ�µ� 
	int re_len = width * F;
	int remain[re_len], re_idx = 0;
	for(int j = 0; j < J; j++)
	{
		if(p2[j].come == false)
		{
			remain[re_idx++] = p2[j].elem;
		}
	}
	
	//��p2ʣ�µ����OS��
	re_idx = 0;
	for(int f = 0; f < F; f++)
	{
		for(int i = cross_1; i <= cross_2; i++)
		{
			OS->matrix[f][i] = remain[re_idx++];
		}
	}
	OS->Calculate(); 
}

void ICA_I::Swap_1(Country_I* c)
{
	int f1 = Get_Max_Index<double>(c->GetProTime(), F);		//��һ��������Ӧmakespan
	int f2 = Generate_difrdm(f1, F);						//�ڶ����������ܺ͵�һ�����
	int j1, j2;
	Generate_2random(j1, j2, c->len[f1], c->len[f2]);		//�����������λ�õĹ��� 
	Swap<int>(c->matrix[f1][j1], c->matrix[f2][j2]);
	c->Calculate();
}

void ICA_I::Insert_1_rdm(Country_I* c)
{
	int f_ori = rand() % F;
	int j_ori = rand() % c->len[f_ori];									//���ȷ����ʼ���� 
	int f_ins = Generate_difrdm(f_ori, F);
	int j_ins = rand() % c->len[f_ins];									//ȷ��Ŀ�깤�� 
	int elem  = TakeAway_Array<int>(c->matrix[f_ori], j_ori, c->len[f_ori]--);	
	Insert_Array<int>(c->matrix[f_ins], elem, j_ins, c->len[f_ins]++);
	c->Calculate();
}

void ICA_I::Insert_1_ptime(Country_I* c)
{
	int f_ori = Get_Max_Index<double>(c->GetProTime(), F);						//��ʼ����Ϊmakespan��Ӧ 
	int j_ori = rand() % c->len[f_ori];											//���ȷ����ʼ����	
	int f_ins = Get_Min_Index<double>(c->GetProTime(), F);						//Ŀ�깤��Ϊʱ����̵Ĺ��� 
	int j_ins = rand() % c->len[f_ins]; 
	int elem  = TakeAway_Array<int>(c->matrix[f_ori], j_ori, c->len[f_ori]--);
	Insert_Array<int>(c->matrix[f_ins], elem, j_ins, c->len[f_ins]++);
	c->Calculate();
}

void ICA_I::Insert_1_JSP(Country_I* c)
{
	int f_ori = Get_Max_Index<double>(c->GetProTime(), F);						//��ʼ����Ϊmakespan��Ӧ 
	int j_ori = rand() % c->len[f_ori];											//���ȷ����ʼ����	
	double JSP[F];																//����-���ٻ� 
	for(int f = 0; f < F; f++)
	{
		JSP[f] = (J - c->len[f]) * Speed[f]; 
	}
	int f_ins = Get_Max_Index<double>(JSP, F);									//Ŀ�깤��Ϊ������������ 
	int j_ins = rand() % c->len[f_ins]; 
	int elem  = TakeAway_Array<int>(c->matrix[f_ori], j_ori, c->len[f_ori]--);
	Insert_Array<int>(c->matrix[f_ins], elem, j_ins, c->len[f_ins]++);
	c->Calculate();
}

void ICA_I::Assimilation_Op(Empire_I* emp)
{
	//��һ�������Ӵ������۹� 
	int len_c = emp->GetCountries(), crowd_memo = crowd_idx;
	Country_I* imper = emp->GetBestCountry();						//��������������Ϊ���й��Ҷ�Ҫ��������
	for(int c = 0; c < len_c; c++)
	{
		if(emp->empire[c] != emp->GetBestCountry())					//�����������Լ����Լ����� 
		{
			crowd[crowd_idx] = new Country_I;						//�������Ӵ���������ҳ��� 
			crowd[crowd_idx]->SetNumber(crowd_idx);					//�ۻ������Ĺ��ұ�� 
			this->CrossOver(imper, emp->empire[c], crowd[crowd_idx]);				 
			crowd[crowd_idx]->Calculate();
			emp->AddCountry(crowd[crowd_idx]);						//���������Ӵ���ӽ��۹�
			crowd_idx++;
		}
	}
	
	//�ڶ������������ڲ�����makespan��С��������
	for (int i = 0; i < emp->GetCountries() - 1; i++)
	{
		for (int j = 0; j < emp->GetCountries() - i - 1; j++)
    	{
    		if(emp->empire[j]->GetMakespan() > 
				emp->empire[j + 1]->GetMakespan())
				Swap<Country_I*>(emp->empire[j],emp->empire[j + 1]);
		}
	}
    
	//��������ȡǰlen_c��,���һ��������ֳ���
	int last_idx = emp->GetCountries() - 1;
	emp->RemoveCountry(len_c, last_idx);	//����Ϊ2l-1������Ҫ�ټ�1 
	emp->Calculate();
}

void ICA_I::Assimilation_FirstO()
{
	for(int e = 0; e < len_e; e++)
	{
		Assimilation_Op(egroup[e]);
	} 
	this->FindGOAT();
}

void ICA_I::Assimilation_SecondO()
{
	int len_e = this->GetLen();
	int crowd_memo = crowd_idx;
	for(int i = 0; i < len_e; i++)
	{
		crowd[crowd_idx] = new Country_I;						//�������Ӵ���������ҳ��� 
		crowd[crowd_idx]->SetNumber(crowd_idx);					//�ۻ������Ĺ��ұ�� 
//		egroup[i]->GetBestCountry()->Display();
		this->CrossOver(GOAT, egroup[i]->GetBestCountry(), crowd[crowd_idx]);		
//		crowd[crowd_idx]->Display();		 
		crowd[crowd_idx]->Calculate();
		if(egroup[i]->GetBestCountry()->GetMakespan() 
					> crowd[crowd_idx]->GetMakespan())			//ֻ���ܸ��õĽ� 
		{
			int best = egroup[i]->GetBestIdx();
			egroup[i]->empire[best] = crowd[crowd_idx]; 
		}
		crowd_idx++;
	}
	this->FindGOAT();
}

void ICA_I::Revulotion()
{
	for(int e = 0; e < GetLen(); e++)
	{
		//ȷ���������ҵĸ��� 
		int  amount_rev = P1_rev * (egroup[e]->GetCountries() - 1);
		int* number_rev = this->IdxRevCol(egroup[e], amount_rev); 
		
		//ȷ����Щ���Ҹ���
		for(int c = 0; c < amount_rev; c++)
		{
			//egroup[e]->empire[number_rev[c]]->Display('y','y');
			int choose = rand() % 2;
			switch(choose)
			{
				case 0: Swap_1(egroup[e]->empire[number_rev[c]]);
						//cout<<"swap"<<endl;
						break;
				case 1: Insert_1_JSP(egroup[e]->empire[number_rev[c]]);
						//cout<<"insert JSP"<<endl;
			}
			egroup[e]->empire[number_rev[c]]->Calculate();
			//egroup[e]->empire[number_rev[c]]->Display('y','y');
		}
	}
}

void ICA_I::Competition()
{
	//����������С��������
	for(int i = 0; i < GetLen() - 1; i++)
	{
		for (int j = 0; j < GetLen() - i - 1; j++)
    	{
    		if(egroup[j]->GetTotalpower() > egroup[j + 1]->GetTotalpower())
    		{
    			Swap<Empire_I*>(egroup[j], egroup[j + 1]);
			}
		}
	}
	
	//��׼��������������������� 
	double TC[GetLen()];
	double sum_tc = 0.0;
	for(int i = 0; i < GetLen(); i++)
	{
		TC[i] = egroup[i]->GetTotalpower() - egroup[GetLen() - 1]->GetTotalpower();
		sum_tc += TC[i];
	}
	double D[GetLen()];
	for(int i = 0; i < GetLen(); i++)
	{
		double p = fabs(TC[i] / sum_tc);					//�������
		double r = (rand() + 1.0) / (RAND_MAX + 2.0);		//����Ŷ�
		D[i] = p - r;
	}
	
	//����ֳ��� 
	int emp_weak    = Get_Max_Index<double>(D, GetLen());
	int emp_strong  = Get_Min_Index<double>(D, GetLen());
	int col_w2s_idx = 0;
	for(int i = 1; i < egroup[emp_weak]->GetCountries(); i++)
	{
		if(egroup[emp_weak]->empire[col_w2s_idx]->GetMakespan() 
				< egroup[emp_weak]->empire[i]->GetMakespan())
			col_w2s_idx = i;
	}
	Country_I* col_w2s = egroup[emp_weak]->empire[col_w2s_idx];
	egroup[emp_weak]->RemoveCountry(col_w2s_idx);
	egroup[emp_weak]->Calculate();
	egroup[emp_strong]->AddCountry(col_w2s);
	egroup[emp_strong]->Calculate();
	
//	cout<<"Country "<<col_w2s->GetNumber()<<" is taken by Empire "<<egroup[emp_strong]->GetNumber_E()
//	<<" from Empire "<<egroup[emp_weak]->GetNumber_E()<<endl<<endl;
}

void ICA_I::EliminateEmpire()
{
	for(int e = 0; e < GetLen(); e++)
	{
		if(egroup[e]->GetCountries() == 1)
		{
			Country_I* temp = egroup[e]->GetBestCountry();
			egroup[e]->RemoveCountry(egroup[e]->GetBestIdx());				//�ڱ��۹���ɾ��
			egroup[e]->Calculate();
			GetBestEmp()->AddCountry(temp);									//��ӵ���ǿ�ĵ۹����ӿ����� 
			GetBestEmp()->Calculate();
			TakeAway_Array<Empire_I*>(egroup, e, GetLen());					//ɾ��һ���۹�������������
			this->len_e--;
			break;
		}
	}
}

class ICA_II: public ICA_I
{
	private: 
		long long    crowd_idx;						//�ۻ����Ҹ������ñ��)
		Country_II*  GOAT;							//Greatest Of All Time
		Country_II*  crowd[10000 * N_pop]; 			//���ҳ� 
		
	public:
		Empire_II* egroup[N_imp];
		
		//START & END
		ICA_II(Country_I* goat);
		~ICA_II() {}
		
		//FUNCTION
		virtual void FindGOAT();
		virtual Empire_II* GetBestEmp();
		virtual void DisplayGOAT(char show_m = 'y',char show_p = 'n');
		
		//OPERATOR
		void Swap_2(Country_II* c);
		void Insert_2(Country_II* c);
		virtual void Swap_1(Country_II* c);
		virtual void Insert_1_JSP(Country_II* c);
		virtual void CrossOver(Country_II* P1,Country_II* P2);
//		virtual void Assimilation_Op(Empire_II* emp);
		
		//ALGORITHM STEPS	
//		virtual void Assimilation_FirstO();
//		virtual void Assimilation_SecondO();
//		virtual void Revulotion();
//		virtual void Competition();
//		virtual void EliminateEmpire();
		void VNS(Country_II* c, int max_itr = 10);
};

ICA_II::ICA_II(Country_I* goat)
{
	//��ʼ������ 
	this->SetLen(0);
	this->crowd_idx = 0;
	for(int e = 0; e < N_imp; e++)
	{
		egroup[e] = new Empire_II(e + 1, 0);
		this->SetLen('+');
	}
	GOAT = new Country_II;
	GOAT->SetNumber(10000);
	
	//�����ϸ��׶ε����Ž����N_pop���� 
	double makespan_temp[N_pop];
	for(int i = 0; i < N_pop; i++)
	{
		crowd[crowd_idx] = new Country_II(goat, "random");
		crowd[crowd_idx]->SetNumber(crowd_idx + 1);						//��ű�������1 
		crowd[crowd_idx]->Calculate();
		makespan_temp[crowd_idx] = crowd[crowd_idx]->GetMakespan();
		crowd_idx++;
	} 
	
	//����makespan��С�����˳���crowd�ڵĹ��ҽ������� 
	for (int i = 0; i < N_pop - 1; i++)
	{
		for (int j = 0; j < N_pop - i - 1; j++)
    	{
    		if(crowd[j]->GetMakespan() > crowd[j + 1]->GetMakespan())
    		{
    			Swap<Country_II*>(crowd[j], crowd[j + 1]);
    			Swap<double>(makespan_temp[j], makespan_temp[j + 1]);
			}
		}
	}

	//��׼�����й��ҵ�makespan����������������makespan�ĺ� 
	double sum_imp = 0.0;
	for(int i = 0; i < N_pop; i++)
	{
		makespan_temp[i] -= makespan_temp[N_pop - 1];
		if(i < N_imp)
		{
			sum_imp += makespan_temp[i];	
		}
	}

	//ȷ���������� 
	for(int c = 0; c < N_pop; c++)
		crowd[c]->SetAttribute(c < N_imp ? "Imp" : "Col");
	
	//�������������� 
	double power[N_imp];
	for(int i = 0; i < N_imp; i++)
	{
		power[i] = fabs(makespan_temp[i] / sum_imp);
	}
	int* NC = this->NumberColonies(power); 
	int sum_nc = 0; 
	
	//����������С������������ֳ��� 				
	int tabu[N_col] = {0};										//��ǰѡ����ֳ��ض��ŵ������棬�Ժ�ѡ 
	int len_t  = 0;												//��¼�����б��ĵڼ������ĳɶ��л�򵥺ܶ� 
	for(int c = 0; c < N_imp; c++)
	{
		egroup[c]->AddCountry(crowd[c]);
		sum_nc += NC[c];
		for(int i = 0; i < NC[c]; i++)
		{
			int col_i = TabuSearch(tabu, len_t);
			//���ԣ��鿴������� 
//			cout<<endl<<"Empire "<<egroup[c]->GetNumber_E()<<endl; 
//			crowd[col_i]->Display();
			egroup[c]->AddCountry(crowd[col_i]);				//��ӵ�ǰֳ��ص���Ӧ�۹��� 
		}
	}
	
	//�ҳ�ɽ��
	this->FindGOAT();
}

void ICA_II::FindGOAT()
{
	GOAT = egroup[0]->GetBestCountry();
	for(int i = 1; i < this->GetLen(); i++)
	{
		if(egroup[i]->GetBestCountry()->GetMakespan() < GOAT->GetMakespan() )
		{
			GOAT = egroup[i]->GetBestCountry();
		}
	}
}

Empire_II* ICA_II::GetBestEmp()
{
	Empire_II* best = egroup[0];
	for(int i = 1; i < this->GetLen(); i++)
	{
		if(egroup[i]->GetTotalpower() < best->GetTotalpower())
			best = egroup[i];
	}
	return best;
}

void ICA_II::DisplayGOAT(char show_m,char show_p)
{
	cout<<"Here's the GOAT! "<<endl;
	GOAT->Display(show_m,show_p);
	cout<<endl;
}

void ICA_II::Swap_1(Country_II* c)
{
	int f1 = Get_Max_Index<double>(c->GetProTime(), F);				//��һ��������Ӧmakespan
	int f2 = Generate_difrdm(f1, F);								//�ڶ����������ܺ͵�һ�����
	int j1, j2;
	do
	{
		Generate_2random(j1, j2, c->len[f1], c->len[f2]);			//�����������λ�õĹ��� 	
	}
	while(c->matrix[f1][j1] == SEP || c->matrix[f2][j2] == SEP);	//�����Ĺ��������Ƿָ��� 
	Swap<int>(c->matrix[f1][j1], c->matrix[f2][j2]);
	c->Calculate();
}

void ICA_II::Swap_2(Country_II* c)
//����ͬһ������ͬ�����Ĺ��� 
{
	int f = rand() % F;					//���ѡ��һ�����������߲�������� 
	int pos[Factory_Machine[f]];					//��¼�ָ�����λ��
	int rear_pos = 0;
	for(int p = 0; p < c->len[f] - Factory_Machine[f] + 1; p++)
		if(c->matrix[f][p] == SEP)
			pos[rear_pos++] = p;
	int m1,m2;
	Generate_2random(m1,m2,Factory_Machine[f],Factory_Machine[f]);		//��������Ҫ���н����Ļ��� 
	int p1, p2, b1, b2, a1 = 0, a2 = 0;
	if(m1 == 0)
		b1 = pos[m1];
	else 
	{
		b1 = pos[m1] - pos[m1 - 1] - 1;	
		a1 = pos[m1 - 1] + 1;
	}
	if(m2 == 0) 
		b2 = pos[m2];
	else
	{
		b2 = pos[m2] - pos[m2 - 1] - 1;	
		a2 = pos[m2 - 1] + 1;
	}	
	Generate_2random(p1,p2,b1,b2,a1,a2);			//����Ҫ�����Ĺ��� 
	Swap<int>(c->matrix[f][p1],c->matrix[f][p2]);
}

void ICA_II::Insert_1_JSP(Country_II* c)
{
	int f_ori = Get_Max_Index<double>(c->GetProTime(), F), j_ori;				//��ʼ����Ϊmakespan��Ӧ
	do
	{
		j_ori = rand() % c->len[f_ori];											//���ȷ����ʼ����
	}
	while(c->matrix[f_ori][j_ori] == SEP); 	
	double JSP[F];																//����-���ٻ� 
	for(int f = 0; f < F; f++)
	{
		JSP[f] = (J - c->len[f]) * Speed[f]; 
	}
	int f_ins = Get_Max_Index<double>(JSP, F), j_ins;							//Ŀ�깤��Ϊ������������
	do
	{
		j_ins = rand() % c->len[f_ins]; 
	}
	while(c->matrix[f_ins][j_ins] == SEP);
	int elem  = TakeAway_Array<int>(c->matrix[f_ori], j_ori, c->len[f_ori]--);
	Insert_Array<int>(c->matrix[f_ins], elem, j_ins, c->len[f_ins]++);
	c->Calculate();
}

void ICA_II::Insert_2(Country_II* c)
//ͬһ�����ڵĹ����ƶ�����һ������ 
{
	int f = rand() % F;					//���ѡ��һ�����������߲�������� 
	int pos[Factory_Machine[f]];					//��¼�ָ�����λ��
	int rear_pos = 0;
	for(int p = 0; p < c->len[f] - Factory_Machine[f] + 1; p++)
		if(c->matrix[f][p] == SEP)
			pos[rear_pos++] = p;
	int m1,m2;
	Generate_2random(m1,m2,Factory_Machine[f],Factory_Machine[f]);		//��������Ҫ���в���Ļ���
	int p1 = rand() % (pos[m1] - pos[m1 - 1] - 1) + pos[m1 - 1] + 1;	//��������SEP�� 
	int temp;
	temp = TakeAway_Array<int>(c->matrix[f], p1, c->len[f]--); 
	Insert_Array<int>(c->matrix[f], temp, p1, c->len[f]++);
} 

void ICA_II::CrossOver(Country_II* P1,Country_II* P2)				
{
	//��ʼ�� 
	int b[2];
	b[0] = Get_Max<int>(P1->len,F);				//����� 
	b[1] = Get_Max<int>(P2->len,F);
	int bound = Get_Min<int>(b,2);				//���������С 
	
	//������������λ�� 
	int cross_2,cross_1 = rand() % bound; 
	do
	{
		cross_2 = rand() % bound;
	}
	while(cross_1 >= cross_2);
	int width = cross_2 - cross_1 + 1;
	
	//��P1�������ⲿ���Ƶ��Ӵ��� 
	Country_II* OS = new Country_II; 			
	OS->SetFullLen(P1->full_len);
	for(int f = 0; f < F; f++)					//�����˷ָ��� 
	{
		for(int j = 0; j < OS->full_len[f]; j++)
		{
			OS->matrix[f][j] = (cross_1 <= j && j <= cross_2 ? 0 : P1->matrix[f][j]);
		}
	}
	
	//��P2����������һ�ű��� 
	Table p2[J];								//��¼p2��Ԫ�ص�λ�ú��Ƿ���ʹ�						
	int p2_front = 0;							//��¼p2�����ĸ�λ��
	int sep[F][M];								//���չ�������������¼�ָ�����λ�� 
	int idx = 0;
	for(int f = 0; f < F; f++)
	{
		for(int j = 0; j < P2->full_len[f]; j++)
		{
			if(P2->matrix[f][j] == SEP)			//������¼�ָ���				
			{
				sep[f][idx++] = j;				//ֻ��Ҫ��¼�ָ�����P2�ڼ��еڼ��� 
			}
			else
			{
				p2[p2_front].row = f;
				p2[p2_front].col = j;
				p2[p2_front].elem = P2->matrix[f][j];
				p2[p2_front].come = false;		//��¼û�е��ù� 
				p2_front++;
			}
		}
	}
	
	//ȥ���ظ�����ָ���������ȫ�ֳ���˳�������ÿ�г���˳��ɾ�� 
	int temp[F][J] = {0}; 
	int sep_idx[F] = {0};						//��¼�ָ���λ������ÿ�еĵ�ǰλ��
	for(int f = 0; f < F; f++)					//����P2���ݵ���ʱ���� 
		for(int j = 0; j < P2->full_len[f]; j++)
			temp[f][j] = P2->matrix[f][j];				
	for(int f = 0; f < F; f++)						
	{
		for(int j = 0; j < OS->full_len[f]; j++)
		{
			if(cross_1 <= j && j <= cross_2)
				continue;
			else
			{
				if(OS->matrix[f][j] == SEP)
				{
					temp[f][sep[f][sep_idx[f]++]] = 0;
				}
				else
				{
					for(int i = 0; i < p2_front; i++)
					{
						if(OS->matrix[f][j] == p2[i].elem && p2[i].come == false)
						{									 
							temp[p2[i].row][p2[i].col] = 0;
							p2[i].come = true;
						}
					}	
				}
			}
		}
	}
	
	//��p2ʣ�µ����OS�� 
	int count = 0;								//��¼ÿ����䵽�ĸ�Ԫ���� 
	for(int f = 0; f < F; f++)
	{
		for(int j = 0; j < P2->full_len[f]; j++)
		{
			if(temp[f][j] != 0)
			{
				if(count > width)				//����������˵���Ѿ����������������軻�� 
				{
					count = 0;
					break; 
				}
				OS->matrix[f][cross_1 + (count++)] = temp[f][j];
			}
		}
	}
	
	//���ԣ��鿴�����ɵĸ���
	OS->Display();
	cout<<"Makespan is "<<OS->GetMakespan()<<endl;
}

//void ICA_II::Revulotion(Country_II* c)
//{
//	int choose = rand() % 4;
//	switch(choose)
//	{
//		case 0 : Swap_1(c);
//		case 1 : Insert_1(c);
//		case 2 : Swap_2(c);
//		case 3 : Insert_2(c);
//		default: cout<<"Wrong revulotion"<<endl;
//	}
//} 

//void ICA_II::VNS(Country_II* c, int max_itr = 10)
//{
//	int makespan_best,makespan_temp;
//	
//	Country_II* c_best, temp;
//	c_best->Country_II(c);						//����ֱ����c���޸ģ�����Ҫ�ڿ������޸� 
//	c_best->Calculate();
//	makespan_best = c_best->GetMakespan();
//	for(int g = 0; g < 4; g++)					//��ֹ���� 
//	{
//		switch(g)
//		{
//			case 0 : Swap_1(c_best);
//			case 1 : Insert_1(c_best);
//			case 2 : Swap_2(c_best);
//			case 3 : Insert_2(c_best);
//		}
//		for(int i = 0; i < max_itr; i++)
//		{
//			temp->Country_II(c_best);
//			Swap_1(temp);
//			temp->Calculate();
//			makespan_temp = temp->GetMakespan();
//			if(makespan_temp < makespan_best)
//			{
//				c_best = temp;
//				makespan_best = makespan_temp;
//			}
//		}
//	}
//	return c_best;
//}
#endif
