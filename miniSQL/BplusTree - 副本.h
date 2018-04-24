#ifndef _B_PLUS_TREE_H_
#define _B_PLUS_TREE_H_
#include "StdAdx.h"
typedef int Pointer;

template <class T>
class Node
{
public:
	//static int defaultMaxChildrens;
	int MaxChildrens;
	int NumElements;
	Pointer *Elements;
	Node *Father;
	Node **Next;
	Node *SiblingBefore,*SiblingNext;
	bool ifLeaf;
private:
	Node(){}
	Node(const Node &node){}
public:
	Node(int _MaxChildrens=defaultMaxChildrens) //value MaxChildren-1 
	{
		MaxChildrens=_MaxChildrens;
		NumElements=0;
		Elements=new Pointer[MaxChildrens];
		Next=new Node*[MaxChildrens+1];
		Father=NULL;SiblingBefore=NULL;SiblingNext=NULL;
		memset(Elements,0,sizeof(Pointer)*(MaxChildrens));
		memset(Next,0,sizeof(Node*)*(MaxChildrens+1));
		ifLeaf=true;
	}
	void Save(FILE *fp)
	{
		fwrite(&NumElements,sizeof(int),1,fp);
		fwrite(Elements,sizeof(Pointer),MaxChildrens,fp);
		fwrite(&Father,sizeof(Node*),1,fp);
		fwrite(Next,sizeof(Node*),MaxChildrens+1,fp);
		fwrite(&SiblingBefore,sizeof(Node*),1,fp);
		fwrite(&SiblingNext,sizeof(Node*),1,fp);
		fwrite(&ifLeaf,sizeof(bool),1,fp);
	}
	void Load(FILE *fp)
	{
		fread(&NumElements,sizeof(int),1,fp);
		fread(Value,sizeof(T),MaxChildren,fp);
		fread(Elements,sizeof(Pointer),MaxChildrens,fp);
		fread(&Father,sizeof(Node*),1,fp);
		fread(Next,sizeof(Node*),MaxChildrens+1,fp);
		fread(&SiblingBefore,sizeof(Node*),1,fp);
		fread(&SiblingNext,sizeof(Node*),1,fp);
		fread(&ifLeaf,sizeof(bool),1,fp);
	}
	~Node()
	{
		delete []Elements;
		delete []Next;
	}
};
//int Node::defaultMaxChildrens=5;

template <class T>
class BplusTree
{
private:
	typedef T (*pAccessFunc)(Pointer);
	pAccessFunc AccessFunc;
	Node *root;
	//Node qNode;
	int MaxChildrens;
public:
	void testprintA()
	{
		testprint(root,16);
	}
	void testprint(Node *pNode, int k)
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
		 Node *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
			tNode=tNode->Next[0];
		while (tNode!=NULL)
		{
			testFather(tNode);
			tNode=tNode->SiblingNext;
		}
	}
	void testFather(Node *pNode)
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
		Node *tNode=root;
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
		AccessFunc=NULL;
		root=NULL;
	}
	void SetAccessFunc(pAccessFunc _AccessFunc)
	{
		AccessFunc=_AccessFunc;
	}
	int FindFirstLargerThan(Node *pNode, Pointer pointer)
	{
		int i;
		T data=AccessFunc(pointer);
		for (i=0;i<pNode->NumElements;i++)
			if (AccessFunc(pNode->Elements[i])>=data)
				break;
		return i;
	}
	int FindEqual(Node *pNode, Pointer pointer)
	{
		int i;
		T data=AccessFunc(pointer);
		for (i=0;i<pNode->NumElements;i++)
			if (AccessFunc(pNode->Elements[i])==data)
				break;
		return i;
	}
	void MoveElements(Node *pNode, int i)
	{
		for (int k=MaxChildrens-1;k>=i+1;k--)
			pNode->Elements[k]=pNode->Elements[k-1];
	}
	void MoveNexts(Node *pNode, int i)
	{
		for (int k=MaxChildrens;k>=i+1;k--)
			pNode->Next[k]=pNode->Next[k-1];
	}
	Node* _find(Pointer pointer, int &index)
	{
		Node *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
		{
			int i=FindFirstLargerThan(tNode,pointer);
			if (i==MaxChildrens-1)
				tNode=tNode->Next[MaxChildrens-1];
			else 
			{
				if (AccessFunc(pointer)==AccessFunc(tNode->Elements[i]))
					tNode=tNode->Next[i+1];
				else
					tNode=tNode->Next[i];
			}
		}
		if (tNode!=NULL)
			index=FindFirstLargerThan(tNode,pointer);
		return tNode;
	}

	void insert(Pointer pointer)
	{
		Node *pNode;
		int i;
 		if (root==NULL)
		{
			root=new Node(MaxChildrens);
			pNode=root;i=0;
		}
		else pNode=_find(pointer,i);
		MoveElements(pNode,i);
		pNode->Elements[i]=pointer;
		pNode->NumElements++;
		if (pNode->NumElements==MaxChildrens)
			SplitLeafNode(pNode);
	}

	void SplitLeafNode(Node *pNode)
	{
		Node *father=pNode->Father;
		if (father==NULL)
		{
			father=new Node(MaxChildrens);
			father->ifLeaf=false;
			root=father;
		}
		Node *NewNode0=new Node(MaxChildrens);
		int half=MaxChildrens/2;
		NewNode0->NumElements=half;
		memcpy(NewNode0->Elements,pNode->Elements,sizeof(Pointer)*half);
		
		Node *NewNode1=new Node(MaxChildrens);
		NewNode1->NumElements=MaxChildrens-half;
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
		Pointer middle=pNode->Elements[half];
		delete pNode;
		int pos=FindFirstLargerThan(father,middle);
		MoveElements(father,pos);
		MoveNexts(father,pos+1);
		father->Elements[pos]=middle;
		father->Next[pos]=NewNode0;
		father->Next[pos+1]=NewNode1;
		father->NumElements++;
		if (father->NumElements==MaxChildrens)
			SplitNode(father);
	}
	
	void SplitNode(Node *pNode)
	{
		Node *father=pNode->Father;
		if (father==NULL)
		{
			father=new Node(MaxChildrens);
			father->ifLeaf=false;
			root=father;
		}
		Node *NewNode0=new Node(MaxChildrens); //MaxChildren elements, MaxChildren+1 nexts
		int half=(MaxChildrens-1)/2;
		NewNode0->NumElements=half;
		memcpy(NewNode0->Elements,pNode->Elements,sizeof(Pointer)*half);
		memcpy(NewNode0->Next,pNode->Next,sizeof(Node*)*(half+1));
		for (int i=0;i<=NewNode0->NumElements;i++)
			NewNode0->Next[i]->Father=NewNode0;

		Node *NewNode1=new Node(MaxChildrens);
		NewNode1->NumElements=MaxChildrens-1-half;
		memcpy(NewNode1->Elements,pNode->Elements+half+1,sizeof(Pointer)*(MaxChildrens-1-half));
		memcpy(NewNode1->Next,pNode->Next+half+1,sizeof(Node*)*(MaxChildrens-half));

		for (int i=0;i<=NewNode1->NumElements;i++)
			NewNode1->Next[i]->Father=NewNode1;

		NewNode0->ifLeaf=NewNode1->ifLeaf=false;
		NewNode0->Father=NewNode1->Father=father;

		Pointer middle=pNode->Elements[half];
		delete pNode;
		int pos=FindFirstLargerThan(father,middle);
		MoveElements(father,pos);
		MoveNexts(father,pos+1);
		father->Elements[pos]=middle;
		father->Next[pos]=NewNode0;father->Next[pos+1]=NewNode1;
		father->NumElements++;
		if (father->NumElements==MaxChildrens)
			SplitNode(father);
	}
	
	void Delete(Pointer pointer)
	{
		int i;
		if (root==NULL) return;
		Node *pNode=_find(pointer,i);
		delete_entry(pNode,pointer);
	}
	void delete_entry(Node *pNode, Pointer pointer)
	{
		int i=FindEqual(pNode,pointer);
		//int i=FindFirstLargerThan(pNode,pointer);
		if (i==pNode->NumElements) return;
		memcpy(pNode->Elements+i,pNode->Elements+i+1,sizeof(Pointer)*(MaxChildrens-1-i));
		memcpy(pNode->Next+i+1,pNode->Next+i+2,sizeof(Node*)*(MaxChildrens-1-i));
		//for (int k=i;k<MaxChildrens-1;k++)
		//	pNode->Elements[k]=pNode->Elements[k+1];
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
			Node *father=pNode->Father;
			Node *sibling;
			int k;
			for (k=0;k<=father->NumElements;k++)
				if (father->Next[k]==pNode)
					break;
			Pointer key;
			if (k==0)
			{
				key=father->Elements[k];
				sibling=father->Next[1];
			}
			else 
			{
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
						memcpy(pNode->Elements+pNode->NumElements,
							sibling->Elements,sizeof(Pointer)*sibling->NumElements);
						pNode->NumElements+=sibling->NumElements;
						pNode->SiblingNext=sibling->SiblingNext;
						if (sibling->SiblingNext!=NULL)
							sibling->SiblingNext->SiblingBefore=pNode;
						delete_entry(father,key);
						delete sibling;
					}
					else 
					{
						pNode->Elements[pNode->NumElements]=key;
						pNode->NumElements++;
						memcpy(pNode->Elements+pNode->NumElements,
							sibling->Elements,sizeof(Pointer)*sibling->NumElements);
						memcpy(pNode->Next+pNode->NumElements,
							sibling->Next,sizeof(Node*)*(sibling->NumElements+1));
						pNode->NumElements+=sibling->NumElements;
						for (int i=0;i<=pNode->NumElements;i++)
							pNode->Next[i]->Father=pNode;
						delete_entry(father,key);
						delete sibling;
					}
				}
				else //sibling---pNode
				{
					if (sibling->ifLeaf) 
					{
						memcpy(sibling->Elements+sibling->NumElements,
							pNode->Elements,sizeof(Pointer)*pNode->NumElements);
						sibling->NumElements+=pNode->NumElements;
						sibling->SiblingNext=pNode->SiblingNext;
						if (pNode->SiblingNext!=NULL)
							pNode->SiblingNext->SiblingBefore=sibling;
						delete_entry(father,key);
						delete pNode;
					}
					else 
					{
						sibling->Elements[sibling->NumElements]=key;
						sibling->NumElements++;
						memcpy(sibling->Elements+sibling->NumElements,
							pNode->Elements,sizeof(Pointer)*pNode->NumElements);
						memcpy(sibling->Next+sibling->NumElements,
							pNode->Next,sizeof(Node*)*(pNode->NumElements+1));
						sibling->NumElements+=pNode->NumElements;
						for (int i=0;i<=sibling->NumElements;i++)
							sibling->Next[i]->Father=sibling;
						delete_entry(father,key);
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
						Pointer kkey=sibling->Elements[sibling->NumElements-1];
						Node *children=sibling->Next[sibling->NumElements];
						memcpy(pNode->Elements+1,
							pNode->Elements,sizeof(Pointer)*pNode->NumElements);
						memcpy(pNode->Next+1,
							pNode->Next,sizeof(Node*)*(pNode->NumElements+1));
						pNode->Elements[0]=key;
						pNode->Next[0]=children;
						children->Father=pNode;
						pNode->NumElements++;
						sibling->Elements[sibling->NumElements-1]=0;//invalid
						sibling->Next[sibling->NumElements]=NULL;
						sibling->NumElements--;
						if (k==0)
							father->Elements[k]=kkey;
						else father->Elements[k-1]=kkey;
					}
					else
					{
						Pointer kkey=sibling->Elements[sibling->NumElements-1];
						sibling->Elements[sibling->NumElements-1]=0;
						sibling->NumElements--;
						memcpy(pNode->Elements+1,
							pNode->Elements,sizeof(Pointer)*pNode->NumElements);
						pNode->Elements[0]=kkey;
						pNode->NumElements++;
						if (k==0)
							father->Elements[k]=kkey;
						else father->Elements[k-1]=kkey;
					}
				}
				else //pNode---sibling
				{
					if (!sibling->ifLeaf)
					{
						Pointer kkey=sibling->Elements[0];
						Node *children=sibling->Next[0];
						pNode->NumElements++;
						pNode->Elements[pNode->NumElements-1]=key;
						pNode->Next[pNode->NumElements]=children;
						children->Father=pNode;
						memcpy(sibling->Elements,
							sibling->Elements+1,sizeof(Pointer)*(sibling->NumElements-1));
						memcpy(sibling->Next,
							sibling->Next+1,sizeof(Node*)*sibling->NumElements);

						sibling->Elements[sibling->NumElements-1]=0;//invalid
						sibling->Next[sibling->NumElements]=NULL;
						sibling->NumElements--;

						if (k==0)
							father->Elements[k]=kkey;//sibling->Elements[0];
						else father->Elements[k-1]=kkey;//sibling->Elements[0];
					}
					else
					{
						Pointer kkey=sibling->Elements[0];
						pNode->NumElements++;
						pNode->Elements[pNode->NumElements-1]=kkey;
						memcpy(sibling->Elements,
							sibling->Elements+1,sizeof(Pointer)*(sibling->NumElements-1));
						sibling->Elements[sibling->NumElements-1]=0;//invalid
						sibling->NumElements--;
						if (k==0)
							father->Elements[k]=sibling->Elements[0];
						else father->Elements[k-1]=sibling->Elements[0];
					}
				}
			}
		}
	}
	void update(Pointer pointer_old, Pointer pointer_new)
	{
		Delete(pointer_old);
		insert(pointer_new);
	}
	void printAll()
	{
 		Node *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
			tNode=tNode->Next[0];
		while (tNode!=NULL)
		{
			for (int i=0;i<tNode->NumElements;i++)
				std::cout<<AccessFunc(tNode->Elements[i])<<" ";
			tNode=tNode->SiblingNext;
		}
	}
	void printAllreverse()
	{
 		Node *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
			tNode=tNode->Next[tNode->NumElements];
		while (tNode!=NULL)
		{
			for (int i=0;i<tNode->NumElements;i++)
				std::cout<<AccessFunc(tNode->Elements[i])<<" ";
			tNode=tNode->SiblingBefore;
		}
	}
	int printAll(std::ofstream &fout)
	{
		int count=0;
 		Node *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
			tNode=tNode->Next[0];
		while (tNode!=NULL)
		{
			for (int i=0;i<tNode->NumElements;i++)
			{
				count++;
				fout<<AccessFunc(tNode->Elements[i])<<" ";
			}
			tNode=tNode->SiblingNext;
		}
		fout<<std::endl;
		fout<<"count="<<count<<std::endl;
		return count;
	}
	bool find(Pointer pointer)
	{
		Node *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
		{
			int i=FindFirstLargerThan(tNode,pointer);
			if (i==MaxChildrens-1)
				tNode=tNode->Next[MaxChildrens-1];
			else 
			{
				if (AccessFunc(pointer)==AccessFunc(tNode->Elements[i]))
					tNode=tNode->Next[i+1];
				else
					tNode=tNode->Next[i];
			}
		}
		if (tNode!=NULL)
		{
			if(FindEqual(tNode,pointer)==tNode->NumElements)
			return false;
			else return true;
		}
	}
	bool find(T data, Pointer &pointer)
	{
		Node *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
		{
			int i;
			for (i=0;i<tNode->NumElements;i++)
				if (AccessFunc(tNode->Elements[i])>=data)
					break;
			if (i==MaxChildrens-1)
				tNode=tNode->Next[MaxChildrens-1];
			else 
			{
				if (data==AccessFunc(tNode->Elements[i]))
					tNode=tNode->Next[i+1];
				else
					tNode=tNode->Next[i];
			}
		}
		if (tNode!=NULL)
		{
			int i;
			for (i=0;i<tNode->NumElements;i++)
				if (AccessFunc(tNode->Elements[i])==data)
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
	int findBetween(T data1, bool CanEqual1, T data2, bool CanEqual2, std::vector<Pointer> &pointers)
	{
		Node *tNode=root;
		while (tNode!=NULL && !tNode->ifLeaf)
		{
			int i;
			for (i=0;i<tNode->NumElements;i++)
				if (AccessFunc(tNode->Elements[i])>=data1)
					break;
			if (i==MaxChildrens-1)
				tNode=tNode->Next[MaxChildrens-1];
			else 
			{
				if (data1==AccessFunc(tNode->Elements[i]))
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
					if (AccessFunc(tNode->Elements[i])>=data1)
						break;
				}
				else
				{
					if (AccessFunc(tNode->Elements[i])>data1)
						break;
				}

			for (int k=i;k<tNode->NumElements;k++)
			{
				if (CanEqual2)
				{
					if (tNode->Elements[k]>data2) {break;}
				}
				else
				{
					if (tNode->Elements[k]>=data2) {break;}
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
						if (tNode->Elements[k]>data2) {out=true;break;}
					}
					else
					{
						if (tNode->Elements[k]>=data2) {out=true;break;}
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
	int CalNodeNum(Node *pNode)
	{
		if (pNode==NULL) return 0;
		int ans=1;
		for (int i=0;i<=pNode->NumElements;i++)
			ans+=CalNodeNum(pNode->Next[i]);
		return ans;
	}
	void DoSave(Node *pNode, FILE *fp)
	{
		if (pNode==NULL) return;
		fwrite(&pNode,sizeof(Node*),1,fp);
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
		Node **tmp;
		Node **old;
		tmp=new Node*[tot];
		old=new Node*[tot];
		for (int i=0;i<tot;i++)
		{
			tmp[i]=new Node(MaxChildrens);
			fread(&old[i],sizeof(Node*),1,fp);
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