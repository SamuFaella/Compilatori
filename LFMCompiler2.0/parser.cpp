// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.



// First part of user prologue.
#line 38 "parser.yy"

  // parser.yy - Aggiungi in testa al prologo
  #define __ASSERT_FUNCTION __extension__ __func__  // Workaround per assert.h
  #include <cassert>  // Usa la versione C++

#line 47 "parser.cpp"


#include "parser.hpp"


// Unqualified %code blocks.
#line 48 "parser.yy"

# include "driver.hpp"
YY_DECL;

#line 59 "parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 151 "parser.cpp"

  /// Build a parser object.
  parser::parser (driver& drv_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      drv (drv_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_def: // def
        value.YY_MOVE_OR_COPY< DefAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_boolexpr: // boolexpr
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_relexpr: // relexpr
        value.YY_MOVE_OR_COPY< ExprAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_forwarddef: // forwarddef
        value.YY_MOVE_OR_COPY< ForwardDeclarationAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_funcdef: // funcdef
        value.YY_MOVE_OR_COPY< FunctionAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_condexpr: // condexpr
        value.YY_MOVE_OR_COPY< IfExprAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_lambda_body: // lambda_body
        value.YY_MOVE_OR_COPY< LambdaBody > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_letexpr: // letexpr
        value.YY_MOVE_OR_COPY< LetExprAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_extdef: // extdef
      case symbol_kind::S_prototype: // prototype
        value.YY_MOVE_OR_COPY< PrototypeAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMBER: // "number"
        value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_pair: // pair
        value.YY_MOVE_OR_COPY< std::pair<ExprAST*, ExprAST*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_binding: // binding
      case symbol_kind::S_capture: // capture
        value.YY_MOVE_OR_COPY< std::pair<std::string, ExprAST*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "id"
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_deflist: // deflist
        value.YY_MOVE_OR_COPY< std::vector<DefAST*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_arglist: // arglist
      case symbol_kind::S_args: // args
        value.YY_MOVE_OR_COPY< std::vector<ExprAST*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_pairs: // pairs
        value.YY_MOVE_OR_COPY< std::vector<std::pair<ExprAST*, ExprAST*>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_bindings: // bindings
      case symbol_kind::S_captures: // captures
        value.YY_MOVE_OR_COPY< std::vector<std::pair<std::string, ExprAST*>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_params: // params
        value.YY_MOVE_OR_COPY< std::vector<std::string> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_def: // def
        value.move< DefAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_boolexpr: // boolexpr
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_relexpr: // relexpr
        value.move< ExprAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_forwarddef: // forwarddef
        value.move< ForwardDeclarationAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_funcdef: // funcdef
        value.move< FunctionAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_condexpr: // condexpr
        value.move< IfExprAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_lambda_body: // lambda_body
        value.move< LambdaBody > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_letexpr: // letexpr
        value.move< LetExprAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_extdef: // extdef
      case symbol_kind::S_prototype: // prototype
        value.move< PrototypeAST* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMBER: // "number"
        value.move< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_pair: // pair
        value.move< std::pair<ExprAST*, ExprAST*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_binding: // binding
      case symbol_kind::S_capture: // capture
        value.move< std::pair<std::string, ExprAST*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "id"
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_deflist: // deflist
        value.move< std::vector<DefAST*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_arglist: // arglist
      case symbol_kind::S_args: // args
        value.move< std::vector<ExprAST*> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_pairs: // pairs
        value.move< std::vector<std::pair<ExprAST*, ExprAST*>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_bindings: // bindings
      case symbol_kind::S_captures: // captures
        value.move< std::vector<std::pair<std::string, ExprAST*>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_params: // params
        value.move< std::vector<std::string> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_def: // def
        value.copy< DefAST* > (that.value);
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_boolexpr: // boolexpr
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_relexpr: // relexpr
        value.copy< ExprAST* > (that.value);
        break;

      case symbol_kind::S_forwarddef: // forwarddef
        value.copy< ForwardDeclarationAST* > (that.value);
        break;

      case symbol_kind::S_funcdef: // funcdef
        value.copy< FunctionAST* > (that.value);
        break;

      case symbol_kind::S_condexpr: // condexpr
        value.copy< IfExprAST* > (that.value);
        break;

      case symbol_kind::S_lambda_body: // lambda_body
        value.copy< LambdaBody > (that.value);
        break;

      case symbol_kind::S_letexpr: // letexpr
        value.copy< LetExprAST* > (that.value);
        break;

      case symbol_kind::S_extdef: // extdef
      case symbol_kind::S_prototype: // prototype
        value.copy< PrototypeAST* > (that.value);
        break;

      case symbol_kind::S_NUMBER: // "number"
        value.copy< int > (that.value);
        break;

      case symbol_kind::S_pair: // pair
        value.copy< std::pair<ExprAST*, ExprAST*> > (that.value);
        break;

      case symbol_kind::S_binding: // binding
      case symbol_kind::S_capture: // capture
        value.copy< std::pair<std::string, ExprAST*> > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // "id"
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_deflist: // deflist
        value.copy< std::vector<DefAST*> > (that.value);
        break;

      case symbol_kind::S_arglist: // arglist
      case symbol_kind::S_args: // args
        value.copy< std::vector<ExprAST*> > (that.value);
        break;

      case symbol_kind::S_pairs: // pairs
        value.copy< std::vector<std::pair<ExprAST*, ExprAST*>> > (that.value);
        break;

      case symbol_kind::S_bindings: // bindings
      case symbol_kind::S_captures: // captures
        value.copy< std::vector<std::pair<std::string, ExprAST*>> > (that.value);
        break;

      case symbol_kind::S_params: // params
        value.copy< std::vector<std::string> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_def: // def
        value.move< DefAST* > (that.value);
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_boolexpr: // boolexpr
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_relexpr: // relexpr
        value.move< ExprAST* > (that.value);
        break;

      case symbol_kind::S_forwarddef: // forwarddef
        value.move< ForwardDeclarationAST* > (that.value);
        break;

      case symbol_kind::S_funcdef: // funcdef
        value.move< FunctionAST* > (that.value);
        break;

      case symbol_kind::S_condexpr: // condexpr
        value.move< IfExprAST* > (that.value);
        break;

      case symbol_kind::S_lambda_body: // lambda_body
        value.move< LambdaBody > (that.value);
        break;

      case symbol_kind::S_letexpr: // letexpr
        value.move< LetExprAST* > (that.value);
        break;

      case symbol_kind::S_extdef: // extdef
      case symbol_kind::S_prototype: // prototype
        value.move< PrototypeAST* > (that.value);
        break;

      case symbol_kind::S_NUMBER: // "number"
        value.move< int > (that.value);
        break;

      case symbol_kind::S_pair: // pair
        value.move< std::pair<ExprAST*, ExprAST*> > (that.value);
        break;

      case symbol_kind::S_binding: // binding
      case symbol_kind::S_capture: // capture
        value.move< std::pair<std::string, ExprAST*> > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // "id"
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_deflist: // deflist
        value.move< std::vector<DefAST*> > (that.value);
        break;

      case symbol_kind::S_arglist: // arglist
      case symbol_kind::S_args: // args
        value.move< std::vector<ExprAST*> > (that.value);
        break;

      case symbol_kind::S_pairs: // pairs
        value.move< std::vector<std::pair<ExprAST*, ExprAST*>> > (that.value);
        break;

      case symbol_kind::S_bindings: // bindings
      case symbol_kind::S_captures: // captures
        value.move< std::vector<std::pair<std::string, ExprAST*>> > (that.value);
        break;

      case symbol_kind::S_params: // params
        value.move< std::vector<std::string> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (drv));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_def: // def
        yylhs.value.emplace< DefAST* > ();
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_boolexpr: // boolexpr
      case symbol_kind::S_literal: // literal
      case symbol_kind::S_relexpr: // relexpr
        yylhs.value.emplace< ExprAST* > ();
        break;

      case symbol_kind::S_forwarddef: // forwarddef
        yylhs.value.emplace< ForwardDeclarationAST* > ();
        break;

      case symbol_kind::S_funcdef: // funcdef
        yylhs.value.emplace< FunctionAST* > ();
        break;

      case symbol_kind::S_condexpr: // condexpr
        yylhs.value.emplace< IfExprAST* > ();
        break;

      case symbol_kind::S_lambda_body: // lambda_body
        yylhs.value.emplace< LambdaBody > ();
        break;

      case symbol_kind::S_letexpr: // letexpr
        yylhs.value.emplace< LetExprAST* > ();
        break;

      case symbol_kind::S_extdef: // extdef
      case symbol_kind::S_prototype: // prototype
        yylhs.value.emplace< PrototypeAST* > ();
        break;

      case symbol_kind::S_NUMBER: // "number"
        yylhs.value.emplace< int > ();
        break;

      case symbol_kind::S_pair: // pair
        yylhs.value.emplace< std::pair<ExprAST*, ExprAST*> > ();
        break;

      case symbol_kind::S_binding: // binding
      case symbol_kind::S_capture: // capture
        yylhs.value.emplace< std::pair<std::string, ExprAST*> > ();
        break;

      case symbol_kind::S_IDENTIFIER: // "id"
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_deflist: // deflist
        yylhs.value.emplace< std::vector<DefAST*> > ();
        break;

      case symbol_kind::S_arglist: // arglist
      case symbol_kind::S_args: // args
        yylhs.value.emplace< std::vector<ExprAST*> > ();
        break;

      case symbol_kind::S_pairs: // pairs
        yylhs.value.emplace< std::vector<std::pair<ExprAST*, ExprAST*>> > ();
        break;

      case symbol_kind::S_bindings: // bindings
      case symbol_kind::S_captures: // captures
        yylhs.value.emplace< std::vector<std::pair<std::string, ExprAST*>> > ();
        break;

      case symbol_kind::S_params: // params
        yylhs.value.emplace< std::vector<std::string> > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // startsymb: deflist
#line 121 "parser.yy"
                        { drv.root = yystack_[0].value.as < std::vector<DefAST*> > ();}
#line 920 "parser.cpp"
    break;

  case 3: // deflist: def deflist
#line 124 "parser.yy"
                        { yystack_[0].value.as < std::vector<DefAST*> > ().insert(yystack_[0].value.as < std::vector<DefAST*> > ().begin(),yystack_[1].value.as < DefAST* > ()); yylhs.value.as < std::vector<DefAST*> > () = yystack_[0].value.as < std::vector<DefAST*> > (); }
#line 926 "parser.cpp"
    break;

  case 4: // deflist: def
#line 125 "parser.yy"
                        { std::vector<DefAST*> D = {yystack_[0].value.as < DefAST* > ()}; yylhs.value.as < std::vector<DefAST*> > () = D; }
#line 932 "parser.cpp"
    break;

  case 5: // def: extdef
#line 128 "parser.yy"
                        { yylhs.value.as < DefAST* > () = yystack_[0].value.as < PrototypeAST* > (); }
#line 938 "parser.cpp"
    break;

  case 6: // def: funcdef
#line 129 "parser.yy"
                        { yylhs.value.as < DefAST* > () = yystack_[0].value.as < FunctionAST* > (); }
#line 944 "parser.cpp"
    break;

  case 7: // def: forwarddef
#line 130 "parser.yy"
                        { yylhs.value.as < DefAST* > () = yystack_[0].value.as < ForwardDeclarationAST* > (); }
#line 950 "parser.cpp"
    break;

  case 8: // extdef: "external" prototype
#line 133 "parser.yy"
                        { yystack_[0].value.as < PrototypeAST* > ()->setext(); yylhs.value.as < PrototypeAST* > () = yystack_[0].value.as < PrototypeAST* > (); }
#line 956 "parser.cpp"
    break;

  case 9: // funcdef: "function" prototype expr "end"
#line 136 "parser.yy"
                                   { yylhs.value.as < FunctionAST* > () = new FunctionAST(yystack_[2].value.as < PrototypeAST* > (),yystack_[1].value.as < ExprAST* > ()); }
#line 962 "parser.cpp"
    break;

  case 10: // forwarddef: "forward" "id" "(" params ")"
#line 139 "parser.yy"
                                          { yylhs.value.as < ForwardDeclarationAST* > () = new ForwardDeclarationAST(yystack_[3].value.as < std::string > (), yystack_[1].value.as < std::vector<std::string> > ()); }
#line 968 "parser.cpp"
    break;

  case 11: // prototype: "id" "(" params ")"
#line 142 "parser.yy"
                        { yylhs.value.as < PrototypeAST* > () = new PrototypeAST(yystack_[3].value.as < std::string > (),yystack_[1].value.as < std::vector<std::string> > ()); }
#line 974 "parser.cpp"
    break;

  case 12: // params: %empty
#line 145 "parser.yy"
                        { std::vector<std::string> params; yylhs.value.as < std::vector<std::string> > () = params; }
#line 980 "parser.cpp"
    break;

  case 13: // params: "id" params
#line 146 "parser.yy"
                        { yystack_[0].value.as < std::vector<std::string> > ().insert(yystack_[0].value.as < std::vector<std::string> > ().begin(),yystack_[1].value.as < std::string > ()); yylhs.value.as < std::vector<std::string> > () = yystack_[0].value.as < std::vector<std::string> > ();}
#line 986 "parser.cpp"
    break;

  case 14: // expr: expr "+" expr
#line 157 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new BinaryExprAST("+",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 992 "parser.cpp"
    break;

  case 15: // expr: expr "-" expr
#line 158 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new BinaryExprAST("-",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 998 "parser.cpp"
    break;

  case 16: // expr: expr "*" expr
#line 159 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new BinaryExprAST("*",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1004 "parser.cpp"
    break;

  case 17: // expr: expr "/" expr
#line 160 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new BinaryExprAST("/",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1010 "parser.cpp"
    break;

  case 18: // expr: expr "%" expr
#line 161 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new BinaryExprAST("%",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1016 "parser.cpp"
    break;

  case 19: // expr: "-" expr
#line 162 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new UnaryExprAST("-",yystack_[0].value.as < ExprAST* > ()); }
#line 1022 "parser.cpp"
    break;

  case 20: // expr: "(" expr ")"
#line 163 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = yystack_[1].value.as < ExprAST* > (); }
#line 1028 "parser.cpp"
    break;

  case 21: // expr: "id"
#line 164 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new IdeExprAST(yystack_[0].value.as < std::string > ()); }
#line 1034 "parser.cpp"
    break;

  case 22: // expr: "id" "(" arglist ")"
#line 165 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new CallExprAST(yystack_[3].value.as < std::string > (),yystack_[1].value.as < std::vector<ExprAST*> > ()); }
#line 1040 "parser.cpp"
    break;

  case 23: // expr: "number"
#line 166 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new NumberExprAST(yystack_[0].value.as < int > ()); }
#line 1046 "parser.cpp"
    break;

  case 24: // expr: condexpr
#line 167 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = yystack_[0].value.as < IfExprAST* > (); }
#line 1052 "parser.cpp"
    break;

  case 25: // expr: letexpr
#line 168 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = yystack_[0].value.as < LetExprAST* > (); }
#line 1058 "parser.cpp"
    break;

  case 26: // expr: "lambda" "(" params ")" lambda_body
#line 169 "parser.yy"
                                                       { yylhs.value.as < ExprAST* > () = new LambdaExprAST(yystack_[2].value.as < std::vector<std::string> > (), yystack_[0].value.as < LambdaBody > ().expr, yystack_[0].value.as < LambdaBody > ().captures); }
#line 1064 "parser.cpp"
    break;

  case 27: // lambda_body: expr
#line 172 "parser.yy"
       {
    LambdaBody lb;
    lb.expr = yystack_[0].value.as < ExprAST* > ();
    lb.captures = {};
    yylhs.value.as < LambdaBody > () = lb;
  }
#line 1075 "parser.cpp"
    break;

  case 28: // lambda_body: "[" captures "]" expr
#line 178 "parser.yy"
                                 {
    LambdaBody lb;
    lb.expr = yystack_[0].value.as < ExprAST* > ();
    lb.captures = yystack_[2].value.as < std::vector<std::pair<std::string, ExprAST*>> > ();
    yylhs.value.as < LambdaBody > () = lb;
  }
#line 1086 "parser.cpp"
    break;

  case 29: // arglist: %empty
#line 187 "parser.yy"
                         { std::vector<ExprAST*> args; yylhs.value.as < std::vector<ExprAST*> > () = args; }
#line 1092 "parser.cpp"
    break;

  case 30: // arglist: args
#line 188 "parser.yy"
                         { yylhs.value.as < std::vector<ExprAST*> > () = yystack_[0].value.as < std::vector<ExprAST*> > (); }
#line 1098 "parser.cpp"
    break;

  case 31: // args: expr
#line 191 "parser.yy"
                         { std::vector<ExprAST*> V = {yystack_[0].value.as < ExprAST* > ()}; yylhs.value.as < std::vector<ExprAST*> > () = V; }
#line 1104 "parser.cpp"
    break;

  case 32: // args: expr "," args
#line 192 "parser.yy"
                         { yystack_[0].value.as < std::vector<ExprAST*> > ().insert(yystack_[0].value.as < std::vector<ExprAST*> > ().begin(),yystack_[2].value.as < ExprAST* > ()); yylhs.value.as < std::vector<ExprAST*> > () = yystack_[0].value.as < std::vector<ExprAST*> > (); }
#line 1110 "parser.cpp"
    break;

  case 33: // condexpr: "if" pairs "end"
#line 195 "parser.yy"
                         { yylhs.value.as < IfExprAST* > () = new IfExprAST(yystack_[1].value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > ()); }
#line 1116 "parser.cpp"
    break;

  case 34: // pairs: pair
#line 198 "parser.yy"
                         { std::vector<std::pair<ExprAST*, ExprAST*>> P = {yystack_[0].value.as < std::pair<ExprAST*, ExprAST*> > ()}; yylhs.value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > () = P; }
#line 1122 "parser.cpp"
    break;

  case 35: // pairs: pair ";" pairs
#line 199 "parser.yy"
                         { yystack_[0].value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > ().insert(yystack_[0].value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > ().begin(),yystack_[2].value.as < std::pair<ExprAST*, ExprAST*> > ()); yylhs.value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > () = yystack_[0].value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > (); }
#line 1128 "parser.cpp"
    break;

  case 36: // pairs: pair ";"
#line 200 "parser.yy"
                         {std::vector<std::pair<ExprAST*, ExprAST*>> P = {yystack_[1].value.as < std::pair<ExprAST*, ExprAST*> > ()}; yylhs.value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > () = P; }
#line 1134 "parser.cpp"
    break;

  case 37: // pair: boolexpr ":" expr
#line 204 "parser.yy"
                         { std::pair<ExprAST*,ExprAST*> P (yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); yylhs.value.as < std::pair<ExprAST*, ExprAST*> > () = P; }
#line 1140 "parser.cpp"
    break;

  case 38: // boolexpr: boolexpr "and" boolexpr
#line 207 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("and",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1146 "parser.cpp"
    break;

  case 39: // boolexpr: boolexpr "or" boolexpr
#line 208 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("or",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1152 "parser.cpp"
    break;

  case 40: // boolexpr: "not" boolexpr
#line 209 "parser.yy"
                               { yylhs.value.as < ExprAST* > () = new UnaryExprAST("not",yystack_[0].value.as < ExprAST* > ()); }
#line 1158 "parser.cpp"
    break;

  case 41: // boolexpr: literal
#line 210 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = yystack_[0].value.as < ExprAST* > (); }
#line 1164 "parser.cpp"
    break;

  case 42: // boolexpr: relexpr
#line 211 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = yystack_[0].value.as < ExprAST* > (); }
#line 1170 "parser.cpp"
    break;

  case 43: // literal: "true"
#line 214 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BoolConstAST(1); }
#line 1176 "parser.cpp"
    break;

  case 44: // literal: "false"
#line 215 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BoolConstAST(0); }
#line 1182 "parser.cpp"
    break;

  case 45: // relexpr: expr "<" expr
#line 218 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("<",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1188 "parser.cpp"
    break;

  case 46: // relexpr: expr "==" expr
#line 219 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("==",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1194 "parser.cpp"
    break;

  case 47: // relexpr: expr "<>" expr
#line 220 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("<>",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1200 "parser.cpp"
    break;

  case 48: // relexpr: expr "<=" expr
#line 221 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("<=",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1206 "parser.cpp"
    break;

  case 49: // relexpr: expr ">" expr
#line 222 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST(">",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1212 "parser.cpp"
    break;

  case 50: // relexpr: expr ">=" expr
#line 223 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST(">=",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1218 "parser.cpp"
    break;

  case 51: // letexpr: "let" bindings "in" expr "end"
#line 226 "parser.yy"
                                 { yylhs.value.as < LetExprAST* > () = new LetExprAST(yystack_[3].value.as < std::vector<std::pair<std::string, ExprAST*>> > (),yystack_[1].value.as < ExprAST* > ()); }
#line 1224 "parser.cpp"
    break;

  case 52: // bindings: binding
#line 229 "parser.yy"
                          { std::vector<std::pair<std::string, ExprAST*>> B = {yystack_[0].value.as < std::pair<std::string, ExprAST*> > ()}; yylhs.value.as < std::vector<std::pair<std::string, ExprAST*>> > () = B; }
#line 1230 "parser.cpp"
    break;

  case 53: // bindings: binding "," bindings
#line 230 "parser.yy"
                          { yystack_[0].value.as < std::vector<std::pair<std::string, ExprAST*>> > ().insert(yystack_[0].value.as < std::vector<std::pair<std::string, ExprAST*>> > ().begin(),yystack_[2].value.as < std::pair<std::string, ExprAST*> > ()); yylhs.value.as < std::vector<std::pair<std::string, ExprAST*>> > () = yystack_[0].value.as < std::vector<std::pair<std::string, ExprAST*>> > (); }
#line 1236 "parser.cpp"
    break;

  case 54: // binding: "id" "=" expr
#line 233 "parser.yy"
                          { std::pair<std::string, ExprAST*> C (yystack_[2].value.as < std::string > (),yystack_[0].value.as < ExprAST* > ()); yylhs.value.as < std::pair<std::string, ExprAST*> > () = C; }
#line 1242 "parser.cpp"
    break;

  case 55: // captures: capture
#line 236 "parser.yy"
                           { yylhs.value.as < std::vector<std::pair<std::string, ExprAST*>> > () = {yystack_[0].value.as < std::pair<std::string, ExprAST*> > ()}; }
#line 1248 "parser.cpp"
    break;

  case 56: // captures: capture "," captures
#line 237 "parser.yy"
                          { yystack_[0].value.as < std::vector<std::pair<std::string, ExprAST*>> > ().insert(yystack_[0].value.as < std::vector<std::pair<std::string, ExprAST*>> > ().begin(), yystack_[2].value.as < std::pair<std::string, ExprAST*> > ()); yylhs.value.as < std::vector<std::pair<std::string, ExprAST*>> > () = yystack_[0].value.as < std::vector<std::pair<std::string, ExprAST*>> > (); }
#line 1254 "parser.cpp"
    break;

  case 57: // capture: "id" "=" expr
#line 240 "parser.yy"
                          { yylhs.value.as < std::pair<std::string, ExprAST*> > () = std::make_pair(yystack_[2].value.as < std::string > (), yystack_[0].value.as < ExprAST* > ()); }
#line 1260 "parser.cpp"
    break;


#line 1264 "parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char parser::yypact_ninf_ = -35;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
     -16,   -34,   -34,     7,    42,   -35,   -16,   -35,   -35,   -35,
      33,   -35,    59,    34,   -35,   -35,    20,    59,    59,    48,
      49,    25,    51,   -35,     6,   -35,   -35,    20,    20,    41,
     -35,    91,    20,   -35,   -35,    49,   110,    28,    60,    -2,
     -35,   -35,    44,    37,    62,    59,    59,    59,    59,    59,
      59,   -35,    56,   -35,   -35,   -35,    63,   -35,    59,    59,
      59,    59,    59,    59,   -35,    49,    59,    49,    49,    59,
      59,    25,    12,    64,   -35,    22,    22,   -35,   -35,   -35,
     -35,    78,   115,   115,   115,   115,   115,   115,   -35,   115,
     -35,    50,   115,    17,   -35,    59,   -35,    38,   115,   -35,
     -35,   -35,    57,    76,    97,    59,    59,    38,   115,   115,
     -35
  };

  const signed char
  parser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     2,     4,     5,     6,     7,
       0,     8,     0,     0,     1,     3,    12,     0,     0,     0,
       0,     0,    21,    23,     0,    24,    25,    12,    12,     0,
      19,     0,    12,    43,    44,     0,     0,     0,    34,     0,
      41,    42,     0,     0,    52,    29,     0,     0,     0,     0,
       0,     9,     0,    13,    11,    20,     0,    40,     0,     0,
       0,     0,     0,     0,    33,    36,     0,     0,     0,     0,
       0,     0,    31,     0,    30,    15,    14,    16,    17,    18,
      10,     0,    48,    45,    46,    47,    50,    49,    35,    37,
      38,    39,    54,     0,    53,     0,    22,     0,    27,    26,
      51,    32,     0,     0,    55,     0,     0,     0,    57,    28,
      56
  };

  const signed char
  parser::yypgoto_[] =
  {
     -35,   -35,    98,   -35,   -35,   -35,   -35,   103,   -24,   -12,
     -35,   -35,    13,   -35,    45,   -35,   -28,   -35,   -35,   -35,
      61,   -35,     2,   -35
  };

  const signed char
  parser::yydefgoto_[] =
  {
       0,     4,     5,     6,     7,     8,     9,    11,    29,    36,
      99,    73,    74,    25,    37,    38,    39,    40,    41,    26,
      43,    44,   103,   104
  };

  const signed char
  parser::yytable_[] =
  {
      24,    66,    10,    52,    53,    30,    31,    57,    56,     1,
       2,     3,    46,    47,    48,    49,    50,    95,    46,    47,
      48,    49,    50,    46,    47,    48,    49,    50,    67,    68,
      48,    49,    50,    72,    75,    76,    77,    78,    79,    90,
      91,    51,    14,    13,    16,    27,    82,    83,    84,    85,
      86,    87,   100,    54,    89,    17,    28,    92,    93,    32,
      18,    42,    45,    64,    65,    17,    69,    71,    80,    98,
      18,    70,    33,    34,   102,    81,    96,    19,    20,   105,
      67,    35,    21,    72,    17,    22,    23,    19,    20,    18,
     106,    97,    21,   108,   109,    22,    23,    46,    47,    48,
      49,    50,   107,    55,    15,    12,    19,    20,   101,   110,
      88,    21,     0,     0,    22,    23,    46,    47,    48,    49,
      50,    46,    47,    48,    49,    50,    58,    59,    60,    61,
      62,    63,    94
  };

  const signed char
  parser::yycheck_[] =
  {
      12,     3,    36,    27,    28,    17,    18,    35,    32,    25,
      26,    27,     6,     7,     8,     9,    10,     5,     6,     7,
       8,     9,    10,     6,     7,     8,     9,    10,    30,    31,
       8,     9,    10,    45,    46,    47,    48,    49,    50,    67,
      68,    35,     0,    36,    11,    11,    58,    59,    60,    61,
      62,    63,    35,    12,    66,     6,    36,    69,    70,    11,
      11,    36,    11,    35,     4,     6,    22,     5,    12,    81,
      11,    34,    23,    24,    36,    12,    12,    28,    29,    22,
      30,    32,    33,    95,     6,    36,    37,    28,    29,    11,
      14,    13,    33,   105,   106,    36,    37,     6,     7,     8,
       9,    10,     5,    12,     6,     2,    28,    29,    95,   107,
      65,    33,    -1,    -1,    36,    37,     6,     7,     8,     9,
      10,     6,     7,     8,     9,    10,    16,    17,    18,    19,
      20,    21,    71
  };

  const signed char
  parser::yystos_[] =
  {
       0,    25,    26,    27,    41,    42,    43,    44,    45,    46,
      36,    47,    47,    36,     0,    42,    11,     6,    11,    28,
      29,    33,    36,    37,    49,    53,    59,    11,    36,    48,
      49,    49,    11,    23,    24,    32,    49,    54,    55,    56,
      57,    58,    36,    60,    61,    11,     6,     7,     8,     9,
      10,    35,    48,    48,    12,    12,    48,    56,    16,    17,
      18,    19,    20,    21,    35,     4,     3,    30,    31,    22,
      34,     5,    49,    51,    52,    49,    49,    49,    49,    49,
      12,    12,    49,    49,    49,    49,    49,    49,    54,    49,
      56,    56,    49,    49,    60,     5,    12,    13,    49,    50,
      35,    52,    36,    62,    63,    22,    14,     5,    49,    49,
      62
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    40,    41,    42,    42,    43,    43,    43,    44,    45,
      46,    47,    48,    48,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    50,    50,    51,
      51,    52,    52,    53,    54,    54,    54,    55,    56,    56,
      56,    56,    56,    57,    57,    58,    58,    58,    58,    58,
      58,    59,    60,    60,    61,    62,    62,    63
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     2,     1,     1,     1,     1,     2,     4,
       5,     4,     0,     2,     3,     3,     3,     3,     3,     2,
       3,     1,     4,     1,     1,     1,     5,     1,     4,     0,
       1,     1,     3,     3,     1,     3,     2,     3,     3,     3,
       2,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     5,     1,     3,     3,     1,     3,     3
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\":\"", "\";\"",
  "\",\"", "\"-\"", "\"+\"", "\"*\"", "\"/\"", "\"%\"", "\"(\"", "\")\"",
  "\"[\"", "\"]\"", "\"|\"", "\"<=\"", "\"<\"", "\"==\"", "\"<>\"",
  "\">=\"", "\">\"", "\"=\"", "\"true\"", "\"false\"", "\"external\"",
  "\"function\"", "\"forward\"", "\"lambda\"", "\"if\"", "\"and\"",
  "\"or\"", "\"not\"", "\"let\"", "\"in\"", "\"end\"", "\"id\"",
  "\"number\"", "UMINUS", "NEGATE", "$accept", "startsymb", "deflist",
  "def", "extdef", "funcdef", "forwarddef", "prototype", "params", "expr",
  "lambda_body", "arglist", "args", "condexpr", "pairs", "pair",
  "boolexpr", "literal", "relexpr", "letexpr", "bindings", "binding",
  "captures", "capture", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const unsigned char
  parser::yyrline_[] =
  {
       0,   121,   121,   124,   125,   128,   129,   130,   133,   136,
     139,   142,   145,   146,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   172,   178,   187,
     188,   191,   192,   195,   198,   199,   200,   204,   207,   208,
     209,   210,   211,   214,   215,   218,   219,   220,   221,   222,
     223,   226,   229,   230,   233,   236,   237,   240
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 1810 "parser.cpp"

#line 242 "parser.yy"


void yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}
