#include "milex.h"

static Token_en g_TokenChars[256];
const char* g_Token_Names[T_MAXTOKEN] =
{
	"EOF",
	"EOL",
	"UNKNOWN",
	"KEYWORD",
#	define MLXGROUP(name) #name,
#	define END_MLXGROUP
#	define MLXSEQUENCE(name) #name,
#	define END_MLXSEQUENCE
#	define IS(name, hit)
#	define MLXCHAR(ch, name) #name,
#	define MLXRANGE(from, to, name) #name,
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
};
const char* g_Hit_Names[H_MAXHIT] =
{
	"Optional", //hit* 0..1
	"Once",		//hit* 1
	"Multiple",	//hit* 1..N
	"MayMore",	//hit* 0..N
	"Not",		//hit*0, miss*1
	"WhileNot"	//hit*0, miss*N
};

MiTokens::MiTokens(Token_en t){
	set= 1<< t;
}
MiTokens& MiTokens::operator()(Token_en t){
	set|= 1 << t;
	return *this;
}
bool MiTokens::In(Token_en t){
	return (1<<t) & set;
}
bool MiTokens::NotIn(Token_en t){
	return 0==((1<<t) & set);
}
//------------------------------------------------------------
MiLex::MiLex(){
	Init();
	stop=0;
}
char MiLex::Char(){
	return p[offset];
}
char MiLex::Next(){
	char c=0;
	do{
		c=p[offset++];
		if (!c)
			GetLine();
	}while(!c);
	return c;
}
bool MiLex::IsString(const char* str){
	return 0==strnicmp(str, &p[offset], strlen(str));
}
void MiLex::SetPtr(const char* pt, int x, int y){
	p=pt;
	line=y;
	column=x;
	offset=0;
}
void MiLex::Accept(int plus){
	begin=p;
	offset+=plus;
	len=offset;
	column+=offset;
	p+=offset;
	offset=0;
}
void MiLex::Undo(){
	offset--;
	if (offset<0){
		offset=0;
	}
}
void MiLex::Begin(Token_en state){
	stack[++sp]=state;
}
void MiLex::End(){
	sp--;
	if (sp<0){
		sp=0;
	}
}
bool MiLex::Is(const Token_en rule, fn_rule fn, Token_en& token, bool ex, const Hit_en hit){
	int accept=0;
	bool repeat=false;
	bool negate=false;
	bool ok=false;
	bool wasok=false;
	bool mandatory=false;
	//init phase
	if (!fn) return false;
	if (stop>2) return false;
	Begin(rule);
	switch(hit){
	case H_Optional:    //hit* 0..1
		break;
	case H_Once:		//hit* 1
		mandatory=true;
		break;
	case H_Multiple:	//hit* 1..N
		mandatory=true;
		repeat=true;
		break;
	case H_MayMore:	//hit* 0..N
		repeat=true;
		break;
	case H_WhileNot:	//hit*0, miss*N
		repeat=true;
		negate=true;
		break;
	case H_Not:		//hit*0, miss*1
		negate=true;
		mandatory=true; //one negate hit
		break;
	}
	
	//process phase
	again:
	ok=false;
	Token_en ret=(this->*fn)();
	
	switch(hit){
	case H_Optional: //hit* 0..1
	case H_Once:		//hit* 1
	case H_Multiple:	//hit* 1..N
	case H_MayMore:	//hit* 0..N
	case H_Not:		//hit*0, miss*1
	case H_WhileNot:	//hit*0, miss*N
	break;
	}
	
	if (negate){
		ok= ret != rule;
	}else{
		ok= ret == rule;
	}
	if(ok) {
		Accept(accept);
		if (T_Nl==ret){
			line++; column=1;
		}
		token= rule;
		wasok=true;
		mandatory=false;
	}else{
		Undo();
		if (mandatory){
			err("Mandatory token missing", rule, ret, hit);
		}
	}
	if (ok && repeat) goto again;
	if (wasok |!mandatory) {
		token=rule;
		info("a", rule, ret, hit);
		stop=0;
	}else{
		info("u", rule, ret, hit);
	}
	End();
	return wasok;
}
void MiLex::infoKeyword(const char* estr, Token_en rule, const char* k){
	FILE* err=fopen("milex.txt", "a");
	fprintf(err, "%s:%i:%i: info: %s ", filename, line, column, estr);
	for (int i=1; i<sp; i++){
		fprintf(err, "%s.", g_Token_Names[ stack[i] ] );
	}
	fprintf(err, "%s \"%s\" +%i \n", g_Token_Names[rule], k, offset);
	fclose(err);
}
void MiLex::info(const char* estr, Token_en rule, Token_en ret, const Hit_en hit){
	FILE* err=fopen("milex.txt", "a");
	fprintf(err, "%s:%i:%i: info: %s ", filename, line, column, estr);
	for (int i=1; i<sp; i++){
		fprintf(err, "%s.", g_Token_Names[ stack[i] ] );
	}
	char c=Char();
	if (c==10 || c==13) c='.';
	fprintf(err, "%s(%s)%s +%i %c \n", g_Token_Names[rule], g_Hit_Names[hit], g_Token_Names[ret], offset, c);
	fclose(err);
}
void MiLex::err(const char* estr, Token_en need, Token_en have, const Hit_en hit){
	FILE* err=fopen("milex.log", "a");
	if (!stop) fprintf(err, "\n");
	fprintf(err, "%s:%i:%i: error: %s: ",filename, line, column, estr);
	for (int i=1; i<sp; i++){
		fprintf(err, "%s.", g_Token_Names[ stack[i] ] );
	}
	fprintf(err, "%s found:%s %s+%i \n", g_Token_Names[need], g_Token_Names[have], g_Hit_Names[hit], offset);
	fclose(err);
	stop++;
}
void MiLex::Init(){
	g_TokenChars[0]=T_EOL;
	//init code from mlx description file
#	define MLXGROUP(name)
#	define END_MLXGROUP
#	define MLXSEQUENCE(name)
#	define END_MLXSEQUENCE
#	define IS(name, hit)
#	define MLXCHAR(ch, name) g_TokenChars[ ch ]= T_ ## name;
#	define MLXRANGE(from, to, name) for(int i=from; i<=to; i++){ g_TokenChars[i]= T_ ## name; };
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
}

Token_en MiLex::Token(){
	Token_en token=g_TokenChars[Next()];
	return token;
}
Token_en MiLex::IsChar(Token_en rule){
	Token_en token=Token(); //get next
	return token;
}

// rule function body for CHAR and RANGE
#	define MLXGROUP(name)
#	define END_MLXGROUP
#	define MLXSEQUENCE(name)
#	define END_MLXSEQUENCE
#	define IS(name, hit)
#	define MLXCHAR(ch, name)		Token_en MiLex::rule_ ## name() { return IsChar( T_ ## name); };
#	define MLXRANGE(from, to, name) Token_en MiLex::rule_ ## name() { return IsChar( T_ ## name); };
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

// rule function body for other rules GRUP and SEQUENCE
#	define MLXGROUP(name) Token_en MiLex::rule_ ## name() { Token_en token=T_UNKNOWN; bool ex=true; Token_en ret=T_ ## name;
#	define END_MLXGROUP return token; }
#	define MLXSEQUENCE(name)  Token_en MiLex::rule_ ## name() { Token_en token=T_UNKNOWN; bool ex=false; Token_en ret=T_ ## name;
#	define END_MLXSEQUENCE return ret; }
#	define IS(name, hit) if (Is(T_ ## name, &MiLex::rule_ ## name, token, ex, H_ ## hit)) { if (ex) return ret; }else if (!ex) return T_UNKNOWN;
#	define MLXCHAR(ch, name)
#	define MLXRANGE(from, to, name)
#	define KEYWORD(str) if(IsString(str)){ infoKeyword("a",T_KEYWORD,str); Accept(strlen(str)); token=T_KEYWORD; if (ex) return ret; } else if (!ex) return T_UNKNOWN;
#	include LEXFILE
#	undef MLXGROUP
#	undef END_MLXGROUP
#	undef MLXSEQUENCE
#	undef END_MLXSEQUENCE
#	undef IS
#	undef MLXCHAR
#	undef MLXRANGE
#	undef KEYWORD

Token_en MiLex::Parse(){
	bool ok=false;
	Token_en token= T_UNKNOWN;//Token();
	//if (token != T_UNKNOWN) return token;
	switch (stack[sp]){
#	define MLXGROUP(name) case T_ ## name: { token= rule_ ## name(); }; break;
#	define END_MLXGROUP
#	define MLXSEQUENCE(name)  case T_ ## name: {  token= rule_ ## name();}; break;
#	define END_MLXSEQUENCE
#	define IS(name, hit)
#	define MLXCHAR(ch, name)
#	define MLXRANGE(from, to, name)
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
		default: break;
	};//switch
//	Next();
}
