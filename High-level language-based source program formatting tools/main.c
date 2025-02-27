#include <stdio.h>
#include <stdlib.h>
#include <string.h>
          /**     +  -   *  /   (   )   =    >  <  >=  <=  != ==  #  %  && ||                                                                                             [  ]   {   }  ;  ,  .   "    '  */
enum token_kind {ADD,SUB,MUL,DIV,LSP,RSP,ASSIGN,BG,SM,BGEQ,SMEQ,NEQ,EQ,DEF,MOD,AND,OR,ERROR_TOKEN,INT,LONG,FLOAT,DOUBLE,CHAR,IDENT,INT_CONST,LONG_CONST,FLOAT_CONST,DOUBLE_CONST,CHAR_CONST,LMP,RMP,LBP,RBP,SEM,COM,RAD,DQUA,SQUA,IF,ELSE,WHILE,FOR,RETURN,BREAK,CONTINUE,DEFINE,INCLUDE,ENTER};
///0������� -1����С�� 1������� -2�����ޣ������Ǵ���
int priority[17][17]={
                       1, 1,-1,-1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1,-1, 1, 1,
                       1, 1,-1,-1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1,-1, 1, 1,
                       1, 1, 1, 1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                       1, 1, 1, 1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                      -1,-1,-1,-1,-1, 0,-2, 1, 1, 1, 1, 1, 1, 1,-1,-1,-1,
                       1, 1, 1, 1, 1,-2,-2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                      -1,-1,-1,-1,-1,-2,-1,-1,-1,-1,-1,-1,-1, 1,-1,-1,-1,
                      -1,-1,-1,-1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1,-1,-1,-1,
                      -1,-1,-1,-1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1,-1,-1,-1,
                      -1,-1,-1,-1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1,-1,-1,-1,
                      -1,-1,-1,-1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1,-1,-1,-1,
                      -1,-1,-1,-1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1,-1,-1,-1,
                      -1,-1,-1,-1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1,-1,-1,-1,
                      -1,-1,-1,-1,-1,-2,-1,-1,-1,-1,-1,-1,-1, 0,-1,-1,-1,
                       1, 1, 1, 1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                      -1,-1,-1,-1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1,-1, 1, 1,
                      -1,-1,-1,-1,-1, 1,-2, 1, 1, 1, 1, 1, 1, 1,-1,-1, 1,
                     };


typedef struct
{
    int placeinenum;
    char name[10];
}_keywords;

typedef struct STEP
{
    int define;//0:���ݽڵ�  1:����ָ��ڵ� 2:����ָ��ڵ�  �Դ�����
    char name[20];
    char text[20];
    struct STEP * child[4];
}_step;

FILE * fp,*nfp;//�ļ�ָ��
_keywords keywords[13];//���������ؼ��ֵĲ��ұ�
char token_text[20];//���ڱ����ַ�����ֵ
int wordtype;//���ڼ�¼��ȡ�����ַ������ĸ����
int errors=0;//������¼����������Ƿ���ִ���
int definitionflag=0;//�����ж��Ƿ��ڱ���Ԥ���������
int line=1;//������¼�ڼ���
int annotatorflag=0;//���ڼ�¼�����ַ�������λ���ǲ�����ע������
int indentflag=0;//���ڸ�ʽ�������ʱ���ж��Ƿ�Ҫ����

_step* makepoint(int define,char name[],char text[]);
void setupkeywords();
int checkkeywords();
int gettoken();
int program();
_step* ExtDefList();
_step* ExtDef();
_step* MacDef();
_step* ExtVar(char type[],char ident[]);
_step* FuncDef(char type[],char ident[]);
_step* FuncPraDef();
_step* ComplexStence();
_step* LocVar();
_step* StenceList();
_step* Stence();
_step* Expression(int endtype);
void ASTTraverse(_step* root);
void CodeTraverse(_step* root);

void main()
{
    char filename[20],newfilename[20];
    setupkeywords();//�����ؼ��ֲ��ұ�
    printf("�������Ҫ���з�����Դ�ļ��������뱾����ͬ�����ļ�����\n");
    printf("����Ҫ���з�����Դ�ļ������ƣ����������չ�����ǣ�");
    gets(filename);//�����û���Ҫ���з������ļ���
    newfilename[0]='\0';
    strcat(newfilename,filename);
    strcat(filename,".c");
    fp=fopen(filename,"r");
    strcat(newfilename,"1.c");
    nfp=fopen(newfilename,"w");
    program();
    printf("\nԴ�����Ѿ������������ŵİ�ʽ�����ļ� %s ��",newfilename);
    printf("\n�������������");
    getch();
}

_step* makepoint(int define,char name[],char text[])//����һ�������ڵ�
{
    _step * p=malloc(sizeof(_step));
    p->define=define;
    p->name[0]='\0';
    strcat(p->name,name);
    strcat(p->name,"\0");
    p->text[0]='\0';
    strcat(p->text,text);
    strcat(p->text,"\0");
    return p;
}

void setupkeywords()//�����ؼ��ֲ��ұ�
{
    strcat(keywords[0].name,"int");
    keywords[0].placeinenum=INT;
    strcat(keywords[1].name,"long");
    keywords[1].placeinenum=LONG;
    strcat(keywords[2].name,"char");
    keywords[2].placeinenum=CHAR;
    strcat(keywords[3].name,"float");
    keywords[3].placeinenum=FLOAT;
    strcat(keywords[4].name,"if");
    keywords[4].placeinenum=IF;
    strcat(keywords[5].name,"else");
    keywords[5].placeinenum=ELSE;
    strcat(keywords[6].name,"while");
    keywords[6].placeinenum=WHILE;
    strcat(keywords[7].name,"for");
    keywords[7].placeinenum=FOR;
    strcat(keywords[8].name,"return");
    keywords[8].placeinenum=RETURN;
    strcat(keywords[9].name,"break");
    keywords[9].placeinenum=BREAK;
    strcat(keywords[10].name,"continue");
    keywords[10].placeinenum=CONTINUE;
    strcat(keywords[11].name,"define");
    keywords[11].placeinenum=DEFINE;
    strcat(keywords[12].name,"include");
    keywords[12].placeinenum=INCLUDE;
}

int checkkeywords()//���ڼ���token_text���Ƿ�Ϊ�ؼ��֣������Ϊ�ؼ����򷵻�  IDENT����֮���ض�Ӧ�Ĺؼ���
{
    int i;
    for(i=0;i<13;i++)
    {
        if(strcmp(token_text,keywords[i].name)==0)
            return keywords[i].placeinenum;
    }
    return IDENT;
}

int gettoken()  //������ȡһ����ʶ����ؼ���
{
    int i=0;//���ڼ�¼token_text ���� annotator����ĵ�ǰ�洢λ��
    int c;//������ȡ�����ַ�
    token_text[0]='\0';//��ʼ�������ڴ洢�µı�ʶ����ؼ���
    if(annotatorflag==0)
    {
        while((c=getc(fp))==' '||c=='\n'||c=='\t')
        {
            if(c=='\n')
            {
                line++;
                if(definitionflag==1) return ENTER;
            }
        }//���˿ո���Լ��س���
        if((c>='a'&&c<='z')||(c>='A'&&c<='Z'))//�����ȡ���ĵ�һ���ַ�Ϊ��ĸ
        {
            do
            {
                strcat(token_text,&c);
                c=fgetc(fp);
                if(c=='[')//�����⵽��������
                {
                    strcat(token_text,&c);
                    c=fgetc(fp);
                    while(c>='0'&&c<='9'||c==']')
                    {
                        strcat(token_text,&c);
                        if(c==']')
                        {
                            printf("��ʶ��    %s\n",token_text);
                            return IDENT;//����Ԫ��Ĭ��Ϊ��ʶ��
                        }
                        c=fgetc(fp);
                    }
                    return ERROR_TOKEN;
                }
            }
            while((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9'));//��������ַ���ȻΪ��ĸ�������֣�������������token_text��
            ungetc(c,fp);
            return checkkeywords();
        }
        else if(c>='0'&&c<='9')//�����ȡ���ĵ�һ���ַ�Ϊ����
        {
            //strcat(token_text,&c);
            if(c=='0')//�����һλ��0����ô��Ҫ���ֵ����ǰ˽��ƻ���ʮ������
            {
                strcat(token_text,&c);
                c=fgetc(fp);
                if(c=='x')//���Ϊx��˵����ʮ��������
                {
                    for(strcat(token_text,&c),c=fgetc(fp);(c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='.';c=fgetc(fp))
                    {
                        if((c>'f'&&c<='z')||(c>'F'&&c<'Z'))
                        {
                            if(c=='l'||c=='L')//���ڼ���Ƿ�Ϊlong����
                            {
                                strcat(token_text,&c);
                                if(strcmp(token_text,'0xl')==0||strcmp(token_text,'0xL')==0) return ERROR_TOKEN;//�������Ϊ0xl��0xL�򷵻ش���
                                printf("��������  %s\n",token_text);
                                return LONG_CONST;//��ʱ˵���ַ������ʾ���ǳ�������
                            }
                            else return ERROR_TOKEN;
                        }
                        else if(c=='.')
                        {
                            strcat(token_text,&c);
                            for(c=fgetc(fp);(c>='0'&&c<='9')||(c>='a'&&c<='f')||(c>='A'&&c<='F');c=fgetc(fp))
                            {
                                strcat(token_text,&c);
                            }
                            ungetc(c,fp);
                            printf("˫������  %s\n",token_text);
                            return DOUBLE_CONST;
                        }
                        strcat(token_text,&c);
                    }
                    ungetc(c,fp);
                    printf("������    %s\n",token_text);
                    return INT_CONST;
                }
                else if(c=='.')//˵������һ��double
                {
                    strcat(token_text,&c);
                    for(c=fgetc(fp);c>='0'&&c<='9'||c=='f'||c=='F';c=fgetc(fp))
                    {
                        if(c=='f'||c=='F')//���ڼ���Ƿ�Ϊfloat����
                        {
                            strcat(token_text,&c);
                            if(strcmp(token_text,'0.f')==0||strcmp(token_text,'0.F')==0) return ERROR_TOKEN;//�������Ϊ0xl��0xL�򷵻ش���
                            return FLOAT_CONST;//��ʱ˵���ַ������ʾ���Ǹ�����
                        }
                        strcat(token_text,&c);
                    }
                    ungetc(c,fp);
                    printf("˫������  %s\n",token_text);
                    return DOUBLE_CONST;
                }
                else//���������˵��Ϊ�˽�����
                {
                    for(;(c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='.';c=fgetc(fp))
                    {
                        if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='8'||c=='9')
                        {
                            if(c=='l'||c=='L')//���ڼ���Ƿ�Ϊlong����
                            {
                                strcat(token_text,&c);
                                c=fgetc(fp);
                                if((c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z'))
                                    return ERROR_TOKEN;
                                ungetc(c,fp);
                                if(strcmp(token_text,'0l')==0||strcmp(token_text,'0L')==0) return ERROR_TOKEN;//�������Ϊ0l��0L�򷵻ش���
                                printf("��������  %s\n",token_text);
                                return LONG_CONST;//��ʱ˵���ַ������ʾ���ǳ�������
                            }
                            else return ERROR_TOKEN;
                        }
                        else if(c=='.')
                        {
                            strcat(token_text,&c);
                            for(c=fgetc(fp);c>='0'&&c<='7'||c=='f'||c=='F';c=fgetc(fp))
                            {
                                if(c=='f'||c=='F')//���ڼ���Ƿ�Ϊfloat����
                                {
                                    strcat(token_text,&c);
                                    printf("������    %s\n",token_text);
                                    return FLOAT_CONST;//��ʱ˵���ַ������ʾ���Ǹ�����
                                }
                                strcat(token_text,&c);
                            }
                            ungetc(c,fp);
                            printf("˫������  %s\n",token_text);
                            return DOUBLE_CONST;
                        }
                        strcat(token_text,&c);
                    }
                    ungetc(c,fp);
                    printf("������    %s\n",token_text);
                    return INT_CONST;
                }
            }
            else//�����һλ����0��˵����ʱ������Ϊʮ����
            {
                for(;(c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='.';c=fgetc(fp))
                {
                    if((c>='a'&&c<='z')||(c>='A'&&c<='Z'))
                    {
                        if(c=='l'||c=='L')//���ڼ���Ƿ�Ϊlong����
                        {
                            strcat(token_text,&c);
                            c=fgetc(fp);
                            if((c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z'))
                                return ERROR_TOKEN;
                            ungetc(c,fp);
                            printf("��������  %s\n",token_text);
                            return LONG_CONST;//��ʱ˵���ַ������ʾ���ǳ�������
                        }
                        else return ERROR_TOKEN;
                    }
                    else if(c=='.')
                    {
                        strcat(token_text,&c);
                        for(c=fgetc(fp);c>='0'&&c<='9'||c=='f'||c=='F';c=fgetc(fp))
                        {
                            if(c=='f'||c=='F')
                            {
                                strcat(token_text,&c);
                                printf("������    %s\n",token_text);
                                return FLOAT_CONST;
                            }
                            strcat(token_text,&c);
                        }
                        ungetc(c,fp);
                        printf("˫������  %s\n",token_text);
                        return DOUBLE_CONST;
                    }
                    strcat(token_text,&c);
                }
                ungetc(c,fp);
                printf("������    %s\n",token_text);
                return INT_CONST;
            }
        }
        else switch(c)//�����ȡ�����ַ��Ȳ�����ĸ�ֲ�������
        {
        case '='://�� = ��ʱ����Ҫ������ǹ�ϵ��仹�Ǹ�ֵ���
            strcat(token_text,&c);
            c=fgetc(fp);
            if(c=='=')
            {
                strcat(token_text,&c);
                printf("�߼���    %s\n",token_text);
                return EQ;//�� ==
            }
            ungetc(c,fp);
            printf("��ֵ��    %s\n",token_text);
            return ASSIGN;//�� =
        case '+':
            strcat(token_text,&c);
            printf("�����    %s\n",token_text);
            return ADD;
        case '-':
            strcat(token_text,&c);
            printf("�����    %s\n",token_text);
            return SUB;
        case '*':
            strcat(token_text,&c);
            printf("�����    %s\n",token_text);
            return MUL;
        case '/':
            strcat(token_text,&c);
            if((c=fgetc(fp))=='/')
            {
                annotatorflag=1;
                break;
            }
            else if(c=='*')
            {
                annotatorflag=2;
                break;
            }
            ungetc(c,fp);
            printf("�����    %s\n",token_text);
            return DIV;
        case '%':
            strcat(token_text,&c);
            printf("�����    %s\n",token_text);
            return MOD;
        case '!':
            strcat(token_text,&c);
            if((c=fgetc(fp))=='=')
            {
                strcat(token_text,&c);
                printf("�߼���    %s\n",token_text);
                return NEQ;
            }
            ungetc(c,fp);
            return ERROR_TOKEN;// ! �������ǵ�Ŀ�������  �������������
        case '>'://��Ҫ������ > ���� >=
            strcat(token_text,&c);
            if((c=fgetc(fp))=='=')
            {
                strcat(token_text,&c);
                printf("�߼���    %s\n",token_text);
                return BGEQ;
            }
            else ungetc(c,fp);
            printf("�߼���    %s\n",token_text);
            return BG;
        case '<'://��Ҫ������ < ���� <=
            strcat(token_text,&c);
            if((c=fgetc(fp))=='=')
            {
                strcat(token_text,&c);
                printf("�߼���    %s\n",token_text);
                return SMEQ;
            }
            else ungetc(c,fp);
            printf("�߼���    %s\n",token_text);
            return SM;
        case '&':
            strcat(token_text,&c);
            if((c=fgetc(fp))=='&')
                strcat(token_text,&c);
            else return ERROR_TOKEN;
            printf("�߼���    %s\n",token_text);
            return AND;
        case '|':
            strcat(token_text,&c);
            if((c=fgetc(fp))=='|')
                strcat(token_text,&c);
            else return ERROR_TOKEN;
            printf("�߼���    %s\n",token_text);
            return OR;
        case '#':
            strcat(token_text,&c);
            printf("�궨���  %s\n",token_text);
            return DEF;
        case ';':
            strcat(token_text,&c);
            printf("�ֺŷ�    %s\n",token_text);
            return SEM;
        case ',':
            strcat(token_text,&c);
            return COM;
        case '(':
            strcat(token_text,&c);
            printf("���ŷ�    %s\n",token_text);
            return LSP;
        case ')':
            strcat(token_text,&c);
            printf("���ŷ�    %s\n",token_text);
            return RSP;
        case '{':
            strcat(token_text,&c);
            printf("���ŷ�    %s\n",token_text);
            return LBP;
        case '}':
            strcat(token_text,&c);
            printf("���ŷ�    %s\n",token_text);
            return RBP;
        case '"':
            strcat(token_text,&c);
            return DQUA;
        case '\''://������뵥��������Ҫ�����ַ�����
            strcat(token_text,&c);
            c=fgetc(fp);
            if(c!='\\'&&c!='\'')
            {
                strcat(token_text,&c);
            }
            else if(c=='\\')//�������ת���
            {
                strcat(token_text,&c);
                c=fgetc(fp);
                strcat(token_text,&c);
            }
            else return ERROR_TOKEN;//ֱ������д���������ţ��ʷ�����
            c=fgetc(fp);
            if(c!='\'')
            {
                ungetc(c,fp);
                return ERROR_TOKEN;
            }
            strcat(token_text,&c);
            printf("�ַ�����  %s\n",token_text);
            return CHAR_CONST;
        case '.':
            strcat(token_text,&c);
            return RAD;
        }
    }
    if(annotatorflag==1)//���ǰһ��������ע�ͷ�//
    {
        while((c=fgetc(fp))!='\n'&&feof(fp)==0);//ֱ������س�Ϊֹ  ע�ͽ���  ����EOF�����ļ�����
        ungetc(c,fp);
        annotatorflag=0;//������ע��//״̬��flag�Ļز�����ע��״̬
        return gettoken();
    }
    else if(annotatorflag==2)//���ǰһ��������ע�ͷ�/*
    {
        while(feof(fp)==0)
        {
            c=fgetc(fp);
            if(c=='\n') line+=1;
            else if(c=='*')
            {
                if((c=fgetc(fp))=='/')//ֱ������*/Ϊֹ  ע�ͽ���  ����EOF�����ļ�����
                {
                    annotatorflag=0;//������ע��/*״̬��flag�Ļز�����ע��״̬
                    return gettoken();
                }
                ungetc(c,fp);
            }
        }
        return EOF;
    }
    return EOF;
}

int program()
{
    _step * root;
    printf("\n�ڸ�Դ�����м�⵽�������͵��ַ�����\n");
    if((root=ExtDefList())!=NULL)//�����ⲿ�������к�������ȷʱ���ظ��ڵ㣬���Ա���
    {
        printf("\n����Դ������������﷨����\n");//puts("-2"); 
        ASTTraverse(root);//puts("-1"); 
        CodeTraverse(root);//puts("0"); 
    }
    else exit(1);
}

_step* ExtDefList()//�ⲿ��������
{
    wordtype=gettoken();
    if(wordtype==EOF) return NULL;//�����ļ�β�����ؿ�ָ��
    _step * step=makepoint(2,"�ⲿ��������","\0");//��������ڵ�
    step->child[0]=ExtDef();//��һ������Ϊ�ⲿ����
    step->child[1]=ExtDefList();//�ڶ�������Ϊ��һ���ⲿ��������
    return step;
}

_step* ExtDef()//�ⲿ���壺��=�ⲿ��������||��������
{
    _step * step;//�����ڵ����ڷ���ָ��
    char type[5]="\0";//���ڴ洢��������
    char ident[10]="\0";//���ڴ洢������
    if(wordtype<=CHAR&&wordtype>=INT) //����������ַ���Ϊ�������͹ؼ��֣���������֮��������NULL
    {
        strcat(type,token_text);//����������͹ؼ���
        wordtype=gettoken();//��ȡ��һ���ַ���
        if(wordtype!=IDENT)
        {
            printf("�� %d ���ⲿ������ִ���\n",line);
            return NULL;
        }
        strcat(ident,token_text);//��������ı�����
        wordtype=gettoken();
        if(wordtype==LSP) step=FuncDef(type,ident);//���Ϊ ( ������뺯������
        else step=ExtVar(type,ident);//��֮�����������
    }
    else if(wordtype==DEF)//����������ַ�Ϊ #
    {
        step=MacDef();
    }
    else
    {
        printf("�� %d ���ⲿ�������\n",line);
        return NULL;
    }
    if(step!=NULL) return step;//���step��Ϊ�գ���˵�������һ���ⲿ���壬����step
}

_step* MacDef()//����Ԥ����
{
    definitionflag=1;
    _step * root=makepoint(1,"����Ԥ����",token_text);
    _step * r;//����ѭ����ȡ�궨�����
    char filename[20]="\0";
    char t;//�����ж��Ƿ������س���
    int sign;//���ڱ����ļ�����ʱ�ķ����� " ���� <
    wordtype=gettoken();
    if(wordtype!=INCLUDE&&wordtype!=DEFINE)
    {
        printf("�� %d �б���Ԥ�������\n",line);
        return NULL;
    }
    else if(wordtype==INCLUDE)
    {
        root->child[0]=makepoint(1,"�ļ�����",token_text);
        wordtype=gettoken();
        if(wordtype!=DQUA&&wordtype!=SM)//���������ַ��Ȳ��� " �ֲ��� < ˵���д��� ��������null
        {
            printf("�� %d �б���Ԥ�����ļ���������\n",line);
            return NULL;
        }
        else
        {
            sign=wordtype;//�������
            if(sign==SM)
            {
                sign=BG;
            }
            wordtype=gettoken();
            if(wordtype!=IDENT)
            {
                printf("�� %d �б���Ԥ�����ļ���������\n",line);
                return NULL;
            }
            strcat(filename,token_text);
            wordtype=gettoken();
            if(wordtype!=RAD)//�����һ���ַ�����С����
            {
                printf("�� %d �б���Ԥ�����ļ���������\n",line);
                return NULL;
            }
            strcat(filename,token_text);//����С����
            wordtype=gettoken();
            if(wordtype!=IDENT&&(strcmp(token_text,"h")!=0||strcmp(token_text,"c")!=0))
            {
                printf("�� %d �б���Ԥ�����ļ���������\n",line);
                return NULL;
            }
            strcat(filename,token_text);//�����ļ���ʽ
            wordtype=gettoken();//�ӹ��ַ�  >
            if(wordtype!=sign)
            {
                printf("�� %d �б���Ԥ�����ļ���������\n",line);
                return NULL;
            }
            root->child[0]->child[0]=makepoint(0,"�ļ���",filename);
        }
        wordtype=gettoken();
    }
    else
    {
        root->child[0]=makepoint(2,"\0",token_text);
        wordtype=gettoken();
        if(wordtype!=IDENT)//�����һ���ַ����Ǳ�ʶ������������null
        {
            printf("�� %d �к궨�����\n",line);
            return NULL;
        }
        if((t=fgetc(fp))=='(')//�����һ����ĸ���� ( ˵���Ǻ��κ궨��
        {
            strcat(root->child[0]->name,"���κ궨��");
            root->child[0]->define=3;
            root->child[0]->child[0]=makepoint(0,"���滻��",token_text);
            r=root->child[0]->child[1]=makepoint(2,"�β�","\0");
            wordtype=gettoken();
            while(1)
            {
                if(wordtype!=RSP)
                {
                    if(wordtype!=IDENT)//����Ȳ��������ֲ��Ǳ�ʶ������������NULL
                    {
                        printf("�� %d �к궨���������\n",line);
                        return NULL;
                    }
                    r->child[0]=makepoint(0,"  ID",token_text);
                }
                else//���else����������һ�ν���whileѭ��ʱ�ǲ������ţ�����ǵĻ�ֱ���ò���Ϊ�վͺ���
                {
                    free(r);
                    root->child[0]->child[0]=NULL;
                    break;
                }
                wordtype=gettoken();
                if(wordtype==COM)
                {
                    wordtype=gettoken();
                    r->child[1]=makepoint(2,"\0","\0");
                    r=r->child[1];
                }
                else if(wordtype==RSP)//�����һ���ַ���������
                {
                    r->child[1]=NULL;
                    break;
                }
            }
            wordtype=gettoken();
            root->child[0]->child[2]=makepoint(1,"���滻Ϊ���ʽ","\0");
            root->child[0]->child[2]->child[0]=Expression(ENTER);
            wordtype=ENTER;//�����һ����ԭ����  ���Expression�������н���  ��ô��ʱwordtype��ֵ������13  Ҳ����Expression�������ջ����ֹ�� #
        }
        else if(t==' ')//����ǿո���˵���ǲ��������ĺ궨��
        {
            strcat(root->child[0]->name,"�����κ궨��");
            root->child[0]->child[0]=makepoint(0,"���滻��ʶ��",token_text);
            wordtype=gettoken();
            if(wordtype<  IDENT&&wordtype>CHAR_CONST)
            {
                printf("�� %d �к궨�����\n",line);
                return NULL;
            }
            root->child[0]->child[1]=makepoint(0,"�滻��",token_text);
            wordtype=gettoken();
        }
        else//����Ȳ��ǿո��Ҳ�������ţ���������null
        {
            printf("�� %d �к궨�����\n",line);
            return NULL;
        }
    }
    if(wordtype!=ENTER)
    {
        printf("�� %d �б���Ԥ�������\n",line);
        return NULL;
    }
    definitionflag=0;
    return root;
}

_step* ExtVar(char type[],char ident[])//�ⲿ��������
{
    _step * now;
    _step * step=makepoint(2,"�ⲿ��������","\0");//�������� 1 �ͽڵ�
    step->child[0]=makepoint(0,"����",type);//�������� 0 �ͽڵ�
    step->child[1]=makepoint(2,"�ⲿ��������","\0");
    step->child[1]->child[0]=makepoint(0,"  ID",ident);
    now=step->child[1];
    while(1)
    {
        if(wordtype!=COM&&wordtype!=SEM)
        {
            printf("�� %d ��������������\n",line);
            return NULL;
        }
        if(wordtype==SEM)//��������ֺ� ��˵�����е��ⲿ�����������
        {
            now->child[1]=NULL;
            return step;
        }
        wordtype=gettoken();
        if(wordtype!=IDENT)
        {
            printf("�� %d ��������������\n",line);
            return NULL;
        }
        now->child[1]=makepoint(2,"�ⲿ��������","\0");
        now->child[1]->child[0]=makepoint(0,"  ID",token_text);
        now=now->child[1];
        wordtype=gettoken();
    }
}

_step* FuncDef(char type[],char ident[])//��������
{
    _step * step=makepoint(3,"��������","\0");//�������������ӵĽڵ�
    step->child[0]=makepoint(0,"����",type);//���溯������
    step->child[1]=makepoint(1,"������",ident);//���溯�����Լ����������Ľڵ�
    wordtype=gettoken();
    step->child[1]->child[0]=FuncPraDef();//�������������﷨����Ϊ������������
    wordtype=gettoken();
    if(wordtype==SEM) strcpy(step->name,"��������");//�������ֺţ���Ϊ����ԭ��������������Ϊ��
    else if(wordtype==LBP)//�������������ţ�����ú������ӳ��򣬽��䷵�ص�ָ����Ϊstep�ĵ���������
        step->child[2]=ComplexStence();
    else //�������Ȳ��Ƿֺ��ֲ���������ţ���˵�����Ŵ��󣬱�������null
    {
        printf("�� %d �к������������ִ���\n",line);
        return NULL;
    }
    return step;
}

_step* FuncPraDef()//������������
{
    if(wordtype==RSP)//���ֱ�Ӷ��������ţ���˵���˺���û���βΣ�����null
        return NULL;
    _step * root=makepoint(2,"�β�����","\0");
    if(wordtype>CHAR||wordtype<INT)//��������Ĳ��ǲ������͹ؼ��֣���������null
    {
        printf("�� %d �к�������������ִ���\n",line);
        return NULL;
    }
    root->child[0]=makepoint(2,"�β�","\0");
    root->child[0]->child[0]=makepoint(0,"����",token_text);
    wordtype=gettoken();
    if(wordtype!=IDENT)//��������Ĳ��Ǳ�ʶ������������null
    {
        printf("�� %d �к�������������ִ���\n",line);
        return NULL;
    }
    root->child[0]->child[1]=makepoint(0,"  ID",token_text);
    wordtype=gettoken();
    if(wordtype==COM)
    {
        wordtype=gettoken();
        root->child[1]=FuncPraDef();
    }
    if(wordtype!=RSP)//����������ַ��Ȳ��Ƕ����ֲ��������ţ���������null
    {
        printf("�� %d �к�������������ִ���\n",line);
        return NULL;
    }
    return root;
}

_step* ComplexStence()//�����嶨�壨������䣩
{
    _step* root=makepoint(2,"�������","\0");//��һ�����������洢�ֲ��������壬�ڶ������������洢�������
    wordtype=gettoken();
    if(wordtype>=INT&&wordtype<=CHAR)//��������ĵ�һ���ַ�Ϊ�ؼ������ͣ������ֲ����������ӳ���
    {
        root->child[0]=LocVar();
        wordtype=gettoken();
    }
    else root->child[0]=NULL;
    root->child[1]=StenceList();
    
    if(wordtype!=RBP)
    {
    	printf("\ncheck********%d****%d**%d****\n",wordtype,LBP,RBP);
        printf("�� %d �к��������\n",line);
        return NULL;
    }
    return root;
}

_step* LocVar()//�ֲ���������
{
    _step* root=makepoint(2,"�ֲ�������������","\0");
    root->child[0]=makepoint(2,"�ֲ���������","\0");
    root->child[0]->child[0]=makepoint(0,"����",token_text);
    _step* now=root->child[0];
    while(1)
    {
        wordtype=gettoken();
        if(wordtype!=  IDENT)
        {
            printf("�� %d �оֲ��������г���\n",line);
            return NULL;
        }
        now->child[1]=makepoint(2,"\0","\0");
        now->child[1]->child[0]=makepoint(0,"  ID",token_text);
        wordtype=gettoken();
        if(wordtype!=SEM&&wordtype!=COM)
        {
            printf("�� %d �оֲ��������г���\n",line);
            return NULL;
        }
        if(wordtype==SEM)
        {
            now->child[1]->child[1]=NULL;
            return root;
        }
        now=now->child[1];
    }
}

_step* StenceList()//��������������� 
{
    _step * root=makepoint(2,"�������","\0");
    root->child[0]=Stence();
    if(root->child[0]==NULL&&errors>0)//����ӳ��򷵻ص�ָ��Ϊ��  �ұ����errors������  ˵��������г����˴��� ���� ������null
    {
        printf("�� %d �к�����������\n",line);
        return NULL;
    }
    else if(root->child[0]==NULL&&errors==0)//���errors==0��stence������null ˵��û��������� û�д��� �����豨��ֱ�ӷ���null
    {
        free(root);
        root=NULL;
    }
    else//���û�г��ִ�����root�ĵڶ������ӵ�����һ���������
    {
        if(strcmp(root->child[0]->name,"if���")!=0)
            wordtype=gettoken();
        root->child[1]=StenceList();
    }
    return root;//����root���ڵ�
}

_step* Stence()//���
{
    _step * root;//�������ڵ㣬���ڷ���
    switch(wordtype)
    {
    case IF:
        wordtype=gettoken();
        if(wordtype!=LSP)//���û�ж���С����
        {
            errors+=1;//���ִ���
            return NULL;
        }
        wordtype=gettoken();
        _step * conditionofif=makepoint(1,"����","\0");
        conditionofif->child[0]=Expression(RSP);//���ڴ洢if��������
        if(conditionofif==NULL)//����������������û���������������
        {
            errors+=1;
            printf("�� %d ��if�����������\n",line);
            return NULL;
        }
        wordtype=gettoken();//��ȡ��һ���ַ���
        _step * contentofif=makepoint(1,"if�Ӿ�","\0");
        contentofif->child[0]=Stence();//����һ���µ���䴦����򣬱����䷵�صĸ��ڵ�
        wordtype=gettoken();//��ȡ��һ���ַ���
        if(wordtype!=ELSE) root=makepoint(2,"if���","if");//�������else ����if���ڵ�
        else//�����else����if-else���ڵ�
        {
            root=makepoint(3,"if-else���","if");
            wordtype=gettoken();
            root->child[2]=makepoint(1,"else�Ӿ�","else");
            root->child[2]->child[0]=Stence();
        }
        root->child[0]=conditionofif;
        root->child[1]=contentofif;
        ///�ƺ�����ͳһ��return root;
        break;
    case WHILE:
        wordtype=gettoken();
        if(wordtype!=LSP)//���û�ж���С����
        {
            errors+=1;//���ִ���
            return NULL;
        }
        root=makepoint(2,"while���","while");
        wordtype=gettoken();
        root->child[0]=makepoint(1,"����","\0");
        root->child[0]->child[0]=Expression(RSP);//���ڴ洢while��������
        if(root->child[0]==NULL)
        {
            errors+=1;
            printf("�� %d ��while�����������\n",line);
            return NULL;
        }
        wordtype=gettoken();//��ȡ��һ���ַ���
        root->child[1]=makepoint(1,"ѭ����","\0");
        root->child[1]->child[0]=Stence();//����whileѭ�����м�ѭ����
        break;
    case FOR:
        wordtype=gettoken();
        if(wordtype!=LSP)//���û�ж���С����
        {
            errors+=1;
            printf("�� %d ��for�����������\n",line);
            return NULL;
        }
        root=makepoint(4,"for���","for");
        wordtype=gettoken();
        root->child[0]=makepoint(1,"���α��ʽ","\0");
        root->child[0]->child[0]=Stence();//for���ĵ��α��ʽ
        wordtype=gettoken();
        root->child[1]=makepoint(1,"����","\0");
        root->child[1]->child[0]=Expression(SEM);//for�����������ʽ
        if(root->child[1]==NULL)
        {
            errors+=1;
            printf("�� %d ��for�����������\n",line);
            return NULL;
        }
        wordtype=gettoken();
        if(wordtype!=RSP)
        {
            root->child[3]=makepoint(1,"ĩβѭ����","\0");
            root->child[3]->child[0]=Expression(RSP);//���ڴ洢for����ĩβѭ����
        }
        wordtype=gettoken();
        root->child[2]=makepoint(1,"�м�ѭ����","\0");
        root->child[2]->child[0]=Stence();//����for����м�ѭ����
        break;
    case BREAK:
        wordtype=gettoken();
        if(wordtype!=SEM)//���break����ֱ�Ӹ��ŵĲ��Ƿֺţ�˵�����Ը�ʽ���󣬱�������null
        {
            errors+=1;
            printf("�� %d ��break����ʽ����\n",line);
            return NULL;
        }
        root=makepoint(0,"break���","\0");
        break;
    case CONTINUE:
        wordtype=gettoken();
        if(wordtype!=SEM)//ͬ��
        {
            errors+=1;
            printf("�� %d ��continue����ʽ����\n",line);
            return NULL;
        }
        root=makepoint(0,"continue���","\0");
        break;
    case RETURN:
        wordtype=gettoken();
        if(wordtype!=  IDENT&&wordtype!=INT_CONST&&wordtype!=CHAR_CONST&&wordtype!=LONG_CONST&&wordtype!=FLOAT_CONST)
        {
            errors+=1;
            printf("�� %d ��return����ʽ����\n",line);
            return NULL;
        }
        root=makepoint(1,"return���","\0");//used to be 1
        wordtype=gettoken();
        break;
    case LBP:
        wordtype=gettoken();
        root=StenceList();
        break;
    case LSP:
    case IDENT:
    case INT_CONST:
    case CHAR_CONST:
    case LONG_CONST:
    case FLOAT_CONST:
        root=Expression(SEM);
        if(root==NULL)//������ʽ���ִ��󣬱�������null
        {
            errors+=1;
            printf("�� %d �б��ʽ��ʽ����\n",line);
            return NULL;
        }
        break;
    case RBP:
        return NULL;//������������־
    case SEM:
        return NULL;//��ѭ��
    }
    return root;
}

_step* Expression(int endtype)//�����б������ǣ�Ϊ0ʱ�ֺŽ�����Ϊ1ʱ�����Ž���
{
    int topofop=0,topofopn=0;
    _keywords op[20];
    op[0].name[0]='#';
    op[0].name[1]='\0';
    op[0].placeinenum=DEF;//��ֹ����ջ
    _step* opn[20],*point;
    while((wordtype!=DEF||op[topofop].placeinenum!=DEF)&&errors==0)
    {
        if(wordtype==IDENT||wordtype>=INT_CONST&&wordtype<=CHAR_CONST)
        {
            point=makepoint(0,"  ID",token_text);
            opn[topofopn++]=point;
            wordtype=gettoken();
        }
        else if(wordtype>=ADD&&wordtype<=OR)
        {
            if(wordtype==endtype) wordtype=DEF;
            else if(wordtype==LSP)//����������ŵĻ������⿼��
            {
                point=makepoint(2,"�����",token_text);
                wordtype=gettoken();
                point->child[0]=Expression(RSP);
                point->child[1]=makepoint(0,"�����",")");
                opn[topofopn++]=point;
                wordtype=gettoken();
            }
            else switch(priority[op[topofop].placeinenum][wordtype])
            {
            case -1://���ջ����������ȼ�С���¶��������
                op[++topofop].name[0]='\0';
                strcat(op[topofop].name,token_text);//�����������
                op[topofop].placeinenum=wordtype;
                wordtype=gettoken();
                break;
            case 1://���ջ����������ȼ������¶���������
                if(topofop>=1) point=makepoint(2,"�����",op[topofop--].name);//���������ջ��Ԫ�س�ջ����Ҫ��ջ��λ������һ����Ԫ
                else errors+=1;
                if(topofopn>=2) point->child[1]=opn[--topofopn];
                else errors+=1;
                if(topofopn>=1) point->child[0]=opn[--topofopn];
                else errors+=1;
                opn[topofopn++]=point;
                break;
            default://���û�д�С��ϵ��˵��Ӧ���ǵ����˱��ʽ�Ľ�����־
                if(wordtype==endtype) wordtype=DEF;
                else errors+=1;
            }
        }
        else if(wordtype==endtype) wordtype=DEF;
        else errors+=1;
    }
    if(topofopn==1&&op[topofop].placeinenum==DEF&&errors==0)
        return opn[--topofopn];
    errors+=1;
    printf("�� %d �б��ʽ����\n",line);
    return NULL;
}

void ASTTraverse(_step* root)
{
	//puts("AST");
    int i;
    if(root!=NULL)
    {
        if(root->name[0]!='\0')
        {
            if(strcmp(root->name,"�ⲿ��������")!=0&&strcmp(root->name,"�ⲿ��������")!=0&&strcmp(root->name,"�ֲ�������������")!=0&&strcmp(root->name,"�������")!=0&&strcmp(root->name,"�β�����")!=0)
            {
                for(i=0;i<indentflag;i++) printf("   ");
                printf("%s  ",root->name);
                printf("%s\n",root->text);
            }
        }
        if(strcmp(root->name,"�������")==0||strcmp(root->name,"while���")==0||strcmp(root->name,"if���")==0||strcmp(root->name,"if-else���")==0||strcmp(root->name,"for���")==0)//��������������
        {
            indentflag+=1;
        }
    }
    else return;
    for(i=0;i<root->define;i++)
    {
        ASTTraverse(root->child[i]);
    }
    if(strcmp(root->name,"�������")==0||strcmp(root->name,"while���")==0||strcmp(root->name,"if���")==0||strcmp(root->name,"if-else���")==0||strcmp(root->name,"for���")==0)//��������������
    {
        indentflag-=1;
    }
    //else return;
}

void CodeTraverse(_step* root)
{
	//puts("1"); 
    int i;
    _step* r;//����д������
    if(root!=NULL)
    {
        if(strcmp("�ⲿ��������",root->name)==0||strcmp("�������",root->name)==0)
        {
            fprintf(nfp,"\n");
            for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
        }
        if(strcmp("�����",root->name)==0)
        {
            ExpTraverse(root);
            fprintf(nfp,";");
            return;
        }
        else if(strcmp("��������",root->name)==0)
        {
            fprintf(nfp,"%s ",root->child[0]->text);
            fprintf(nfp,"%s(",root->child[1]->text);
            CodeTraverse(root->child[1]->child[0]);
            fprintf(nfp,")\n{");
            CodeTraverse(root->child[2]);
            fprintf(nfp,"\n}");
            return;
        }
        else if(strcmp("if���",root->name)==0||strcmp("if-else���",root->name)==0||strcmp("while���",root->name)==0)//if  ���� if-else
        {
            fprintf(nfp,"%s(",root->text);
            ExpTraverse(root->child[0]);
            fprintf(nfp,")\n");
            for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
            fprintf(nfp,"{");
            indentflag+=1;
            if(strcmp(root->child[1]->child[0]->name,"�������")!=0)
            {
                fprintf(nfp,"\n");
                for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
            }
            CodeTraverse(root->child[1]);
            indentflag-=1;
            fprintf(nfp,"\n");
            for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
            fprintf(nfp,"}");
            if(strcmp("if-else���",root->name)==0)// �����if-else
            {
                fprintf(nfp,"\n");
                for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
                fprintf(nfp,"%s\n",root->child[2]->text);
                for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
                fprintf(nfp,"{");
                indentflag+=1;
                if(strcmp(root->child[2]->child[0]->name,"�������")!=0)
                {
                    fprintf(nfp,"\n");
                    for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
                }
                for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
                CodeTraverse(root->child[2]->child[0]);
                indentflag-=1;
                fprintf(nfp,"\n");
                for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
                fprintf(nfp,"}");
            }
            return;
        }
        else if(strcmp("for���",root->name)==0)
        {
            fprintf(nfp,"%s(",root->text);
            CodeTraverse(root->child[0]);
            ExpTraverse(root->child[1]);
            fprintf(nfp,";");
            ExpTraverse(root->child[3]);
            fprintf(nfp,")\n");
            for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
            fprintf(nfp,"{");
            indentflag+=1;
            if(strcmp(root->child[2]->child[0]->name,"�������")!=0)
            {
                fprintf(nfp,"\n");
                for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
            }
            CodeTraverse(root->child[2]->child[0]);
            indentflag-=1;
            fprintf(nfp,"\n");
            for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
            fprintf(nfp,"}");
            return;
        }
        else if(strcmp("����Ԥ����",root->name)==0)
        {
            fprintf(nfp,"#%s ",root->child[0]->text);
            if(strcmp("�ļ�����",root->child[0]->name)==0)//������ļ�����
            {
                fprintf(nfp,"<%s>",root->child[0]->child[0]->text);
            }
            else if(strcmp("�����κ궨��",root->child[0]->name)==0)//����ǲ����κ궨��
            {
                fprintf(nfp,"%s %s",root->child[0]->child[0]->text,root->child[0]->child[1]->text);
            }
            else
            {
                fprintf(nfp,"%s(",root->child[0]->child[0]->text);
                r=root->child[0]->child[1];
                while(1)
                {
                    fprintf(nfp,"%s",r->child[0]->text);
                    if(r->child[1]!=NULL)
                        fprintf(nfp,",");
                    else break;
                    r=r->child[1];
                }
                fprintf(nfp,") ");
                ExpTraverse(root->child[0]->child[2]->child[0]);
            }
            return ;
        }
        if(root->text[0]!='\0'&&strcmp("�β�",root->name)!=0)
        {
                fprintf(nfp,"%s ",root->text);
        }
        else if(strcmp("�β�����",root->name)==0)
        {
            r=root;
            while(1)
            {
                fprintf(nfp,"%s ",r->child[0]->child[0]->text);
                fprintf(nfp,"%s",r->child[0]->child[1]->text);
                if(r->child[1]!=NULL)
                    fprintf(nfp,",");
                else break;
                r=r->child[1];
            }
            return;
        }
        if(strcmp(root->name,"�������")==0)//��������������
        {
            indentflag+=1;
        }
    }
    else return;
    for(i=0;i<root->define;i++)
    {
        CodeTraverse(root->child[i]);
    }
    if(strcmp("�ⲿ��������",root->name)==0)
    {
        fprintf(nfp,";");
    }
    if(strcmp(root->name,"�������")==0)//��������������
    {
        indentflag-=1;
    }
}

void ExpTraverse(_step* root)
{
    if(root!=NULL)
    {
        if(strcmp(root->text,"(")==0)
        {
            fprintf(nfp,"(");
            ExpTraverse(root->child[0]);
            fprintf(nfp,")");
            return;
        }
        else
        {
            ExpTraverse(root->child[0]);
            fprintf(nfp,"%s",root->text);
            ExpTraverse(root->child[1]);
        }
    }
    return;
}
