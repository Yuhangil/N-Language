# N-Language

의미 : N(Natural, 자연) 언어

목적 : 자연어를 이용하여 직관적인 명령문으로 코딩

기능 : 한글을 이용한 명령문으로 직관적으로 코딩 가능

목표 : 1차. 변수 선언 및, 사칙연산등 기초적 기능 구현 완료
      2차. 기능의 심도화 및 라이브러리 지원
      3차. 실사용 가능 ~~대학 소프트웨어 특기자~~

문법 : 인자가 여러개일 경우 ',' 로 구분하고 명령문의 끝은 '.' 으로 구분한다.

## 치환목록

에서 == 에  
을 == 를  
은 == 는   
각종 조사 등...

## 자료형

기본 자료형 : int(4 byte), double(8 byte)

## 기본 문법

'와' 의 앞이 source 'b' 가 destination

1. 대입 연산 '='

ex) a = b  
a는 b이다.

2. 덧셈 연산 '+'

ex) a + b  
a에 b를 더한 값

3. 뺄셈 연산 '-'

ex) a - b  
a에 b를 뺀 값

4. 곱셈 연산 '*'

ex) a * b  
a에 b를 곱한 값

5. 나눗셈 연산 '/'

ex) a / b  
a에서 b를 나눈 값

6. 변수 선언 :

-primitive :

ex) int a; int a = 3
정수 a를 만든다.
정수 a는 3이다.

ex) double a; double a = 3.14
실수 a를 만든다.
실수 a는 3.14이다.

-reference :

정수 a[9]를 만든다.
정수 a[9]는 1, 2, 3, 4, 5이다.

이렇게 넣을경우 1, 2, 3, 4, 5, 0, 0, 0, 0

7. 조건문 :

(명제 P) (이)면 명령문  
아니고 (명제Q)라면 명령문  
아니면 명령문

[개편 필요]

8. 반복문 :

(while) & (for) 통합

(명제 P)동안 명령문

(do while)

{

명령문

}

명제Q 동안

## 함수

```
핵심 함수
{
      ...
      함수 이름(인자, 인자).
      ...
}
```

```
정수형 함수 함수 이름(정수형 인자1, 실수형 인자2)
{
      ...
}
```

## Context-Free-Grammer

```
19
F->fPS
P->(LM)
S->{EF}
L->TV
M->$
M->,L
E->TVWVWO.
F->E
F->$
O->add
O->sub
O->mul
O->div
W->to
W->from
T->int
T->double
V->i
V->j

f(inti,intj){intitojfromadd.}#

# F = Function
# P = Parameter_list
# S = Statement_list
# L = Parameter_First
# M = Parameter_Multi
# E = Expression_First
# F = Expression_Multi
# O = Operator
# V = Variable
# W = Wording
# T = Type
```