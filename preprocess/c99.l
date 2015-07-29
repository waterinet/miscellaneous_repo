%option noyywrap nodefault yylineno

D            [0-9]
L            [a-zA-Z_]
H            [a-fA-F0-9]
E            ([Ee][+-]?{D}+)
P            ([Pp][+-]?{D}+)
FS           (f|F|l|L)
IS           ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%{

typedef struct token {
	int lineno;
	char* word;
} token;

typedef struct snode {
    void* data;
    struct snode* next;
} snode;

typedef struct slist {
    size_t size;
    snode* head;
    snode* tail;
} slist;

slist token_list = { 0, 0, 0 };

token* new_token(const char* word, int lineno) {
	token* t = (token*)malloc(sizeof(token));
	t->word = (char*)malloc(strlen(word) + 1);
	strcpy(t->word, word);
	t->lineno = lineno;
	return t;
}

snode* new_node(void* data) {
    snode* node = (snode*)malloc(sizeof(snode));
    node->data = data;
    node->next = 0;
    return node;
}

void add_node(slist* list, snode* node) {
    if (list->size > 0) {
        list->tail->next = node;
        list->tail = list->tail->next;
        ++list->size;
    }
    else {
        list->tail = node;
        list->head = list->tail;
        ++list->size;
    }
}

void comment();

%}

%%

"/*"         { comment(); }
"//"[^\n]*   {}

"auto"       { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"_Bool"      { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"break"      { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"case"       { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"char"       { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"_Complex"   { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"const"      { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"continue"   { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"default"    { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"do"         { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"double"     { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"else"       { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"enum"       { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"extern"     { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"float"      { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"for"        { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"goto"       { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"if"         { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"_Imaginary" { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"inline"     { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"int"        { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"long"       { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"register"   { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"restrict"   { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"return"     { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"short"      { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"signed"     { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"sizeof"     { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"static"     { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"struct"     { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"switch"     { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"typedef"    { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"union"      { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"unsigned"   { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"void"       { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"volatile"   { add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"while"      { add_node(&token_list, new_node(new_token(yytext, yylineno))); }

{L}({L}|{D})*                 {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }

0[xX]{H}+{IS}?                {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
0[0-7]*{IS}?                  {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
[1-9]{D}*{IS}?                {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
L?'(\\.|[^\\'\n])+'           {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }

{D}+{E}{FS}?                  {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
{D}*"."{D}+{E}?{FS}?          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
{D}+"."{D}*{E}?{FS}?          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
0[xX]{H}+{P}{FS}?             {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
0[xX]{H}*"."{H}+{P}?{FS}?     {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
0[xX]{H}+"."{H}*{P}?{FS}?     {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }

L?\"(\\.|[^\\"\n])*\"         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }

"..."        {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
">>="        {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"<<="        {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"+="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"-="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"*="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"/="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"%="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"&="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"^="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"|="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
">>"         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"<<"         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"++"         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"--"         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"->"         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"&&"         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"||"         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"<="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
">="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"=="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"!="         {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
";"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
("{"|"<%")   {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
("}"|"%>")   {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
","          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
":"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"="          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"("          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
")"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
("["|"<:")   {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
("]"|":>")   {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"."          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"&"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"!"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"~"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"-"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"+"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"*"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"/"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"%"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"<"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
">"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"^"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"|"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }
"?"          {  add_node(&token_list, new_node(new_token(yytext, yylineno))); }

[ \t\v\n\f]  {}
.            {}

%%

void comment()
{
	char c, prev = 0;

	while ((c = input()) != 0)      /* (EOF maps to 0) */
	{
		if (c == '/' && prev == '*')
			return;
		prev = c;
	}
	/* unterminated comment*/
}

int main(int argc, char** argv) 
{
	yyin = fopen("c99_test.c", "r");
	yylex();
	
	snode* p = token_list.head;
	while (p != 0) {
		token* t = (token*)(p->data);
		printf("%d %s\n", t->lineno, t->word);
		p = p->next;
	};
	printf("size: %zu\n", token_list.size);
}
