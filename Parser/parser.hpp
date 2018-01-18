#pragma once
#ifndef __PARSER_H__
#define __PARSER_H__
#include <iostream>
#include <fstream>
#include <regex>
#include <string>


enum Token
{
	Keyword_RETURN,			// ��ȯ�Ѵ�.
	Keyword_IF,				// ����
	Keyword_ELSE,			// �ƴϸ�
	Keyword_WHILE,			// ~ �Ѵ� ex) (n�� 3���� ũ��) { ������ �� } �Ѵ�.
	Keyword_BREAK,			// �׸��Ѵ�.
	Keyword_INT,			// ����
	Keyword_DOUBLE,			// �Ǽ�
	Keyword_DECLARATION,	// - ��

	Op_ASSIGN,				// �̴�
	Op_ADD,					// ����
	Op_SUB,					// ��
	Op_MUL,					// ����
	Op_DIV,					// ����
	Op_EQUAL,				// ������
	Op_NOTEQUAL,			// �ٸ���
	Op_LESSTHAN,			// ~���� ������
	Op_GREATERTHAN,			// ~���� ũ��
	Op_LESSTHANEQUAL,		// ~���� �۰ų� ������
	Op_GREATERTHANEQUAL,	// ~���� ũ�ų� ������
	
	Punct_LPAREN,			// (
	Punct_RPAREN,			// )
	Punct_LBRACE,			// {
	Punct_RBRACE,			// }
	Punct_LBRACK,			// [
	Punct_RBRACK,			// ]

	Id,						// �ĺ���

	Num_INT,				// ���� ex) 12348579
	Num_DOUBLE,				// �Ǽ� ex) 1358.74897

	END_OF_FILE,			// -1

	ERROR,					// ex) �� �� ��

	UNKNOWN,				

};

int GetToken();

#endif // ! Parser_H