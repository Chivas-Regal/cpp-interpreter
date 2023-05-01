<h1 align="center">脚本语言解释器</h1>

（暂且不支持语法检查）

## 使用说明

例如以下程序

```
FUNCTION hw (ope) {
    s1 = "hello";
    s2 = "world";
    IF (ope == 1) {
        i = 1;
        WHILE (i <= 3) {
            i = i + 1;
            OUTPUT("HELLO WORLD");
        }
    } ELIF (ope == 2) {
        OUTPUT(s1 + " " + s2);
    } ELSE {
        OUTPUT("HELLO WORLD", s1 + " " + s2);
    }
    OUTPUT("");
    RETURN 0;
}

FUNCTION fibonacci (n, x, y) {
    i = 1;
    WHILE (i <= n) {
        OUTPUT(x);
        i = i + 1;
        t = y;
        y = x + y;
        x = t;
    }
    OUTPUT("");
    RETURN 0;
}

FUNCTION calc (a, b) {
    RETURN a + 2 * (1 + 3) * b + a * b;
}

OUTPUT("请输入INPUT函数的参数：");
INPUT(ope);
hw(ope);

OUTPUT("请输入fibonacci的项数：");
INPUT(n);
OUTPUT("请输入fibonacci的前两项：");
INPUT(x, y);
OUTPUT("fibonacci序列：");
fibonacci(n, x, y);

OUTPUT("测试多级calc函数：");
OUTPUT(calc(1, 1), calc(2, 2));
OUTPUT(calc(calc(1, 1), calc(1, 1)));
OUTPUT(calc(10, 10));
```

执行结果：

```
$ 请输入INPUT函数的参数：
3
$ HELLO WORLD hello world
$ 
$ 请输入fibonacci的项数：
5
$ 请输入fibonacci的前两项：
1 2
$ fibonacci序列：
$ 1
$ 2
$ 3
$ 5
$ 8
$ 
$ 测试多级calc函数：
$ 10 22
$ 190
$ 190
```

执行方法：将编写的脚本程序形成字符串 `program` ，然后调用函数 `Program(la->lexer(program))` 即可

## 技术支持

技术核心为编译原理相关内容  

- 设计词法分析器将程序拆解为终结符集
- 对一句话利用单调符号栈建立语法树获得结果
- 对语句集建立文法，形成以 “语句集”、“语句”、“循环语句”、“分支语句”、“算术表达式”、“关系表达式” 为主的有穷自动机，并在此基础上分析语义
- 对产生式建立 select 表，利用预测分析法进行语法检查 //TODO

文法：
[程序] -> [语句] | [语句] [程序]  
[语句] -> [WHILE语句] | [IF语句] | [常规语句];  
[常规语句] -> [算术表达式] | [INPUT语句] | [OUTPUT语句]  
[WHILE语句] -> WHILE ( [关系表达式] ) { [语句组] }  
[IF语句] -> IF ( [关系表达式] ) { [语句组] } [IF额外分支]  
[IF额外分支] -> [空] | ELSE { [语句组] } | ELIF ( [关系表达式] ) { [语句组] }  
[INPUT语句] -> INPUT( [变量] )  
[OUTPUT语句] -> OUTPUT( [变量组] )  
[FUNCTION语句] -> FUNCTION [标识符] ([变量组]) {[程序] RETURN [算术表达式]}  
[函数调用] -> 标识符( [变量组] )  
[变量组] -> [变量] | [变量],[变量组]  

## 语法规则

变量无需声明，形式类似于 `python` 可以直接定义，不定义直接使用的话类型为 `UNDEF`   
所有独立语句必须以分号结尾，`WHILE/FOR` 以 `}` 结尾  

### 输入

`INPUT(x);` 表示输入 `x` 的值，暂且只能为 `int/double` 类型的输入

### 输出

`OUTPUT(x1, x2, ..., xn);` 在一行内输出 `x1 x2 x3 ... xn` 中间以空格分隔开，`xi` 可以为表达式

### 赋值语句

`x = 1.0` 将浮点数 `1.0` 赋给 `x`  
`y = x` 将浮点数 `x` 赋给 `y`

### 循环语句

```
WHILE (关系表达式) {
    待执行程序集
}
```

伪产生式为：  
[关系表达式] -> ![算术表达式] | [算术表达式][关系运算符][算术表达式]  
[程序集] -> [程序]; | [程序];[程序集]

### 分支语句

```
IF (关系表达式1) {
    待执行程序集1
} ELIF (关系表达式2) {
    待执行程序集2
} ELIF (关系表达式3) {
    待执行程序集3
} ELSE {
    待执行程序集4
}
```

也可以支持这种写法

```
IF (关系表达式1) {
    待执行程序集1
} ELIF (关系表达式2) {
    待执行程序集2
}
```

或者 

```
IF (关系表达式1) {
    待执行程序集1
}
```

产生式和循环语句部分一样

### 函数

#### 函数定义

在最开头

```
FUNCTION 函数名 (参数1, 参数2, 参数3, ...) {
    待执行程序集
    RETURN 算术表达式;
}
```

调用便可直接  

```
函数名 ( 参数1, 参数2, 参数3, ...);
```

当然也可以进行嵌套，例如 

```
函数1(函数2(参数), 函数2(参数));
```

