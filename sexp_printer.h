#ifndef SEXP_WRITER
#define SEXP_WRITER

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>

#include <iostream>

class SexpPrinter
{
 public:
 SexpPrinter(std::ostream &o, unsigned int margin) :o(o), margin(margin) {}
  void startList();
  void printAtom(const std::string &atom);
  void endList();
  void addComment(const std::string &comment);
  void lineBreak();
  
 private:
  enum class State
  {
    FRESH, ONE_LINE, MULTI_LINE
  };
  friend std::ostream& operator << (std::ostream &o, State s)
  {
    switch(s)
      {
      case SexpPrinter::State::FRESH: o << "FRESH"; break;
      case SexpPrinter::State::ONE_LINE: o << "ONE_LINE"; break;
      case SexpPrinter::State::MULTI_LINE: o << "MULTI_LINE"; break;
      }
    return o;
  }
  struct PrintState
  {
    int idnt;
    State st;
    std::vector<std::string> acc;
    int acc_len;
    void push_to_acc(const std::string &s)
    {
      acc.push_back(s);
      acc_len += s.size();
      ++acc_len;
      if(st == SexpPrinter::State::FRESH)
	{
	  st = SexpPrinter::State::ONE_LINE;
	}
    }
  };
  std::vector<PrintState> stack;
  std::ostream &o;
  const unsigned int margin;
};

#endif
