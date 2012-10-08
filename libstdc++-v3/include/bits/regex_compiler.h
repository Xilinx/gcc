// class template regex -*- C++ -*-

// Copyright (C) 2010, 2011, 2012 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/**
 *  @file bits/regex_compiler.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{regex}
 */

#include <map>

namespace std _GLIBCXX_VISIBILITY(default)
{
namespace __detail
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   * @addtogroup regex-detail
   * @{
   */

  /// Base class for scanner.
  struct _Scanner_base
  {
    typedef unsigned int _StateT;

    static constexpr _StateT _S_state_at_start    = 1 << 0;
    static constexpr _StateT _S_state_in_brace    = 1 << 2;
    static constexpr _StateT _S_state_in_bracket  = 1 << 3;

    virtual ~_Scanner_base() { };
  };

  /**
   * @brief struct _Scanner. Scans an input range for regex tokens.
   *
   * The %_Scanner class interprets the regular expression pattern in
   * the input range passed to its constructor as a sequence of parse
   * tokens passed to the regular expression compiler.  The sequence
   * of tokens provided depends on the flag settings passed to the
   * constructor: different regular expression grammars will interpret
   * the same input pattern in syntactically different ways.
   */
  template<typename _InputIterator>
  class _Scanner: public _Scanner_base
  {
  public:
    typedef _InputIterator                                        _IteratorT;

    /**
     * @brief The character container type used in the
     * implementation of template
     */
    typedef typename std::iterator_traits<_IteratorT>::value_type _CharT;

    typedef std::basic_string<_CharT> _StringT;

    typedef regex_constants::syntax_option_type _FlagT;
    typedef const std::ctype<_CharT> _CtypeT;

    /// Token types returned from the scanner.
    enum _TokenT
      {
	_S_token_anychar,
	_S_token_backref,
	_S_token_bracket_begin,
	_S_token_bracket_end,
	_S_token_inverse_class,
	_S_token_char_class_name,
	_S_token_closure0,
	_S_token_closure1,
	_S_token_collelem_multi,
	_S_token_collelem_single,
	_S_token_collsymbol,
	_S_token_comma,
	_S_token_dash,
	_S_token_dup_count,
	_S_token_eof,
	_S_token_equiv_class_name,
	_S_token_interval_begin,
	_S_token_interval_end,
	_S_token_line_begin,
	_S_token_line_end,
	_S_token_opt,
	_S_token_or,
	_S_token_ord_char,
	_S_token_quoted_char,
	_S_token_subexpr_begin,
	_S_token_subexpr_end,
	_S_token_word_begin,
	_S_token_word_end,
	_S_token_unknown
      };

    /// Char classes acceptable by scanner.
    enum _CharClassT
      {
	_S_class_alpha,
	_S_class_ascii,
	_S_class_blank,
	_S_class_cntrl,
	_S_class_digit,
	_S_class_graph,
	_S_class_lower,
	_S_class_print,
	_S_class_punct,
	_S_class_space,
	_S_class_upper,
	_S_class_word,
	_S_class_alnum,
	_S_class_xdigit,
	_S_class_unknown
      };

    typedef std::map<_StringT, _CharClassT> _MapT;
    typedef std::pair<std::string, _CharClassT> _ClassPairT;

  public:
    _Scanner(_IteratorT __begin, _IteratorT __end, _FlagT __flags,
	     std::locale __loc)
      : _M_begin(__begin), _M_current(__begin) , _M_end(__end) ,
	_M_flags(__flags),
	_M_ctype(std::use_facet<_CtypeT>(__loc)), _M_state(_S_state_at_start)
    {
      _M_dot = _M_ctype.widen('.');
      _M_star = _M_ctype.widen('*');
      _M_plus = _M_ctype.widen('+');
      _M_or = _M_ctype.widen('|');
      _M_bracket_beg = _M_ctype.widen('[');
      _M_bracket_end = _M_ctype.widen(']');
      _M_esc = _M_ctype.widen('\\');
      _M_sub_beg = _M_ctype.widen('(');
      _M_sub_end = _M_ctype.widen(')');
      _M_brace_beg = _M_ctype.widen('{');
      _M_brace_end = _M_ctype.widen('}');
      _M_comma = _M_ctype.widen(',');
      _M_carret = _M_ctype.widen('^');
      _M_dash = _M_ctype.widen('-');
      _M_eof = _M_ctype.widen('x');
      _M_eol = _M_ctype.widen('$');
      _M_colon = _M_ctype.widen(':');
      _M_equals = _M_ctype.widen('=');

      // init of names of char classes.
      std::vector<_ClassPairT > _names;
      _names.push_back(_ClassPairT("alnum", _S_class_alnum));
      _names.push_back(_ClassPairT("alpha", _S_class_alpha));
      _names.push_back(_ClassPairT("ascii", _S_class_ascii));
      _names.push_back(_ClassPairT("blank", _S_class_blank));
      _names.push_back(_ClassPairT("digit", _S_class_digit));
      _names.push_back(_ClassPairT("graph", _S_class_graph));
      _names.push_back(_ClassPairT("lower", _S_class_lower));
      _names.push_back(_ClassPairT("print", _S_class_print));
      _names.push_back(_ClassPairT("punct", _S_class_punct));
      _names.push_back(_ClassPairT("space", _S_class_space));
      _names.push_back(_ClassPairT("upper", _S_class_upper));
      _names.push_back(_ClassPairT("word", _S_class_word));
      _names.push_back(_ClassPairT("xdigit", _S_class_xdigit));

      for (typename std::vector<_ClassPairT>::iterator _str = _names.begin();
	   _str != _names.end();
	   _str++)
	{
	  _StringT _current;
	  for (std::string::iterator _ch = _str->first.begin();
	       _ch != _str->first.end();
	       _ch++)
	    _current += _M_ctype.widen(*_ch);

	  _M_classnames[_current] = _str->second;
	}


      _M_advance();
    }

    void
    _M_advance();

    _TokenT
    _M_token() const
    { return _M_curToken; }

    _CharClassT
    _M_charclass (const _StringT& __classname) const;

    const _StringT&
    _M_value() const
    { return _M_curValue; }

#ifdef _GLIBCXX_DEBUG
    std::ostream&
    _M_print(std::ostream&);
#endif

  private:
    void
    _M_eat_escape();

    void
    _M_scan_in_brace();

    void
    _M_scan_in_bracket();

    void
    _M_eat_charclass();

    void
    _M_eat_equivclass();

    void
    _M_eat_collsymbol();

    _CharT      _M_dot;
    _CharT      _M_star;
    _CharT      _M_plus;
    _CharT      _M_or;
    _CharT      _M_bracket_beg;
    _CharT      _M_bracket_end;
    _CharT      _M_esc;
    _CharT      _M_sub_beg;
    _CharT      _M_sub_end;
    _CharT      _M_brace_beg;
    _CharT      _M_brace_end;
    _CharT      _M_carret;
    _CharT      _M_comma;
    _CharT      _M_dash;
    _CharT      _M_eof;
    _CharT      _M_eol;
    _CharT      _M_colon;
    _CharT      _M_equals;
    // char classes names.
    _MapT _M_classnames;
    _IteratorT  _M_current;
    _IteratorT  _M_end;
    _IteratorT  _M_begin;
    _FlagT      _M_flags;
    _CtypeT&    _M_ctype;
    _TokenT     _M_curToken;
    _StringT    _M_curValue;
    _StateT     _M_state;
  };

  template<typename _InputIterator>
    typename _Scanner<_InputIterator>::_CharClassT
    _Scanner<_InputIterator>::
    _M_charclass (const _StringT& __classname) const
    {
      typename _MapT::const_iterator __class = _M_classnames.find(__classname);
      return __class == _M_classnames.end()? _S_class_unknown: __class->second;
    }


  template<typename _InputIterator>
    void
    _Scanner<_InputIterator>::
    _M_advance()
    {
      if (_M_current == _M_end)
	{
	  _M_curToken = _S_token_eof;
	  return;
	}

      _CharT __c = *_M_current;
      if (_M_state & _S_state_in_bracket)
	{
	  _M_scan_in_bracket();
	  return;
	}
      if (_M_state & _S_state_in_brace)
	{
	  _M_scan_in_brace();
	  return;
	}
#if 0
      // TODO: re-enable line anchors when _M_assertion is implemented.
      // See PR libstdc++/47724
      else if (_M_state & _S_state_at_start && __c == _M_carret)
	{
	  _M_curToken = _S_token_line_begin;
	  ++_M_current;
	  return;
	}
      else if (__c == _M_dot)
	{
	  _M_curToken = _S_token_line_end;
	  ++_M_current;
	  return;
	}
#endif
      else if (__c == _M_dot)
	{
	  _M_curToken = _S_token_anychar;
	  ++_M_current;
	  return;
	}
      else if (__c == _M_star)
	{
	  if (_M_current == _M_begin)
	    {
	      __throw_regex_error(regex_constants::error_badrepeat);
	      return;
	    }
	  _M_curToken = _S_token_closure0;
	  ++_M_current;
	  return;
	}
      else if (__c == _M_plus)
	{
	  if (_M_current == _M_begin)
	    {
	      __throw_regex_error(regex_constants::error_badrepeat);
	      return;
	    }
	  _M_curToken = _S_token_closure1;
	  ++_M_current;
	  return;
	}
      else if (__c == _M_or)
	{
	  _M_curToken = _S_token_or;
	  ++_M_current;
	  return;
	}
      else if (__c == _M_bracket_beg)
	{
	  _M_curToken = _S_token_bracket_begin;
	  _M_state |= (_S_state_in_bracket | _S_state_at_start);
	  ++_M_current;
	  return;
	}
      else if (__c == _M_esc)
	{
	  _M_eat_escape();
	  return;
	}
      else if (!(_M_flags & (regex_constants::basic | regex_constants::grep)))
	{
	  if (__c == _M_sub_beg)
	    {
	      _M_curToken = _S_token_subexpr_begin;
	      ++_M_current;
	      return;
	    }
	  else if (__c == _M_sub_end)
	    {
	      _M_curToken = _S_token_subexpr_end;
	      ++_M_current;
	      return;
	    }
	  else if (__c == _M_brace_beg)
	    {
	      _M_curToken = _S_token_interval_begin;
	      _M_state |= _S_state_in_brace;
	      ++_M_current;
	      return;
	    }
	  else if (__c == _M_bracket_beg)
	    {
	      if (_M_current == _M_begin)
		{
		  __throw_regex_error(regex_constants::error_brack);
		  return;
		}
	      _M_curToken = _S_token_bracket_begin;
	      _M_state |= (_S_state_in_bracket | _S_state_at_start);
	      ++_M_current;
	      return;
	    }
	}

      _M_curToken = _S_token_ord_char;
      _M_curValue.assign(1, __c);
      ++_M_current;
    }


  template<typename _InputIterator>
    void
    _Scanner<_InputIterator>::
    _M_scan_in_brace()
    {
      if (_M_ctype.is(_CtypeT::digit, *_M_current))
	{
	  _M_curToken = _S_token_dup_count;
	  _M_curValue.assign(1, *_M_current);
	  ++_M_current;
	  while (_M_current != _M_end
		 && _M_ctype.is(_CtypeT::digit, *_M_current))
	    {
	      _M_curValue += *_M_current;
	      ++_M_current;
	    }
	  return;
	}
      else if (*_M_current == _M_comma)
	{
	  _M_curToken = _S_token_comma;
	  ++_M_current;
	  return;
	}
      if (_M_flags & (regex_constants::basic | regex_constants::grep))
	{
	  if (*_M_current == _M_esc)
	    _M_eat_escape();
	}
      else
	{
	  if (*_M_current == _M_brace_end)
	    {
	      _M_curToken = _S_token_interval_end;
	      _M_state &= ~_S_state_in_brace;
	      ++_M_current;
	      return;
	    }
	}
    }

  template<typename _InputIterator>
    void
    _Scanner<_InputIterator>::
    _M_scan_in_bracket()
    {
      if (_M_state & _S_state_at_start && *_M_current == _M_ctype.widen('^'))
	{
	  _M_curToken = _S_token_inverse_class;
	  // XXX
	 if (*(_M_current - 1) == _M_bracket_beg)
	    _M_curValue.assign(1, (*(_M_current++)));
	  else
	    _M_curValue += *(_M_current++);

	  _M_state &= ~_S_state_at_start;
	  _M_scan_in_bracket();
	}
      else if (*_M_current == _M_bracket_beg)
	{
	  ++_M_current;
	  if (*_M_current == *_M_end)
	    {
	      _M_curToken = _S_token_eof;
	      return;
	    }

	  if (*_M_current == _M_dot)
	    {
	      _M_curToken = _S_token_collsymbol;
	      _M_eat_collsymbol();
	      return;
	    }
	  else if (*_M_current == _M_colon)
	    {
	      _M_curToken = _S_token_char_class_name;
	      _M_eat_charclass();
	      return;
	    }
	  else if (*_M_current == _M_equals)
	    {
	      _M_curToken = _S_token_equiv_class_name;
	      _M_eat_equivclass();
	      return;
	    }
	}
      else if (*_M_current == _M_dash)
	{
	  _M_curToken = _S_token_dash;
	   _M_curValue += *(_M_current++);
	  _M_scan_in_bracket ();
	}
      else if (*_M_current == _M_bracket_end)
	{
	  if (!(_M_flags & regex_constants::ECMAScript)
	      || !(_M_state & _S_state_at_start))
	    {
	      // special case: only if  _not_ chr first after
	      // '[' or '[^' and if not ECMAscript
	      _M_curToken = _S_token_bracket_end;
	      ++_M_current;
	      return;
	    }
	}
      _M_curToken = _S_token_collelem_single;
      _M_curValue.assign(1, *_M_current);
      ++_M_current;
    }

  template<typename _InputIterator>
    void
    _Scanner<_InputIterator>::
    _M_eat_escape()
    {
      ++_M_current;
      if (_M_current == _M_end)
	{
	  _M_curToken = _S_token_eof;
	  return;
	}
      _CharT __c = *_M_current;
      ++_M_current;

      if (__c == _M_sub_beg)
	{
	  if (!(_M_flags & (regex_constants::basic | regex_constants::grep)))
	    {
	      _M_curToken = _S_token_ord_char;
	      _M_curValue.assign(1, __c);
	    }
	  else
	    _M_curToken = _S_token_subexpr_begin;
	}
      else if (__c == _M_sub_end)
	{
	  if (!(_M_flags & (regex_constants::basic | regex_constants::grep)))
	    {
	      _M_curToken = _S_token_ord_char;
	      _M_curValue.assign(1, __c);
	    }
	  else
	    _M_curToken = _S_token_subexpr_end;
	}
      else if (__c == _M_brace_beg)
	{
	  if (!(_M_flags & (regex_constants::basic | regex_constants::grep)))
	    {
	      _M_curToken = _S_token_ord_char;
	      _M_curValue.assign(1, __c);
	    }
	  else
	    {
	      _M_curToken = _S_token_interval_begin;
	      _M_state |= _S_state_in_brace;
	    }
	}
      else if (__c == _M_brace_end)
	{
	  if (!(_M_flags & (regex_constants::basic | regex_constants::grep)))
	    {
	      _M_curToken = _S_token_ord_char;
	      _M_curValue.assign(1, __c);
	    }
	  else
	    {
	      if (!(_M_state && _S_state_in_brace))
		__throw_regex_error(regex_constants::error_badbrace);
	      _M_state &= ~_S_state_in_brace;
	      _M_curToken = _S_token_interval_end;
	    }
	}
      else if (__c == _M_eof)
	{
	  ++_M_current;
	  if (_M_current == _M_end)
	    {
	      _M_curToken = _S_token_eof;
	      return;
	    }
	  if (_M_ctype.is(_CtypeT::digit, *_M_current))
	    {
	      _M_curValue.assign(1, *_M_current);
	      ++_M_current;
	      if (_M_current == _M_end)
		{
		  _M_curToken = _S_token_eof;
		  return;
		}
	      if (_M_ctype.is(_CtypeT::digit, *_M_current))
		{
		  _M_curValue += *_M_current;
		  ++_M_current;
		  return;
		}
	    }
	}
      else if (__c == _M_carret
	       || __c == _M_dot
	       || __c == _M_star
	       || __c == _M_eol
	       || __c == _M_esc)
	{
	  _M_curToken = _S_token_ord_char;
	  _M_curValue.assign(1, __c);
	}
      else if (_M_ctype.is(_CtypeT::digit, __c))
	{
	  _M_curToken = _S_token_backref;
	  _M_curValue.assign(1, __c);
	}
      else if (__c == _M_bracket_beg)
	{
	  if (!(_M_flags & (regex_constants::basic | regex_constants::grep)))
	    {
	      _M_curToken = _S_token_ord_char;
	      _M_curValue.assign(1, __c);
	    }
	  else
	    {
	      _M_curToken = _S_token_bracket_begin;
	      _M_state |= _S_state_in_brace;
	    }
	}
      else if (__c == _M_bracket_end)
	{
	  if (!(_M_flags & (regex_constants::basic | regex_constants::grep)))
	    {
	      _M_curToken = _S_token_ord_char;
	      _M_curValue.assign(1, __c);
	    }
	  else
	    {
	      if (!(_M_state && _S_state_in_brace))
		__throw_regex_error(regex_constants::error_badbrace);
	      _M_state &= ~_S_state_in_brace;
	      _M_curToken = _S_token_bracket_end;
	    }
	}
      else
	__throw_regex_error(regex_constants::error_escape);
    }


  // Eats a character class or throwns an exception.
  // current point to ':' delimiter on entry, char after ']' on return
  template<typename _InputIterator>
    void
    _Scanner<_InputIterator>::
    _M_eat_charclass()
    {
      typedef std::basic_string<_CharT> _StringT;

      if (_M_current == _M_end)
	__throw_regex_error(regex_constants::error_ctype);

      // skip ':'
      _M_curValue += *(_M_current++);

      _StringT _classname;
      for (;
	   _M_current != _M_end && *_M_current != _M_colon;
	   ++_M_current)
	_classname += *_M_current;
      if (_M_classnames.find(_classname) == _M_classnames.end())
	__throw_regex_error(regex_constants::error_ctype);
      if (_M_current == _M_end)
	__throw_regex_error(regex_constants::error_ctype);
      // skip ':'
      _M_curValue += _classname;
      _M_curValue += *(_M_current++);
      //if (*_M_current != _M_bracket_end)
      // __throw_regex_error(regex_constants::error_ctype);
      //++_M_current;  skip ']'
    }


  template<typename _InputIterator>
    void
    _Scanner<_InputIterator>::
    _M_eat_equivclass()
    {
      ++_M_current; // skip '='
      if (_M_current == _M_end)
	__throw_regex_error(regex_constants::error_collate);
      for (_M_curValue.clear();
	   _M_current != _M_end && *_M_current != _M_equals;
	   ++_M_current)
	_M_curValue += *_M_current;
      if (_M_current == _M_end)
	__throw_regex_error(regex_constants::error_collate);
      ++_M_current; // skip '='
      if (*_M_current != _M_bracket_end)
	__throw_regex_error(regex_constants::error_collate);
      ++_M_current; // skip ']'
    }


  template<typename _InputIterator>
    void
    _Scanner<_InputIterator>::
    _M_eat_collsymbol()
    {
      ++_M_current; // skip '.'
      if (_M_current == _M_end)
	__throw_regex_error(regex_constants::error_collate);
      for (_M_curValue.clear();
	   _M_current != _M_end && *_M_current != _M_dot;
	   ++_M_current)
	_M_curValue += *_M_current;
      if (_M_current == _M_end)
	__throw_regex_error(regex_constants::error_collate);
      ++_M_current; // skip '.'
      if (*_M_current != _M_bracket_end)
	__throw_regex_error(regex_constants::error_collate);
      ++_M_current; // skip ']'
    }

#ifdef _GLIBCXX_DEBUG
  template<typename _InputIterator>
    std::ostream&
    _Scanner<_InputIterator>::
    _M_print(std::ostream& ostr)
    {
      ostr << "Symbol " << *_M_current <<"\n";
      ostr << "Token:\n";
      switch (_M_curToken)
      {
	case _S_token_anychar:
	  ostr << "any-character\n";
	  break;
	case _S_token_backref:
	  ostr << "backref\n";
	  break;
	case _S_token_bracket_begin:
	  ostr << "bracket-begin\n";
	  break;
	case _S_token_bracket_end:
	  ostr << "bracket-end\n";
	  break;
	case _S_token_char_class_name:
	  ostr << "char-class-name \"" << _M_curValue << "\"\n";
	  break;
	case _S_token_closure0:
	  ostr << "closure0\n";
	  break;
	case _S_token_closure1:
	  ostr << "closure1\n";
	  break;
	case _S_token_collelem_multi:
	  ostr << "coll-elem-multi \"" << _M_curValue << "\"\n";
	  break;
	case _S_token_collelem_single:
	  ostr << "coll-elem-single \"" << _M_curValue << "\"\n";
	  break;
	case _S_token_collsymbol:
	  ostr << "collsymbol \"" << _M_curValue << "\"\n";
	  break;
	case _S_token_comma:
	  ostr << "comma\n";
	  break;
	case _S_token_dash:
	  ostr << "dash\n";
	  break;
	case _S_token_dup_count:
	  ostr << "dup count: " << _M_curValue << "\n";
	  break;
	case _S_token_eof:
	  ostr << "EOF\n";
	  break;
	case _S_token_equiv_class_name:
	  ostr << "equiv-class-name \"" << _M_curValue << "\"\n";
	  break;
	case _S_token_interval_begin:
	  ostr << "interval begin\n";
	  break;
	case _S_token_interval_end:
	  ostr << "interval end\n";
	  break;
	case _S_token_line_begin:
	  ostr << "line begin\n";
	  break;
	case _S_token_line_end:
	  ostr << "line end\n";
	  break;
	case _S_token_opt:
	  ostr << "opt\n";
	  break;
	case _S_token_or:
	  ostr << "or\n";
	  break;
	case _S_token_ord_char:
	  ostr << "ordinary character: \"" << _M_value() << "\"\n";
	  break;
	case _S_token_quoted_char:
	  ostr << "quoted char\n";
	  break;
	case _S_token_subexpr_begin:
	  ostr << "subexpr begin\n";
	  break;
	case _S_token_subexpr_end:
	  ostr << "subexpr end\n";
	  break;
	case _S_token_word_begin:
	  ostr << "word begin\n";
	  break;
	case _S_token_word_end:
	  ostr << "word end\n";
	  break;
	case _S_token_unknown:
	  ostr << "-- unknown token --\n";
	  break;
      }
      return ostr;
    }
#endif

  /// Builds an NFA from an input iterator interval.
  template<typename _InIter, typename _TraitsT>
    class _Compiler
    {
    public:
      typedef _InIter                                            _IterT;
      typedef typename std::iterator_traits<_InIter>::value_type _CharT;
      typedef std::basic_string<_CharT>                          _StringT;
      typedef regex_constants::syntax_option_type                _FlagT;
      typedef _IntervalMatcher<_InIter, _TraitsT>                _IMatcherT;

      _Compiler(const _InIter& __b, const _InIter& __e,
		_TraitsT& __traits, _FlagT __flags);

      const _Nfa&
      _M_nfa() const
      { return _M_state_store; }

#ifdef _GLIBCXX_DEBUG
      std::ostream&
      _M_print(std::ostream&);
#endif

    private:
      typedef _Scanner<_InIter>                              _ScannerT;
      typedef typename _ScannerT::_TokenT                    _TokenT;
      typedef std::stack<_StateSeq, std::vector<_StateSeq> > _StackT;
      typedef const std::ctype<_CharT>                       _CtypeT;
      typedef _RangeMatcher<_InIter, _TraitsT>               _RMatcherT;

      // accepts a specific token or returns false.
      bool
      _M_match_token(_TokenT __token);

      void
      _M_disjunction();

      bool
      _M_alternative();

      bool
      _M_term();

      bool
      _M_assertion();

      bool
      _M_quantifier();

      bool
      _M_atom();

      bool
      _M_bracket_expression();

      bool
      _M_bracket_list(_RMatcherT& __matcher);

      bool
      _M_follow_list(_RMatcherT& __matcher);

      bool
      _M_follow_list2(_RMatcherT& __matcher);

      bool
      _M_expression_term(_RMatcherT& __matcher);

      bool
      _M_range_expression(_RMatcherT& __matcher);

      bool
      _M_start_range(_RMatcherT& __matcher);

      bool
      _M_collating_symbol(_RMatcherT& __matcher);

      bool
      _M_equivalence_class(_RMatcherT& __matcher);

      bool
      _M_character_class(_RMatcherT& __matcher);

      int
      _M_cur_int_value(int __radix);

      _TraitsT&      _M_traits;
      _ScannerT      _M_scanner;
      _StringT       _M_cur_value;
      _Nfa           _M_state_store;
      _StackT        _M_stack;
      _CharT         _M_esc;
      _CharT         _M_carret;
      _CharT         _M_dash;
    };

  template<typename _InIter, typename _TraitsT>
    _Compiler<_InIter, _TraitsT>::
    _Compiler(const _InIter& __b, const _InIter& __e, _TraitsT& __traits,
	      _Compiler<_InIter, _TraitsT>::_FlagT __flags)
    : _M_traits(__traits), _M_scanner(__b, __e, __flags, _M_traits.getloc()),
      _M_state_store(__flags)
    {
      typedef _StartTagger<_InIter, _TraitsT> _Start;
      typedef _EndTagger<_InIter, _TraitsT> _End;

      _CtypeT& ctype(std::use_facet<_CtypeT>(_M_traits.getloc()));

      _M_esc = ctype.widen('\\');
      _M_carret = ctype.widen('^');
      _M_dash = ctype.widen('-');

      _StateSeq __r(_M_state_store,
		    _M_state_store._M_insert_subexpr_begin(_Start(0)));
      _M_disjunction();
      if (!_M_stack.empty())
	{
	  __r._M_append(_M_stack.top());
	  _M_stack.pop();
	}
      __r._M_append(_M_state_store._M_insert_subexpr_end(0, _End(0)));
      __r._M_append(_M_state_store._M_insert_accept());
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_match_token(_Compiler<_InIter, _TraitsT>::_TokenT token)
    {
      if (token == _M_scanner._M_token())
	{
	  _M_scanner._M_advance();
	  _M_cur_value = _M_scanner._M_value();
	  return true;
	}
      return false;
    }

  template<typename _InIter, typename _TraitsT>
    void
    _Compiler<_InIter, _TraitsT>::
    _M_disjunction()
    {
      this->_M_alternative();
      if (_M_match_token(_ScannerT::_S_token_or))
	{
	  _StateSeq __alt1 = _M_stack.top(); _M_stack.pop();
	  this->_M_disjunction();
	  _StateSeq __alt2 = _M_stack.top(); _M_stack.pop();
	  _M_stack.push(_StateSeq(__alt1, __alt2));
	}
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_alternative()
    {
      if (this->_M_term())
	{
	  _StateSeq __re = _M_stack.top(); _M_stack.pop();
	  this->_M_alternative();
	  if (!_M_stack.empty())
	    {
	      __re._M_append(_M_stack.top());
	      _M_stack.pop();
	    }
	  _M_stack.push(__re);
	  return true;
	}
      return false;
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_term()
    {
      if (this->_M_assertion())
	return true;
      if (this->_M_atom())
	{
	  this->_M_quantifier();
	  return true;
	}
      return false;
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_assertion()
    {
      if (_M_match_token(_ScannerT::_S_token_line_begin))
	{
	  // __m.push(_Matcher::_S_opcode_line_begin);
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_line_end))
	{
	  // __m.push(_Matcher::_S_opcode_line_end);
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_word_begin))
	{
	  // __m.push(_Matcher::_S_opcode_word_begin);
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_word_end))
	{
	  // __m.push(_Matcher::_S_opcode_word_end);
	  return true;
	}
      return false;
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_quantifier()
    {
      if (_M_match_token(_ScannerT::_S_token_closure0))
	{
	  if (_M_stack.empty())
	    __throw_regex_error(regex_constants::error_badrepeat);
	  _StateSeq __r(_M_stack.top(), -1);
	  __r._M_append(__r._M_front());
	  _M_stack.pop();
	  _M_stack.push(__r);
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_closure1))
	{
	  if (_M_stack.empty())
	    __throw_regex_error(regex_constants::error_badrepeat);
	  _StateSeq __r(_M_state_store,
			_M_state_store.
			_M_insert_alt(_S_invalid_state_id,
				      _M_stack.top()._M_front()));
	  _M_stack.top()._M_append(__r);
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_opt))
	{
	  if (_M_stack.empty())
	  __throw_regex_error(regex_constants::error_badrepeat);
	  _StateSeq __r(_M_stack.top(), -1);
	  _M_stack.pop();
	  _M_stack.push(__r);
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_interval_begin))
	{
	  if (_M_stack.empty())
	    __throw_regex_error(regex_constants::error_badrepeat);
	  if (!_M_match_token(_ScannerT::_S_token_dup_count))
	    __throw_regex_error(regex_constants::error_badbrace);
	  _StateSeq __r(_M_stack.top());
	  int __min_rep = _M_cur_int_value(10);
	  for (int __i = 1; __i < __min_rep; ++__i)
	    _M_stack.top()._M_append(__r._M_clone());
	  if (_M_match_token(_ScannerT::_S_token_comma))
	    if (_M_match_token(_ScannerT::_S_token_dup_count))
	      {
		int __n = _M_cur_int_value(10) - __min_rep;
		if (__n < 0)
		  __throw_regex_error(regex_constants::error_badbrace);
		for (int __i = 0; __i < __n; ++__i)
		  {
		    _StateSeq __r(_M_state_store,
				  _M_state_store.
				  _M_insert_alt(_S_invalid_state_id,
						_M_stack.top()._M_front()));
		    _M_stack.top()._M_append(__r);
		  }
	      }
	    else
	      {
		_StateSeq __r(_M_stack.top(), -1);
		__r._M_push_back(__r._M_front());
		_M_stack.pop();
		_M_stack.push(__r);
	      }
	  if (!_M_match_token(_ScannerT::_S_token_interval_end))
	    __throw_regex_error(regex_constants::error_brace);
	  return true;
	}
      return false;
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_atom()
    {
      typedef _CharMatcher<_InIter, _TraitsT> _CMatcher;
      typedef _StartTagger<_InIter, _TraitsT> _Start;
      typedef _EndTagger<_InIter, _TraitsT> _End;

      if (_M_match_token(_ScannerT::_S_token_anychar))
	{
	  _M_stack.push(_StateSeq(_M_state_store,
				  _M_state_store._M_insert_matcher
				  (_AnyMatcher)));
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_ord_char))
	{
	  _M_stack.push(_StateSeq(_M_state_store,
				  _M_state_store._M_insert_matcher
				  (_CMatcher(_M_cur_value[0], _M_traits))));
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_quoted_char))
	{
	  // note that in the ECMA grammar, this case covers backrefs.
	  _M_stack.push(_StateSeq(_M_state_store,
				  _M_state_store._M_insert_matcher
				  (_CMatcher(_M_cur_value[0], _M_traits))));
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_backref))
	{
	  // __m.push(_Matcher::_S_opcode_ordchar, _M_cur_value);
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_subexpr_begin))
	{
	  int __mark = _M_state_store._M_sub_count();
	  _StateSeq __r(_M_state_store,
			_M_state_store.
			_M_insert_subexpr_begin(_Start(__mark)));
	  this->_M_disjunction();
	  if (!_M_match_token(_ScannerT::_S_token_subexpr_end))
	    __throw_regex_error(regex_constants::error_paren);
	  if (!_M_stack.empty())
	    {
	      __r._M_append(_M_stack.top());
	      _M_stack.pop();
	    }
	  __r._M_append(_M_state_store._M_insert_subexpr_end
			(__mark, _End(__mark)));
	  _M_stack.push(__r);
	  return true;
	}
      return _M_bracket_expression();
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_bracket_expression()
    {
      typedef typename _TraitsT::char_type char_type;
      typedef std::pair<char_type, char_type> _M_PairT;
      typedef std::vector<_TokenFactory<_TraitsT> > _M_TokenListT;
      typedef _TokenFactory<_TraitsT> _TokFactory;

      if (_M_match_token(_ScannerT::_S_token_bracket_begin))
	{

	  const _M_PairT cp(_M_cur_value[0], _M_cur_value[1]);
	  _M_TokenListT __ml;

	  bool negation = false;
	  for (int i = 0; i< _M_cur_value.length(); i++)
	    {
	      if (_M_cur_value[i] == _M_carret)
		negation = true;
	      else if (_M_cur_value[i] == _M_dash)
		__ml.push_back(_TokFactory(_M_cur_value[i-1], _M_cur_value[++i], negation));
	      else if ((_M_cur_value[i + 1] == _M_dash)
		  || (_M_cur_value[i + 1] == _M_carret))
		__ml.push_back(_TokFactory(_M_cur_value[i], negation));
	    }
	  _IMatcherT __matcher(__ml, _M_traits);

	  // if (!(_M_bracket_list(__matcher)))
	  // __throw_regex_error(regex_constants::error_brack);

	  _M_stack.push(_StateSeq(_M_state_store,
				  _M_state_store._M_insert_matcher(__matcher)));
	  return true;
	}
      return false;
    }

  // If the dash is the last character in the bracket expression, it is not
  // special.
  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_bracket_list(_RMatcherT& __matcher)
    {
      if (_M_follow_list(__matcher))
	{
	  if (_M_match_token(_ScannerT::_S_token_dash))
	    __matcher._M_add_char(_M_cur_value[0]);
	  return true;
	}
      return false;
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_follow_list(_RMatcherT& __matcher)
    { return _M_expression_term(__matcher) && _M_follow_list2(__matcher); }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_follow_list2(_RMatcherT& __matcher)
    {
      if (_M_expression_term(__matcher))
	return _M_follow_list2(__matcher);
      return true;
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_expression_term(_RMatcherT& __matcher)
    {
      return (_M_collating_symbol(__matcher)
	      || _M_character_class(__matcher)
	      || _M_equivalence_class(__matcher)
	      || (_M_start_range(__matcher)
		  && _M_range_expression(__matcher)));
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_range_expression(_RMatcherT& __matcher)
    {
      if (!_M_collating_symbol(__matcher))
	if (!_M_match_token(_ScannerT::_S_token_dash))
	  __throw_regex_error(regex_constants::error_range);
      __matcher._M_make_range();
      return true;
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_start_range(_RMatcherT& __matcher)
    { return _M_match_token(_ScannerT::_S_token_dash); }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_collating_symbol(_RMatcherT& __matcher)
    {
      if (_M_match_token(_ScannerT::_S_token_collelem_single))
	{
	  __matcher._M_add_char(_M_cur_value[0]);
	  return true;
	}
      if (_M_match_token(_ScannerT::_S_token_collsymbol))
	{
	  __matcher._M_add_collating_element(_M_cur_value);
	  return true;
	}
      return false;
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_equivalence_class(_RMatcherT& __matcher)
    {
      if (_M_match_token(_ScannerT::_S_token_equiv_class_name))
	{
	  __matcher._M_add_equivalence_class(_M_cur_value);
	  return true;
	}
      return false;
    }

  template<typename _InIter, typename _TraitsT>
    bool
    _Compiler<_InIter, _TraitsT>::
    _M_character_class(_RMatcherT& __matcher)
    {
      if (_M_match_token(_ScannerT::_S_token_char_class_name))
	{
	  __matcher._M_add_character_class(_M_cur_value);
	  return true;
	}
      return false;
    }

  template<typename _InIter, typename _TraitsT>
    int
    _Compiler<_InIter, _TraitsT>::
    _M_cur_int_value(int __radix)
    {
      int __v = 0;
      for (typename _StringT::size_type __i = 0;
	   __i < _M_cur_value.length(); ++__i)
	__v =__v * __radix + _M_traits.value(_M_cur_value[__i], __radix);
      return __v;
    }

#ifdef _GLIBCXX_DEBUG
  template<typename _InIter, typename _TraitsT>
    std::ostream&
    _Compiler<_InIter, _TraitsT>::
    _M_print(std::ostream& ostr)
    {
      ostream << "Top of _Compiler::_M_stack: " << _M_stack.top(); << "\n";
      ostream << "_M_cur_value: " << _M_cur_value << "\n";
      ostrean << "_M_stack.size: " << _M_stack.size() << '\n';
    }
#endif

  template<typename _InIter, typename _TraitsT>
    _AutomatonPtr
    __compile(const _InIter& __b, const _InIter& __e, _TraitsT& __t,
	      regex_constants::syntax_option_type __f)
    { return _AutomatonPtr(new _Nfa(_Compiler<_InIter, _TraitsT>(__b, __e, __t,
					__f)._M_nfa())); }

 //@} regex-detail
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace __detail
} // namespace std
