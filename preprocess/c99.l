D            [0-9]
L            [a-zA-Z_]
H            [a-fA-F0-9]
E            ([Ee][+-]?{D}+)
P            ([Pp][+-]?{D}+)
FS           (f|F|l|L)
IS           ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%{

typedef struct snode {
    char* data;
    struct snode* next;
} snode;

typedef struct slist {
    size_t size;
    snode* head;
    snode* tail;
} slist;

slist token_list = { 0, 0, 0 };

snode* new_node(const char* data) {
    snode* node = (snode*)malloc(sizeof(snode));
    node->data = (char*)malloc(strlen(data) + 1);
    strcpy(node->data, data);
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

%}

%%

"/*"         {}
"//"[^\n]*   {}

"auto"       { add_node(&token_list, new_node(yytext)); }
"_Bool"      { add_node(&token_list, new_node(yytext)); }
"break"      { add_node(&token_list, new_node(yytext)); }
"case"       { add_node(&token_list, new_node(yytext)); }
"char"       { add_node(&token_list, new_node(yytext)); }
"_Complex"   { add_node(&token_list, new_node(yytext)); }
"const"      { add_node(&token_list, new_node(yytext)); }
"continue"   { add_node(&token_list, new_node(yytext)); }
"default"    { add_node(&token_list, new_node(yytext)); }
"do"         { add_node(&token_list, new_node(yytext)); }
"double"     { add_node(&token_list, new_node(yytext)); }
"else"       { add_node(&token_list, new_node(yytext)); }
"enum"       { add_node(&token_list, new_node(yytext)); }
"extern"     { add_node(&token_list, new_node(yytext)); }
"float"      { add_node(&token_list, new_node(yytext)); }
"for"        { add_node(&token_list, new_node(yytext)); }
"goto"       { add_node(&token_list, new_node(yytext)); }
"if"         { add_node(&token_list, new_node(yytext)); }
"_Imaginary" { add_node(&token_list, new_node(yytext)); }
"inline"     { add_node(&token_list, new_node(yytext)); }
"int"        { add_node(&token_list, new_node(yytext)); }
"long"       { add_node(&token_list, new_node(yytext)); }
"register"   { add_node(&token_list, new_node(yytext)); }
"restrict"   { add_node(&token_list, new_node(yytext)); }
"return"     { add_node(&token_list, new_node(yytext)); }
"short"      { add_node(&token_list, new_node(yytext)); }
"signed"     { add_node(&token_list, new_node(yytext)); }
"sizeof"     { add_node(&token_list, new_node(yytext)); }
"static"     { add_node(&token_list, new_node(yytext)); }
"struct"     { add_node(&token_list, new_node(yytext)); }
"switch"     { add_node(&token_list, new_node(yytext)); }
"typedef"    { add_node(&token_list, new_node(yytext)); }
"union"      { add_node(&token_list, new_node(yytext)); }
"unsigned"   { add_node(&token_list, new_node(yytext)); }
"void"       { add_node(&token_list, new_node(yytext)); }
"volatile"   { add_node(&token_list, new_node(yytext)); }
"while"      { add_node(&token_list, new_node(yytext)); }

{L}({L}|{D})*                 {  add_node(&token_list, new_node(yytext)); }

0[xX]{H}+{IS}?                {  add_node(&token_list, new_node(yytext)); }
0[0-7]*{IS}?                  {  add_node(&token_list, new_node(yytext)); }
[1-9]{D}*{IS}?                {  add_node(&token_list, new_node(yytext)); }
L?'(\\.|[^\\'\n])+'           {  add_node(&token_list, new_node(yytext)); }

{D}+{E}{FS}?                  {  add_node(&token_list, new_node(yytext)); }
{D}*"."{D}+{E}?{FS}?          {  add_node(&token_list, new_node(yytext)); }
{D}+"."{D}*{E}?{FS}?          {  add_node(&token_list, new_node(yytext)); }
0[xX]{H}+{P}{FS}?             {  add_node(&token_list, new_node(yytext)); }
0[xX]{H}*"."{H}+{P}?{FS}?     {  add_node(&token_list, new_node(yytext)); }
0[xX]{H}+"."{H}*{P}?{FS}?     {  add_node(&token_list, new_node(yytext)); }

L?\"(\\.|[^\\"\n])*\"         {  add_node(&token_list, new_node(yytext)); }

"..."        {  add_node(&token_list, new_node(yytext)); }
">>="        {  add_node(&token_list, new_node(yytext)); }
"<<="        {  add_node(&token_list, new_node(yytext)); }
"+="         {  add_node(&token_list, new_node(yytext)); }
"-="         {  add_node(&token_list, new_node(yytext)); }
"*="         {  add_node(&token_list, new_node(yytext)); }
"/="         {  add_node(&token_list, new_node(yytext)); }
"%="         {  add_node(&token_list, new_node(yytext)); }
"&="         {  add_node(&token_list, new_node(yytext)); }
"^="         {  add_node(&token_list, new_node(yytext)); }
"|="         {  add_node(&token_list, new_node(yytext)); }
">>"         {  add_node(&token_list, new_node(yytext)); }
"<<"         {  add_node(&token_list, new_node(yytext)); }
"++"         {  add_node(&token_list, new_node(yytext)); }
"--"         {  add_node(&token_list, new_node(yytext)); }
"->"         {  add_node(&token_list, new_node(yytext)); }
"&&"         {  add_node(&token_list, new_node(yytext)); }
"||"         {  add_node(&token_list, new_node(yytext)); }
"<="         {  add_node(&token_list, new_node(yytext)); }
">="         {  add_node(&token_list, new_node(yytext)); }
"=="         {  add_node(&token_list, new_node(yytext)); }
"!="         {  add_node(&token_list, new_node(yytext)); }
";"          {  add_node(&token_list, new_node(yytext)); }
("{"|"<%")   {  add_node(&token_list, new_node(yytext)); }
("}"|"%>")   {  add_node(&token_list, new_node(yytext)); }
","          {  add_node(&token_list, new_node(yytext)); }
":"          {  add_node(&token_list, new_node(yytext)); }
"="          {  add_node(&token_list, new_node(yytext)); }
"("          {  add_node(&token_list, new_node(yytext)); }
")"          {  add_node(&token_list, new_node(yytext)); }
("["|"<:")   {  add_node(&token_list, new_node(yytext)); }
("]"|":>")   {  add_node(&token_list, new_node(yytext)); }
"."          {  add_node(&token_list, new_node(yytext)); }
"&"          {  add_node(&token_list, new_node(yytext)); }
"!"          {  add_node(&token_list, new_node(yytext)); }
"~"          {  add_node(&token_list, new_node(yytext)); }
"-"          {  add_node(&token_list, new_node(yytext)); }
"+"          {  add_node(&token_list, new_node(yytext)); }
"*"          {  add_node(&token_list, new_node(yytext)); }
"/"          {  add_node(&token_list, new_node(yytext)); }
"%"          {  add_node(&token_list, new_node(yytext)); }
"<"          {  add_node(&token_list, new_node(yytext)); }
">"          {  add_node(&token_list, new_node(yytext)); }
"^"          {  add_node(&token_list, new_node(yytext)); }
"|"          {  add_node(&token_list, new_node(yytext)); }
"?"          {  add_node(&token_list, new_node(yytext)); }

[ \t\v\n\f]  {}
.            {}

%%

int main(int argc, char** argv) 
{
	yylex();
	
	snode* p = token_list.head;
	while (p != 0) {
		printf("%s\n", p->data);
		p = p->next;
	};
}
