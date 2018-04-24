 //head file including

#include "Base.h"
#include "CatalogManager.h"
#include "Interpret.h"
//#include"RecordManager.h"

using namespace std;

//static CCatalogManager* m_pCatalog = new CCatalogManager;
CatalogManager catalog; 
//extern RecordManager  record;
//This function is used to initiate the 'column' structure
void Interpret::initcol()
{
	if(column.size()>0){
		column.clear();
	}
}

//This function is used to initiate the 'condition' structure
void Interpret::initcond()
{
	if(condition.size()>0){
		condition.clear();
	}
}

//This function is used to initiate the 'insertvalue' structure
void Interpret::initvalue()
{
	if(row.columns.size()>0){
		row.columns.clear();
	}
}

//This function is used to get a word ( maybe a 'token' is more appropriate )
//from the 'src' string and copy it into the 'des' string
bool Interpret::GetWord(string & src, string & des)
{   
	des.clear();
	unsigned int srcpos = 0, despos = 0;
	char temp = ' ';

	for(;srcpos<src.length();srcpos++) {    
		if(temp == ' ' || temp == '\t' || temp == 10 || temp == 13)
			temp = src[srcpos];
		else break;
	}
	if(srcpos == src.length() && (temp == ' ' || temp == '\t' || temp == 10 || temp == 13))
		return false;
			
	switch(temp)
	{
	case ',':
	case '(':
	case ')':
	case '*':
	case '=':
	case '\'':
		des+=temp;
		src.erase(0,srcpos);
		break;
	case '<':
		des+=temp;
		temp = src[srcpos++];
		if(temp == '=' || temp == '>')
		{
			des+=temp;
			src.erase(0,srcpos);
		}
		else
		{
			src.erase(0,srcpos-1);
		}
		break;
	case '>':
		des+=temp;
		temp = src[srcpos++];
		if(temp == '=')
		{
			des+=temp;
			src.erase(0,srcpos);
		}
		else
		{
			src.erase(0,srcpos-1);
		}
		break;
	default:
		do{
			des+=temp;
			if (srcpos < src.length())
				temp = src[srcpos++];
			else  {
				src.erase(0,srcpos);
				des[despos++]='\0';
				return true;
			}
		}while(temp != '*' && temp != ',' && temp != '(' && temp != ')'
			&& temp != ' ' && temp != '\t' && temp != '=' && temp != '>' 
			&& temp != '<' && temp != '\'' && temp != 10 && temp != 13 );
		src.erase(0,srcpos-1);
	}
	return true;
}

bool Interpret::GetStr(string& src, string& des)
{
	unsigned int pos=0;
	des.clear();
	char temp;
	if (src[0] == '\'') {
		des = "";
		return true;
	}
	else for(pos=0;pos<src.length();pos++) {
		if(src[pos] != '\''){
			temp=src[pos];
			des+=temp;
		}
		else {
			src.erase(0,pos);
			return true;
		} 
	}
	return false;
}

//This function is used to parse the 'command' string which comes from user's 
//input, and return a corresponding parse parsetree
void Interpret::Parse(char* command)
{
	string word ="";
	string temp = command;
	short int FindPriKey = 0, FindUnique = 0;
	Attribute tempcol;
	Condition tempcon;
	string   temprow;
	bool flag;
	
	MakeInitilate();//���������еı���ȫ����ʼ��Ϊ��ֵ��NULL
	flag = GetWord(temp, word);//��temp�ַ����е�һ���ո�ǰ�ĵ���ȥ���������õ��ʸ���WORD��Ϊ�ַ���,flag����1
	//���ǿղ�ѯ
	if(!flag)
	{
		m_operation = EMPTY;
		return;
	}

	//�����˳�����
	else if(strcmp(word.c_str(),"quit")==0)
	{
		flag = GetWord(temp, word);
		if(!flag)
			m_operation = QUIT;
		return;
	}

	//����select����
	else if(strcmp(word.c_str(),"select")==0)
	{
		m_operation = SELERR;
		
		//����select�Ӿ�
		flag = GetWord(temp, word);
		if(!flag)
			return;
		//initcol(column);
		tempcol.name=word;
		column.push_back(tempcol);//�����￪ʼpushҪ���ص���
		flag = GetWord(temp, word);
		if(!flag)
			return;
		
		while(strcmp(word.c_str(),",")==0)
		{
			flag = GetWord(temp, word);
			if(!flag)
				return;
			tempcol.name=word;
			column.push_back(tempcol);//ѭ��pushҪ���ص���
			flag = GetWord(temp, word);
			if(!flag)
				return;
		}
	
		if(strcmp(word.c_str(),"from")!=0)
			return;
		//����from�Ӿ�
		flag = GetWord(temp, word);
		if(!flag)
			return;
		m_tabname  = word;
		
		if(!catalog.ExistTable(word)){
			m_operation=TABLEERROR;
			return;
		}
		getTableInfo=catalog.getTableInformation(m_tabname);//�ڱ���ڵ�ǰ���»�ȡ�����Ϣ

		//���û��where�Ӿ䣬��ȷ����
		flag = GetWord(temp, word);
		if(!flag)
		{
			m_operation = SELECT_NOWHERE_CAULSE;
			return;
		}

		if(strcmp(word.c_str(),"where")!=0)
			return;
		//����where�Ӿ�
		flag = GetWord(temp, word);
		if(!flag)
		{
			m_operation = SELERR;
		}
		tempcon.columnNum=catalog.GetColumnNumber(getTableInfo,word);
		m_colname=word;
		if(tempcon.columnNum==-1){
			m_operation=COLUMNERROR;
			return;
		}
		flag = GetWord(temp, word);
		if(!flag)
			return;
		if(strcmp(word.c_str(),"<")==0)
			tempcon .op = Lt;
		else if(strcmp(word.c_str(),"<=")==0)
			tempcon.op = Le;
		else if(strcmp(word.c_str(),">")==0)
			tempcon .op = Gt;
		else if(strcmp(word.c_str(),">=")==0)
			tempcon .op = Ge;
		else if(strcmp(word.c_str(),"=")==0)
			tempcon .op = Eq;
		else if(strcmp(word.c_str(),"<>")==0)
			tempcon .op = Ne;
		else return;
			
		flag = GetWord(temp, word);
		if(!flag)
			return;
		if(strcmp(word.c_str(),"\'")==0)
		{
			flag = GetStr(temp, word);
			if(!flag)
				return;
			tempcon.value=word;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"\'")!=0)
				return;
		}
		else
		{
			tempcon.value=word;
		}
		condition.push_back(tempcon);//��ʼ������������
		flag = GetWord(temp, word);
		if(!flag)
		{
			m_operation = SELECT_WHERE_CAULSE;
			return;
		}
		while(strcmp(word.c_str(),"and")==0)
		{
			flag = GetWord(temp, word);
			if(!flag)
				return;
			tempcon.columnNum=catalog.GetColumnNumber(getTableInfo,word);
			if(tempcon.columnNum==-1){
				m_operation=COLUMNERROR;
				return;
			}
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"<")==0)
				tempcon .op = Lt;
			else if(strcmp(word.c_str(),"<=")==0)
				tempcon .op = Le;
			else if(strcmp(word.c_str(),">")==0)
				tempcon .op = Gt;
			else if(strcmp(word.c_str(),">=")==0)
				tempcon .op = Ge;
			else if(strcmp(word.c_str(),"=")==0)
				tempcon .op = Eq;
			else if(strcmp(word.c_str(),"<>")==0)
				tempcon .op = Ne;
			else return;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"\'")==0)
			{
				flag = GetStr(temp, word);
				if(!flag)
					return;
				tempcon.value=word;
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),"\'")!=0)
					return;
			}
			else
			{
				tempcon.value=word;
			}
			condition.push_back(tempcon);//ѭ����������
			flag = GetWord(temp, word);
			if(!flag)
			{
				m_operation = SELECT_WHERE_CAULSE;
				return;
			}
		}
	}
	//����create table/create index����
	else if(strcmp(word.c_str(),"create")==0)
	{
		flag = GetWord(temp, word);
		if(!flag)
			return;
		//����create table����
		if(strcmp(word.c_str(),"table")==0)
		{
			m_operation = CRETABERR;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			m_tabname  = word;
			if(catalog.ExistTable(m_tabname)){
				m_operation=TABLEEXISTED;
				return;
			}
			getTableInfo.name=word;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"(")!=0)
				return;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"unique")==0||strcmp(word.c_str(),"primary")==0)
				return;
			tempcol.name=word;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"int")==0)
			{
				tempcol.type = INT;
				tempcol.length = INTLEN;
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),"unique")==0)
				{
					tempcol.isUnique = 1;
					flag = GetWord(temp, word);
					if(!flag)
						return;
				}
			}
			else if(strcmp(word.c_str(),"float")==0)
			{
				tempcol.type = FLOAT;
				tempcol.length = FLOATLEN;
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),"unique")==0)
				{
					tempcol.isUnique = 1;
					flag = GetWord(temp, word);
					if(!flag)
						return;
				}
			}
			else if(strcmp(word.c_str(),"char")==0)
			{
				tempcol.type = CHAR;
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),"(")!=0)
					return;
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(!IsInt(word.c_str()))
					return;
				tempcol.length = atoi(word.c_str()) + 1;
				if(tempcol.length > 256 || tempcol.length < 2)
				{
					m_operation = CHARBOUD;
					return;
				}
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),")")!=0)
					return;
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),"unique")==0)
				{
					tempcol.isUnique = 1;
					flag = GetWord(temp, word);
					if(!flag)
						return;
				}
			}
			else return;
			getTableInfo.attributes.push_back(tempcol);//��ʼ������Ϣ����
			tempcol.isUnique=tempcol.isPrimeryKey=0;
			while(strcmp(word.c_str(),",")==0)
			{
				flag = GetWord(temp, word);
				if(!flag)
					return;
				//�����primary key����
				if(strcmp(word.c_str(),"primary")==0)
				{
					flag = GetWord(temp, word);
					if(!flag)
						return;
					if(strcmp(word.c_str(),"key")!=0)
						return;
					flag = GetWord(temp, word);
					if(!flag)
						return;
					if(strcmp(word.c_str(),"(")!=0)
						return;
					flag = GetWord(temp, word);
					if(!flag)
						return;
					for(int i=0;i<getTableInfo.attributes.size();i++){
						if(strcmp(getTableInfo.attributes[i].name.c_str(),word.c_str())==0){
							FindPriKey=1;
							getTableInfo.attributes[i].isPrimeryKey=1;
							getTableInfo.attributes[i].isUnique=1;
						}
					}
					if(!FindPriKey)
					{
						m_operation = VOIDPRI;
						return;
					}
					FindPriKey = 0;
					flag = GetWord(temp, word);
					if(!flag)
						return;
					if(strcmp(word.c_str(),")")!=0)
						return;
					flag = GetWord(temp, word);
					if(!flag)
						return;
				}
				//�����unique����
				else if(strcmp(word.c_str(),"unique")==0)
				{
					flag = GetWord(temp, word);
					if(!flag)
						return;
					if(strcmp(word.c_str(),"(")!=0)
						return;
					flag = GetWord(temp, word);
					if(!flag)
						return;
						for(int i=0;i<getTableInfo.attributes.size();i++){
						if(strcmp(getTableInfo.attributes[i].name.c_str(),word.c_str())==0){
							FindUnique=1;
							getTableInfo.attributes[i].isUnique=1;
						}
					}
					if(!FindUnique)
					{
						m_operation = VOIDUNI;
						return;
					}
					FindUnique = 0;
					flag = GetWord(temp, word);
					if(!flag)
						return;
					if(strcmp(word.c_str(),")")!=0)
						return;
					flag = GetWord(temp, word);
					if(!flag)
						return;
				}
				//�������unique��primary key����
				else
				{
					tempcol.name=word;
					flag = GetWord(temp, word);
					if(!flag)
						return;
					if(strcmp(word.c_str(),"int")==0)
					{
						tempcol.type = INT;
						tempcol.length = INTLEN;
						flag = GetWord(temp, word);
						if(!flag)
							return;
						if(strcmp(word.c_str(),"unique")==0)
						{	
							tempcol.isUnique = 1;
							flag = GetWord(temp, word);
							if(!flag)
								return;
						}
					}
					else if(strcmp(word.c_str(),"float")==0)
					{
						tempcol.type = FLOAT;
						tempcol.length = FLOATLEN;
						flag = GetWord(temp, word);
						if(!flag)
							return;
						if(strcmp(word.c_str(),"unique")==0)
						{
							tempcol.isUnique = 1;
							flag = GetWord(temp, word);
							if(!flag)
							return;
						}
					}
					else if(strcmp(word.c_str(),"char")==0)
					{
						tempcol.type = CHAR;
						flag = GetWord(temp, word);
						if(!flag)
							return;
						if(strcmp(word.c_str(),"(")!=0)
							return;
						flag = GetWord(temp, word);
						if(!flag)
							return;
						if(!IsInt(word.c_str()))
							return;
						tempcol.length = atoi(word.c_str())+1;
						if(tempcol.length > 255 || tempcol.length < 1)
						{
							m_operation = CHARBOUD;
							return;
						}
						flag = GetWord(temp, word);
						if(!flag)
							return;
						if(strcmp(word.c_str(),")")!=0)
							return;
						flag = GetWord(temp, word);
						if(!flag)
							return;
						if(strcmp(word.c_str(),"unique")==0)
						{
							tempcol.isUnique = 1;
							flag = GetWord(temp, word);
							if(!flag)
								return;
						}
					}
					else return;
					getTableInfo.attributes.push_back(tempcol);//����Ϊֹ���ѻ��table�е�name��attributes����
					//����Ҫ��api�м����attriNum��totalLength
					tempcol.isPrimeryKey=tempcol.isUnique=0;
				}
			}
			if(strcmp(word.c_str(),")")!=0)
				return;
			flag = GetWord(temp, word);
			if(!flag)
				m_operation = CRETAB;
		}
		//����create index����
		else if(strcmp(word.c_str(),"index")==0)
		{
			m_operation = CREINDERR;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			m_indname=word;
			getIndexInfo.index_name=word;
			if(catalog.ExistIndex(m_indname)){
				m_operation=INDEXERROR;
				return;
			}
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"on")!=0)
				return;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			m_tabname  = word;
			if(!catalog.ExistTable(word)){
				m_operation=TABLEERROR;
				return;
			}
			getTableInfo=catalog.getTableInformation(m_tabname);
			/*for(int i=0;i<getTableInfo.attributes.size();i++){
				if(getTableInfo.attributes[i].isPrimeryKey==true){
					getIndexInfo.columnLength=getTableInfo.attributes[i].length;
				}
			}*/
			getIndexInfo.table_name=word;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"(")!=0)
				return;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			int tempint;
			tempint=catalog.GetColumnNumber(getTableInfo,word);
			if(tempint==-1){
				m_operation=COLUMNERROR;
				return;
			}
			if(catalog.ExistIndex(m_tabname,tempint)){
				m_operation=INDEXERROR;
				return;
			}
			getIndexInfo.column=tempint;
			getIndexInfo.blockNum=0;
			getIndexInfo.columnLength=getTableInfo.attributes[tempint].length;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),")")!=0)
				return;
			flag = GetWord(temp, word);
			if(!flag)
				m_operation = CREIND;
		}
	}
	//����delete from����
	else if(strcmp(word.c_str(),"delete")==0)
	{
		flag = GetWord(temp, word);
		if(!flag)
			return;
		if(strcmp(word.c_str(),"from")==0)
		{
			m_operation = DELETEERR;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			m_tabname  = word;
			if(!catalog.ExistTable(word)){
				m_operation=TABLEERROR;
				return;
			}
			flag = GetWord(temp, word);
			//��û��where�Ӿ䣬��ȷ����
			getTableInfo=catalog.getTableInformation(m_tabname);
			if(!flag)
			{
				m_operation = DELETE;
				return;
			}
			if(strcmp(word.c_str(),"where")!=0)
				return;
			//��ʼ����where�Ӿ�
			flag = GetWord(temp, word);
			if(!flag)
				return;
			int tempint;
			tempint=catalog.GetColumnNumber(getTableInfo,word);
			if(tempint==-1){
				m_operation=COLUMNERROR;
				return;
			}
			tempcon.columnNum=tempint;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"<")==0)
				tempcon.op = Lt;
			else if(strcmp(word.c_str(),"<=")==0)
				tempcon.op = Le;
			else if(strcmp(word.c_str(),">")==0)
				tempcon.op = Gt;
			else if(strcmp(word.c_str(),">=")==0)
				tempcon.op = Ge;
			else if(strcmp(word.c_str(),"=")==0)
				tempcon.op = Eq;
			else if(strcmp(word.c_str(),"<>")==0)
				tempcon.op = Ne;
			else return;
				
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"\'")==0)
			{
				flag = GetStr(temp, word);
				if(!flag)
					return;
				tempcon.value=word;
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),"\'")!=0)
					return;
			}
			else
			{
				tempcon.value=word;
			}
			condition.push_back(tempcon);
			flag = GetWord(temp, word);
			if(!flag)
			{
				m_operation = DELETE;
				return;
			}
			while(strcmp(word.c_str(),"and")==0)
			{
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(catalog.GetColumnNumber(getTableInfo,word)==-1){
					m_operation=COLUMNERROR;
					return;
				}
				tempcon.columnNum=catalog.GetColumnNumber(getTableInfo,word);
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),"<")==0)
					tempcon.op = Lt;
				else if(strcmp(word.c_str(),"<=")==0)
					tempcon.op = Le;
				else if(strcmp(word.c_str(),">")==0)
					tempcon.op = Gt;
				else if(strcmp(word.c_str(),">=")==0)
					tempcon.op = Ge;
				else if(strcmp(word.c_str(),"=")==0)
					tempcon.op = Eq;
				else if(strcmp(word.c_str(),"<>")==0)
					tempcon.op = Ne;
				else return;
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),"\'")==0)
				{
					flag = GetStr(temp, word);
					if(!flag)
						return;
					tempcon.value=word;
					flag = GetWord(temp, word);
					if(!flag)
						return;
					if(strcmp(word.c_str(),"\'")!=0)
						return;
				}
				else
				{
					tempcon.value=word;
				}
				condition.push_back(tempcon);
				flag = GetWord(temp, word);
				if(!flag)
				{
					m_operation = DELETE;
					return;
				}
			}	
		}
	}
	//����insert into����
	else if(strcmp(word.c_str(),"insert")==0)
	{
		flag = GetWord(temp, word);
		if(!flag)
			return;			
		if(strcmp(word.c_str(),"into")==0)
		{
			m_operation = INSERTERR;
			flag = GetWord(temp, word);
			if(!flag)	
				return;
			m_tabname  = word;
			if(!catalog.ExistTable(word)){
				m_operation=TABLEERROR;
				return;
			}
			getTableInfo=catalog.getTableInformation(m_tabname);
			
			flag = GetWord(temp, word);
			if(!flag)	
				return;
			if(strcmp(word.c_str(),"values")!=0)
				return;
			flag = GetWord(temp, word);
			if(!flag)	
				return;
			if(word != "(")
				return;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			if(strcmp(word.c_str(),"\'")==0)
			{
				flag = GetStr(temp, word);
				if(!flag)
					return;
				temprow=word;
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),"\'")!=0)
					return;
			}
			else
			{
				temprow=word;
			}
			flag = GetWord(temp, word);
			if(!flag)
				return;
			row.columns.push_back(temprow);
			while(strcmp(word.c_str(),",")==0)
			{
				
				flag = GetWord(temp, word);
				if(!flag)
					return;
				if(strcmp(word.c_str(),"\'")==0)
				{
					flag = GetStr(temp, word);
					if(!flag)
						return;
					temprow=word;
					flag = GetWord(temp, word);
					if(!flag)
						return;
					if(strcmp(word.c_str(),"\'")!=0)
						return;
				}
				else
				{
					temprow=word;
				}
				row.columns.push_back(temprow);
				flag = GetWord(temp, word);
				if(!flag)
					return;
			}
			if(word != ")")
				return;
			if(row.columns.size()!=catalog.GetColumnAmount(getTableInfo)){
				m_operation=INSERTNUMBERERROR;
				return;
			}
			//if(PrimaryKeyPosition!=-1){
			//	tempcon.op=Eq;
			//	tempcon.columnNum=PrimaryKeyPosition;
			//	tempcon.value=row.columns[PrimaryKeyPosition];
			//	condition.push_back(tempcon);//���������ж��Ƿ����ظ���PrimaryKey
			//}
			flag = GetWord(temp, word);
			for(int i=0;i<getTableInfo.attributes.size();i++){
				if(getTableInfo.attributes[i].isPrimeryKey){
					PrimaryKeyPosition=i;
					tempcon.columnNum=i;
					tempcon.op=Eq;
					tempcon.value=row.columns[i];
					condition.push_back(tempcon);
				}
				if(getTableInfo.attributes[i].isPrimeryKey!=1&&getTableInfo.attributes[i].isUnique){
					UniquePostion=i;
					tempcon.columnNum=i;
					tempcon.op=Eq;
					tempcon.value=row.columns[i];
					UniqueCondition.push_back(tempcon);
				}
			}
			if(!flag){
				m_operation  = INSERT;
				return;
			}
		}
	}
	//����drop table/drop index����
	else if(strcmp(word.c_str(),"drop")==0)
	{
		flag = GetWord(temp, word);
		if(!flag)
			return;
		if(strcmp(word.c_str(),"table")==0)
		{
			m_operation = DRPTABERR;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			m_tabname  = word;
			if(!catalog.ExistTable(m_tabname)){
				m_operation=TABLEERROR;
				return;
			}
			getTableInfo=catalog.getTableInformation(m_tabname);
			flag = GetWord(temp, word);
			if(!flag)
				m_operation = DRPTAB;
		}
		else if(strcmp(word.c_str(),"index")==0)
		{
			m_operation = DRPINDERR;
			flag = GetWord(temp, word);
			if(!flag)
				return;
			m_indname=word;
			if(!catalog.ExistIndex(m_indname)){
				m_operation=INDEXEROR;
				return;
			}
			flag = GetWord(temp, word);
			if(!flag)
				m_operation = DRPIND;
			//getIndexInfo=catalog.getIndexInformation(
		}	

	}
	return;
}

//This function is used to check is the string 'input' represented an int variable
//return 1 if is, 0 otherwise
short int Interpret::IsInt(const char *input)
{
	int i;
	int length = strlen(input);
	if(!isdigit(input[0]) && !(input[0] == '-'))
		return 0;
	for(i = 1 ; i < length ; i ++)
	{
		if(!isdigit(input[i]))
			return 0;
	}
	return 1;
}

//This function is used to check is the string 'input' represented an float variable
//return 1 if is, 0 otherwise
short int Interpret::IsFloat(char *input)
{
		int dot = 0;
		int i;
		int length = strlen(input);
		if(!isdigit(input[0]) && !(input[0] == '-'))
			return 0;
		for(i = 1 ; i < length ; i ++)
		{
			if(!isdigit(input[i]) &&  input[i] != '.')
				return 0;
			else if(input[i] == '.')
			switch(dot)
			{
				case 0:
					dot++;
					break;
				default:
					return 0;
			}
		}
		return 1;
}


