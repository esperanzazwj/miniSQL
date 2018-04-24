#ifndef _B_PLUS_TREE_H_
#define _B_PLUS_TREE_H_
#include "StdAdx.h"
typedef int Pointer;

template <class T>
class Node
{
public:
	int MaxChildrens;
	int NumElements;
	T *Values;
	Pointer *Elements;
	Node<T> *Father;
	Node<T> **Next;
	Node<T> *SiblingBefore,*SiblingNext;
	bool ifLeaf;
private:
	Node(){}
	Node(const Node &node){}
public:
	Node(int _MaxChildrens=defaultMaxChildrens) //value MaxChildren-1 
	{
		MaxChildrens=_MaxChildrens;
		NumElements=0;
		Values=new T[MaxChildrens];
		Elements=new Pointer[MaxChildrens];
		Next=new Node<T>*[MaxChildrens+1];
		Father=NULL;SiblingBefore=NULL;SiblingNext=NULL;
		memset(Elements,0,sizeof(Pointer)*(MaxChildrens));
		memset(Next,0,sizeof(Node<T>*)*(MaxChildrens+1));
		ifLeaf=true;
	}
	void Save(FILE *fp)
	{
		fwrite(&NumElements,sizeof(int),1,fp);
		fwrite(Values,sizeof(T),MaxChildren,fp);
		fwrite(Elements,sizeof(Pointer),MaxChildrens,fp);
		fwrite(&Father,sizeof(Node<T>*),1,fp);
		fwrite(Next,sizeof(Node<T>*),MaxChildrens+1,fp);
		fwrite(&SiblingBefore,sizeof(Node<T>*),1,fp);
		fwrite(&SiblingNext,sizeof(Node<T>*),1,fp);
		fwrite(&ifLeaf,sizeof(bool),1,fp);
	}
	void Load(FILE *fp)
	{
		fread(&NumElements,sizeof(int),1,fp);
		fread(Values,sizeof(T),MaxChildren,fp);
		fread(Elements,sizeof(Pointer),MaxChildrens,fp);
		fread(&Father,sizeof(Node<T>*),1,fp);
		fread(Next,sizeof(Node<T>*),MaxChildrens+1,fp);
		fread(&SiblingBefore,sizeof(Node<T>*),1,fp);
		fread(&SiblingNext,sizeof(Node<T>*),1,fp);
		fread(&ifLeaf,sizeof(bool),1,fp);
	}
	~Node()
	{
		//delete []Values;
		delete []Elements;
		delete []Next;
	}
};

template <class T>
class BplusTree
{
private:
	Node<T> *root;
	int MaxChildrens;
public:
	void testprintA()
	{
		testprint(root,16);
	}
	void testprint(Node<T> *pNode, int k)
	{
		if (pNode==NULL) return;
		using namespace std;
		for (int i=0;i<pNode->NumElements;i++)
		{
			testprint(pNode->Next[i],k-4);
			for (int j=0;j<k;j++) cout<<" ";
			cout<<pNode->Elements[i]<<endl;
		}
		testprint(pNode->Next[pNode->NumElements],k-4);
	}
	void testFatherA()
	{
		 Node<T> *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
			tNode=tNode->Next[0];
		while (tNode!=NULL)
		{
			testFather(tNode);
			tNode=tNode->SiblingNext;
		}
	}
	void testFather(Node<T> *pNode)
	{
		using namespace std;
		while (pNode!=NULL)
		{
			cout<<pNode->Elements[0]<<"->";
			pNode=pNode->Father;
		}
		cout<<"end"<<endl;
	}
	int testTot()
	{
		using namespace std;
		int count=0;
		Node<T> *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
			tNode=tNode->Next[0];
		while (tNode!=NULL)
		{
			count+=tNode->NumElements;
			tNode=tNode->SiblingNext;
		}
		cout<<"total: "<<count<<endl;
		return count;
	}
	BplusTree(int _MaxChildrens)
	{
		MaxChildrens=_MaxChildrens;
		root=NULL;
	}
	int FindFirstLargerThan(Node<T> *pNode, T &data)
	{
		int i;
		for (i=0;i<pNode->NumElements;i++)
			if (pNode->Values[i]>=data)
				break;
		return i;
	}
	int FindEqual(Node<T> *pNode, T &data)
	{
		int i;
		for (i=0;i<pNode->NumElements;i++)
			if (pNode->Values[i]==data)
				break;
		return i;
	}
	void MoveElements(Node<T> *pNode, int i)
	{
		for (int k=MaxChildrens-1;k>=i+1;k--)
		{
			pNode->Values[k]=pNode->Values[k-1];
			pNode->Elements[k]=pNode->Elements[k-1];
		}
	}
	void MoveNexts(Node<T> *pNode, int i)
	{
		for (int k=MaxChildrens;k>=i+1;k--)
			pNode->Next[k]=pNode->Next[k-1];
	}
	Node<T>* _find(T &data, int &index)
	{
		Node<T> *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
		{
			int i=FindFirstLargerThan(tNode, data);
			if (i==tNode->NumElements)
				tNode=tNode->Next[tNode->NumElements];
			else 
			{
				if (data==tNode->Values[i])
					tNode=tNode->Next[i+1];
				else
					tNode=tNode->Next[i];
			}
		}
		if (tNode!=NULL)
			index=FindFirstLargerThan(tNode, data);
		return tNode;
	}

	void insert(T &data, Pointer pointer)
	{
		Node<T> *pNode;
		int i;
 		if (root==NULL)
		{
			root=new Node<T>(MaxChildrens);
			pNode=root;i=0;
		}
		else pNode=_find(data,i);
		MoveElements(pNode,i);
		pNode->Values[i]=data;
		pNode->Elements[i]=pointer;
		pNode->NumElements++;
		if (pNode->NumElements==MaxChildrens)
			SplitLeafNode(pNode);
	}

	void SplitLeafNode(Node<T> *pNode)
	{
		Node<T> *father=pNode->Father;
		if (father==NULL)
		{
			father=new Node<T>(MaxChildrens);
			father->ifLeaf=false;
			root=father;
		}
		Node<T> *NewNode0=new Node<T>(MaxChildrens);
		int half=MaxChildrens/2;
		NewNode0->NumElements=half;
		memcpy(NewNode0->Values,pNode->Values,sizeof(T)*half);
		memcpy(NewNode0->Elements,pNode->Elements,sizeof(Pointer)*half);
		
		Node<T> *NewNode1=new Node<T>(MaxChildrens);
		NewNode1->NumElements=MaxChildrens-half;
		memcpy(NewNode1->Values,pNode->Values+half,sizeof(T)*(MaxChildrens-half));
		memcpy(NewNode1->Elements,pNode->Elements+half,sizeof(Pointer)*(MaxChildrens-half));

		NewNode0->Father=NewNode1->Father=father;
		NewNode0->SiblingBefore=pNode->SiblingBefore;
		NewNode1->SiblingBefore=NewNode0;
		NewNode0->SiblingNext=NewNode1;
		NewNode1->SiblingNext=pNode->SiblingNext;
		if (pNode->SiblingBefore!=NULL)
			pNode->SiblingBefore->SiblingNext=NewNode0;
		if (pNode->SiblingNext!=NULL)
			pNode->SiblingNext->SiblingBefore=NewNode1;
		T middleData=pNode->Values[half];
		Pointer middle=pNode->Elements[half];
		delete pNode;
		int pos=FindFirstLargerThan(father,middleData);
		MoveElements(father,pos);
		MoveNexts(father,pos+1);
		father->Values[pos]=middleData;
		father->Elements[pos]=middle;
		father->Next[pos]=NewNode0;
		father->Next[pos+1]=NewNode1;
		father->NumElements++;
		if (father->NumElements==MaxChildrens)
			SplitNode(father);
	}
	
	void SplitNode(Node<T> *pNode)
	{
		Node<T> *father=pNode->Father;
		if (father==NULL)
		{
			father=new Node<T>(MaxChildrens);
			father->ifLeaf=false;
			root=father;
		}
		Node<T> *NewNode0=new Node<T>(MaxChildrens); //MaxChildren elements, MaxChildren+1 nexts
		int half=(MaxChildrens-1)/2;
		NewNode0->NumElements=half;
		memcpy(NewNode0->Values,pNode->Values,sizeof(T)*half);
		memcpy(NewNode0->Elements,pNode->Elements,sizeof(Pointer)*half);
		memcpy(NewNode0->Next,pNode->Next,sizeof(Node<T>*)*(half+1));
		for (int i=0;i<=NewNode0->NumElements;i++)
			NewNode0->Next[i]->Father=NewNode0;

		Node<T> *NewNode1=new Node<T>(MaxChildrens);
		NewNode1->NumElements=MaxChildrens-1-half;
		memcpy(NewNode1->Values,pNode->Values+half+1,sizeof(T)*(MaxChildrens-1-half));
		memcpy(NewNode1->Elements,pNode->Elements+half+1,sizeof(Pointer)*(MaxChildrens-1-half));
		memcpy(NewNode1->Next,pNode->Next+half+1,sizeof(Node<T>*)*(MaxChildrens-half));

		for (int i=0;i<=NewNode1->NumElements;i++)
			NewNode1->Next[i]->Father=NewNode1;

		NewNode0->ifLeaf=NewNode1->ifLeaf=false;
		NewNode0->Father=NewNode1->Father=father;

		T middleData=pNode->Values[half];
		Pointer middle=pNode->Elements[half];
		delete pNode;
		int pos=FindFirstLargerThan(father,middleData);
		MoveElements(father,pos);
		MoveNexts(father,pos+1);
		father->Values[pos]=middleData;
		father->Elements[pos]=middle;
		father->Next[pos]=NewNode0;father->Next[pos+1]=NewNode1;
		father->NumElements++;
		if (father->NumElements==MaxChildrens)
			SplitNode(father);
	}
	
	void Delete(T &data)
	{
		int i;
		if (root==NULL) return;
		Node<T> *pNode=_find(data,i);
		if (pNode==NULL)
		{
			
			std::cout<<"current:\n";
			testTot();
			system("pause");
			_find(data,i);
			return;
		}
		delete_entry(pNode,data);
	}
	void delete_entry(Node<T> *pNode, T &data)
	{
		int i=FindEqual(pNode,data);
		if (i==pNode->NumElements) return;
		memcpy(pNode->Values+i,pNode->Values+i+1,sizeof(T)*(MaxChildrens-1-i));
		memcpy(pNode->Elements+i,pNode->Elements+i+1,sizeof(Pointer)*(MaxChildrens-1-i));
		memcpy(pNode->Next+i+1,pNode->Next+i+2,sizeof(Node<T>*)*(MaxChildrens-1-i));
		pNode->NumElements--;
		if (pNode==root && pNode->NumElements==0)
		{
			root=pNode->Next[0];
			if (root!=NULL) root->Father=NULL;
			delete pNode;
			return;
		}
		if (pNode!=root && ((pNode->ifLeaf && pNode->NumElements<(MaxChildrens/2) ||
			(!pNode->ifLeaf && pNode->NumElements<(MaxChildrens-1)/2))))
		{
			Node<T> *father=pNode->Father;
			Node<T> *sibling;
			int k;
			for (k=0;k<=father->NumElements;k++)
				if (father->Next[k]==pNode)
					break;
			T keyData;
			Pointer key;
			if (k==0)
			{
				keyData=father->Values[0];
				key=father->Elements[0];
				sibling=father->Next[1];
			}
			else 
			{
				keyData=father->Values[k-1];
				key=father->Elements[k-1];
				sibling=father->Next[k-1];
			}
			if ((pNode->ifLeaf && pNode->NumElements+sibling->NumElements<=MaxChildrens-1) ||
				(!pNode->ifLeaf && pNode->NumElements+sibling->NumElements<=MaxChildrens-2))
			{
				
				if (k==0) //pNode----sibling
				{
					if (pNode->ifLeaf) 
					{
						memcpy(pNode->Values+pNode->NumElements,
							sibling->Values,sizeof(T)*sibling->NumElements);
						memcpy(pNode->Elements+pNode->NumElements,
							sibling->Elements,sizeof(Pointer)*sibling->NumElements);
						pNode->NumElements+=sibling->NumElements;
						pNode->SiblingNext=sibling->SiblingNext;
						if (sibling->SiblingNext!=NULL)
							sibling->SiblingNext->SiblingBefore=pNode;
						delete_entry(father,keyData);
						delete sibling;
					}
					else 
					{
						pNode->Values[pNode->NumElements]=keyData;
						pNode->Elements[pNode->NumElements]=key;
						pNode->NumElements++;
						memcpy(pNode->Values+pNode->NumElements,
							sibling->Values,sizeof(T)*sibling->NumElements);
						memcpy(pNode->Elements+pNode->NumElements,
							sibling->Elements,sizeof(Pointer)*sibling->NumElements);
						memcpy(pNode->Next+pNode->NumElements,
							sibling->Next,sizeof(Node<T>*)*(sibling->NumElements+1));
						pNode->NumElements+=sibling->NumElements;
						for (int i=0;i<=pNode->NumElements;i++)
							pNode->Next[i]->Father=pNode;
						delete_entry(father,keyData);
						delete sibling;
					}
				}
				else //sibling---pNode
				{
					if (sibling->ifLeaf) 
					{
						memcpy(sibling->Values+sibling->NumElements,
							pNode->Values,sizeof(T)*pNode->NumElements);
						memcpy(sibling->Elements+sibling->NumElements,
							pNode->Elements,sizeof(Pointer)*pNode->NumElements);
						sibling->NumElements+=pNode->NumElements;
						sibling->SiblingNext=pNode->SiblingNext;
						if (pNode->SiblingNext!=NULL)
							pNode->SiblingNext->SiblingBefore=sibling;
						delete_entry(father,keyData);
						delete pNode;
					}
					else 
					{
						sibling->Values[sibling->NumElements]=keyData;
						sibling->Elements[sibling->NumElements]=key;
						sibling->NumElements++;
						memcpy(sibling->Values+sibling->NumElements,
							pNode->Values,sizeof(T)*pNode->NumElements);
						memcpy(sibling->Elements+sibling->NumElements,
							pNode->Elements,sizeof(Pointer)*pNode->NumElements);
						memcpy(sibling->Next+sibling->NumElements,
							pNode->Next,sizeof(Node<T>*)*(pNode->NumElements+1));
						sibling->NumElements+=pNode->NumElements;
						for (int i=0;i<=sibling->NumElements;i++)
							sibling->Next[i]->Father=sibling;
						delete_entry(father,keyData);
						delete pNode;
					}
				}
			}
			else 
			{
				if (k!=0) //sibling---pNode
				{
					if (!pNode->ifLeaf)
					{
						T kkeyData=sibling->Values[sibling->NumElements-1];
						Pointer kkey=sibling->Elements[sibling->NumElements-1];
						Node<T> *children=sibling->Next[sibling->NumElements];
						memcpy(pNode->Values+1,
							pNode->Values,sizeof(T)*pNode->NumElements);
						memcpy(pNode->Elements+1,
							pNode->Elements,sizeof(Pointer)*pNode->NumElements);
						memcpy(pNode->Next+1,
							pNode->Next,sizeof(Node<T>*)*(pNode->NumElements+1));
						pNode->Values[0]=keyData;
						pNode->Elements[0]=key;
						pNode->Next[0]=children;
						children->Father=pNode;
						pNode->NumElements++;
						//need clear value
						sibling->Elements[sibling->NumElements-1]=0;//invalid
						sibling->Next[sibling->NumElements]=NULL;
						sibling->NumElements--;
						if (k==0)
						{
							father->Values[k]=kkeyData;
							father->Elements[k]=kkey;
						}
						else 
						{
							father->Values[k-1]=kkeyData;
							father->Elements[k-1]=kkey;
						}
					}
					else
					{
						T kkeyData=sibling->Values[sibling->NumElements-1];
						Pointer kkey=sibling->Elements[sibling->NumElements-1];
						//need clear value
						sibling->Elements[sibling->NumElements-1]=0;
						sibling->NumElements--;
						memcpy(pNode->Values+1,
							pNode->Values,sizeof(T)*pNode->NumElements);
						memcpy(pNode->Elements+1,
							pNode->Elements,sizeof(Pointer)*pNode->NumElements);
						pNode->Values[0]=kkeyData;
						pNode->Elements[0]=kkey;
						pNode->NumElements++;
						if (k==0)
						{
							father->Values[k]=kkeyData;
							father->Elements[k]=kkey;
						}
						else
						{
							father->Values[k-1]=kkeyData;
							father->Elements[k-1]=kkey;
						}
					}
				}
				else //pNode---sibling
				{
					if (!sibling->ifLeaf)
					{
						T kkeyData=sibling->Values[0];
						Pointer kkey=sibling->Elements[0];
						Node<T> *children=sibling->Next[0];
						pNode->NumElements++;
						pNode->Values[pNode->NumElements-1]=keyData;
						pNode->Elements[pNode->NumElements-1]=key;
						pNode->Next[pNode->NumElements]=children;
						children->Father=pNode;
						memcpy(sibling->Values,
							sibling->Values+1,sizeof(T)*(sibling->NumElements-1));
						memcpy(sibling->Elements,
							sibling->Elements+1,sizeof(Pointer)*(sibling->NumElements-1));
						memcpy(sibling->Next,
							sibling->Next+1,sizeof(Node<T>*)*sibling->NumElements);
						//need clear value
						sibling->Elements[sibling->NumElements-1]=0;//invalid
						sibling->Next[sibling->NumElements]=NULL;
						sibling->NumElements--;

						if (k==0)
						{
							father->Values[k]=kkeyData;
							father->Elements[k]=kkey;//sibling->Elements[0];
						}
						else 
						{
							father->Values[k-1]=kkeyData;
							father->Elements[k-1]=kkey;//sibling->Elements[0];
						}
					}
					else
					{
						T kkeyData=sibling->Values[0];
						Pointer kkey=sibling->Elements[0];
						pNode->NumElements++;
						pNode->Values[pNode->NumElements-1]=kkeyData;
						pNode->Elements[pNode->NumElements-1]=kkey;
						memcpy(sibling->Values,
							sibling->Values+1,sizeof(T)*(sibling->NumElements-1));
						memcpy(sibling->Elements,
							sibling->Elements+1,sizeof(Pointer)*(sibling->NumElements-1));
						//need clear value
						sibling->Elements[sibling->NumElements-1]=0;//invalid
						sibling->NumElements--;
						if (k==0)
						{
							father->Values[k]=sibling->Values[0];
							father->Elements[k]=sibling->Elements[0];
						}
						else
						{
							father->Values[k-1]=sibling->Values[0];
							father->Elements[k-1]=sibling->Elements[0];
						}
					}
				}
			}
		}
	}
	void update(T &data_old, T &data_new, Pointer pointer_new)
	{
		Delete(data_old);
		insert(data_new, pointer_new);
	}
	int printAll()
	{
		int count=0;
 		Node<T> *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
			tNode=tNode->Next[0];
		while (tNode!=NULL)
		{
			for (int i=0;i<tNode->NumElements;i++)
			{
				count++;
				std::cout<<tNode->Values[i]<<" ";
			}
			tNode=tNode->SiblingNext;
		}
		std::cout<<std::endl;
		std::cout<<"count="<<count<<std::endl;
		return count;
	}
	int printAllreverse()
	{
		int count=0;
 		Node<T> *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
			tNode=tNode->Next[tNode->NumElements];
		while (tNode!=NULL)
		{
			for (int i=0;i<tNode->NumElements;i++)
			{
				count++;
				std::cout<<tNode->Values[i]<<" ";
			}
			tNode=tNode->SiblingBefore;
		}
		std::cout<<std::endl;
		std::cout<<"count="<<count<<std::endl;
		return count;
	}
	int printAll(std::ofstream &fout)
	{
		int count=0;
 		Node<T> *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
			tNode=tNode->Next[0];
		while (tNode!=NULL)
		{
			for (int i=0;i<tNode->NumElements;i++)
			{
				count++;
				fout<<tNode->Values[i]<<" ";
			}
			tNode=tNode->SiblingNext;
		}
		fout<<std::endl;
		fout<<"count="<<count<<std::endl;
		return count;
	}
	bool find(T &data)
	{
		Node<T> *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
		{
			int i=FindFirstLargerThan(tNode,data);
			if (i==MaxChildrens-1)
				tNode=tNode->Next[MaxChildrens-1];
			else 
			{
				if (data==tNode->Values[i])
					tNode=tNode->Next[i+1];
				else
					tNode=tNode->Next[i];
			}
		}
		if (tNode!=NULL)
		{
			if(FindEqual(tNode,data)==tNode->NumElements)
			return false;
			else return true;
		}
	}
	bool find(T &data, Pointer &pointer)
	{
		Node<T> *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
		{
			int i;
			for (i=0;i<tNode->NumElements;i++)
				if (tNode->Values[i]>=data)
					break;
			if (i==MaxChildrens-1)
				tNode=tNode->Next[MaxChildrens-1];
			else 
			{
				if (data==tNode->Values[i])
					tNode=tNode->Next[i+1];
				else
					tNode=tNode->Next[i];
			}
		}
		if (tNode!=NULL)
		{
			int i;
			for (i=0;i<tNode->NumElements;i++)
				if (tNode->Values[i]==data)
					break;
			if (i==tNode->NumElements)
			{
				pointer=0;return false;
			}
			else
			{
				pointer=tNode->Elements[i];return true;
			}
		}
		else {pointer=0;return false;}
	}
	int findBetween(T &data1, bool CanEqual1, T &data2, bool CanEqual2, std::vector<Pointer> &pointers)
	{
		Node<T> *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
		{
			int i;
			for (i=0;i<tNode->NumElements;i++)
				if (tNode->Values[i]>=data1)
					break;
			if (i==MaxChildrens-1)
				tNode=tNode->Next[MaxChildrens-1];
			else 
			{
				if (data1==tNode->Values[i])
					tNode=tNode->Next[i+1];
				else
					tNode=tNode->Next[i];
			}
		}
		pointers.clear();
		int count=0;
		if (tNode!=NULL)
		{
			int i;
			for (i=0;i<tNode->NumElements;i++)
				if (CanEqual1)
				{
					if (tNode->Values[i]>=data1)
						break;
				}
				else
				{
					if (tNode->Values[i]>data1)
						break;
				}

			for (int k=i;k<tNode->NumElements;k++)
			{
				if (CanEqual2)
				{
					if (tNode->Values[k]>data2) {break;}
				}
				else
				{
					if (tNode->Values[k]>=data2) {break;}
				}
				pointers.push_back(tNode->Elements[k]);
				count++;
			}
			tNode=tNode->SiblingNext;
			bool out=false;
			while (tNode!=NULL && !out)
			{
				for (int k=0;k<tNode->NumElements;k++)
				{
					if (CanEqual2)
					{
						if (tNode->Values[k]>data2) {out=true;break;}
					}
					else
					{
						if (tNode->Values[k]>=data2) {out=true;break;}
					}
					pointers.push_back(tNode->Elements[k]);
					count++;
				}
				tNode=tNode->SiblingNext;
			}
		}
		return count;
	}
	void Save(const char *filename)
	{
		FILE *fp=fopen(filename,"wb");
		fwrite(&MaxChildrens,sizeof(int),1,fp);
		int tot=CalNodeNum(root);
		fwrite(&tot,sizeof(int),1,fp);
		DoSave(root,fp);
		fclose(fp);
	}
	int CalNodeNum(Node<T> *pNode)
	{
		if (pNode==NULL) return 0;
		int ans=1;
		for (int i=0;i<=pNode->NumElements;i++)
			ans+=CalNodeNum(pNode->Next[i]);
		return ans;
	}
	void DoSave(Node<T> *pNode, FILE *fp)
	{
		if (pNode==NULL) return;
		fwrite(&pNode,sizeof(Node<T>*),1,fp);
		pNode->Save(fp);
		for (int i=0;i<=pNode->NumElements;i++)
			DoSave(pNode->Next[i],fp);
	}

	void Load(const char *filename)
	{
		FILE *fp=fopen(filename,"rb");
		fread(&MaxChildrens,sizeof(int),1,fp);
		int tot;
		fread(&tot,sizeof(int),1,fp);
		if (tot==0)
		{
			root=NULL;
			return;
		}
		Node<T> **tmp;
		Node<T> **old;
		tmp=new Node<T>*[tot];
		old=new Node<T>*[tot];
		for (int i=0;i<tot;i++)
		{
			tmp[i]=new Node<T>(MaxChildrens);
			fread(&old[i],sizeof(Node<T>*),1,fp);
			tmp[i]->Load(fp);
		}
		for (int i=0;i<tot;i++)
		{
			for (int k=0;k<tot;k++)
				if (tmp[i]->Father==old[k])
				{
					tmp[i]->Father=tmp[k];
					break;
				}
			for (int j=0;j<=tmp[i]->NumElements;j++)
 				for (int k=0;k<tot;k++)
					if (tmp[i]->Next[j]==old[k])
					{
						tmp[i]->Next[j]=tmp[k];
						break;
					}
			for (int k=0;k<tot;k++)
				if (tmp[i]->SiblingBefore==old[k])
				{
					tmp[i]->SiblingBefore=tmp[k];
					break;
				}
			for (int k=0;k<tot;k++)
				if (tmp[i]->SiblingNext==old[k])
				{
					tmp[i]->SiblingNext=tmp[k];
					break;
				}
		}
		root=tmp[0];
		delete []tmp;
		delete []old;
	}
};
#endif