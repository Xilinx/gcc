%{
/**
 * The lexer the lexical tokens which are important get parsed here!
 *
 * IMPORTANT: We now requite Flex to generate the lexer now!
 **/

%}

%option yylineno

DIGIT    [0-9]
ID       [_a-zA-Z][a-zA_Z0-9_$]*
qstring  \"[^\"\n]*[\"\n]

%%

#.*                     ; /* comment */

class                   {  return CLASS; }
def                     {  return DEF; }

break                   {  return BREAK; }
continue                {  return CONTINUE; }
return                  {  return RETURN; }
for                     {  return FOR; }
while                   {  return WHILE; }
print                   {  return PRINT; }

if                      {  return IF; }
elif                    {  return ELIF; }
else                    {  return ELSE; }

\[                      {  return '['; }
\]                      {  return ']'; }
\(                      {  return '('; }
\)                      {  return ')'; }
\{                      {  return '{'; }
\}                      {  return '}'; }

","                     {  return ','; }
"."                     {  return '.'; }

"="                     { return '='; }
"+"                     { return '+'; }
"-"                     { return '-'; }
"/"                     { return '/'; }
"*"                     { return '*'; }
"^"                     { return '^'; }

"=="                    { return EQUAL; }
"+="                    { return PLUS_EQUAL; }
"*="                    { return MULTIPLY_EQUAL; }
"-="                    { return MINUS_EQUAL; }
"/="                    { return DIVIDE_EQUAL; }

"!="                    { return NOT_EQUAL; }
"<"                     { return LESS; }
"<="                    { return LESS_EQUAL; }
">"                     { return GREATER; }
">="                    { return GREATER_EQUAL; }

"||"                    { return OR; }
"&&"                    { return AND; }

{qstring}               {
                          yylval.string= crl_strdup( (yytext+1) );
                          if( yylval.string[ yyleng-2 ] != '\"' ) {
                            crl_error("Un-termintated character string!\n");
                          }
                          else {
                            yylval.string[yyleng-2] = '\0';
                          }
                          return STRING;
                        }

{DIGIT}+                {
                          mpfr_t x; // folding the constants to avoid overflow
			  mpfr_init2( x, SIZEOF_INT*8 );
			  if( mpfr_set_str( x, yytext, 10, GMP_RNDU) )
			    crl_fatal("error initilizing integer value <%s>!\n", yytext );
                          yylval.integer = mpfr_get_si( x, GMP_RNDU );
			  mpfr_clear( x );
                          return INTEGER;
                        }

{DIGIT}+"."{DIGIT}*     {
                          mpfr_t x; // folding the constants to avoid overflow
			  mpfr_init2( x, SIZEOF_DOUBLE*8 );
			  if( mpfr_set_str( x, yytext, 10, GMP_RNDU) )
			    crl_fatal("error initilizing floating point value <%s>!\n", yytext );
                          yylval.floating_point= mpfr_get_ld( x, GMP_RNDU );
			  mpfr_clear( x );
                          return DOUBLE;
                        }

{ID}                    {
                          yylval.string= crl_strdup( yytext );
                          return IDENTIFIER;
                        }

[\n]                    ;
[^f\n]                  ;

%%

int yywrap( )
{
  return 1;
}

int gcc_lex_parse( FILE * in )
{
  return 0;
}
