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





#include "parser.hpp"


// Unqualified %code blocks.
#line 32 "parser.yy"

# include "driver.hpp"
YY_DECL;

#line 51 "parser.cpp"


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
#line 143 "parser.cpp"

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
#line 99 "parser.yy"
                        { drv.root = yystack_[0].value.as < std::vector<DefAST*> > ();}
#line 882 "parser.cpp"
    break;

  case 3: // deflist: def deflist
#line 102 "parser.yy"
                        { yystack_[0].value.as < std::vector<DefAST*> > ().insert(yystack_[0].value.as < std::vector<DefAST*> > ().begin(),yystack_[1].value.as < DefAST* > ()); yylhs.value.as < std::vector<DefAST*> > () = yystack_[0].value.as < std::vector<DefAST*> > (); }
#line 888 "parser.cpp"
    break;

  case 4: // deflist: def
#line 103 "parser.yy"
                        { std::vector<DefAST*> D = {yystack_[0].value.as < DefAST* > ()}; yylhs.value.as < std::vector<DefAST*> > () = D; }
#line 894 "parser.cpp"
    break;

  case 5: // def: extdef
#line 106 "parser.yy"
                        { yylhs.value.as < DefAST* > () = yystack_[0].value.as < PrototypeAST* > (); }
#line 900 "parser.cpp"
    break;

  case 6: // def: funcdef
#line 107 "parser.yy"
                        { yylhs.value.as < DefAST* > () = yystack_[0].value.as < FunctionAST* > (); }
#line 906 "parser.cpp"
    break;

  case 7: // def: forwarddef
#line 108 "parser.yy"
                        { yylhs.value.as < DefAST* > () = yystack_[0].value.as < ForwardDeclarationAST* > (); }
#line 912 "parser.cpp"
    break;

  case 8: // extdef: "external" prototype
#line 111 "parser.yy"
                        { yystack_[0].value.as < PrototypeAST* > ()->setext(); yylhs.value.as < PrototypeAST* > () = yystack_[0].value.as < PrototypeAST* > (); }
#line 918 "parser.cpp"
    break;

  case 9: // funcdef: "function" prototype expr "end"
#line 114 "parser.yy"
                                   { yylhs.value.as < FunctionAST* > () = new FunctionAST(yystack_[2].value.as < PrototypeAST* > (),yystack_[1].value.as < ExprAST* > ()); }
#line 924 "parser.cpp"
    break;

  case 10: // forwarddef: "forward" "id" '(' params ')'
#line 117 "parser.yy"
                                  {yylhs.value.as < ForwardDeclarationAST* > () = new ForwardDeclarationAST(yystack_[3].value.as < std::string > (),yystack_[1].value.as < std::vector<std::string> > ()); }
#line 930 "parser.cpp"
    break;

  case 11: // prototype: "id" "(" params ")"
#line 120 "parser.yy"
                        { yylhs.value.as < PrototypeAST* > () = new PrototypeAST(yystack_[3].value.as < std::string > (),yystack_[1].value.as < std::vector<std::string> > ()); }
#line 936 "parser.cpp"
    break;

  case 12: // params: %empty
#line 123 "parser.yy"
                        { std::vector<std::string> params; yylhs.value.as < std::vector<std::string> > () = params; }
#line 942 "parser.cpp"
    break;

  case 13: // params: "id" params
#line 124 "parser.yy"
                        { yystack_[0].value.as < std::vector<std::string> > ().insert(yystack_[0].value.as < std::vector<std::string> > ().begin(),yystack_[1].value.as < std::string > ()); yylhs.value.as < std::vector<std::string> > () = yystack_[0].value.as < std::vector<std::string> > ();}
#line 948 "parser.cpp"
    break;

  case 14: // expr: expr "+" expr
#line 135 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new BinaryExprAST("+",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 954 "parser.cpp"
    break;

  case 15: // expr: expr "-" expr
#line 136 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new BinaryExprAST("-",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 960 "parser.cpp"
    break;

  case 16: // expr: expr "*" expr
#line 137 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new BinaryExprAST("*",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 966 "parser.cpp"
    break;

  case 17: // expr: expr "/" expr
#line 138 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new BinaryExprAST("/",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 972 "parser.cpp"
    break;

  case 18: // expr: expr "%" expr
#line 139 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new BinaryExprAST("%",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 978 "parser.cpp"
    break;

  case 19: // expr: "-" expr
#line 140 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new UnaryExprAST("-",yystack_[0].value.as < ExprAST* > ()); }
#line 984 "parser.cpp"
    break;

  case 20: // expr: "(" expr ")"
#line 141 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = yystack_[1].value.as < ExprAST* > (); }
#line 990 "parser.cpp"
    break;

  case 21: // expr: "id"
#line 142 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new IdeExprAST(yystack_[0].value.as < std::string > ()); }
#line 996 "parser.cpp"
    break;

  case 22: // expr: "id" "(" arglist ")"
#line 143 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new CallExprAST(yystack_[3].value.as < std::string > (),yystack_[1].value.as < std::vector<ExprAST*> > ()); }
#line 1002 "parser.cpp"
    break;

  case 23: // expr: "number"
#line 144 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = new NumberExprAST(yystack_[0].value.as < int > ()); }
#line 1008 "parser.cpp"
    break;

  case 24: // expr: condexpr
#line 145 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = yystack_[0].value.as < IfExprAST* > (); }
#line 1014 "parser.cpp"
    break;

  case 25: // expr: letexpr
#line 146 "parser.yy"
                         { yylhs.value.as < ExprAST* > () = yystack_[0].value.as < LetExprAST* > (); }
#line 1020 "parser.cpp"
    break;

  case 26: // arglist: %empty
#line 149 "parser.yy"
                         { std::vector<ExprAST*> args; yylhs.value.as < std::vector<ExprAST*> > () = args; }
#line 1026 "parser.cpp"
    break;

  case 27: // arglist: args
#line 150 "parser.yy"
                         { yylhs.value.as < std::vector<ExprAST*> > () = yystack_[0].value.as < std::vector<ExprAST*> > (); }
#line 1032 "parser.cpp"
    break;

  case 28: // args: expr
#line 153 "parser.yy"
                         { std::vector<ExprAST*> V = {yystack_[0].value.as < ExprAST* > ()}; yylhs.value.as < std::vector<ExprAST*> > () = V; }
#line 1038 "parser.cpp"
    break;

  case 29: // args: expr "," args
#line 154 "parser.yy"
                         { yystack_[0].value.as < std::vector<ExprAST*> > ().insert(yystack_[0].value.as < std::vector<ExprAST*> > ().begin(),yystack_[2].value.as < ExprAST* > ()); yylhs.value.as < std::vector<ExprAST*> > () = yystack_[0].value.as < std::vector<ExprAST*> > (); }
#line 1044 "parser.cpp"
    break;

  case 30: // condexpr: "if" pairs "end"
#line 157 "parser.yy"
                         { yylhs.value.as < IfExprAST* > () = new IfExprAST(yystack_[1].value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > ()); }
#line 1050 "parser.cpp"
    break;

  case 31: // pairs: pair
#line 160 "parser.yy"
                         { std::vector<std::pair<ExprAST*, ExprAST*>> P = {yystack_[0].value.as < std::pair<ExprAST*, ExprAST*> > ()}; yylhs.value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > () = P; }
#line 1056 "parser.cpp"
    break;

  case 32: // pairs: pair ";" pairs
#line 161 "parser.yy"
                         { yystack_[0].value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > ().insert(yystack_[0].value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > ().begin(),yystack_[2].value.as < std::pair<ExprAST*, ExprAST*> > ()); yylhs.value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > () = yystack_[0].value.as < std::vector<std::pair<ExprAST*, ExprAST*>> > (); }
#line 1062 "parser.cpp"
    break;

  case 33: // pair: boolexpr ":" expr
#line 164 "parser.yy"
                         { std::pair<ExprAST*,ExprAST*> P (yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); yylhs.value.as < std::pair<ExprAST*, ExprAST*> > () = P; }
#line 1068 "parser.cpp"
    break;

  case 34: // boolexpr: boolexpr "and" boolexpr
#line 167 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("and",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1074 "parser.cpp"
    break;

  case 35: // boolexpr: boolexpr "or" boolexpr
#line 168 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("or",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1080 "parser.cpp"
    break;

  case 36: // boolexpr: "not" boolexpr
#line 169 "parser.yy"
                               { yylhs.value.as < ExprAST* > () = new UnaryExprAST("not",yystack_[0].value.as < ExprAST* > ()); }
#line 1086 "parser.cpp"
    break;

  case 37: // boolexpr: literal
#line 170 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = yystack_[0].value.as < ExprAST* > (); }
#line 1092 "parser.cpp"
    break;

  case 38: // boolexpr: relexpr
#line 171 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = yystack_[0].value.as < ExprAST* > (); }
#line 1098 "parser.cpp"
    break;

  case 39: // literal: "true"
#line 174 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BoolConstAST(1); }
#line 1104 "parser.cpp"
    break;

  case 40: // literal: "false"
#line 175 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BoolConstAST(0); }
#line 1110 "parser.cpp"
    break;

  case 41: // relexpr: expr "<" expr
#line 178 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("<",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1116 "parser.cpp"
    break;

  case 42: // relexpr: expr "==" expr
#line 179 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("==",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1122 "parser.cpp"
    break;

  case 43: // relexpr: expr "<>" expr
#line 180 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("<>",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1128 "parser.cpp"
    break;

  case 44: // relexpr: expr "<=" expr
#line 181 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST("<=",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1134 "parser.cpp"
    break;

  case 45: // relexpr: expr ">" expr
#line 182 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST(">",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1140 "parser.cpp"
    break;

  case 46: // relexpr: expr ">=" expr
#line 183 "parser.yy"
                          { yylhs.value.as < ExprAST* > () = new BinaryExprAST(">=",yystack_[2].value.as < ExprAST* > (),yystack_[0].value.as < ExprAST* > ()); }
#line 1146 "parser.cpp"
    break;

  case 47: // letexpr: "let" bindings "in" expr "end"
#line 186 "parser.yy"
                                 { yylhs.value.as < LetExprAST* > () = new LetExprAST(yystack_[3].value.as < std::vector<std::pair<std::string, ExprAST*>> > (),yystack_[1].value.as < ExprAST* > ()); }
#line 1152 "parser.cpp"
    break;

  case 48: // bindings: binding
#line 189 "parser.yy"
                          { std::vector<std::pair<std::string, ExprAST*>> B = {yystack_[0].value.as < std::pair<std::string, ExprAST*> > ()}; yylhs.value.as < std::vector<std::pair<std::string, ExprAST*>> > () = B; }
#line 1158 "parser.cpp"
    break;

  case 49: // bindings: binding "," bindings
#line 190 "parser.yy"
                          { yystack_[0].value.as < std::vector<std::pair<std::string, ExprAST*>> > ().insert(yystack_[0].value.as < std::vector<std::pair<std::string, ExprAST*>> > ().begin(),yystack_[2].value.as < std::pair<std::string, ExprAST*> > ()); yylhs.value.as < std::vector<std::pair<std::string, ExprAST*>> > () = yystack_[0].value.as < std::vector<std::pair<std::string, ExprAST*>> > (); }
#line 1164 "parser.cpp"
    break;

  case 50: // binding: "id" "=" expr
#line 193 "parser.yy"
                          { std::pair<std::string, ExprAST*> C (yystack_[2].value.as < std::string > (),yystack_[0].value.as < ExprAST* > ()); yylhs.value.as < std::pair<std::string, ExprAST*> > () = C; }
#line 1170 "parser.cpp"
    break;


#line 1174 "parser.cpp"

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


  const signed char parser::yypact_ninf_ = -27;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
     -22,   -11,   -11,    -3,    41,   -27,   -22,   -27,   -27,   -27,
      39,   -27,    52,    21,   -27,   -27,    26,    52,    52,    31,
      29,    72,   -27,     8,   -27,   -27,    26,    26,    59,   -27,
      60,   -27,   -27,    31,    81,    61,    80,     1,   -27,   -27,
      74,    70,    87,    52,    52,    52,    52,    52,    52,   -27,
      66,   -27,   -27,   -27,   -27,    52,    52,    52,    52,    52,
      52,   -27,    31,    52,    31,    31,    52,    52,    29,     3,
      91,   -27,    11,    11,   -27,   -27,   -27,   -27,    67,    67,
      67,    67,    67,    67,   -27,    67,   -27,    77,    67,    17,
     -27,    52,   -27,   -27,   -27
  };

  const signed char
  parser::yydefact_[] =
  {
       0,     0,     0,     0,     0,     2,     4,     5,     6,     7,
       0,     8,     0,     0,     1,     3,    12,     0,     0,     0,
       0,    21,    23,     0,    24,    25,    12,    12,     0,    19,
       0,    39,    40,     0,     0,     0,    31,     0,    37,    38,
       0,     0,    48,    26,     0,     0,     0,     0,     0,     9,
       0,    13,    11,    20,    36,     0,     0,     0,     0,     0,
       0,    30,     0,     0,     0,     0,     0,     0,     0,    28,
       0,    27,    15,    14,    16,    17,    18,    10,    44,    41,
      42,    43,    46,    45,    32,    33,    34,    35,    50,     0,
      49,     0,    22,    47,    29
  };

  const signed char
  parser::yypgoto_[] =
  {
     -27,   -27,    99,   -27,   -27,   -27,   -27,   104,    54,   -12,
     -27,    16,   -27,    46,   -27,   -26,   -27,   -27,   -27,    42,
     -27
  };

  const signed char
  parser::yydefgoto_[] =
  {
       0,     4,     5,     6,     7,     8,     9,    11,    28,    34,
      70,    71,    24,    35,    36,    37,    38,    39,    25,    41,
      42
  };

  const signed char
  parser::yytable_[] =
  {
      23,     1,     2,     3,    63,    29,    30,    54,    91,    44,
      45,    46,    47,    48,    44,    45,    46,    47,    48,    46,
      47,    48,    10,    44,    45,    46,    47,    48,    64,    65,
      13,    69,    72,    73,    74,    75,    76,    17,    86,    87,
      49,    14,    18,    78,    79,    80,    81,    82,    83,    93,
      16,    85,    31,    32,    88,    89,    26,    19,    17,    27,
      33,    20,    40,    18,    21,    22,    44,    45,    46,    47,
      48,    52,    53,    44,    45,    46,    47,    48,    19,    69,
      50,    51,    20,    43,    62,    21,    22,    44,    45,    46,
      47,    48,    68,    61,    66,    55,    56,    57,    58,    59,
      60,    67,    77,    92,    64,    15,    12,    94,    84,     0,
      90
  };

  const signed char
  parser::yycheck_[] =
  {
      12,    23,    24,    25,     3,    17,    18,    33,     5,     6,
       7,     8,     9,    10,     6,     7,     8,     9,    10,     8,
       9,    10,    33,     6,     7,     8,     9,    10,    27,    28,
      33,    43,    44,    45,    46,    47,    48,     6,    64,    65,
      32,     0,    11,    55,    56,    57,    58,    59,    60,    32,
      11,    63,    21,    22,    66,    67,    35,    26,     6,    33,
      29,    30,    33,    11,    33,    34,     6,     7,     8,     9,
      10,    12,    12,     6,     7,     8,     9,    10,    26,    91,
      26,    27,    30,    11,     4,    33,    34,     6,     7,     8,
       9,    10,     5,    32,    20,    14,    15,    16,    17,    18,
      19,    31,    36,    12,    27,     6,     2,    91,    62,    -1,
      68
  };

  const signed char
  parser::yystos_[] =
  {
       0,    23,    24,    25,    40,    41,    42,    43,    44,    45,
      33,    46,    46,    33,     0,    41,    11,     6,    11,    26,
      30,    33,    34,    48,    51,    57,    35,    33,    47,    48,
      48,    21,    22,    29,    48,    52,    53,    54,    55,    56,
      33,    58,    59,    11,     6,     7,     8,     9,    10,    32,
      47,    47,    12,    12,    54,    14,    15,    16,    17,    18,
      19,    32,     4,     3,    27,    28,    20,    31,     5,    48,
      49,    50,    48,    48,    48,    48,    48,    36,    48,    48,
      48,    48,    48,    48,    52,    48,    54,    54,    48,    48,
      58,     5,    12,    32,    50
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    39,    40,    41,    41,    42,    42,    42,    43,    44,
      45,    46,    47,    47,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    48,    49,    49,    50,    50,
      51,    52,    52,    53,    54,    54,    54,    54,    54,    55,
      55,    56,    56,    56,    56,    56,    56,    57,    58,    58,
      59
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     2,     1,     1,     1,     1,     2,     4,
       5,     4,     0,     2,     3,     3,     3,     3,     3,     2,
       3,     1,     4,     1,     1,     1,     0,     1,     1,     3,
       3,     1,     3,     3,     3,     3,     2,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     5,     1,     3,
       3
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\":\"", "\";\"",
  "\",\"", "\"-\"", "\"+\"", "\"*\"", "\"/\"", "\"%\"", "\"(\"", "\")\"",
  "\"|\"", "\"<=\"", "\"<\"", "\"==\"", "\"<>\"", "\">=\"", "\">\"",
  "\"=\"", "\"true\"", "\"false\"", "\"external\"", "\"function\"",
  "\"forward\"", "\"if\"", "\"and\"", "\"or\"", "\"not\"", "\"let\"",
  "\"in\"", "\"end\"", "\"id\"", "\"number\"", "'('", "')'", "UMINUS",
  "NEGATE", "$accept", "startsymb", "deflist", "def", "extdef", "funcdef",
  "forwarddef", "prototype", "params", "expr", "arglist", "args",
  "condexpr", "pairs", "pair", "boolexpr", "literal", "relexpr", "letexpr",
  "bindings", "binding", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const unsigned char
  parser::yyrline_[] =
  {
       0,    99,    99,   102,   103,   106,   107,   108,   111,   114,
     117,   120,   123,   124,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   149,   150,   153,   154,
     157,   160,   161,   164,   167,   168,   169,   170,   171,   174,
     175,   178,   179,   180,   181,   182,   183,   186,   189,   190,
     193
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
#line 1709 "parser.cpp"

#line 195 "parser.yy"


void yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}
