#include <iostream>
#include "driver.hpp"
#include <fstream>
#include <string>

extern LLVMContext *context;
extern Module *module;
extern IRBuilder<> *builder;

driver drv;

int main (int argc, char *argv[])
{
  bool verbose = false;
  bool latex = false;
  bool gencode = false;
  static std::ofstream outfile;
  std::string latex_preamble = R"PREAMBLE(
\documentclass[12pt]{article}
\usepackage{synttree}
\usepackage{pdflscape}
\begin{document}
\thispagestyle{empty} 
\begin{landscape}
)PREAMBLE";
  std::cout << argv[0] << std::endl;
  for (int i = 1; i < argc; ++i)
    if (argv[i] == std::string ("-p"))
      drv.trace_parsing = true;
    else if (argv[i] == std::string ("-s"))
      drv.trace_scanning = true;
    else if  (argv[i] == std::string("-v"))
      verbose = true;
    else if (argv[i] == std::string("-l"))
      latex = true;
    else if (argv[i] == std::string("-c"))
      gencode = true;
    else if (!drv.parse (argv[i])) {
      std::cout << "Parse successful\n";
      if (latex or verbose) {
      	if (latex) {
          drv.toLatex = true;
          drv.opening = "[$";
          drv.closing = "$]";
          outfile.open(argv[i]+std::string(".tex"));
          drv.outputTarget = &outfile;
          *drv.outputTarget << latex_preamble << std::endl;
          *drv.outputTarget << "{\\bf\\LARGE Foresta AST di " << argv[i] << "}\n";
          *drv.outputTarget << "\\vspace{1cm}\n\n";
        } else { 
          drv.outputTarget = &std::cout;
        }
        for (DefAST* tree: drv.root) {
          if (latex) *drv.outputTarget << "\\synttree";
          tree->visit();
          if (latex) *drv.outputTarget << "\n\\par\\vspace{1cm}\n";
          else std::cout << std::endl;
        }
        if (latex) *drv.outputTarget << "\\end{landscape}\n\\end{document}";
        outfile.close();
      }
      if (gencode) {
      	drv.codegen();
      }
    } else return 1;
  return 0;
}
