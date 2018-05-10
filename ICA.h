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

//人口数量 
const int N_pop = 100;

//帝国数量 
const int N_imp = 10;

//殖民地数量 
const int N_col = N_pop - N_imp;

const double P1_rev = 0.3;

typedef struct
{
	int row;									//原矩阵的行坐标 
	int col;									//原矩阵的列坐标 
	int elem;									//原矩阵的元素内容 
	bool come;									//标记是否到访过，方便禁忌搜索 
} Table; 	

class ICA_I{
	private:
		int         len_e;							//长度（所有帝国个数） 
		long long   crowd_idx;						//累积国家个数（好编号)
		Country_I*  GOAT;							//Greatest Of All Time
		Country_I*  crowd[10000 * N_pop]; 			//国家池 

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
		virtual void Assimilation_FirstO();						//正常的同化，发生在同一个帝国中 
		virtual void Assimilation_SecondO();					//二阶同化，发生在所有宗主国之中 
		virtual void Revulotion();
		virtual void Competition();
		virtual void EliminateEmpire();
};

ICA_I::ICA_I()
{	
	//私有变量初始化 
	this->crowd_idx = 1;
	this->len_e = 0;
	for(int e = 0; e < N_imp; e++)
	{
		egroup[e] = new Empire_I(e + 1, 0);
		len_e++;
	}
	GOAT = new Country_I;
	GOAT->SetNumber(10000);
	
	//生成N_pop个国家
	double makespan_temp[N_pop];							//记录makespan，便于后期标准化 
	for(int c = 0; c < N_pop; c++)
	{
		crowd[c] = new Country_I(crowd_idx++);
		crowd[c]->Calculate();
		makespan_temp[c] = crowd[c]->GetMakespan();
	}

	//按照makespan从小到大的顺序对crowd内的国家进行排序 
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

//	//测试：makespan 
//	Print_Array<double>(makespan_temp,N_pop);

	//标准化所有国家的makespan，并求所有宗主国makespan的和 
	double sum_imp = 0.0;
	for(int i = 0; i < N_pop; i++)
	{
		makespan_temp[i] -= makespan_temp[N_pop - 1];
		if(i < N_imp)
		{
			sum_imp += makespan_temp[i];	
		}
	}
	
//	//测试：标准化makespan和宗主国总和 
//	Print_Array<double>(makespan_temp,N_pop); 
//	cout<<"sum of imps is "<<sum_imp<<endl;

	//确定国家属性 
	for(int c = 0; c < N_pop; c++)
		crowd[c]->SetAttribute(c < N_imp ? "Imp" : "Col");
	
	//求宗主国的势力 
	double power[N_imp];
	for(int i = 0; i < N_imp; i++)
	{
		power[i] = fabs(makespan_temp[i] / sum_imp);
	}
	int* NC = this->NumberColonies(power); 
	int sum_nc = 0; 

	//测试：检查分配情况 
//	Print_Array<int>(NC,N_imp);
//	for(int i = 0; i < N_imp; i++)
//	{
//		sum_nc += NC[i];
//	}
//	cout<<endl<<"Total nc are "<<sum_nc<<" (should be "<<N_col<<")"<<endl;
	
	//按照势力大小给宗主国分配殖民地 				
	int tabu[N_col] = {0};										//以前选过的殖民地都放到这里面，以后不选 
	int len_t  = 0;												//记录禁忌列表到的第几个，改成队列会简单很多 
	for(int c = 0; c < N_imp; c++)
	{
		egroup[c]->AddCountry(crowd[c]);
		sum_nc += NC[c];
		for(int i = 0; i < NC[c]; i++)
		{
			int col_i = TabuSearch(tabu, len_t);
			//测试：查看国家情况 
//			cout<<endl<<"Empire "<<egroup[c]->GetNumber_E()<<endl; 
//			crowd[col_i]->Display();
			egroup[c]->AddCountry(crowd[col_i]);				//添加当前殖民地到对应帝国中 
		}
	}
	
	//找出山羊
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
	if(all == 'Y' || all == 'y')			//所有国家的工件分布全部显示 
	{
		for(int e = 0; e < len_e; e++)
		{
			egroup[e]->Display();
			cout<<endl;
		}
	}
	if(goat == 'y' || goat == 'Y')			//显示最好的一个解	
	{
		this->DisplayGOAT();
	}
}

int ICA_I::TabuSearch(int (&tabu)[N_col], int& len_t)
{
	bool flag = false;									//结束标记，如果生成的在禁忌列表中就正确，重新再来 
	int col_i;											//随机被选取的殖民地的索引 
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
	tabu[len_t++] = col_i;								//添加当前殖民地标号到禁忌列表 
	return col_i;
}

int* ICA_I::IdxRevCol(Empire_I* emp, int amount_rev)
{
	int  len_t = 0;
	int* number_rev = Build_Array<int>(amount_rev);
	for(int i = 0; i < amount_rev; i++)
	{
		bool flag = false;									//结束标记，如果生成的在禁忌列表中就正确，重新再来 
		int col_i;											//随机被选取的殖民地对应的在帝国数组中的下标 
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
		number_rev[len_t++] = col_i;								//添加当前殖民地标号到禁忌列表
	}
	return number_rev;
}

int* ICA_I::NumberColonies(const double power[])
{
	int* NC = Build_Array<int>(N_imp);
	int zeros[N_imp], len_z = 0;									//记录为0的索引 
	int sum_nc = 0;
	
	//初始生成 
	for(int e = 0; e < N_imp; e++)
	{
		NC[e] = round(power[e] * (double)N_col);
		sum_nc += NC[e];
		if(NC[e] == 0)
		{
			zeros[len_z++] = e;
		}
	}
	
	//去掉为0的 
	int ind;
	for(int i = 0; i < len_z; i++)
	{
		NC[zeros[i]] = 1;										//先让为0的国家至少有一个
		do
		{
			ind = rand() % N_imp;
		}
		while(NC[ind] < 1);
		NC[ind]--;												//然后随机生成一个国家让其殖民地数目少一个 
	}
	
	//总数检查
	int diff = sum_nc - N_col;
	if(diff > 0)
	{
		for(int i = N_imp - 1; i >= N_imp - diff; i--)			//大于0就从最少的那几个国家减少（多样性） 
			NC[i]--;
	}
	else if(diff < 0)
	{
		for(int i = 0; i < abs(diff); i++)						//小于0就从最多的那几个国家增加（多样性） 
			NC[i]++;
	}
	
	return NC;
}

void ICA_I::CrossOver(Country_I* P1, Country_I* P2, Country_I* OS)
{
	//确定交叉位置的边界：找第一个国家最短的 
	int bound = Get_Min<int>(P1->len,F);
	
	//生成两个交叉位置
	int cross_1, cross_2, width;
	Generate_2random(cross_1, cross_2, bound, bound);	
	if(cross_1 > cross_2)										//保证cross_1永远小于cross_2 
	{	
		Swap<int>(cross_1,cross_2);
	}
	width = cross_2 - cross_1 + 1;								//交叉区域的宽度（cross_1到cross_2）

	//将P1交叉区域外部复制到子代中 
	OS->SetLen(P1->len);
	for(int f = 0; f < F; f++)
	{
		for(int j = 0; j < OS->len[f]; j++)
		{
			OS->matrix[f][j] = (cross_1 <= j && j <= cross_2 ? 0 : P1->matrix[f][j]);
		}
	}

	//将P2的内容整理到一张表上
	Table p2[J];												//记录p2中元素的位置和是否访问过
	int p2_front = 0;											//记录p2到了哪个位置
	for(int f = 0; f < F; f++)
	{
		for(int j = 0; j < P2->len[f]; j++)
		{
			p2[p2_front].row = f;
			p2[p2_front].col = j;
			p2[p2_front].elem = P2->matrix[f][j];
			p2[p2_front].come = false;							//记录有没有访问过
			p2_front++;
		}
	}

	//标记重复的项 
	for(int f = 0; f < F; f++)
	{
		for(int j = 0; j < OS->len[f]; j++)
		{
			if(cross_1 <= j && j <= cross_2) 			//如果在交叉区内部就跳过这次循环
				continue;
			else										//否则就在p2表中查
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
	
	//找出剩下的 
	int re_len = width * F;
	int remain[re_len], re_idx = 0;
	for(int j = 0; j < J; j++)
	{
		if(p2[j].come == false)
		{
			remain[re_idx++] = p2[j].elem;
		}
	}
	
	//把p2剩下的填进OS中
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
	int f1 = Get_Max_Index<double>(c->GetProTime(), F);		//第一个工厂对应makespan
	int f2 = Generate_difrdm(f1, F);						//第二个工厂不能和第一个相等
	int j1, j2;
	Generate_2random(j1, j2, c->len[f1], c->len[f2]);		//产生两个随机位置的工件 
	Swap<int>(c->matrix[f1][j1], c->matrix[f2][j2]);
	c->Calculate();
}

void ICA_I::Insert_1_rdm(Country_I* c)
{
	int f_ori = rand() % F;
	int j_ori = rand() % c->len[f_ori];									//随机确定初始工件 
	int f_ins = Generate_difrdm(f_ori, F);
	int j_ins = rand() % c->len[f_ins];									//确定目标工件 
	int elem  = TakeAway_Array<int>(c->matrix[f_ori], j_ori, c->len[f_ori]--);	
	Insert_Array<int>(c->matrix[f_ins], elem, j_ins, c->len[f_ins]++);
	c->Calculate();
}

void ICA_I::Insert_1_ptime(Country_I* c)
{
	int f_ori = Get_Max_Index<double>(c->GetProTime(), F);						//初始工厂为makespan对应 
	int j_ori = rand() % c->len[f_ori];											//随机确定初始工件	
	int f_ins = Get_Min_Index<double>(c->GetProTime(), F);						//目标工厂为时间最短的工厂 
	int j_ins = rand() % c->len[f_ins]; 
	int elem  = TakeAway_Array<int>(c->matrix[f_ori], j_ori, c->len[f_ori]--);
	Insert_Array<int>(c->matrix[f_ins], elem, j_ins, c->len[f_ins]++);
	c->Calculate();
}

void ICA_I::Insert_1_JSP(Country_I* c)
{
	int f_ori = Get_Max_Index<double>(c->GetProTime(), F);						//初始工厂为makespan对应 
	int j_ori = rand() % c->len[f_ori];											//随机确定初始工件	
	double JSP[F];																//工件-工速积 
	for(int f = 0; f < F; f++)
	{
		JSP[f] = (J - c->len[f]) * Speed[f]; 
	}
	int f_ins = Get_Max_Index<double>(JSP, F);									//目标工厂为工件个数最少 
	int j_ins = rand() % c->len[f_ins]; 
	int elem  = TakeAway_Array<int>(c->matrix[f_ori], j_ori, c->len[f_ori]--);
	Insert_Array<int>(c->matrix[f_ins], elem, j_ins, c->len[f_ins]++);
	c->Calculate();
}

void ICA_I::Assimilation_Op(Empire_I* emp)
{
	//第一步：将子代添加入帝国 
	int len_c = emp->GetCountries(), crowd_memo = crowd_idx;
	Country_I* imper = emp->GetBestCountry();						//单独挑出来，因为所有国家都要跟它交叉
	for(int c = 0; c < len_c; c++)
	{
		if(emp->empire[c] != emp->GetBestCountry())					//避免宗主国自己和自己交叉 
		{
			crowd[crowd_idx] = new Country_I;						//产生空子代并加入国家池中 
			crowd[crowd_idx]->SetNumber(crowd_idx);					//累积产生的国家编号 
			this->CrossOver(imper, emp->empire[c], crowd[crowd_idx]);				 
			crowd[crowd_idx]->Calculate();
			emp->AddCountry(crowd[crowd_idx]);						//将产生的子代添加进帝国
			crowd_idx++;
		}
	}
	
	//第二步：给数组内部按照makespan从小到大排序
	for (int i = 0; i < emp->GetCountries() - 1; i++)
	{
		for (int j = 0; j < emp->GetCountries() - i - 1; j++)
    	{
    		if(emp->empire[j]->GetMakespan() > 
				emp->empire[j + 1]->GetMakespan())
				Swap<Country_I*>(emp->empire[j],emp->empire[j + 1]);
		}
	}
    
	//第三步：取前len_c个,标记一下宗主国殖民地
	int last_idx = emp->GetCountries() - 1;
	emp->RemoveCountry(len_c, last_idx);	//总数为2l-1，索引要再减1 
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
		crowd[crowd_idx] = new Country_I;						//产生空子代并加入国家池中 
		crowd[crowd_idx]->SetNumber(crowd_idx);					//累积产生的国家编号 
//		egroup[i]->GetBestCountry()->Display();
		this->CrossOver(GOAT, egroup[i]->GetBestCountry(), crowd[crowd_idx]);		
//		crowd[crowd_idx]->Display();		 
		crowd[crowd_idx]->Calculate();
		if(egroup[i]->GetBestCountry()->GetMakespan() 
					> crowd[crowd_idx]->GetMakespan())			//只接受更好的解 
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
		//确定革命国家的个数 
		int  amount_rev = P1_rev * (egroup[e]->GetCountries() - 1);
		int* number_rev = this->IdxRevCol(egroup[e], amount_rev); 
		
		//确定哪些国家革命
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
	//按照势力从小到大排序
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
	
	//标准化总势力、计算相对势力 
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
		double p = fabs(TC[i] / sum_tc);					//相对势力
		double r = (rand() + 1.0) / (RAND_MAX + 2.0);		//随机扰动
		D[i] = p - r;
	}
	
	//割让殖民地 
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
			egroup[e]->RemoveCountry(egroup[e]->GetBestIdx());				//在本帝国内删除
			egroup[e]->Calculate();
			GetBestEmp()->AddCountry(temp);									//添加到最强的帝国，加快收敛 
			GetBestEmp()->Calculate();
			TakeAway_Array<Empire_I*>(egroup, e, GetLen());					//删除一个帝国（不能析构）
			this->len_e--;
			break;
		}
	}
}

class ICA_II: public ICA_I
{
	private: 
		long long    crowd_idx;						//累积国家个数（好编号)
		Country_II*  GOAT;							//Greatest Of All Time
		Country_II*  crowd[10000 * N_pop]; 			//国家池 
		
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
	//初始化变量 
	this->SetLen(0);
	this->crowd_idx = 0;
	for(int e = 0; e < N_imp; e++)
	{
		egroup[e] = new Empire_II(e + 1, 0);
		this->SetLen('+');
	}
	GOAT = new Country_II;
	GOAT->SetNumber(10000);
	
	//按照上个阶段的最优解产生N_pop个解 
	double makespan_temp[N_pop];
	for(int i = 0; i < N_pop; i++)
	{
		crowd[crowd_idx] = new Country_II(goat, "random");
		crowd[crowd_idx]->SetNumber(crowd_idx + 1);						//编号比索引大1 
		crowd[crowd_idx]->Calculate();
		makespan_temp[crowd_idx] = crowd[crowd_idx]->GetMakespan();
		crowd_idx++;
	} 
	
	//按照makespan从小到大的顺序对crowd内的国家进行排序 
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

	//标准化所有国家的makespan，并求所有宗主国makespan的和 
	double sum_imp = 0.0;
	for(int i = 0; i < N_pop; i++)
	{
		makespan_temp[i] -= makespan_temp[N_pop - 1];
		if(i < N_imp)
		{
			sum_imp += makespan_temp[i];	
		}
	}

	//确定国家属性 
	for(int c = 0; c < N_pop; c++)
		crowd[c]->SetAttribute(c < N_imp ? "Imp" : "Col");
	
	//求宗主国的势力 
	double power[N_imp];
	for(int i = 0; i < N_imp; i++)
	{
		power[i] = fabs(makespan_temp[i] / sum_imp);
	}
	int* NC = this->NumberColonies(power); 
	int sum_nc = 0; 
	
	//按照势力大小给宗主国分配殖民地 				
	int tabu[N_col] = {0};										//以前选过的殖民地都放到这里面，以后不选 
	int len_t  = 0;												//记录禁忌列表到的第几个，改成队列会简单很多 
	for(int c = 0; c < N_imp; c++)
	{
		egroup[c]->AddCountry(crowd[c]);
		sum_nc += NC[c];
		for(int i = 0; i < NC[c]; i++)
		{
			int col_i = TabuSearch(tabu, len_t);
			//测试：查看国家情况 
//			cout<<endl<<"Empire "<<egroup[c]->GetNumber_E()<<endl; 
//			crowd[col_i]->Display();
			egroup[c]->AddCountry(crowd[col_i]);				//添加当前殖民地到对应帝国中 
		}
	}
	
	//找出山羊
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
	int f1 = Get_Max_Index<double>(c->GetProTime(), F);				//第一个工厂对应makespan
	int f2 = Generate_difrdm(f1, F);								//第二个工厂不能和第一个相等
	int j1, j2;
	do
	{
		Generate_2random(j1, j2, c->len[f1], c->len[f2]);			//产生两个随机位置的工件 	
	}
	while(c->matrix[f1][j1] == SEP || c->matrix[f2][j2] == SEP);	//交换的工件不能是分隔符 
	Swap<int>(c->matrix[f1][j1], c->matrix[f2][j2]);
	c->Calculate();
}

void ICA_II::Swap_2(Country_II* c)
//交换同一工厂不同机器的工件 
{
	int f = rand() % F;					//随机选择一个工厂，或者不随机？？ 
	int pos[Factory_Machine[f]];					//记录分隔符的位置
	int rear_pos = 0;
	for(int p = 0; p < c->len[f] - Factory_Machine[f] + 1; p++)
		if(c->matrix[f][p] == SEP)
			pos[rear_pos++] = p;
	int m1,m2;
	Generate_2random(m1,m2,Factory_Machine[f],Factory_Machine[f]);		//产生两个要进行交换的机器 
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
	Generate_2random(p1,p2,b1,b2,a1,a2);			//产生要交换的工件 
	Swap<int>(c->matrix[f][p1],c->matrix[f][p2]);
}

void ICA_II::Insert_1_JSP(Country_II* c)
{
	int f_ori = Get_Max_Index<double>(c->GetProTime(), F), j_ori;				//初始工厂为makespan对应
	do
	{
		j_ori = rand() % c->len[f_ori];											//随机确定初始工件
	}
	while(c->matrix[f_ori][j_ori] == SEP); 	
	double JSP[F];																//工件-工速积 
	for(int f = 0; f < F; f++)
	{
		JSP[f] = (J - c->len[f]) * Speed[f]; 
	}
	int f_ins = Get_Max_Index<double>(JSP, F), j_ins;							//目标工厂为工件个数最少
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
//同一工厂内的工件移动到另一机器上 
{
	int f = rand() % F;					//随机选择一个工厂，或者不随机？？ 
	int pos[Factory_Machine[f]];					//记录分隔符的位置
	int rear_pos = 0;
	for(int p = 0; p < c->len[f] - Factory_Machine[f] + 1; p++)
		if(c->matrix[f][p] == SEP)
			pos[rear_pos++] = p;
	int m1,m2;
	Generate_2random(m1,m2,Factory_Machine[f],Factory_Machine[f]);		//产生两个要进行插入的机器
	int p1 = rand() % (pos[m1] - pos[m1 - 1] - 1) + pos[m1 - 1] + 1;	//不可能是SEP的 
	int temp;
	temp = TakeAway_Array<int>(c->matrix[f], p1, c->len[f]--); 
	Insert_Array<int>(c->matrix[f], temp, p1, c->len[f]++);
} 

void ICA_II::CrossOver(Country_II* P1,Country_II* P2)				
{
	//初始化 
	int b[2];
	b[0] = Get_Max<int>(P1->len,F);				//找最大 
	b[1] = Get_Max<int>(P2->len,F);
	int bound = Get_Min<int>(b,2);				//最大中找最小 
	
	//生成两个交叉位置 
	int cross_2,cross_1 = rand() % bound; 
	do
	{
		cross_2 = rand() % bound;
	}
	while(cross_1 >= cross_2);
	int width = cross_2 - cross_1 + 1;
	
	//将P1交叉区外部复制到子代中 
	Country_II* OS = new Country_II; 			
	OS->SetFullLen(P1->full_len);
	for(int f = 0; f < F; f++)					//包括了分隔符 
	{
		for(int j = 0; j < OS->full_len[f]; j++)
		{
			OS->matrix[f][j] = (cross_1 <= j && j <= cross_2 ? 0 : P1->matrix[f][j]);
		}
	}
	
	//将P2的内容整理到一张表上 
	Table p2[J];								//记录p2中元素的位置和是否访问过						
	int p2_front = 0;							//记录p2到了哪个位置
	int sep[F][M];								//按照工厂机器单独记录分隔符的位置 
	int idx = 0;
	for(int f = 0; f < F; f++)
	{
		for(int j = 0; j < P2->full_len[f]; j++)
		{
			if(P2->matrix[f][j] == SEP)			//单独记录分隔符				
			{
				sep[f][idx++] = j;				//只需要记录分隔符在P2第几行第几列 
			}
			else
			{
				p2[p2_front].row = f;
				p2[p2_front].col = j;
				p2[p2_front].elem = P2->matrix[f][j];
				p2[p2_front].come = false;		//记录没有到访过 
				p2_front++;
			}
		}
	}
	
	//去除重复的项，分隔符不按照全局出现顺序而按照每行出现顺序删除 
	int temp[F][J] = {0}; 
	int sep_idx[F] = {0};						//记录分隔符位置数组每行的当前位置
	for(int f = 0; f < F; f++)					//复制P2内容到临时矩阵 
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
	
	//把p2剩下的填进OS中 
	int count = 0;								//记录每行填充到哪个元素了 
	for(int f = 0; f < F; f++)
	{
		for(int j = 0; j < P2->full_len[f]; j++)
		{
			if(temp[f][j] != 0)
			{
				if(count > width)				//满足条件则说明已经超过交叉区，则需换行 
				{
					count = 0;
					break; 
				}
				OS->matrix[f][cross_1 + (count++)] = temp[f][j];
			}
		}
	}
	
	//测试：查看新生成的个体
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
//	c_best->Country_II(c);						//不能直接在c上修改，所以要在拷贝上修改 
//	c_best->Calculate();
//	makespan_best = c_best->GetMakespan();
//	for(int g = 0; g < 4; g++)					//终止条件 
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
