#ifndef _midbc_milex_header_h_
#define _midbc_milex_header_h_

typedef enum{
	T_EOF,
	T_EOL,
	T_UNKNOWN,
	T_KEYWORD,
#	define MLXGROUP(name) T_##name,
#	define END_MLXGROUP
#	define MLXSEQUENCE(name) T_##name,
#	define END_MLXSEQUENCE
#	define IS(name, hit)
#	define MLXCHAR(ch, name) T_##name,
#	define MLXRANGE(from, to, name) T_##name,
#	define KEYWORD(str)
#	define NOT(name)
#	include LEXFILE
#	undef MLXGROUP
#	undef END_MLXGROUP
#	undef MLXSEQUENCE
#	undef END_MLXSEQUENCE
#	undef IS
#	undef MLXCHAR
#	undef MLXRANGE
#	undef KEYWORD
	T_MAXTOKEN
}Token_en;

//const char* g_Token_Names[T_MAXTOKEN];

typedef enum{
	H_Optional, //hit* 0..1
	H_Once,		//hit* 1
	H_Multiple,	//hit* 1..N
	H_MayMore,	//hit* 0..N
	H_Not,		//hit*0, miss*1
	H_WhileNot,	//hit*0, miss*N
	H_MAXHIT
}Hit_en;

class MiTokens{
	public:
	MiTokens(Token_en t);
	MiTokens& operator()(Token_en t);
	bool In(Token_en t);
	bool NotIn(Token_en t);
	private:
	uint32 set;
};

#define MAX_STACK 32

class MiContext {
	public:
	MiContext(bool x);
	Token_en token;
	bool ex;
};

class MiLex {
public:
 MiLex();
 void Init();
 void SetPtr(const char* pt, int x, int y);
 Token_en Parse();
 typedef Token_en (MiLex::*fn_rule)();
 virtual void GetLine()=0;
protected:
 char Next();
 
 char Char();
 void Accept(int plus);
 void Undo();
 void Begin(Token_en state);
 void End();
 Token_en stack[MAX_STACK];
 int line;
 int column;
 int offset;
 int sp;
 int stop;
 const char* p;
 const char* begin;
 const char* filename;
 int len;
 void infoKeyword(const char* estr, Token_en rule, const char* k);
 void info(const char* estr, Token_en rule, Token_en ret, const Hit_en hit);
 void err(const char* estr, Token_en need, Token_en have, const Hit_en hit);
 Token_en Token();
 Token_en IsChar(Token_en rule);
 bool IsString(const char* str);
 bool Is(const Token_en rule, fn_rule fn, MiContext* context, const Hit_en hit);
#	define MLXGROUP(name) Token_en rule_ ## name();
#	define END_MLXGROUP
#	define MLXSEQUENCE(name)  Token_en rule_ ## name();
#	define END_MLXSEQUENCE
#	define IS(name, hit)
#	define MLXCHAR(ch, name) Token_en rule_ ## name();
#	define MLXRANGE(from, to, name) Token_en rule_ ## name();
#	define KEYWORD(str)
#	include LEXFILE
#	undef MLXGROUP
#	undef END_MLXGROUP
#	undef MLXSEQUENCE
#	undef END_MLXSEQUENCE
#	undef IS
#	undef MLXCHAR
#	undef MLXRANGE
#	undef KEYWORD
};



#endif
