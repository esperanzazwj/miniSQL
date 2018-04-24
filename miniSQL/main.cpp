#include "BplusTree.h"

#include <time.h>
#include <string>
using namespace std;
string a[50000];
int v[50000];
int main()
{
		BplusTree<int> bpt(4);
	for (int i=0;i<100;i++)
	{
		int k=i*2;
		bpt.insert(k,i);
	}
	for (int i=0;i<10;i++)
	{
		int k=i*2;
		bpt.Delete(k);
	}
	vector<Pointer> ptrs;
	int left=2;int right=40;
	bpt.findBetween(left,true,right,false,ptrs); 
	for (int i=0;i<ptrs.size();i++)
		cout<<ptrs[i]<<endl;

}
/*
int main()
{
	ifstream fin("testdata.txt");
	//ofstream dfout("testdata.txt");
	ofstream fout("testout.txt");
	srand((int)time(NULL));
	BplusTree<string> bpt(4);
	for (int i=0;i<100;i++)
	{
		int k;
		//int k=rand()/(double)(RAND_MAX+1)*50000;
		//while (v[k])
		//	k=rand()/(double)(RAND_MAX+1)*50000;
		//v[k]=1;
		fin>>k;
		char xx[100];
		sprintf(xx,"%d",k);
		a[i]=string(xx);
		//fin>>a[i];
		//dfout<<k<<" "<<endl;
	}
	//dfout.close();
	for (int i=0;i<100;i++)
	{
		bpt.insert(a[i],i);
		bpt.printAll();
	}
	//
	int old=bpt.testTot();
	for (int i=0;i<10;i++)
	{
		bpt.Delete(a[i]);
	//	int newv=bpt.testTot();
	/*	if (old-newv!=1)
		{
			cout<<"warning"<<endl;
			system("pause");
		}
		old=newv;
	}

	bpt.testTot();
	bpt.printAll(fout);
	system("pause");
}*/