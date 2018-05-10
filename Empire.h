/*
	Name: Empire.h
	Copyright: 2018 Wuhan University of Technology 
					all rights reserved  
	Author: LightersWang
	Date: 19/01/18 17:05
	Description: 
*/

#ifndef H_EMPIRE
#define H_EMPIRE

#include <iostream>
using namespace std;
#include "Country.h"

//Ĭ�ϲ���ʢ�ŵ۹��������Ĵ�С 
const int MAX_COUNTRIES = 100;

//��������ֳ��ص�Ӱ��ϵ��
const double a_cpt  = 0.1; 

class Empire_I{
	private:
		int    total_countries;									//��������
		int    best_index;											//����������
		int    number_e;											//�۹��ı�� 
		double total_power;
		
	public:
		Country_I** empire = NULL;								//ָ�����ָ�������ָ��,Ϊʲô��ʼ�����ܽ��empireָ�����͵����⣿  
		
		//START & END
		Empire_I() {}
		Empire_I(int n, int total_c);							//������ɣ�ûʲô�ã� 
		Empire_I(Country_I* c[], int len_c, int n);				//�������Ⱥ�����һ���۹� 
		~Empire_I();
		
		//GETTER
		int GetCountries();
		int GetBestIdx();
		int GetNumber_E();
		double GetTotalpower();
		Country_I* GetBestCountry();
		
		//SETTER
		void SetCountries(int total_c);
		void SetBestIdx(int best);
		void SetNumber_E(int n);
		void SetTotalpower(double p);							//��Ϊ�̳еĹ�ϵ����ֻ���ڳ�ʼ��ʱʹ�� 
		
		//FUNCTION 
		void Initialization(int n, int total_c);
		virtual void AddCountry(Country_I* cou);
		virtual void AddCountry(Country_I* cou[], int m);
		virtual void RemoveCountry(int idx);
		virtual void RemoveCountry(int idx_1, int idx_2);
		virtual void Display();
		virtual void Calculate();								//�����һ��ֵȲ�����������
		virtual void CalcuTotalpower();
		
};

void Empire_I::Initialization(int n, int total_c)
{
	this->SetBestIdx(-1);										//��ʾ��û������õ� 
	this->SetNumber_E(n);
	this->SetTotalpower(0.0); 
	this->SetCountries(total_c);
}

Empire_I::Empire_I(int n, int total_c)
{
	Initialization(n,total_c);
	this->empire = Build_Array<Country_I*>(MAX_COUNTRIES);		//Ϊ���������أ�������ÿ�ν���ָ�룬�ӿ��ٶ� 
}

Empire_I::Empire_I(Country_I* c[], int len_c, int n)
{
	Initialization(n,len_c);
	this->empire = Build_Array<Country_I*>(MAX_COUNTRIES);		//Ϊ���������أ�������ÿ�ν���ָ�룬�ӿ��ٶ� 
	for(int i = 0; i < len_c; i++)
	{
		empire[i] = c[i];
		if(empire[i]->GetMakespan() == -1) 
			empire[i]->Calculate();								//���û������������makespan
		empire[i]->SetNumber(i + 1);
	}
	this->Calculate();
}

Empire_I::~Empire_I()
{
	for(int c = 0; c < total_countries; c++)
		empire[c]->~Country_I();
	Destory_Array<Country_I*>(empire);
}

int Empire_I::GetCountries()
{
	return total_countries;
}

int Empire_I::GetBestIdx()
{
	return best_index;
}

Country_I* Empire_I::GetBestCountry()
{
	while(this->GetBestIdx() == -1)
	{
		this->Calculate();
	}
	return this->empire[best_index];
} 

int Empire_I::GetNumber_E()
{
	return number_e;
}

double Empire_I::GetTotalpower()
{
	return total_power;
}

void Empire_I::SetCountries(int total_c)
{
	this->total_countries = total_c;
}

void Empire_I::SetBestIdx(int best)
{
	this->best_index = best;
}

void Empire_I::SetNumber_E(int n)
{
	this->number_e = n;
}

void Empire_I::SetTotalpower(double p)
{
	this->total_power = p;
}

void Empire_I::AddCountry(Country_I* cou[], int m)  
{
	if(total_countries + m > MAX_COUNTRIES)
	{
		Country_I** temp = Build_Array<Country_I*>(total_countries + m);		//�¿���һ������ 
		for(int c = 0; c < total_countries + m; c++)
		{
			if(c < total_countries)
				temp[c] = this->empire[c];										//�Ȱ�ԭ�������ݸ��ƽ�ȥ
			else
			{
				temp[c] =  cou[c];
				temp[c]->SetNumber(c + 1);
			} 
		}
		this->total_countries += m;
		Swap<Country_I**>(temp,empire);											//����ָ��ָ������� 
		Destory_Array<Country_I*>(temp);
	}
	else
	{
		for(int i = 0; i < m; i++)
		{
			empire[total_countries] = cou[i];
			total_countries++;
		}
	}
	for(int i = this->GetCountries() - m; i < this->GetCountries(); i++)//û���� 
	{
		empire[i]->Calculate();
//		if(this->GetBestIdx() != -1 && empire[i]->GetMakespan() < GetBestCountry()->GetMakespan())
//			best_index = i;
//		else if(this->GetBestIdx() == -1)
//			best_index = i;
	}
	this->Calculate();
}

void Empire_I::AddCountry(Country_I* cou)
{
	if(this->total_countries + 1 > MAX_COUNTRIES)
	{
		Country_I** temp = Build_Array<Country_I*>(total_countries + 1);		//�¿���һ������ 
		for(int c = 0; c < total_countries + 1; c++)
		{
			temp[c] = (c < total_countries ? this->empire[c] : cou);			//�Ȱ�ԭ�������ݸ��ƽ�ȥ 
		}
		this->total_countries++;
		Swap<Country_I**>(temp,empire);											//����ָ��ָ������� 
		Destory_Array<Country_I*>(temp);
	}
	else
	{
		empire[total_countries] = cou;
		total_countries++;
	}
	empire[total_countries - 1]->Calculate();									//����makespan 
//	if(this->GetBestIdx() != -1 && empire[total_countries - 1]->GetMakespan() < 
//	empire[this->GetBestIdx()]->GetMakespan()) 							//�������Ĺ��Ҹ����ı�������
//		this->best_index = total_countries - 1;
//	else if(this->GetBestIdx() == -1)
//		this->best_index = total_countries - 1;
	this->Calculate();
} 

void Empire_I::RemoveCountry(int idx)
{
	int len_c = GetCountries();
	if(idx >= len_c)
	{
		cout<<"Wrong idx!"<<endl;
	}
	else
	{
		Country_I* drop = 
		TakeAway_Array<Country_I*>(empire, idx, len_c);
		this->SetCountries(len_c - 1);
		this->Calculate();
	}
}

void Empire_I::RemoveCountry(int idx_1, int idx_2)
{
	if(idx_1 == idx_2)								
	{
		RemoveCountry(idx_1);
	}
	else
	{
		int len_c = this->GetCountries();
		if(idx_1 >= len_c)
		{
			cout<<"Wrong idx_1!"<<endl;
		}
		else if(idx_2 >= len_c)
		{
			cout<<"Wrong idx_2!"<<endl;
		}
		else
		{
			if(idx_1 > idx_2)							//��֤1С��2���ȽϺò��� 
			{
				Swap<int>(idx_1,idx_2);
			}
			for(int i = idx_1; i <= idx_2; i++)
			{
				len_c = GetCountries();
				Country_I* drop = 
				TakeAway_Array<Country_I*>(empire, i, len_c);
				SetCountries(len_c - 1);
			}
			this->Calculate();
		}
	}
}

void Empire_I::Display()
{
	printf("Empire %d : \n",number_e);
	cout<<"Total Countries : "<<this->GetCountries()<<endl;
	cout<<"Imperialist : "<<this->GetBestCountry()->GetNumber()<<endl;
	cout<<"Colony : ";
	for(int c = 0; c < this->GetCountries(); c++)
	{
		if(c == this->GetBestIdx())
			continue;
		else
		{
			cout<<empire[c]->GetNumber();
			if(c < this->GetCountries() - 1)
				cout<<",";
			else
				cout<<endl;
		}
	}
	cout<<endl;
	for(int c = 0; c < total_countries; c++)
	{
		empire[c]->Display();
		cout<<"This Country is "<<(this->empire[c]->isColony() ? "Colony" : "Imperialist")<<endl;
		cout<<"Makespan is "<<this->empire[c]->GetMakespan()<<endl<<endl;
	}
}

void Empire_I::Calculate()
{
	best_index = 0;												//�ȼٶ���õ��ǵ�һ������ 
	for(int i = 1; i < total_countries; i++)					//�ҳ������� 
	{
		if(empire[i]->GetMakespan() < empire[best_index]->GetMakespan())
			best_index = i;
	}
	for(int c = 0; c < total_countries; c++)					//��ע�������� 
		empire[c]->SetAttribute(c == best_index ? "Imp" : "Col");
	CalcuTotalpower();
}

void Empire_I::CalcuTotalpower()
{
	double TC_col = 0.0;
	for(int c = 0; c < GetCountries(); c++)
	{
		if(c != GetBestIdx())
		{
			TC_col += empire[c]->GetMakespan();
		}
	} 
	TC_col       = a_cpt * TC_col / GetCountries();			//��ϵ������ƽ�� 
	total_power  = TC_col + GetBestCountry()->GetMakespan();	//��������ֳ���������� 
}

class Empire_II : public Empire_I
{
	public:
		Country_II** empire;
		
		//START & END
		Empire_II() {}
		Empire_II(int n, int total_c);
		Empire_II(Empire_I* e);
		~Empire_II();
		
		//FUNCTION
		Country_II* GetBestCountry();
		virtual void AddCountry(Country_II* cou);
		virtual void AddCountry(Country_II* cou[], int m);
		virtual void Calculate();
		virtual void Display();
};

Empire_II::Empire_II(int n, int total_c)
{
	Initialization(n, total_c);
	this->empire = Build_Array<Country_II*>(MAX_COUNTRIES);		//Ϊ���������أ�������ÿ�ν���ָ�룬�ӿ��ٶ� 
}

Empire_II::Empire_II(Empire_I* e)
{
	this->Initialization(e->GetNumber_E(), e->GetCountries());		//��ʼ��
	this->empire = Build_Array<Country_II*>(MAX_COUNTRIES);		//Ϊ���������أ�������ÿ�ν���ָ�룬�ӿ��ٶ� 
	for(int c = 0; c < this->GetCountries(); c++)
	{
		empire[c] = new Country_II(e->empire[c]);					//��ʼ���������� 
		empire[c]->Calculate();										//����makespan
	}
	this->Calculate();												//�ȼ������ĺ������� 
}

Empire_II::~Empire_II()
{
	for(int c = 0; c < this->GetCountries(); c++)
		empire[c]->~Country_II();
	Destory_Array<Country_II*>(empire);
}

Country_II* Empire_II::GetBestCountry()
{
	while(GetBestIdx() == -1)
	{
		this->Calculate();
	}
	return this->empire[GetBestIdx()];
} 

void Empire_II::AddCountry(Country_II* cou[], int m)
{
	int total_c = this->GetCountries();
	if(total_c + m > MAX_COUNTRIES)
	{
		Country_II** temp = Build_Array<Country_II*>(total_c + m);		//�¿���һ������ 
		for(int c = 0; c < total_c + m; c++)
		{
			if(c < total_c)
				temp[c] = this->empire[c];										//�Ȱ�ԭ�������ݸ��ƽ�ȥ
			else
			{
				temp[c] =  cou[c];
				temp[c]->SetNumber(c + 1);
			} 
		}
		this->SetCountries(total_c + m);
		Swap<Country_II**>(temp,empire);											//����ָ��ָ������� 
		Destory_Array<Country_II*>(temp);
	}
	else
	{
		for(int i = 0; i < m; i++)
		{
			empire[total_c] = cou[i];
			empire[total_c]->SetNumber(total_c + 1);	
			this->SetCountries(total_c + 1);
		}
	}
	total_c = this->GetCountries();
	for(int i = m; i > 0; i--)
	{
		empire[total_c - i]->Calculate();
		if(empire[total_c - i] < empire[this->GetBestIdx()])
			this->SetBestIdx(total_c + i);
	}
}

void Empire_II::AddCountry(Country_II* cou)
{
	int total_c = this->GetCountries();
	if(total_c + 1 > MAX_COUNTRIES)
	{
		Country_II** temp = Build_Array<Country_II*>(total_c + 1);		//�¿���һ������ 
		for(int c = 0; c < total_c + 1; c++)
		{
			temp[c] = (c < total_c ? this->empire[c] : cou);			//�Ȱ�ԭ�������ݸ��ƽ�ȥ 
		}
		this->SetCountries(total_c + 1);
		Swap<Country_II**>(temp,empire);											//����ָ��ָ������� 
		Destory_Array<Country_II*>(temp);
	}
	else
	{
		empire[total_c] = cou;
		this->SetCountries(total_c + 1); 
	}
	empire[total_c - 1]->SetNumber(total_c);					//���±�ţ��������Ѿ��ӹ���
	empire[total_c - 1]->Calculate();									//����makespan 
	if(empire[total_c - 1]->GetMakespan() < empire[this->GetBestIdx()]->GetMakespan()) 	//�ı�������
		this->SetBestIdx(total_c - 1);
}

void Empire_II::Calculate()
{
	this->SetBestIdx(0);												//�ȼٶ���õ��ǵ�һ������
	int total_c = this->GetCountries();
	int best_i = this->GetBestIdx(); 
	for(int i = 1; i < total_c; i++)					//�ҳ������� 
	{
		if(this->empire[i]->GetMakespan() < empire[best_i]->GetMakespan())
		{
			this->SetBestIdx(i);
			best_i = this->GetBestIdx();
		}
	}
	for(int c = 0; c < total_c; c++)					//��ע�������� 
		empire[c]->SetAttribute(c == best_i ? "Imp" : "Col");
}

void Empire_II::Display()
{
	printf("Empire %d : \n",this->GetNumber_E());
	for(int c = 0; c < this->GetCountries(); c++)
	{
		empire[c]->Display();
		cout<<"This Country is "<<(this->empire[c]->isColony() ? "Colony" : "Imperialist")<<endl;
		cout<<"Makespan is "<<this->empire[c]->GetMakespan()<<endl<<endl;
	}
}
#endif
