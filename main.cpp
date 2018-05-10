///*
//	Name: main.cpp
//	Copyright: 2018 Wuhan University of Technology 
//					all rights reserved 
//	Author: LightersWang 
//	Date: 21/01/18 13:22
//*/
//
//#include <fstream>
//using namespace std;
//#include <stdlib.h>
//#include <time.h>
//#include <direct.h>
//
//#include "Country.h"
//#include "Empire.h"
//#include "ICA.h"
//#include "GA_test.cpp"
//
//const int BEST_SOLUTION = 114; 
//
//const int GOOD_SOLUTION = 130;
//
//const int MID_SOLUTION  = 160; 
//
//const int TOTAL_TEST    = 1000;
//
//const int RUNTIMES      = 20;
//
//int main()
//{  
//	//setting seeds
//	srand((unsigned)time(NULL));
//	
//	//generate example
//	Generate_Example(10, 5, 5, 100, 70, 100);
//	
//	//creating data folder
//	string curPath = "E:/智能优化/分布式并行机调度/ICA_DPMS_C++/Experiment";
//	string p,factory,jobs;
//	int2str(F,factory);
//	int2str(J,jobs);
//	p = "/" + factory + "F" + jobs + "J";
//	string dir = curPath + p;
//    _mkdir(dir.c_str()); 
//    
//	//creating data and example
//	ofstream data;    data.open(dir + "/data.dat");
//	data<<"The lower bound is "<<Get_LowerBound()<<endl<<endl;
//	ofstream example; example.open(dir + "/example.dat");
//	Output_example_ICA(example);
//	
//	//GA input example
//	
//	
//	//main procedure(just for testing)
//	data<<"ICA : "<<endl;
//	int best_fit[20];
//	int total_g = 0;
//	for(int i = 0; i < RUNTIMES; i++)
//	{
//		double fit_aver = 0;
//		int generation_I = 1;
//		ICA_I* stage_1 = new ICA_I;
//		while(stage_1->GetLen() > 1)
//		{
//			cout<<"Generation "<<generation_I<<" : "<<endl;
//			stage_1->Assimilation_FirstO();
//			stage_1->Revulotion();
//			stage_1->Assimilation_SecondO();
//			stage_1->Competition();
//			stage_1->EliminateEmpire();
//			stage_1->Display();
//			generation_I++;
////			int total = 0;
////			for(int i = 0; i < stage_1->GetLen(); i++)
////			{
////				total += stage_1->egroup[i]->GetCountries();
////			}
////			cout<<"total : "<<total<<endl<<endl;
////			fit_aver += stage_1->GetGOAT()->GetMakespan();
////			data<<stage_1->GetGOAT()->GetMakespan()<<endl;
//		}
//		stage_1->GetGOAT()->Display_sep();
//		
//		if(i = 0)
//			best_fit[i] = stage_1->GetGOAT()->GetMakespan();
//		else if(stage_1->GetGOAT()->GetMakespan() < best_fit[i - 1])
//			best_fit[i] = stage_1->GetGOAT()->GetMakespan();
//		total_g += generation_I;
//		fit_aver /= generation_I;
//		data<<"aver of fitness : "<<fit_aver<<endl;
//	}
//	for(int j= 0; j < RUNTIMES; j++)
//	{
//		
//	}
//	for(int i = 0; i < 2; i++)
//		data<<endl<<"best fit : "<<best_fit[i]<<endl;
//}
