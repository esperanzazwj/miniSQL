#ifndef _interpret_h
#define _interpret_h
#include "StdAdx.h"
#include "Base.h"
using namespace std;

class Interpret {
public:
	//������,���е���������Է�ӳ������һ������
	int m_operation;		//Ҫִ�еĲ�����������,�ú��ʾ
	string m_tabname;		//Ҫ�����ı����
	string m_indname;		//Ҫ������������
	string m_colname;
	vector<Attribute> column;
	vector<Condition> condition;			//Ҫ�Ƚϵ�where�־������
	Row row;		//Ҫ�����ֵ����
	Table getTableInfo;
	Index getIndexInfo;
	int PrimaryKeyPosition;
	int UniquePostion;
	vector<Condition> UniqueCondition;
	//static CCatalogManager Catalog;
	friend void Execute();
	Interpret(){
		m_operation =UNKNOW;
		m_tabname = "";
		m_indname = "";
		m_colname="";
		PrimaryKeyPosition=-1;	
		UniquePostion=-1;
	}
	~Interpret(){}

	void Parse(char* command);
	/*void ExecSelect();
	void ExecDelete();
	void ExecDropTable();
	void ExecDropIndex();
	void ExecCreateTable();
	void ExecCreateIndex();
	void ExecInsert();*/
	//void Execute();
	void initcol();
	void initcond();
	void initvalue();
	void inittable(){
		getTableInfo.name="";
		getTableInfo.attriNum=getTableInfo.blockNum=getTableInfo.totalLength=0;
		getTableInfo.attributes.clear();
	}
	void initindex(){
		getIndexInfo.blockNum=getIndexInfo.column=-1;
		getIndexInfo.index_name="";
		//getIndexInfo.table_name="";
	}
	bool GetWord(string & src, string & des);

protected:
	
	void MakeInitilate(){
		m_operation =UNKNOW;
		m_tabname = "";
		m_indname = "";
		initcol();
		initcond();
		initvalue();
		inittable();
		initindex();
	}

	//bool GetWord(string & src, string & des);
	bool GetStr(string & src, string & des);

	short int IsInt(const char *);
	short int IsFloat(char *input);
};

#endif
