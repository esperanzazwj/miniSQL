#ifndef __CatalogManager_h__
#define __CatalogManager_h__
#include "StdAdx.h"
#include "Base.h"

/*实现的功能： 
 *在程序运行之前把catalog信息读入内存 
 *在程序结束之前把catalog信息读入硬盘
 */
class CatalogManager{
private:
	vector<Table> tables;
	int tableNum;          //the number of tables in the database, should be equels to tables.size()
	vector<Index> indexes;                	//索引列表
	int indexNum;                   	//索引数目
private:
	void InitialTalbeCatalog(){
		const string filename = "table.catlog";
		fstream  fin(filename.c_str(), ios::in);
		fin >> tableNum;
		for(int i = 0; i < tableNum; i++)
		{//fill in the vector of tables
			Table temp_table;
			fin >> temp_table.name;
			fin >> temp_table.attriNum;
			fin >> temp_table.blockNum;
			for(int j = 0; j < temp_table.attriNum; j++)
			{//fill in the vector of temp_table.attributes
				Attribute temp_attri;
		 		fin >> temp_attri.name;
		 		fin >> temp_attri.type;
		 		fin >> temp_attri.length;
		 		fin >> temp_attri.isPrimeryKey;
		 		fin >> temp_attri.isUnique;
		 		temp_table.attributes.push_back(temp_attri);
		 		temp_table.totalLength += temp_attri.length;
		 	}
		 	tables.push_back(temp_table);
		}
		fin.close();
	}
	
	void InitialIndexCatalog(){
		const string filename = "index.catlog";
		fstream  fin(filename.c_str(), ios::in);
		fin >> indexNum;
		for(int i = 0; i < indexNum; i++)
		{//fill in the vector of indexes
			Index temp_index;
			fin >> temp_index.index_name;
			fin >> temp_index.table_name;
			fin >> temp_index.column;
			fin >> temp_index.columnLength;
			fin >> temp_index.blockNum;
		 	indexes.push_back(temp_index);
		}
		fin.close();
	}
	
	void StoreTalbeCatalog(){
		string filename = "table.catlog";
		fstream  fout(filename.c_str(), ios::out);
		
		fout << tableNum << endl;
		for(int i = 0; i < tableNum; i++)
		{
			fout << tables[i].name << " ";
			fout << tables[i].attriNum << " ";
			fout << tables[i].blockNum << endl;
			
			for(int j = 0; j < tables[i].attriNum; j++)
			{
				fout << tables[i].attributes[j].name << " ";
				fout << tables[i].attributes[j].type << " ";
				fout << tables[i].attributes[j].length << " ";
				fout << tables[i].attributes[j].isUnique  << " ";
				fout << tables[i].attributes[j].isPrimeryKey  << endl;
		 	}
		}
		fout.close(); 
	}

	void StoreIndexCatalog(){
		string filename = "index.catlog";
		fstream  fout(filename.c_str(), ios::out);
		fout << indexNum << endl;
		for(int i = 0; i < indexNum; i++)
		{
			fout << indexes[i].index_name << " ";
			fout << indexes[i].table_name << " ";
			fout << indexes[i].column << " ";
			fout << indexes[i].columnLength << " ";
			fout << indexes[i].blockNum << endl;
		}
		fout.close(); 
	}
public: 
	CatalogManager(){
		InitialTalbeCatalog();
		InitialIndexCatalog();
	}
	
	~CatalogManager(){
		StoreTalbeCatalog();
		StoreIndexCatalog();
	}

	void createTable(Table& table){
		tableNum++;
		for(int i = 0; i < table.attributes.size(); i++){
			table.totalLength += table.attributes[i].length;
		}
		tables.push_back(table);
	}

	void createIndex(Index index){
		indexNum++;
		indexes.push_back(index);
	}
		
	void dropTable(Table table){
		dropTable(table.name);
	}
	
	void dropIndex(Index index){
		dropIndex(index.index_name);
	}
	
	void dropTable(string tablename){
		for(int i = tableNum -1; i >= 0; i--){
			if(tables[i].name == tablename){
				tables.erase (tables.begin()+i);
				tableNum--;
			}
		}
		for(int i = indexNum - 1; i >= 0; i--){//表被删除了，建在这个表上的所有的index也要跟着删除
			if(indexes[i].table_name == tablename){
				indexes.erase (indexes.begin()+i);
				indexNum--;
			}
		}
	}
	
	void dropIndex(string index_name){
		for(int i = indexNum - 1; i >= 0; i--){
			if(indexes[i].index_name == index_name){
				indexes.erase (indexes.begin()+i);
				indexNum--;
			}
		}
	}
	
	void update(Table& tableinfor){
		for(int i = 0; i < tableNum; i++){
			if(tables[i].name == tableinfor.name){
				tables[i].attriNum = tableinfor.attriNum;
				tables[i].blockNum = tableinfor.blockNum;
				tables[i].totalLength = tableinfor.totalLength;
				tables[i].attributes = tableinfor.attributes;
			}
		}
	}

	void update(Index& index){
		for(int i = 0; i< indexNum; i++){
			if(indexes[i].index_name == index.index_name){
				indexes[i].table_name = index.table_name;
				indexes[i].column = index.column;
				indexes[i].blockNum = index.blockNum;
			}
		}
	}
	
	bool ExistTable(string tablename){
		int i;
		for(i=0;i<tables.size();i++){
			if(tables[i].name==tablename)
				return true;
		}
		return false;
	}
	
	bool ExistIndex(string tablename, int column){
		int i;
		for(i = 0; i < indexes.size(); i++){
			if(indexes[i].table_name == tablename && indexes[i].column==column)
				break;//found it
		}
		if(i >= indexes.size()) return 0;
		else return 1;
	}

	bool ExistIndex(string indexname){
		int i;
		for(i = 0; i <indexes.size(); i++){
			if(indexes[i].index_name == indexname)
				break;//found it
		}
		if(i >= indexes.size()) return 0;
		else return 1;
	}

	Table getTableInformation(string tablename){	
		int i;
		Table temp;
		for(i=0;i<tableNum;i++){
			if((tables[i].name)==tablename){
				
				return tables[i];
			}
		}		
		return temp;
	}
	
	Index getIndexInformation(string tablename, int column){
		int i;
		for(i = 0; i < indexes.size(); i++){
			if(indexes[i].table_name == tablename && indexes[i].column==column)
				break;//found it
		}
		if(i >= indexNum){
			Index tmpt;
			return tmpt;//indicate that table information not found
		}
		return indexes[i];
	}

	Index getIndexInformation(string indexName){
		int i;
		for(i = 0; i < tableNum; i++){
			if(indexes[i].index_name == indexName)
				break;//found it
		}
		if(i >= indexNum){
			Index tmpt;
			return tmpt;//indicate that table information not found
		}
		return indexes[i];
	}

	void ShowCatalog(){
		ShowTableCatalog();
		ShowIndexCatalog();
	}
	
	void ShowTableCatalog(){//this method is for debug only
		cout << "##    Number of tables:" <<tableNum << endl;
		for(int i = 0; i < tableNum; i++)
		{
			cout << "TABLE " << i << endl;
			cout << "Table Name: " << tables[i].name << endl;
			cout << "Number of attributes: "<< tables[i].attriNum << endl;
			cout << "Number of blocks occupied in disk: " << tables[i].blockNum << endl;
			for(int j = 0; j < tables[i].attriNum; j++)
			{
				cout << tables[i].attributes[j].name << "\t";
				switch(tables[i].attributes[j].type)
				{
					case CHAR:	cout << "CHAR("<<tables[i].attributes[j].length << ")\t";	break;
					case INT:	cout << "INT\t";		break;
					case FLOAT:	cout << "FLOAT\t";	break;
					default:	cout << "UNKNOW TYPE\t";	break;
				}
				if(tables[i].attributes[j].isUnique)	cout << "Unique\t";
				else cout << "NotUnique ";
				if(tables[i].attributes[j].isPrimeryKey) cout << "PrimeryKey\t" << endl;
				else cout << endl;
		 	}
		}
	}
	
	void ShowIndexCatalog(){//this method is for debug also
		cout << "##    Number of indexes:" <<indexNum << endl;
		for(int i = 0; i < indexNum; i++)
		{
			cout << "INDEX " << i << endl;
			cout << "Index Name: " << indexes[i].index_name << endl;
			cout << "Table Name: "<< indexes[i].table_name << endl;
			cout << "Column Number: " << indexes[i].column << endl;
			cout << "Column Number of blocks occupied in disk: " << indexes[i].blockNum << endl;
		}
	}
	int GetColumnNumber(Table& tableinfo,string columnname)
	{
		for(int i=0;i<tableinfo.attributes.size();i++){
			if(tableinfo.attributes[i].name==columnname){
				return i;
			}
		}
		return -1;
	}
	int GetColumnAmount(Table& tableinfo){
		return tableinfo.attributes.size();
	}
};
#endif
