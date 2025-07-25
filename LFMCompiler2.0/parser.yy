%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.2"
%defines

%define api.token.constructor
%define api.location.file none
%define api.value.type variant
%define parse.assert
%define parse.trace
%define parse.error verbose

%code requires {

  #include <string>
  #include <vector>
  #include <utility>

  class driver;
  class RootAST;
  class ExprAST;
  class FunctionAST;
  class PrototypeAST;
  class IfExprAST;
  class LetExprAST;
  class DefAST;
  class ForwardDeclarationAST;
  class LambdaExprAst;
  struct LambdaBody {
    ExprAST* expr;
    std::vector<std::pair<std::string, ExprAST*>> captures;
  };
  
  // Tell Flex the lexer's prototype ...
# define YY_DECL \
  yy::parser::symbol_type yylex (driver& drv)
}

%{
  // parser.yy - Aggiungi in testa al prologo
  #define __ASSERT_FUNCTION __extension__ __func__  // Workaround per assert.h
  #include <cassert>  // Usa la versione C++
%}

%param { driver& drv }

%locations

%code {
# include "driver.hpp"
YY_DECL;
}

%define api.token.prefix {TOK_}
%token
  EOF  0  "end of file"
  COLON      ":"
  SEMICOLON  ";"
  COMMA      ","
  MINUS      "-"
  PLUS       "+"
  STAR       "*"
  SLASH      "/"
  MOD        "%"
  LPAREN     "("
  RPAREN     ")"
  LSPAREN    "["
  RSPAREN    "]"  
  ALT        "|"
  LE         "<="
  LT         "<"
  EQ         "=="
  NEQ        "<>"
  GE         ">="
  GT         ">"
  BIND       "="
  TRUE       "true"
  FALSE      "false"
  EXTERN     "external"
  DEF        "function"
  FORWARD    "forward"
  LAMBDA     "lambda"
  IF         "if"
  AND        "and"
  OR         "or"
  NOT        "not"
  LET        "let"
  IN         "in"
  END        "end"
;

%token <std::string> IDENTIFIER "id"
%token <int> NUMBER "number"

%type <std::vector<DefAST*>> deflist
%type <DefAST*> def
%type <ExprAST*> expr
%type <ExprAST*> boolexpr
%type <FunctionAST*> funcdef
%type <PrototypeAST*> extdef
%type <ForwardDeclarationAST*> forwarddef
%type <LambdaBody> lambda_body
%type <std::vector<std::pair<std::string, ExprAST*>>> captures
%type <std::pair<std::string, ExprAST*>> capture
%type <PrototypeAST*> prototype
%type <std::vector<std::string>> params
%type <std::vector<ExprAST*>> arglist
%type <std::vector<ExprAST*>> args
%type <IfExprAST*> condexpr
%type <LetExprAST*> letexpr
%type <std::vector<std::pair<ExprAST*, ExprAST*>>> pairs;
%type <std::pair<ExprAST*, ExprAST*>> pair;
%type <std::vector<std::pair<std::string, ExprAST*>>> bindings;
%type <std::pair<std::string, ExprAST*>> binding;
%type <ExprAST*> literal
%type <ExprAST*> relexpr
%%

%start startsymb;

startsymb:
  deflist               { drv.root = $1;};

deflist:
  def deflist           { $2.insert($2.begin(),$1); $$ = $2; }
| def                   { std::vector<DefAST*> D = {$1}; $$ = D; };

def:
  extdef                { $$ = $1; }
| funcdef               { $$ = $1; };
| forwarddef            { $$ = $1; };

extdef:
  "external" prototype  { $2->setext(); $$ = $2; };

funcdef:
  "function" prototype expr "end"  { $$ = new FunctionAST($2,$3); };

forwarddef:
  FORWARD IDENTIFIER LPAREN params RPAREN { $$ = new ForwardDeclarationAST($2, $4); };

prototype:
  "id" "(" params ")"   { $$ = new PrototypeAST($1,$3); };

params:
  %empty                { std::vector<std::string> params; $$ = params; }
| "id" params           { $2.insert($2.begin(),$1); $$ = $2;};
 
%nonassoc "<" "==" "<>" "<=" ">" ">=";
%left "+" "-";
%left "*" "/" "%";
%nonassoc UMINUS;
%left "or";
%left "and";
%nonassoc NEGATE;

expr:
  expr "+" expr          { $$ = new BinaryExprAST("+",$1,$3); }
| expr "-" expr          { $$ = new BinaryExprAST("-",$1,$3); }
| expr "*" expr          { $$ = new BinaryExprAST("*",$1,$3); }
| expr "/" expr          { $$ = new BinaryExprAST("/",$1,$3); }
| expr "%" expr          { $$ = new BinaryExprAST("%",$1,$3); }
| "-" expr %prec UMINUS  { $$ = new UnaryExprAST("-",$2); }
| "(" expr ")"           { $$ = $2; }
| "id"                   { $$ = new IdeExprAST($1); }
| "id" "(" arglist ")"   { $$ = new CallExprAST($1,$3); }
| "number"               { $$ = new NumberExprAST($1); }
| condexpr               { $$ = $1; }
| letexpr                { $$ = $1; };
| LAMBDA LPAREN params RPAREN lambda_body %prec LAMBDA { $$ = new LambdaExprAST($3, $5.expr, $5.captures); }

lambda_body:
  expr {
    LambdaBody lb;
    lb.expr = $1;
    lb.captures = {};
    $$ = lb;
  }
| LSPAREN captures RSPAREN expr  {
    LambdaBody lb;
    lb.expr = $4;
    lb.captures = $2;
    $$ = lb;
  }
;
  
arglist: 
  %empty                 { std::vector<ExprAST*> args; $$ = args; }
| args                   { $$ = $1; };

args:
  expr                   { std::vector<ExprAST*> V = {$1}; $$ = V; }
| expr "," args          { $3.insert($3.begin(),$1); $$ = $3; };

condexpr:
 "if" pairs "end"        { $$ = new IfExprAST($2); };
 
pairs:
  pair                   { std::vector<std::pair<ExprAST*, ExprAST*>> P = {$1}; $$ = P; }
| pair ";" pairs         { $3.insert($3.begin(),$1); $$ = $3; }
| pair ";"               {std::vector<std::pair<ExprAST*, ExprAST*>> P = {$1}; $$ = P; };


pair: 
  boolexpr ":" expr      { std::pair<ExprAST*,ExprAST*> P ($1,$3); $$ = P; };
  
boolexpr:
  boolexpr "and" boolexpr { $$ = new BinaryExprAST("and",$1,$3); }
| boolexpr "or" boolexpr  { $$ = new BinaryExprAST("or",$1,$3); }
| "not" boolexpr  %prec NEGATE { $$ = new UnaryExprAST("not",$2); }
| literal                 { $$ = $1; }
| relexpr                 { $$ = $1; };

literal:
  "true"                  { $$ = new BoolConstAST(1); }
| "false"                 { $$ = new BoolConstAST(0); };

relexpr:
  expr "<"  expr          { $$ = new BinaryExprAST("<",$1,$3); }
| expr "==" expr          { $$ = new BinaryExprAST("==",$1,$3); }
| expr "<>" expr          { $$ = new BinaryExprAST("<>",$1,$3); }
| expr "<=" expr          { $$ = new BinaryExprAST("<=",$1,$3); }
| expr ">"  expr          { $$ = new BinaryExprAST(">",$1,$3); }
| expr ">=" expr          { $$ = new BinaryExprAST(">=",$1,$3); }

letexpr: 
  "let" bindings "in" expr "end" { $$ = new LetExprAST($2,$4); };

bindings:
  binding                 { std::vector<std::pair<std::string, ExprAST*>> B = {$1}; $$ = B; }
| binding "," bindings    { $3.insert($3.begin(),$1); $$ = $3; };
  
binding:
  "id" "=" expr           { std::pair<std::string, ExprAST*> C ($1,$3); $$ = C; };

captures:
  capture                  { $$ = {$1}; }
| capture "," captures    { $3.insert($3.begin(), $1); $$ = $3; };

capture:
  "id" "=" expr           { $$ = std::make_pair($1, $3); };
 
%%

void yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}
