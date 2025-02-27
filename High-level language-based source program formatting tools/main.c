#include <stdio.h>
#include <stdlib.h>
#include <string.h>
          /**     +  -   *  /   (   )   =    >  <  >=  <=  != ==  #  %  && ||                                                                                             [  ]   {   }  ;  ,  .   "    '  */
enum token_kind {ADD,SUB,MUL,DIV,LSP,RSP,ASSIGN,BG,SM,BGEQ,SMEQ,NEQ,EQ,DEF,MOD,AND,OR,ERROR_TOKEN,INT,LONG,FLOAT,DOUBLE,CHAR,IDENT,INT_CONST,LONG_CONST,FLOAT_CONST,DOUBLE_CONST,CHAR_CONST,LMP,RMP,LBP,RBP,SEM,COM,RAD,DQUA,SQUA,IF,ELSE,WHILE,FOR,RETURN,BREAK,CONTINUE,DEFINE,INCLUDE,ENTER};
///0代表相等 -1代表小于 1代表大于 -2代表无（可能是错误）
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
    int define;//0:数据节点  1:两个指针节点 2:三个指针节点  以此类推
    char name[20];
    char text[20];
    struct STEP * child[4];
}_step;

FILE * fp,*nfp;//文件指针
_keywords keywords[13];//用来建立关键字的查找表
char token_text[20];//用于保存字符自身值
int wordtype;//用于记录提取出的字符属于哪个类别
int errors=0;//用来记录各种语句中是否出现错误
int definitionflag=0;//用于判断是否处在编译预处理情况下
int line=1;//用来记录第几行
int annotatorflag=0;//用于记录现在字符所处的位置是不是在注释行内
int indentflag=0;//用于格式化输出的时候判断是否要缩进

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
    setupkeywords();//建立关键字查找表
    printf("请把您想要进行分析的源文件放置在与本程序同级的文件夹中\n");
    printf("您想要进行分析的源文件的名称（无须添加扩展名）是：");
    gets(filename);//读入用户需要进行分析的文件名
    newfilename[0]='\0';
    strcat(newfilename,filename);
    strcat(filename,".c");
    fp=fopen(filename,"r");
    strcat(newfilename,"1.c");
    nfp=fopen(newfilename,"w");
    program();
    printf("\n源程序已经按照缩进编排的版式存如文件 %s 中",newfilename);
    printf("\n按任意键继续！");
    getch();
}

_step* makepoint(int define,char name[],char text[])//建立一个广义表节点
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

void setupkeywords()//建立关键字查找表
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

int checkkeywords()//用于检验token_text中是否为关键字，如果不为关键字则返回  IDENT，反之返回对应的关键字
{
    int i;
    for(i=0;i<13;i++)
    {
        if(strcmp(token_text,keywords[i].name)==0)
            return keywords[i].placeinenum;
    }
    return IDENT;
}

int gettoken()  //用于提取一个标识符或关键字
{
    int i=0;//用于记录token_text 或者 annotator数组的当前存储位置
    int c;//用于提取单个字符
    token_text[0]='\0';//初始化，便于存储新的标识符或关键字
    if(annotatorflag==0)
    {
        while((c=getc(fp))==' '||c=='\n'||c=='\t')
        {
            if(c=='\n')
            {
                line++;
                if(definitionflag==1) return ENTER;
            }
        }//过滤空格符以及回车符
        if((c>='a'&&c<='z')||(c>='A'&&c<='Z'))//如果提取出的第一个字符为字母
        {
            do
            {
                strcat(token_text,&c);
                c=fgetc(fp);
                if(c=='[')//如果检测到的是数组
                {
                    strcat(token_text,&c);
                    c=fgetc(fp);
                    while(c>='0'&&c<='9'||c==']')
                    {
                        strcat(token_text,&c);
                        if(c==']')
                        {
                            printf("标识符    %s\n",token_text);
                            return IDENT;//数组元素默认为标识符
                        }
                        c=fgetc(fp);
                    }
                    return ERROR_TOKEN;
                }
            }
            while((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9'));//如果后续字符仍然为字母或者数字，则将其继续添加在token_text中
            ungetc(c,fp);
            return checkkeywords();
        }
        else if(c>='0'&&c<='9')//如果提取出的第一个字符为数字
        {
            //strcat(token_text,&c);
            if(c=='0')//如果第一位是0，那么需要区分到底是八进制还是十六进制
            {
                strcat(token_text,&c);
                c=fgetc(fp);
                if(c=='x')//如果为x则说明是十六进制数
                {
                    for(strcat(token_text,&c),c=fgetc(fp);(c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='.';c=fgetc(fp))
                    {
                        if((c>'f'&&c<='z')||(c>'F'&&c<'Z'))
                        {
                            if(c=='l'||c=='L')//用于检测是否为long类型
                            {
                                strcat(token_text,&c);
                                if(strcmp(token_text,'0xl')==0||strcmp(token_text,'0xL')==0) return ERROR_TOKEN;//如果读入为0xl或0xL则返回错误
                                printf("长整形数  %s\n",token_text);
                                return LONG_CONST;//此时说明字符串表表示的是长整型数
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
                            printf("双精度数  %s\n",token_text);
                            return DOUBLE_CONST;
                        }
                        strcat(token_text,&c);
                    }
                    ungetc(c,fp);
                    printf("整形数    %s\n",token_text);
                    return INT_CONST;
                }
                else if(c=='.')//说明它是一个double
                {
                    strcat(token_text,&c);
                    for(c=fgetc(fp);c>='0'&&c<='9'||c=='f'||c=='F';c=fgetc(fp))
                    {
                        if(c=='f'||c=='F')//用于检测是否为float类型
                        {
                            strcat(token_text,&c);
                            if(strcmp(token_text,'0.f')==0||strcmp(token_text,'0.F')==0) return ERROR_TOKEN;//如果读入为0xl或0xL则返回错误
                            return FLOAT_CONST;//此时说明字符串表表示的是浮点数
                        }
                        strcat(token_text,&c);
                    }
                    ungetc(c,fp);
                    printf("双精度数  %s\n",token_text);
                    return DOUBLE_CONST;
                }
                else//如果不是则说明为八进制数
                {
                    for(;(c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='.';c=fgetc(fp))
                    {
                        if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='8'||c=='9')
                        {
                            if(c=='l'||c=='L')//用于检测是否为long类型
                            {
                                strcat(token_text,&c);
                                c=fgetc(fp);
                                if((c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z'))
                                    return ERROR_TOKEN;
                                ungetc(c,fp);
                                if(strcmp(token_text,'0l')==0||strcmp(token_text,'0L')==0) return ERROR_TOKEN;//如果读入为0l或0L则返回错误
                                printf("长整形数  %s\n",token_text);
                                return LONG_CONST;//此时说明字符串表表示的是长整型数
                            }
                            else return ERROR_TOKEN;
                        }
                        else if(c=='.')
                        {
                            strcat(token_text,&c);
                            for(c=fgetc(fp);c>='0'&&c<='7'||c=='f'||c=='F';c=fgetc(fp))
                            {
                                if(c=='f'||c=='F')//用于检测是否为float类型
                                {
                                    strcat(token_text,&c);
                                    printf("浮点数    %s\n",token_text);
                                    return FLOAT_CONST;//此时说明字符串表表示的是浮点数
                                }
                                strcat(token_text,&c);
                            }
                            ungetc(c,fp);
                            printf("双精度数  %s\n",token_text);
                            return DOUBLE_CONST;
                        }
                        strcat(token_text,&c);
                    }
                    ungetc(c,fp);
                    printf("整形数    %s\n",token_text);
                    return INT_CONST;
                }
            }
            else//如果第一位不是0则说明此时该数字为十进制
            {
                for(;(c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='.';c=fgetc(fp))
                {
                    if((c>='a'&&c<='z')||(c>='A'&&c<='Z'))
                    {
                        if(c=='l'||c=='L')//用于检测是否为long类型
                        {
                            strcat(token_text,&c);
                            c=fgetc(fp);
                            if((c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z'))
                                return ERROR_TOKEN;
                            ungetc(c,fp);
                            printf("长整型数  %s\n",token_text);
                            return LONG_CONST;//此时说明字符串表表示的是长整型数
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
                                printf("浮点数    %s\n",token_text);
                                return FLOAT_CONST;
                            }
                            strcat(token_text,&c);
                        }
                        ungetc(c,fp);
                        printf("双精度数  %s\n",token_text);
                        return DOUBLE_CONST;
                    }
                    strcat(token_text,&c);
                }
                ungetc(c,fp);
                printf("整形数    %s\n",token_text);
                return INT_CONST;
            }
        }
        else switch(c)//如果读取到的字符既不是字母又不是数字
        {
        case '='://是 = 的时候需要检测它是关系语句还是赋值语句
            strcat(token_text,&c);
            c=fgetc(fp);
            if(c=='=')
            {
                strcat(token_text,&c);
                printf("逻辑符    %s\n",token_text);
                return EQ;//是 ==
            }
            ungetc(c,fp);
            printf("赋值符    %s\n",token_text);
            return ASSIGN;//是 =
        case '+':
            strcat(token_text,&c);
            printf("运算符    %s\n",token_text);
            return ADD;
        case '-':
            strcat(token_text,&c);
            printf("运算符    %s\n",token_text);
            return SUB;
        case '*':
            strcat(token_text,&c);
            printf("运算符    %s\n",token_text);
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
            printf("运算符    %s\n",token_text);
            return DIV;
        case '%':
            strcat(token_text,&c);
            printf("运算符    %s\n",token_text);
            return MOD;
        case '!':
            strcat(token_text,&c);
            if((c=fgetc(fp))=='=')
            {
                strcat(token_text,&c);
                printf("逻辑符    %s\n",token_text);
                return NEQ;
            }
            ungetc(c,fp);
            return ERROR_TOKEN;// ! 符号算是单目运算符吧  反正先算错误了
        case '>'://需要考虑是 > 还是 >=
            strcat(token_text,&c);
            if((c=fgetc(fp))=='=')
            {
                strcat(token_text,&c);
                printf("逻辑符    %s\n",token_text);
                return BGEQ;
            }
            else ungetc(c,fp);
            printf("逻辑符    %s\n",token_text);
            return BG;
        case '<'://需要考虑是 < 还是 <=
            strcat(token_text,&c);
            if((c=fgetc(fp))=='=')
            {
                strcat(token_text,&c);
                printf("逻辑符    %s\n",token_text);
                return SMEQ;
            }
            else ungetc(c,fp);
            printf("逻辑符    %s\n",token_text);
            return SM;
        case '&':
            strcat(token_text,&c);
            if((c=fgetc(fp))=='&')
                strcat(token_text,&c);
            else return ERROR_TOKEN;
            printf("逻辑符    %s\n",token_text);
            return AND;
        case '|':
            strcat(token_text,&c);
            if((c=fgetc(fp))=='|')
                strcat(token_text,&c);
            else return ERROR_TOKEN;
            printf("逻辑符    %s\n",token_text);
            return OR;
        case '#':
            strcat(token_text,&c);
            printf("宏定义符  %s\n",token_text);
            return DEF;
        case ';':
            strcat(token_text,&c);
            printf("分号符    %s\n",token_text);
            return SEM;
        case ',':
            strcat(token_text,&c);
            return COM;
        case '(':
            strcat(token_text,&c);
            printf("括号符    %s\n",token_text);
            return LSP;
        case ')':
            strcat(token_text,&c);
            printf("括号符    %s\n",token_text);
            return RSP;
        case '{':
            strcat(token_text,&c);
            printf("括号符    %s\n",token_text);
            return LBP;
        case '}':
            strcat(token_text,&c);
            printf("括号符    %s\n",token_text);
            return RBP;
        case '"':
            strcat(token_text,&c);
            return DQUA;
        case '\''://如果读入单引号则需要考虑字符常量
            strcat(token_text,&c);
            c=fgetc(fp);
            if(c!='\\'&&c!='\'')
            {
                strcat(token_text,&c);
            }
            else if(c=='\\')//如果读入转义符
            {
                strcat(token_text,&c);
                c=fgetc(fp);
                strcat(token_text,&c);
            }
            else return ERROR_TOKEN;//直接连续写两个单引号，词法错误
            c=fgetc(fp);
            if(c!='\'')
            {
                ungetc(c,fp);
                return ERROR_TOKEN;
            }
            strcat(token_text,&c);
            printf("字符常量  %s\n",token_text);
            return CHAR_CONST;
        case '.':
            strcat(token_text,&c);
            return RAD;
        }
    }
    if(annotatorflag==1)//如果前一个符号是注释符//
    {
        while((c=fgetc(fp))!='\n'&&feof(fp)==0);//直到读入回车为止  注释结束  或者EOF代表文件结束
        ungetc(c,fp);
        annotatorflag=0;//将处于注释//状态的flag改回不处于注释状态
        return gettoken();
    }
    else if(annotatorflag==2)//如果前一个符号是注释符/*
    {
        while(feof(fp)==0)
        {
            c=fgetc(fp);
            if(c=='\n') line+=1;
            else if(c=='*')
            {
                if((c=fgetc(fp))=='/')//直到读到*/为止  注释结束  或者EOF代表文件结束
                {
                    annotatorflag=0;//将处于注释/*状态的flag改回不处于注释状态
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
    printf("\n在该源程序中检测到如下类型的字符串：\n");
    if((root=ExtDefList())!=NULL)//调用外部变量序列函数，正确时返回根节点，可以遍历
    {
        printf("\n根据源程序输出如下语法树：\n");//puts("-2"); 
        ASTTraverse(root);//puts("-1"); 
        CodeTraverse(root);//puts("0"); 
    }
    else exit(1);
}

_step* ExtDefList()//外部定义序列
{
    wordtype=gettoken();
    if(wordtype==EOF) return NULL;//读到文件尾，返回空指针
    _step * step=makepoint(2,"外部定义序列","\0");//申请广义表节点
    step->child[0]=ExtDef();//第一个子树为外部定义
    step->child[1]=ExtDefList();//第二个子树为下一个外部定义序列
    return step;
}

_step* ExtDef()//外部定义：：=外部变量定义||函数定义
{
    _step * step;//声明节点用于返回指针
    char type[5]="\0";//用于存储变量类型
    char ident[10]="\0";//用于存储变量名
    if(wordtype<=CHAR&&wordtype>=INT) //如果读到的字符串为变量类型关键字，继续，反之报错并返回NULL
    {
        strcat(type,token_text);//保存变量类型关键字
        wordtype=gettoken();//读取下一个字符串
        if(wordtype!=IDENT)
        {
            printf("第 %d 行外部定义出现错误！\n",line);
            return NULL;
        }
        strcat(ident,token_text);//保存读到的变量名
        wordtype=gettoken();
        if(wordtype==LSP) step=FuncDef(type,ident);//如果为 ( 符则进入函数定义
        else step=ExtVar(type,ident);//反之进入变量定义
    }
    else if(wordtype==DEF)//如果读到的字符为 #
    {
        step=MacDef();
    }
    else
    {
        printf("第 %d 行外部定义出错！\n",line);
        return NULL;
    }
    if(step!=NULL) return step;//如果step不为空，则说明完成了一次外部定义，返回step
}

_step* MacDef()//编译预处理
{
    definitionflag=1;
    _step * root=makepoint(1,"编译预处理",token_text);
    _step * r;//用于循环提取宏定义参数
    char filename[20]="\0";
    char t;//用于判断是否遇到回车符
    int sign;//用于保存文件包含时的符号是 " 还是 <
    wordtype=gettoken();
    if(wordtype!=INCLUDE&&wordtype!=DEFINE)
    {
        printf("第 %d 行编译预处理出错！\n",line);
        return NULL;
    }
    else if(wordtype==INCLUDE)
    {
        root->child[0]=makepoint(1,"文件包含",token_text);
        wordtype=gettoken();
        if(wordtype!=DQUA&&wordtype!=SM)//如果读入的字符既不是 " 又不是 < 说明有错误 报错并返回null
        {
            printf("第 %d 行编译预处理文件包含出错！\n",line);
            return NULL;
        }
        else
        {
            sign=wordtype;//保存符号
            if(sign==SM)
            {
                sign=BG;
            }
            wordtype=gettoken();
            if(wordtype!=IDENT)
            {
                printf("第 %d 行编译预处理文件包含出错！\n",line);
                return NULL;
            }
            strcat(filename,token_text);
            wordtype=gettoken();
            if(wordtype!=RAD)//如果下一个字符不是小数点
            {
                printf("第 %d 行编译预处理文件包含出错！\n",line);
                return NULL;
            }
            strcat(filename,token_text);//保存小数点
            wordtype=gettoken();
            if(wordtype!=IDENT&&(strcmp(token_text,"h")!=0||strcmp(token_text,"c")!=0))
            {
                printf("第 %d 行编译预处理文件包含出错！\n",line);
                return NULL;
            }
            strcat(filename,token_text);//保存文件格式
            wordtype=gettoken();//掠过字符  >
            if(wordtype!=sign)
            {
                printf("第 %d 行编译预处理文件包含出错！\n",line);
                return NULL;
            }
            root->child[0]->child[0]=makepoint(0,"文件名",filename);
        }
        wordtype=gettoken();
    }
    else
    {
        root->child[0]=makepoint(2,"\0",token_text);
        wordtype=gettoken();
        if(wordtype!=IDENT)//如果下一个字符不是标识符，报错，返回null
        {
            printf("第 %d 行宏定义出错！\n",line);
            return NULL;
        }
        if((t=fgetc(fp))=='(')//如果下一个字母就是 ( 说明是含参宏定义
        {
            strcat(root->child[0]->name,"含参宏定义");
            root->child[0]->define=3;
            root->child[0]->child[0]=makepoint(0,"被替换量",token_text);
            r=root->child[0]->child[1]=makepoint(2,"形参","\0");
            wordtype=gettoken();
            while(1)
            {
                if(wordtype!=RSP)
                {
                    if(wordtype!=IDENT)//如果既不是括号又不是标识符，报错，返回NULL
                    {
                        printf("第 %d 行宏定义参数错误！\n",line);
                        return NULL;
                    }
                    r->child[0]=makepoint(0,"  ID",token_text);
                }
                else//这个else是用来检测第一次进入while循环时是不是括号，如果是的话直接让参数为空就好了
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
                else if(wordtype==RSP)//如果下一个字符是右括号
                {
                    r->child[1]=NULL;
                    break;
                }
            }
            wordtype=gettoken();
            root->child[0]->child[2]=makepoint(1,"被替换为表达式","\0");
            root->child[0]->child[2]->child[0]=Expression(ENTER);
            wordtype=ENTER;//添加这一步的原因是  如果Expression正常运行结束  那么此时wordtype的值将会是13  也就是Expression中运算符栈的起止符 #
        }
        else if(t==' ')//如果是空格则说明是不含参数的宏定义
        {
            strcat(root->child[0]->name,"不含参宏定义");
            root->child[0]->child[0]=makepoint(0,"被替换标识符",token_text);
            wordtype=gettoken();
            if(wordtype<  IDENT&&wordtype>CHAR_CONST)
            {
                printf("第 %d 行宏定义出错！\n",line);
                return NULL;
            }
            root->child[0]->child[1]=makepoint(0,"替换符",token_text);
            wordtype=gettoken();
        }
        else//如果既不是空格符也不是括号，报错，返回null
        {
            printf("第 %d 行宏定义出错！\n",line);
            return NULL;
        }
    }
    if(wordtype!=ENTER)
    {
        printf("第 %d 行编译预处理出错！\n",line);
        return NULL;
    }
    definitionflag=0;
    return root;
}

_step* ExtVar(char type[],char ident[])//外部变量定义
{
    _step * now;
    _step * step=makepoint(2,"外部变量定义","\0");//申请广义表 1 型节点
    step->child[0]=makepoint(0,"类型",type);//申请广义表 0 型节点
    step->child[1]=makepoint(2,"外部变量序列","\0");
    step->child[1]->child[0]=makepoint(0,"  ID",ident);
    now=step->child[1];
    while(1)
    {
        if(wordtype!=COM&&wordtype!=SEM)
        {
            printf("第 %d 行声明变量出错！\n",line);
            return NULL;
        }
        if(wordtype==SEM)//如果读到分号 则说明本行的外部变量定义结束
        {
            now->child[1]=NULL;
            return step;
        }
        wordtype=gettoken();
        if(wordtype!=IDENT)
        {
            printf("第 %d 行声明变量出错！\n",line);
            return NULL;
        }
        now->child[1]=makepoint(2,"外部变量序列","\0");
        now->child[1]->child[0]=makepoint(0,"  ID",token_text);
        now=now->child[1];
        wordtype=gettoken();
    }
}

_step* FuncDef(char type[],char ident[])//函数定义
{
    _step * step=makepoint(3,"函数定义","\0");//声明有三个孩子的节点
    step->child[0]=makepoint(0,"类型",type);//保存函数类型
    step->child[1]=makepoint(1,"函数名",ident);//保存函数名以及函数参量的节点
    wordtype=gettoken();
    step->child[1]->child[0]=FuncPraDef();//建立函数参数语法树作为函数名的子树
    wordtype=gettoken();
    if(wordtype==SEM) strcpy(step->name,"函数声明");//如果读入分号，则为函数原型声明，函数体为空
    else if(wordtype==LBP)//如果读入左大括号，则调用函数体子程序，将其返回的指针作为step的第三个子树
        step->child[2]=ComplexStence();
    else //如果读入既不是分号又不是左大括号，则说明符号错误，报错，返回null
    {
        printf("第 %d 行函数体声明出现错误！\n",line);
        return NULL;
    }
    return step;
}

_step* FuncPraDef()//函数参数定义
{
    if(wordtype==RSP)//如果直接读到右括号，则说明此函数没有形参，返回null
        return NULL;
    _step * root=makepoint(2,"形参序列","\0");
    if(wordtype>CHAR||wordtype<INT)//如果读到的不是参数类型关键字，报错，返回null
    {
        printf("第 %d 行函数参数定义出现错误！\n",line);
        return NULL;
    }
    root->child[0]=makepoint(2,"形参","\0");
    root->child[0]->child[0]=makepoint(0,"类型",token_text);
    wordtype=gettoken();
    if(wordtype!=IDENT)//如果读到的不是标识符，报错，返回null
    {
        printf("第 %d 行函数参数定义出现错误！\n",line);
        return NULL;
    }
    root->child[0]->child[1]=makepoint(0,"  ID",token_text);
    wordtype=gettoken();
    if(wordtype==COM)
    {
        wordtype=gettoken();
        root->child[1]=FuncPraDef();
    }
    if(wordtype!=RSP)//如果读到的字符既不是逗号又不是右括号，报错，返回null
    {
        printf("第 %d 行函数参数定义出现错误！\n",line);
        return NULL;
    }
    return root;
}

_step* ComplexStence()//函数体定义（复合语句）
{
    _step* root=makepoint(2,"复合语句","\0");//第一个子树用来存储局部变量定义，第二个子树用来存储语句序列
    wordtype=gettoken();
    if(wordtype>=INT&&wordtype<=CHAR)//如果读到的第一个字符为关键字类型，则进入局部变量声明子程序
    {
        root->child[0]=LocVar();
        wordtype=gettoken();
    }
    else root->child[0]=NULL;
    root->child[1]=StenceList();
    
    if(wordtype!=RBP)
    {
    	printf("\ncheck********%d****%d**%d****\n",wordtype,LBP,RBP);
        printf("第 %d 行函数体错误！\n",line);
        return NULL;
    }
    return root;
}

_step* LocVar()//局部变量定义
{
    _step* root=makepoint(2,"局部变量定义序列","\0");
    root->child[0]=makepoint(2,"局部变量定义","\0");
    root->child[0]->child[0]=makepoint(0,"类型",token_text);
    _step* now=root->child[0];
    while(1)
    {
        wordtype=gettoken();
        if(wordtype!=  IDENT)
        {
            printf("第 %d 行局部变量序列出错！\n",line);
            return NULL;
        }
        now->child[1]=makepoint(2,"\0","\0");
        now->child[1]->child[0]=makepoint(0,"  ID",token_text);
        wordtype=gettoken();
        if(wordtype!=SEM&&wordtype!=COM)
        {
            printf("第 %d 行局部变量序列出错！\n",line);
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

_step* StenceList()//复合语句的语句序列 
{
    _step * root=makepoint(2,"语句序列","\0");
    root->child[0]=Stence();
    if(root->child[0]==NULL&&errors>0)//语句子程序返回的指针为空  且标记了errors大于零  说明语句序列出现了错误 报错 并返回null
    {
        printf("第 %d 行函数体语句错误！\n",line);
        return NULL;
    }
    else if(root->child[0]==NULL&&errors==0)//如果errors==0且stence返回了null 说明没有语句序列 没有错误 故无需报错，直接返回null
    {
        free(root);
        root=NULL;
    }
    else//语句没有出现错误，让root的第二个孩子等于下一个语句序列
    {
        if(strcmp(root->child[0]->name,"if语句")!=0)
            wordtype=gettoken();
        root->child[1]=StenceList();
    }
    return root;//返回root根节点
}

_step* Stence()//语句
{
    _step * root;//建立根节点，用于返回
    switch(wordtype)
    {
    case IF:
        wordtype=gettoken();
        if(wordtype!=LSP)//如果没有读到小括号
        {
            errors+=1;//出现错误
            return NULL;
        }
        wordtype=gettoken();
        _step * conditionofif=makepoint(1,"条件","\0");
        conditionofif->child[0]=Expression(RSP);//用于存储if语句的条件
        if(conditionofif==NULL)//不论条件语句出错还是没有条件，都算错误
        {
            errors+=1;
            printf("第 %d 行if语句条件出错！\n",line);
            return NULL;
        }
        wordtype=gettoken();//读取下一个字符串
        _step * contentofif=makepoint(1,"if子句","\0");
        contentofif->child[0]=Stence();//进入一个新的语句处理程序，保存其返回的根节点
        wordtype=gettoken();//读取下一个字符串
        if(wordtype!=ELSE) root=makepoint(2,"if语句","if");//如果不是else 则建立if语句节点
        else//如果是else则建立if-else语句节点
        {
            root=makepoint(3,"if-else语句","if");
            wordtype=gettoken();
            root->child[2]=makepoint(1,"else子句","else");
            root->child[2]->child[0]=Stence();
        }
        root->child[0]=conditionofif;
        root->child[1]=contentofif;
        ///似乎可以统一用return root;
        break;
    case WHILE:
        wordtype=gettoken();
        if(wordtype!=LSP)//如果没有读到小括号
        {
            errors+=1;//出现错误
            return NULL;
        }
        root=makepoint(2,"while语句","while");
        wordtype=gettoken();
        root->child[0]=makepoint(1,"条件","\0");
        root->child[0]->child[0]=Expression(RSP);//用于存储while语句的条件
        if(root->child[0]==NULL)
        {
            errors+=1;
            printf("第 %d 行while语句条件出错！\n",line);
            return NULL;
        }
        wordtype=gettoken();//读取下一个字符串
        root->child[1]=makepoint(1,"循环体","\0");
        root->child[1]->child[0]=Stence();//返回while循环的中间循环体
        break;
    case FOR:
        wordtype=gettoken();
        if(wordtype!=LSP)//如果没有读到小括号
        {
            errors+=1;
            printf("第 %d 行for语句条件出错！\n",line);
            return NULL;
        }
        root=makepoint(4,"for语句","for");
        wordtype=gettoken();
        root->child[0]=makepoint(1,"单次表达式","\0");
        root->child[0]->child[0]=Stence();//for语句的单次表达式
        wordtype=gettoken();
        root->child[1]=makepoint(1,"条件","\0");
        root->child[1]->child[0]=Expression(SEM);//for语句的条件表达式
        if(root->child[1]==NULL)
        {
            errors+=1;
            printf("第 %d 行for语句条件出错！\n",line);
            return NULL;
        }
        wordtype=gettoken();
        if(wordtype!=RSP)
        {
            root->child[3]=makepoint(1,"末尾循环体","\0");
            root->child[3]->child[0]=Expression(RSP);//用于存储for语句的末尾循环体
        }
        wordtype=gettoken();
        root->child[2]=makepoint(1,"中间循环体","\0");
        root->child[2]->child[0]=Stence();//返回for语句中间循环体
        break;
    case BREAK:
        wordtype=gettoken();
        if(wordtype!=SEM)//如果break后面直接跟着的不是分号，说明语言格式错误，报错，返回null
        {
            errors+=1;
            printf("第 %d 行break语句格式出错！\n",line);
            return NULL;
        }
        root=makepoint(0,"break语句","\0");
        break;
    case CONTINUE:
        wordtype=gettoken();
        if(wordtype!=SEM)//同上
        {
            errors+=1;
            printf("第 %d 行continue语句格式出错！\n",line);
            return NULL;
        }
        root=makepoint(0,"continue语句","\0");
        break;
    case RETURN:
        wordtype=gettoken();
        if(wordtype!=  IDENT&&wordtype!=INT_CONST&&wordtype!=CHAR_CONST&&wordtype!=LONG_CONST&&wordtype!=FLOAT_CONST)
        {
            errors+=1;
            printf("第 %d 行return语句格式出错！\n",line);
            return NULL;
        }
        root=makepoint(1,"return语句","\0");//used to be 1
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
        if(root==NULL)//如果表达式出现错误，报错，返回null
        {
            errors+=1;
            printf("第 %d 行表达式格式出错！\n",line);
            return NULL;
        }
        break;
    case RBP:
        return NULL;//复合语句结束标志
    case SEM:
        return NULL;//空循环
    }
    return root;
}

_step* Expression(int endtype)//用来判别结束标记，为0时分号结束，为1时右括号结束
{
    int topofop=0,topofopn=0;
    _keywords op[20];
    op[0].name[0]='#';
    op[0].name[1]='\0';
    op[0].placeinenum=DEF;//起止符入栈
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
            else if(wordtype==LSP)//如果是左括号的话，特殊考虑
            {
                point=makepoint(2,"运算符",token_text);
                wordtype=gettoken();
                point->child[0]=Expression(RSP);
                point->child[1]=makepoint(0,"运算符",")");
                opn[topofopn++]=point;
                wordtype=gettoken();
            }
            else switch(priority[op[topofop].placeinenum][wordtype])
            {
            case -1://如果栈顶运算符优先级小于新读入运算符
                op[++topofop].name[0]='\0';
                strcat(op[topofop].name,token_text);//保存符号内容
                op[topofop].placeinenum=wordtype;
                wordtype=gettoken();
                break;
            case 1://如果栈顶运算符优先级高于新读入的运算符
                if(topofop>=1) point=makepoint(2,"运算符",op[topofop--].name);//由于运算符栈顶元素出栈，需要将栈顶位置下移一个单元
                else errors+=1;
                if(topofopn>=2) point->child[1]=opn[--topofopn];
                else errors+=1;
                if(topofopn>=1) point->child[0]=opn[--topofopn];
                else errors+=1;
                opn[topofopn++]=point;
                break;
            default://如果没有大小关系，说明应该是到达了表达式的结束标志
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
    printf("第 %d 行表达式出错！\n",line);
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
            if(strcmp(root->name,"外部定义序列")!=0&&strcmp(root->name,"外部变量序列")!=0&&strcmp(root->name,"局部变量定义序列")!=0&&strcmp(root->name,"语句序列")!=0&&strcmp(root->name,"形参序列")!=0)
            {
                for(i=0;i<indentflag;i++) printf("   ");
                printf("%s  ",root->name);
                printf("%s\n",root->text);
            }
        }
        if(strcmp(root->name,"复合语句")==0||strcmp(root->name,"while语句")==0||strcmp(root->name,"if语句")==0||strcmp(root->name,"if-else语句")==0||strcmp(root->name,"for语句")==0)//如果遇到复合语句
        {
            indentflag+=1;
        }
    }
    else return;
    for(i=0;i<root->define;i++)
    {
        ASTTraverse(root->child[i]);
    }
    if(strcmp(root->name,"复合语句")==0||strcmp(root->name,"while语句")==0||strcmp(root->name,"if语句")==0||strcmp(root->name,"if-else语句")==0||strcmp(root->name,"for语句")==0)//如果遇到复合语句
    {
        indentflag-=1;
    }
    //else return;
}

void CodeTraverse(_step* root)
{
	//puts("1"); 
    int i;
    _step* r;//用于写出参数
    if(root!=NULL)
    {
        if(strcmp("外部定义序列",root->name)==0||strcmp("语句序列",root->name)==0)
        {
            fprintf(nfp,"\n");
            for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
        }
        if(strcmp("运算符",root->name)==0)
        {
            ExpTraverse(root);
            fprintf(nfp,";");
            return;
        }
        else if(strcmp("函数定义",root->name)==0)
        {
            fprintf(nfp,"%s ",root->child[0]->text);
            fprintf(nfp,"%s(",root->child[1]->text);
            CodeTraverse(root->child[1]->child[0]);
            fprintf(nfp,")\n{");
            CodeTraverse(root->child[2]);
            fprintf(nfp,"\n}");
            return;
        }
        else if(strcmp("if语句",root->name)==0||strcmp("if-else语句",root->name)==0||strcmp("while语句",root->name)==0)//if  或者 if-else
        {
            fprintf(nfp,"%s(",root->text);
            ExpTraverse(root->child[0]);
            fprintf(nfp,")\n");
            for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
            fprintf(nfp,"{");
            indentflag+=1;
            if(strcmp(root->child[1]->child[0]->name,"语句序列")!=0)
            {
                fprintf(nfp,"\n");
                for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
            }
            CodeTraverse(root->child[1]);
            indentflag-=1;
            fprintf(nfp,"\n");
            for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
            fprintf(nfp,"}");
            if(strcmp("if-else语句",root->name)==0)// 如果是if-else
            {
                fprintf(nfp,"\n");
                for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
                fprintf(nfp,"%s\n",root->child[2]->text);
                for(i=0;i<indentflag;i++) fprintf(nfp,"    ");
                fprintf(nfp,"{");
                indentflag+=1;
                if(strcmp(root->child[2]->child[0]->name,"语句序列")!=0)
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
        else if(strcmp("for语句",root->name)==0)
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
            if(strcmp(root->child[2]->child[0]->name,"语句序列")!=0)
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
        else if(strcmp("编译预处理",root->name)==0)
        {
            fprintf(nfp,"#%s ",root->child[0]->text);
            if(strcmp("文件包含",root->child[0]->name)==0)//如果是文件包含
            {
                fprintf(nfp,"<%s>",root->child[0]->child[0]->text);
            }
            else if(strcmp("不含参宏定义",root->child[0]->name)==0)//如果是不含参宏定义
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
        if(root->text[0]!='\0'&&strcmp("形参",root->name)!=0)
        {
                fprintf(nfp,"%s ",root->text);
        }
        else if(strcmp("形参序列",root->name)==0)
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
        if(strcmp(root->name,"复合语句")==0)//如果遇到复合语句
        {
            indentflag+=1;
        }
    }
    else return;
    for(i=0;i<root->define;i++)
    {
        CodeTraverse(root->child[i]);
    }
    if(strcmp("外部变量定义",root->name)==0)
    {
        fprintf(nfp,";");
    }
    if(strcmp(root->name,"复合语句")==0)//如果遇到复合语句
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
