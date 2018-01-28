from lark import Lark
l = Lark('''
            exp : [functionexp | assignexp | operateexp | loopexp | ifexp | returnexp]
            functionexp : TYPE KEYWORD WS (VARIABLE | MAINFLAG) WS SMALLBRACKET [WS parameterexp WS] SMALLBRACKET WS LARGEBRACKET WS exp WS LARGEBRACKET
            parameterexp : TYPE WS VARIABLE ["," WS] [parameterexp]
            statementexp : (factor | exp) WORDING WS (factor | exp) RELATIVEOP
            loopexp : SMALLBRACKET WS statementexp WS SMALLBRACKET WS KEYWORD WS LARGEBRACKET WS exp WS LARGEBRACKET
            ifexp : IFKEYWORD WS SMALLBRACKET WS statementexp WS SMALLBRACKET WS IFKEYWORD WS LARGEBRACKET WS exp WS LARGEBRACKET [WS (ifexp | elseexp)]
            elseexp : IFKEYWORD WS LARGEBRACKET WS exp WS LARGEBRACKET
            returnexp : (factor | exp) WORDING WS KEYWORD
            assignexp : [TYPE] WS VARIABLE WORDING WS (factor | exp) KEYWORD
            operateexp : (factor | exp) WORDING WS (factor | exp) WORDING WS OP WS KEYWORD
            factor : VARIABLE | NUMBER
            VARIABLE : "a" | "b" | "c" | "d" | "y" | "z" | "유한길"
            WORDING : "은" | "는" | "이" | "가" | "을" | "를" | "와" | "과" | "에" | "의" | "에서"
            KEYWORD : "반환한다" | "동안" | "이다" | "형" | "값"
            IFKEYWORD :  "아니고" | "면" | "이면" | "라면" | "만약" | "아니면"
            OP : "더한" | "뺀" | "곱한" | "나눈"
            RELATIVEOP : "보다 크다" | "보다 작다" | "와 같다"
            TYPE : "정수" | "실수"
            MAINFLAG : "중점함수"
            SMALLBRACKET : "(" | ")"
            LARGEBRACKET : "{" | "}"

            %import common.WS
            %import common.NUMBER
         ''', start="exp")
tree = l.parse("정수형 중점함수 ( 정수 y, 정수 z ) { ( b가 d보다 크다 ) 동안 { 만약 ( c가 b보다 크다 ) 면 { 정수 c는 a와 b를 더한 값이다 } 아니고 ( c가 b보다 작다 ) 면 { 정수 c는 a와 b를 뺀 값이다 } 아니면 { 정수 c는 a와 b를 나눈 값이다 } } }")

from lark.tree import pydot__tree_to_png
pydot__tree_to_png(tree, "examples/grammar.png")