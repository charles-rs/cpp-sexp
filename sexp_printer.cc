#include "sexp_printer.h"
#include <ostream>
#include <type_traits>


using strvec = std::vector<std::string>;

void SexpPrinter::startList()
{
  int idnt = 0;
  if(!stack.empty())
    {
      switch(stack.back().st)
	{
	case SexpPrinter::State::MULTI_LINE:
	  idnt = stack.back().idnt;
	  break;
	case SexpPrinter::State::ONE_LINE:
	  idnt = stack.back().idnt + stack.back().acc_len + 1;
	  break;
	case SexpPrinter::State::FRESH:
	  idnt = stack.back().idnt + 1;
	}
    }
  stack.emplace_back(SexpPrinter::PrintState{idnt, SexpPrinter::State::FRESH, strvec{}});
}


void SexpPrinter::printAtom(const std::string &atom)
{
  if(atom.empty())
    return;
  auto &state = stack.back();
  switch(state.st)
    {
    case SexpPrinter::State::FRESH:
      state.push_to_acc(atom);
      break;
    case SexpPrinter::State::ONE_LINE:
      state.push_to_acc(atom);
      if(state.acc_len + state.idnt > margin)
	{
	  bool changed = false;
	  int delta = -1;
	  for(auto &s : stack)
	    {
	      if(delta < 0)
		{
		  if(s.st == SexpPrinter::State::ONE_LINE)
		    {
		      s.st = SexpPrinter::State::MULTI_LINE;
		      //s.idnt -= s.acc.front().size();
		      std::string indent(s.idnt, ' ');
		      s.idnt += 2;
		      for(auto &str : s.acc)
			{
			  if(&str == &s.acc.front())
			    o << indent << "(" << str;
			  else
			    o << std::endl << indent << "  " << str;
			}
		      delta = s.acc_len;
		    }
		} else
		{
		  s.idnt -= delta;
		}
	    }
        }
      break;
    case SexpPrinter::State::MULTI_LINE:
      std::string indent(state.idnt, ' ');
      o << std::endl << indent << atom;
    }
}
void SexpPrinter::endList()
{
  if(stack.empty())
    throw("mismatched delimiters");
  auto &state = stack.back();
  switch(state.st)
    {
    case SexpPrinter::State::FRESH:
    case SexpPrinter::State::ONE_LINE:
      if(stack.size() == 1)
	{
	  o << "(";
	  for(const auto &at : state.acc)
	    {
	      o << at;
	      if(&at != &state.acc.back())
		o << " ";
	    }
	  o << ")";
	} else
	{
	  std::stringstream str;
	  str << "(";
	  for(const auto &at : state.acc)
	    {
	      str << at;
	      if(&at != &state.acc.back())
		str << " ";
	    }
	  str << ")";
	  auto &prev = *(stack.end() - 2);
	  if(prev.st == SexpPrinter::State::MULTI_LINE)
	    o << std::endl << std::string(prev.idnt, ' ') << str.str();
	  else
	    prev.push_to_acc(str.str());
	}
      break;
    case SexpPrinter::State::MULTI_LINE:
      o << ")";
      break;
    }
  stack.pop_back();
  if(stack.empty())
    o << std::endl;
}

void SexpPrinter::addComment(const std::string &comment)
{
  if(!stack.empty())
    throw("cannot add comment inside of S-expression");
  o << ";; " << comment << std::endl;
}

void SexpPrinter::lineBreak()
{
  if(!stack.empty())
    throw("cannot add line break inside of S-expression");
  o << std::endl;
}


void SexpPrinter::startList(const std::string &first)
{
  startList();
  printAtom(first);
}

SexpPrinter::SexpPrinter(std::ostream &o, unsigned int margin) :o(o), margin(margin) {}
