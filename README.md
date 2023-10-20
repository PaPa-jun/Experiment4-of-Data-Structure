# Experiment4-of-Data-Structure
The 4th experiment of Data Structure lesson

## 这个程序是数据结构的第二个实验，之所以叫实验4只是因为老师的实验指导书上面给的是实验4
### 实验要求：
- 创建一种数据结构能够储存高精度数，要求是要能够精确到 2^{-n} 位
- 实现加、减、乘三种运算
- 利用这三种运算实现多项式的计算

### 实现：
- 在我的程序中，使用了如下方式储存高精度数：
```
typedef struct HighAccuracyNum{
  int Integer; //整数部分
  int Decimal[maxn]; //小数部分，Decimal[0]是十分位，Decimal[1]是百分位
  int base; //进制
  int isNegative;
}Number; 
```
- 加法运算：
```
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
```
- 减法运算：
```
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
```
*注意到，加法运算和减法运算之间存在互相调用，目的是为了处理正负号，减法运算中还含有一点递归的逻辑，目的是将减数与被减数的符号差异通过递归调用的方式转换成两个都为正数的最简单的情况进行运算。*
- 乘法运算：
```
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
```
*乘法运算最为复杂，但最基础的算法原理还是模拟人手算列竖式，储存进位，计算当前位结果和进位结果，利用遍历指标实现错位加法。*

#### 剩下的函数要么是服务于这三个主要功能，要么是以这三个函数为主要功能实现其他功能，这里不再赘述。
