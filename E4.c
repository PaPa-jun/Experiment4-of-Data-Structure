#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>
#define maxn 210
#define precision 200

typedef struct frac{
    int a; //分子
    int b; //分母，这两个都是正数
    int isNegative; //是否是负数,1是0否
}Frac;

typedef struct HighAccuracyNum{
    int Integer; //整数部分
    int Decimal[maxn]; //小数部分，Decimal[0]是十分位，Decimal[1]是百分位
    int base; //进制
    int isNegative;
}Number;

typedef struct item{    //项
    int exponent;
    Number coefficient;
}Item;

typedef struct polynomial{  //多项式结构
    Item items;
    struct polynomial *Next;
}Polynomial;

//函数声明
void init(Number *InitNum);
Number FractoNumber(Frac origin);
int Judge(Number A , Number B);
Number NumtoNbase(Number Origin , int N);
void NumberPrint(Number A , int N , int flag);
Number Plus(Number A ,Number B);
Number Subtraction(Number A , Number B);
Number Multiply(Number A , Number B);
Number DealPolynomial();
Polynomial *InitPoly();
Polynomial *ItemInsert(Polynomial *p , int i , Item Element);
Number Read();
void PolyPrint(Polynomial *p , Number X , int N);

//初始化清空
void init(Number *InitNum){
    InitNum->Integer = 0;
    memset(InitNum->Decimal,0,sizeof(InitNum->Decimal));
    InitNum->base = 0;
    InitNum->isNegative = 0;
}

//分数转换成高精度数
Number FractoNumber(Frac origin){
    Number result;
    init(&result);
    result.base = 10;
    result.isNegative = origin.isNegative;
    result.Integer = origin.a/origin.b; //求出整数部分
    origin.a %= origin.b;
    for(int i=0;i<precision;i++){
        origin.a *= 10; //模拟竖式除法
        result.Decimal[i] = origin.a/origin.b;
        origin.a%=origin.b;
    }
    origin.a *= 10;
    if(origin.a/origin.b >= 5){result.Decimal[precision-1]++;}//最后一位四舍五入
    return result;
}

//判断数的大小
int Judge(Number A , Number B){
    if(A.Integer > B.Integer){
        return 1;
    }
    if(A.Integer == B.Integer){
        int i;
        for(i = 0 ; i < precision ; i++){
            if(A.Decimal[i] > B.Decimal[i]){return 1;}
            if(A.Decimal[i] < B.Decimal[i]){return -1;}
        }
        if(i == precision){return 0;}
    }else{
        return -1;
    }
}

//10进制转N进制
Number NumtoNbase(Number Origin , int N){
    if(N > 20 || N < 2){
        printf("输入错误！");
        exit(0);
    }
    Number Nbasenumber;
    init(&Nbasenumber);
    Nbasenumber.isNegative = Origin.isNegative;

    //整数部分
    Nbasenumber.base = N;
    Nbasenumber.Integer = 0;
    int integer = Origin.Integer;
    int temp[maxn];
    int remainder = 0;
    int flag = 0;
    for(int i = 0 ; integer>0 ; i++ , flag++){
        remainder = integer%N;
        temp[i] = remainder;
        integer = integer/N;
    }
    for(int j = 0 ; j < flag ;j++){
        Nbasenumber.Integer += temp[j]*((int)pow(10,j));
    }
    
    //小数部分
    for(int i = 0 ; i < maxn ; i++){
        temp[i] = 0;
    }//将temp初始化用来存进位数
    for(int j = 0 ; j < precision ; j++){
        for(int i = precision-1 ; i >= 0 ; i--){
            temp[i] = ((Origin.Decimal[i]*N)+temp[i+1])/10;
            Origin.Decimal[i] = ((Origin.Decimal[i]*N)+temp[i+1])%10;
        }
        Nbasenumber.Decimal[j] = temp[0];
    }

    return Nbasenumber;
}

//打印数字
void NumberPrint(Number A , int N , int flag){
    if(N == 10){
        if(flag == 1){printf("10 进制：");}
        if(A.isNegative == 1){
            printf("-");
        }else{
            printf("+");
        }
        printf("%d." , A.Integer);
        for(int i = 0 ; i < 3 ; i++){
            printf("%d" , A.Decimal[i]);
        }
    }else{
        NumberPrint(A , 10 , 1);
        printf("\n%d 进制：" , N);
        Number NbaseNumber;
        init(&NbaseNumber);

        NbaseNumber = NumtoNbase(A , N);
        if(NbaseNumber.isNegative == 1){
            printf("-");
        }else{
            printf("+");
        }
        printf("%d." , NbaseNumber.Integer);
        for(int i = 0 ; i < 3 ; i++){
            printf("%d" , NbaseNumber.Decimal[i]);
        }
    }
}

//加法
Number Plus(Number A ,Number B){
    Number Result;
    init(&Result);

    Result.base = 10;

    if(A.isNegative == B.isNegative){
        Result.isNegative = A.isNegative;    
        Result.Integer = A.Integer + B.Integer;
        int temp[maxn];
        //初始化
        for(int i = 0 ; i < maxn ; i++){
            temp[i] = 0;
        }
        for(int i = precision-1 ; i >= 0 ; i--){
            temp[i] = (A.Decimal[i] + B.Decimal[i] + temp[i+1])/10;
            Result.Decimal[i] = (A.Decimal[i] + B.Decimal[i] + temp[i+1])%10;
        }
        Result.Integer += temp[0];
    }else{
        if(A.isNegative == 0){
            B.isNegative = 0;
            Result = Subtraction(A , B);
        }else{
            A.isNegative = 0;
            Result = Subtraction(B , A);
        }
    }
    return Result;
}

//减法
Number Subtraction(Number A , Number B){
    Number Result;
    init(&Result);
    int flag = 0;

    Result.base = 10;

    if(A.isNegative == 1 && B.isNegative == 0){
        A.isNegative = 0;
        Result = Plus(A , B);
        Result.isNegative = 1;
        flag = 1;
    }
    if(A.isNegative == 1 && B.isNegative == 1){
        B.isNegative = 0;
        A.isNegative = 0;
        Result = Subtraction(B , A);
        flag = 1;
    }
    if(A.isNegative == 0 && B.isNegative == 1){
        B.isNegative = 0;
        Result = Plus(A , B);
        flag = 1;
    }
    if(flag != 1){
        if(A.isNegative == 0 && B.isNegative == 0){
            if(Judge(A , B) == 1 || Judge(A , B) == 0){
                Result.isNegative = 0;
                //先处理小数部分
                for(int i = precision-1 ; i >= 0 ; i--){
                    if(A.Decimal[i] >= B.Decimal[i]){
                        Result.Decimal[i] = A.Decimal[i] - B.Decimal[i];
                    }else{
                        if(i > 0){
                            A.Decimal[i-1] -= 1;
                            A.Decimal[i] += 10;
                            Result.Decimal[i] = A.Decimal[i] - B.Decimal[i];
                        }else{
                            A.Integer -= 1;
                            A.Decimal[i] += 10;
                            Result.Decimal[i] = A.Decimal[i] - B.Decimal[i];
                        }
                    }
                }

                //再处理整数部分
                Result.Integer = A.Integer - B.Integer;
            }
            else{
                Result = Subtraction(B , A);
                Result.isNegative = 1;
            }
        }
    }
    return Result;
}

//乘法
Number Multiply(Number A , Number B){
    Number Result;
    init(&Result);

    Result.base = 10;

    //符号处理
    if(A.isNegative ==  B.isNegative){
        Result.isNegative = 0;
    }else{
        Result.isNegative = 1;
    }

    //获得整数数位
    int digitA = 0 , digitB = 0;
    int TempA  = A.Integer , TempB = B.Integer;
    while (TempA){
        digitA++;
        TempA = TempA/10;
    }
    while (TempB){
        digitB++;
        TempB = TempB/10;
    }

    TempA  = A.Integer ;
    TempB = B.Integer;

    int DigitA = precision + digitA, DigitB = precision + digitB;
    int *NumA = (int *)malloc((DigitA)*sizeof(int));
    int *NumB = (int *)malloc((DigitB)*sizeof(int));
    for(int i = digitA-1 ; i >=0 ; i--){
        NumA[i] = TempA % 10;
        TempA = TempA / 10;
    }
    for(int i = digitB-1 ; i >=0 ; i--){
        NumB[i] = TempB % 10;
        TempB = TempB / 10;
    }
    for(int i = digitA ; i < digitA+precision ; i++){
        NumA[i] = A.Decimal[i-digitA];
    }
    for(int i = digitB ; i < digitA+precision ; i++){
        NumB[i] = B.Decimal[i-digitB];
    }

    //乘法实现
    int cary[DigitA+1];
    for(int i = 0 ; i < DigitA ; i++){
        cary[i] = 0;
    }
    int DIGIT = DigitA + DigitB;
    int TempRes[DIGIT];
    for(int i = 0 ; i < DIGIT ; i++){
        TempRes[i] = 0;
    }

    for(int i = 0 ; i < DigitB ; i++){
        int j;
        for(j = 0; j < DigitA ; j++){
            TempRes[DIGIT - 1 - i - j] += NumA[DigitA - 1 -j]*NumB[DigitB - 1 - i];
        }
        for(int k = 0 ; k < DigitA ; k++){
            cary[DigitA-k] = (TempRes[DIGIT-i-k-1])/10;
            TempRes[DIGIT-i-k-2] += cary[DigitA-k];
            TempRes[DIGIT-i-k-1] = (TempRes[DIGIT-i-k-1])%10;
        }
    }

    for(int i = digitA+digitB ; i < digitA+digitB+precision ; i++){
        Result.Decimal[i-digitA-digitB] = TempRes[i];
    }
    if(TempRes[digitA+digitB+precision]>=5)Result.Decimal[precision-1]++;

    for(int i = digitA+digitB-1 ; i >= 0 ; i--){
        Result.Integer += TempRes[i]*pow(10,digitA+digitB-1-i);
    }
    
    return Result;
}

//幂运算
Number Mi(Number X , int N){
    Number Res;
    init(&Res);

    if(N == 0){
        Res.base = 10;
        Res.Integer = 1;
        Res.isNegative = 0;
        return Res;
    }
    Res = X;
    for(int i = 0 ; i < N-1 ; i++){
        Res = Multiply(Res , X);
    }
    return Res;
}

//处理多项式
Number DealPolynomial(){
    Number X;   //未知数
    int itemnumber;
    int base;   //除了十进制之外，还需要打印的进制
    printf("请输入未知数：");
    X = Read();
    printf("请输入项数：");
    scanf("%d" , &itemnumber);
    printf("结果进制（10进制除外，2~20之间）：");
    scanf("%d" , &base);

    Polynomial *Poly;
    Poly = InitPoly();
    Item items[itemnumber];

    //读取多项式
    for(int i = 0 ; i < itemnumber ;i++){
        Number conficient;
        int ex;
        printf("请输入第 %d 项的系数：" , i+1);
        conficient = Read();
        items[i].coefficient = conficient;
        printf("请输入第 %d 项的指数：" , i+1);
        scanf("%d" , &ex);
        items[i].exponent = ex;
        ItemInsert(Poly , i+1 , items[i]);
    }
    PolyPrint(Poly , X , base);


    //运算
    Number Result;
    init(&Result);
    Polynomial *p = Poly;
    while (p = p->Next){
        Result = Plus(Result , Multiply(p->items.coefficient , Mi(X , p->items.exponent)));
    }
    printf("\n 运算结果：f(x) = \n");
    NumberPrint(Result , base , 1);
}

//打印多项式
void PolyPrint(Polynomial *p , Number X , int N){
    printf("f(x) = ");
    while (p->Next){
        NumberPrint(p->Next->items.coefficient , 10 , 0);
        printf("*x^(%d)" , p->Next->items.exponent);
        p = p->Next;
    }
    printf("\t x = ");
    NumberPrint(X , 10 , 0);
}

//读取高精度数
Number Read(){
    Number A;
    init(&A);
    A.base = 10;

    char *num = (char*)malloc(precision*sizeof(char));
    if(!num)printf("分配失败！");
    printf("请输入：");
    scanf("%s" , num);
    int i = 0;
    int flag = 0;
    int j = 0;
    //printf("%d" , num[4]);
    while (num[i]){
        if(num[i] == '-'){
            A.isNegative = 1;
            i++;
            continue;
        }
        if(num[i] == '.'){
            flag = 1;
            i++;
            continue;
        }
        if(flag == 0){
            A.Integer = A.Integer*10+num[i]-'0';
        }else{
            A.Decimal[j] = num[i]-'0';
            j++;
        }
        i++;
    }

    free(num);
    return A;
}

//初始化多项式
Polynomial *InitPoly(){
    Polynomial *p;
    p = (Polynomial*)malloc(sizeof(Polynomial));
    //判断分配是否成功
    if(!p){
        printf("Failure!");
        exit(0);
    }
    p->Next = NULL;
    return p;
}

//插入项
Polynomial *ItemInsert(Polynomial *p , int i , Item Element){
    if(i < 1){
        return NULL;
    }

    while(p){
        i--;
        if(i < 1){
            Polynomial *q = (Polynomial*)malloc(sizeof(Polynomial));
            if(!q){
                printf("Failure!");
                exit(0);
            }
            q->Next = p->Next;
            p->Next = q;
            q->items = Element;
            return q;
        }
        p = p->Next;
    }
}

int main(){
    DealPolynomial();
    return 0;
}