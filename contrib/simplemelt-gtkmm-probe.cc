
/** Simple MELT GTKMM probe example (with GTKMM, GTK3,
    GTKSOURCEVIEWMM3, ...)
   Copyright (C) 2012 Free Software Foundation, Inc.
   Contributed by Basile Starynkevitch <basile@starynkevitch.net>


This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */



/****
This standalone program is a simple compiler probe client - it is a
single C++ source file using gtksourceview & gtk; some of the code is
taken or inspired by the contrib/simple-probe.c file of MELT branch
svn revision 147544 from mid-may 2009; it is not compiled by the GCC
building process. The compilation command is given near the end of
file (as a local.var to emacs)

This probe is a GTK graphical application exchanging with the MELT
plugin using textual protocols. Since mid-march 2012, ie svn revision
185407 [approximately], MELT has the ability to accept asynchronous
messages (by handling SIGIO appropriately, delayed to safe points
where meltgc_poll_inputs may become called by meltgc_handle_sigio &
melt_handle_interrupt thru MELT_CHECK_INTERRUPT), which would be sent
when something happens to be readable by MELT on a file descriptor
like a pipe or a socket).  Notice however that these messages are
handled only by MELT; when GCC is recieving them outside of the MELT
plugin, nothing will happen till the MELT runtime is re-entered
(e.g. at start of any pass, thru a PLUGIN_PASS_EXECUTION hook, or
inside any MELT registered pass or other MELT handling of plugin
events).

Exchanges between the probe and MELT are "asynchronous" and
textual. Each textual message is ended by two consecutive newlines
that is \n\n in C parlance.

A message from probe to MELT is a request. It is syntactically a
sequence of MELT s-expressions which is passed to the channel handler
inside MELT (see CLASS_INPUT_CHANNEL_HANDLER). It should be ended by
two consecutive newlines.

A message from MELT to probe is a command. It uses a syntax which is
lexically a subset of MELT syntax (or lexical)
conventions. Concretely, it should start by a command symbol and
contain symbols known by this probe, strings (encoded in a C-like way,
like MELT does) and numbers, and sub-lists à la MELT. It also should
be ended by two consecutive newlines. Conventionally probe command
symbol (verbs) end with _pcd and are internally converted to uppercase
(so their case does not matter).

Requests and commands are fully asynchronous, and does not correspond
one to one.  MELT may send many commands to the probe, and this probe
may also send several requests to MELT at any moment.

****/


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#include <gtkmm.h>
#include <gtksourceviewmm.h>
#include <utility>
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <map>
#include <cstdio>
#include <cassert>
#include <cctype>


/* from  /usr/share/xemacs21/xemacs-packages/etc/smilies/indifferent.xpm */
const char *const smelt_indifferent_13x14_xpm[] = {
  "13 14 3 1",
  "       c None",
  ".      c #000000",
  "+      c #FFDD00",
  "   .......   ",
  "  ..+++++..  ",
  " .+++++++++. ",
  ".+++++++++++.",
  ".++..+++..++.",
  ".++..+++..++.",
  ".+++++++++++.",
  ".+++++++++++.",
  ".+++++++++++.",
  ".++.......++.",
  ".+++++++++++.",
  " .+++++++++. ",
  "  ..+++++..  ",
  "   .......   "
};

/* from /usr/lib/sourcenav/share/bitmaps/key.xpm */
/* XPM */
const static char *const smelt_key_7x11_xpm[] = {
  /* width height num_colors chars_per_pixel */
  "7 11 3 1",
  /* colors */
  " 	c None",
  ".	c black",
  "X	c #fefe00",
  /* pixels */
  " ..... ",
  ".XXXXX.",
  ".XX.XX.",
  ".XXXXX.",
  " ..XX. ",
  "  .X.  ",
  "  .XX. ",
  "  .X.  ",
  "  .XX. ",
  "  .X.  ",
  "   .   "
};

// from /usr/share/emacs/23.3/etc/images/ezimage/key.xpm
/* XPM */
static const char *const smelt_key_16x16_xpm[] = {
  "16 16 4 1",
  " 	c None",
  ".	c #828282",
  "+	c #000000",
  "@	c #FFF993",
  "    ........    ",
  "   ..++++++..   ",
  "   .+@@@@@@+.   ",
  "   .+@@++@@+.   ",
  "   .+@@@@@@+.   ",
  "   .+@@@@@@+.   ",
  "   .+@@@@@@+.   ",
  "    .+@@@@+.    ",
  "     .+@@+.     ",
  "     .+@@@+.    ",
  "     .+@@+.     ",
  "     .+@@+.     ",
  "     .+@@@+.    ",
  "     .+@@+.     ",
  "      .++.      ",
  "       ..       "
};

#define SMELT_FATAL(C) do { int er = errno;	\
  std::clog << __FILE__ << ":" << __LINE__	\
	    << "@" << __func__ << " " << C;	\
  if (er) std::clog << " ~" << strerror(er) ;	\
  std::clog << std::endl;			\
  abort();					\
} while(0)

bool smelt_debugging;

#define SMELT_DEBUG(C) do { if (smelt_debugging)	\
std::clog <<  __FILE__ << ":" << __LINE__		\
<< "@" << __func__ << " " << C << std::endl; } while(0)


std::string smelt_long_to_string(long l)
{
  char buf[32];
  snprintf(buf, sizeof(buf), "%ld", l);
  return std::string(buf);
}

class SmeltParseErrorAt : public std::runtime_error {
public:
  SmeltParseErrorAt(const std::string& what, const char* file, int lineno, std::string str, int pos):
    std::runtime_error ("SMELT parse error:" +
                        what + " (" + file + ":" + smelt_long_to_string(lineno)
                        + ") @:" + str.substr(pos)) {
    SMELT_DEBUG("parse error " << what << " @" << file << ":" << lineno
                << " :: " << str << " @@" << pos);
  };
  SmeltParseErrorAt(const char*what, const char* file, int lineno, std::string str, int pos) :
    std::runtime_error ("SMELT parse error:" +
                        std::string(what) + " (" + file + ":" + smelt_long_to_string(lineno)
                        + ") @:" + str.substr(pos)) {
    SMELT_DEBUG("parse error " << what << " @" << file << ":" << lineno
                << " :: " << str << " @@" << pos);
  };
  ~SmeltParseErrorAt() throw () {};
};
#define smelt_parse_error(What,Str,Pos) SmeltParseErrorAt((What),__FILE__,__LINE__, \
  (Str),(Pos))

class SmeltDomainErrorAt : public std::domain_error {
public:
  SmeltDomainErrorAt(const std::string& what, const char*file, int lineno, std::string more)
    : std::domain_error(std::string("SMELT domain error: ")
                        + std::string(what) + " (" + file + ":" + smelt_long_to_string(lineno)
                        + "):" + more) {
    SMELT_DEBUG("domain error " << what << " @" << file << ":" << lineno
                << " :: " << more);
  };
  SmeltDomainErrorAt(const char* what, const char*file, int lineno, std::string more)
    : std::domain_error(std::string("SMELT domain error: ")
                        + std::string(what) + " (" + file + ":" + smelt_long_to_string(lineno)
                        + "):" + more) {
    SMELT_DEBUG("domain error " << what << " @" << file << ":" << lineno
                << " :: " << more);
  };
  ~SmeltDomainErrorAt() throw () {};
};
#define smelt_domain_error(What,More) SmeltDomainErrorAt((What),__FILE__,__LINE__, \
	  (More))




static Glib::OptionContext smelt_options_context(" - a simple MELT Gtk probe");


/* The SmeltMainWindow is our graphical interface; allmost all GUI
   code is inside. */
class SmeltMainWindow : public Gtk::Window {
  Gtk::VBox _mainvbox;
  Gtk::MenuBar _mainmenubar;
  Gtk::Label _mainlabel;
  Gtk::Notebook _mainnotebook;
  class ShownFile {
    friend class SmeltMainWindow;
    long _sfilnum;		// unique number in _mainsfilemapnum;
    int _sfilnblines;		// number of lines
    std::string _sfilname;	// file path
    Gsv::View _sfilview;	// source view
  public:
    long number () const {
      return _sfilnum;
    };
    int nblines () const {
      return _sfilnblines;
    };
    const std::string & name() const {
      return _sfilname;
    };
    Gsv::View& view() {
      return _sfilview;
    };
    ShownFile(SmeltMainWindow*,const std::string&filepath,long num);
    ShownFile() =delete;
    ShownFile(const ShownFile&) =delete;
    ~ShownFile();
    Glib::RefPtr<Gsv::Gutter> left_gutter() {
      return _sfilview.get_gutter(Gtk::TEXT_WINDOW_LEFT);
    }
  };				// end internal class ShownFile
  ////
  static std::map<long,ShownFile*> mainsfilemapnum_;
  static std::map<std::string,ShownFile*> mainsfiledict_;
  static ShownFile* shown_file_by_path(const std::string& filepath) {
    if (filepath.empty()) return nullptr;
    auto itsfil = mainsfiledict_.find(filepath);
    if (itsfil == mainsfiledict_.end()) return nullptr;
    return itsfil->second;
  }
  static ShownFile* shown_file_by_number (long l) {
    if (l == 0) return nullptr;
    auto itsfil = mainsfilemapnum_.find(l);
    if (itsfil == mainsfilemapnum_.end()) return nullptr;
    return itsfil->second;
  }
  ////
public:
  SmeltMainWindow() :
    Gtk::Window() {
    set_border_width(5);
    add(_mainvbox);
    Glib::ustring labmarkstr = Glib::ustring::compose
                               ("<big><span color='darkred'>Simple Gcc Melt gtkmm-probe</span></big>\n"
                                "<small>pid %1 on <tt>%2</tt></small>",
                                (int)(getpid()), g_get_host_name());
    _mainlabel.set_markup(labmarkstr);
    _mainlabel.set_justify(Gtk::JUSTIFY_CENTER);
    _mainvbox.pack_start(_mainmenubar,Gtk::PACK_SHRINK);
    _mainvbox.pack_start(_mainlabel,Gtk::PACK_SHRINK);
    _mainvbox.pack_start(_mainnotebook,Gtk::PACK_EXPAND_WIDGET);
    show_all();
  }
  virtual ~SmeltMainWindow() {
  }
  virtual bool on_delete_event(GdkEventAny*ev) {
    // return false to accept the delete event, true to reject it..
    return Gtk::Window::on_delete_event(ev);
  }
  void show_file(const std::string&path, long num);
  void mark_location(long marknum,long filenum,int lineno, int col);
};


std::map<long,SmeltMainWindow::ShownFile*> SmeltMainWindow::mainsfilemapnum_;
std::map<std::string,SmeltMainWindow::ShownFile*> SmeltMainWindow::mainsfiledict_;

class SmeltTraceWindow : public Gtk::Window {
  Gtk::VBox _tracevbox;
  Glib::RefPtr<Gtk::ActionGroup> _traceactgroup;
  Glib::RefPtr<Gtk::CheckMenuItem> _traceitemcheck;
  Gtk::Label _tracelabel;
  Gtk::ScrolledWindow _tracescrollw;
  Gtk::TextView _tracetextview;
  Glib::RefPtr<Gtk::TextTag> _tracetitletag;
  Glib::RefPtr<Gtk::TextTag> _tracedatetag;
  Glib::RefPtr<Gtk::TextTag> _tracecommandtag;
  Glib::RefPtr<Gtk::TextTag> _tracereplytag;
public:
  void add_title(const std::string&);
  void add_command_from_melt(const std::string&);
  void add_reply_to_melt(const std::string&);
  void add_title(std::ostringstream&outs) {
    outs.flush();
    add_title(outs.str());
  };
  void add_date(const std::string& ="");
  void add_reply_to_melt(const std::ostringstream&outs) {
    add_reply_to_melt(outs.str());
  };
  SmeltTraceWindow();
  ~SmeltTraceWindow () {
  }
  bool tracing() {
    if (_traceitemcheck)
      return _traceitemcheck->get_active();
    return false;
  };
  void on_trace_clear();
};


class SmeltAppl;
class SmeltOptionGroup : public Glib::OptionGroup {
  std::string _file_to_melt;
  std::string _file_from_melt;
  bool _trace_melt;
public:
  SmeltOptionGroup()
    : Glib::OptionGroup("smelt_low", "the low level options of smeltgtk",
                        "the low-level options for smeltgtk"),
    _file_to_melt(), _file_from_melt(), _trace_melt(false) {
    ///
    {
      Glib::OptionEntry entry_to_melt;
      entry_to_melt.set_long_name("request-to-MELT");
      entry_to_melt.set_short_name('R');
      entry_to_melt.set_description("the file name, or file descriptor number, for channel sending requests to MELT");
      add_entry_filename(entry_to_melt, _file_to_melt);
    }
    ///
    {
      Glib::OptionEntry entry_from_melt;
      entry_from_melt.set_long_name("command-from-MELT");
      entry_from_melt.set_short_name('C');
      entry_from_melt.set_description("the file name, or file descriptor number, for channel recieving commands from MELT");
      add_entry_filename(entry_from_melt, _file_from_melt);
    }
    ///
    {
      Glib::OptionEntry entry_trace_melt;
      entry_trace_melt.set_long_name("trace");
      entry_trace_melt.set_short_name('t');
      entry_trace_melt.set_description("Show trace window");
      add_entry(entry_trace_melt, _trace_melt);
    }
    ///
#ifndef NDEBUG
    {
      Glib::OptionEntry entry_debug_melt;
      entry_debug_melt.set_long_name("debug");
      entry_debug_melt.set_short_name('D');
      entry_debug_melt.set_description("Show low level debugging");
      add_entry(entry_debug_melt, smelt_debugging);
    }
#endif
  };
  void setup_appl(SmeltAppl&);
};				// end class SmeltOptionGroup


class SmeltSymbol {
  const std::string _symname;
  void* _symdata;
  static std::map<std::string,SmeltSymbol*> dictsym_;
public:
  SmeltSymbol(const std::string& name, void* data=nullptr)
    : _symname(name), _symdata(data) {
    assert (!name.empty());
    assert(dictsym_.find(name) == dictsym_.end());
    dictsym_[_symname] = this;
  };
  SmeltSymbol(const SmeltSymbol&) = delete;
  SmeltSymbol(SmeltSymbol&&) = delete;
  virtual ~SmeltSymbol() {
    dictsym_.erase(_symname);
    _symdata = nullptr;
  }
  const std::string& name() const {
    return _symname;
  };
  const void* data() const {
    return _symdata;
  };
  static SmeltSymbol* find(const std::string& name) {
    return dictsym_[name];
  };
};

std::map<std::string,SmeltSymbol*> SmeltSymbol::dictsym_;

enum SmeltArgKind {
  SmeltArg_None=0,
  SmeltArg_Long,
  SmeltArg_Double,
  SmeltArg_String,
  SmeltArg_Symbol,
  SmeltArg_Vector
};

class SmeltArg;

typedef std::vector<SmeltArg> SmeltVector;

class SmeltArg {
public:
private:
  const SmeltArgKind _argkind;
  union {
    long _arglong;
    double _argdouble;
    std::string* _argstring;
    SmeltSymbol* _argsymbol;
    SmeltVector* _argvector;
    void* _argptr;
  };
public:
  // accessors
  SmeltArgKind kind() const {
    return _argkind;
  };
  bool is_null() const {
    return _argkind == SmeltArg_None;
  };
  bool is_long() const {
    return _argkind == SmeltArg_Long;
  };
  bool is_double() const {
    return _argkind == SmeltArg_Double;
  };
  bool is_symbol() const {
    return _argkind == SmeltArg_Symbol;
  };
  bool is_string() const {
    return _argkind == SmeltArg_String;
  };
  bool is_vector() const {
    return _argkind == SmeltArg_Vector;
  };
  operator bool () const {
    return !is_null();
  };
  bool operator ! () const {
    return is_null();
  };
  long as_long() const {
    return (_argkind==SmeltArg_Long)?_arglong:0L;
  };
  double as_double() const {
    return (_argkind==SmeltArg_Double)?_argdouble:0.0;
  };
  SmeltSymbol* symbol_ptr() const {
    return (_argkind==SmeltArg_Symbol)?_argsymbol:nullptr;
  };
  SmeltVector* vecor_ptr() const {
    return  (_argkind==SmeltArg_Vector)?_argvector:nullptr;
  };
  const std::string& as_string() const {
    static const std::string emptystr;
    if (_argkind == SmeltArg_String) return *_argstring;
    return emptystr;
  };
  double to_double() const {
    if (_argkind != SmeltArg_Double)
      throw smelt_domain_error("to_double: bad argument kind",
                               smelt_long_to_string((int)_argkind));
    return _argdouble;
  };
  long to_long() const {
    if (_argkind != SmeltArg_Long)
      throw smelt_domain_error("to_long: bad argument kind",
                               smelt_long_to_string((int)_argkind));
    return _arglong;
  };
  const std::string &to_string() const {
    if (_argkind != SmeltArg_String)
      throw smelt_domain_error("to_string: bad argument kind",
                               smelt_long_to_string((int)_argkind));
    return *_argstring;
  };
  SmeltSymbol& to_symbol() const {
    if (_argkind != SmeltArg_Symbol)
      throw  smelt_domain_error("to_symbol: bad argument kind",
                                smelt_long_to_string((int)_argkind));
    return *_argsymbol;
  };
  SmeltVector& to_vector() const {
    if (_argkind != SmeltArg_Vector)
      throw  smelt_domain_error("to_vector: bad argument kind",
                                smelt_long_to_string((int)_argkind));
    return *_argvector;
  };
  /// constructors
  SmeltArg() : _argkind(SmeltArg_None), _argptr(nullptr) {};
  SmeltArg(const SmeltArg& a)
    : _argkind(a._argkind), _argptr(nullptr) {
    switch (a._argkind) {
    case SmeltArg_None:
      break;
    case SmeltArg_Long:
      _arglong = a._arglong;
      break;
    case SmeltArg_Double:
      _argdouble = a._argdouble;
      break;
    case SmeltArg_String:
      _argstring = new std::string(*a._argstring);
      break;
    case SmeltArg_Symbol:
      _argsymbol = a._argsymbol;
      assert (_argsymbol != nullptr);
      break;
    case SmeltArg_Vector:
      _argvector = new SmeltVector(*a._argvector);
      break;
    }
  };
  SmeltArg(SmeltArg&& a) : _argkind(a._argkind) {
    switch (a._argkind) {
    case SmeltArg_None:
      break;
    case SmeltArg_Long:
      _arglong = a._arglong;
      break;
    case SmeltArg_Double:
      _argdouble = a._argdouble;
      break;
    case SmeltArg_String:
      _argstring = a._argstring;
      break;
    case SmeltArg_Symbol:
      _argsymbol = a._argsymbol;
      assert (_argsymbol != nullptr);
      break;
    case SmeltArg_Vector:
      _argvector = a._argvector;
      break;
    }
    const_cast<SmeltArgKind&>(a._argkind) = SmeltArg_None;
    a._argptr = nullptr;
  };
  void clear() {
    switch (_argkind) {
    case SmeltArg_None:
      break;
    case SmeltArg_Long:
      _arglong = 0;
      break;
    case SmeltArg_Double:
      _argdouble = 0.0;
      break;
    case SmeltArg_String:
      delete _argstring;
      break;
    case SmeltArg_Symbol:
      _argsymbol = nullptr;
      break;
    case SmeltArg_Vector:
      delete _argvector;
      break;
    }
    _argptr = nullptr;
  }
  ~SmeltArg() {
    clear();
  };
  SmeltArg& operator=(const SmeltArg&a) {
    if (_argkind != SmeltArg_None) clear();
    switch (a._argkind) {
    case SmeltArg_None:
      break;
    case SmeltArg_Long:
      _arglong = a._arglong;
      break;
    case SmeltArg_Double:
      _argdouble = a._argdouble;
      break;
    case SmeltArg_String:
      _argstring = new std::string(*a._argstring);
      break;
    case SmeltArg_Symbol:
      _argsymbol = a._argsymbol;
      assert (_argsymbol != nullptr);
      break;
    case SmeltArg_Vector:
      _argvector = new SmeltVector(*a._argvector);
      break;
    }
    const_cast<SmeltArgKind&>(_argkind) = a._argkind;
    return *this;
  }
  explicit SmeltArg(std::nullptr_t) : _argkind(SmeltArg_None), _argptr(nullptr) {};
  explicit SmeltArg(long l) : _argkind(SmeltArg_Long), _arglong(l) {};
  explicit SmeltArg(double d) : _argkind(SmeltArg_Double), _argdouble(d) {};
  explicit SmeltArg(const std::string& s)
    : _argkind(SmeltArg_String), _argptr(nullptr) {
    _argstring = new std::string(s);
  }
  explicit SmeltArg(SmeltSymbol* s): _argkind(SmeltArg_Symbol), _argsymbol(s) {
    assert(s!=nullptr);
  };
  explicit SmeltArg(const SmeltSymbol& s)
    : _argkind(SmeltArg_Symbol), _argsymbol(const_cast<SmeltSymbol*>(&s)) {};
  explicit SmeltArg(const SmeltVector& v)
    : _argkind(SmeltArg_Vector) {
    _argvector = new SmeltVector(v);
  };
  static SmeltVector parse_string_vector(const std::string& s, int& pos) throw (std::exception);
  static SmeltArg parse_string_arg(const std::string& s, int& pos) throw (std::exception);
  void out(std::ostream&) const;
};				// end class SmeltArg


std::ostream& operator << (std::ostream& os, const SmeltArg&a)
{
  a.out(os);
  return os;
};

std::ostringstream& operator << (std::ostringstream& os, const SmeltArg&a)
{
  a.out(os);
  return os;
};


std::ostream& operator << (std::ostream& os, const SmeltVector& v)
{
  os << "(";
  for (auto& arg: v) {
    os << " " << arg;
  }
  os << ")";
  return os;
}

void SmeltArg::out(std::ostream& os) const
{
  switch (_argkind) {
  case SmeltArg_None:
    os << "!";
    break;
  case SmeltArg_Long:
    os << _arglong;
    break;
  case SmeltArg_Double:
    os << _argdouble;
    break;
  case SmeltArg_String:
    os << "\"";
    for (char c : *_argstring) {
      switch (c) {
      case '0' ... '9':
      case 'a' ... 'z':
      case 'A' ... 'Z':
        os << c;
        break;
      case '\'':
        os << "\\\'";
        break;
      case '\"':
        os << "\\\"";
        break;
      case '\\':
        os << "\\\\";
        break;
      case '\n':
        os << "\\n";
        break;
      case '\r':
        os << "\\r";
        break;
      case '\t':
        os << "\\t";
        break;
      case '\v':
        os << "\\v";
        break;
      case '\f':
        os << "\\f";
        break;
      case '\e':
        os << "\\e";
        break;
      default:
        if (std::isprint(c)) {
          os << c;
        }  else {
          char cbuf[8];
          memset (cbuf, 0, sizeof(cbuf));
          snprintf(cbuf, sizeof(cbuf), "\\x%02x", ((int)c & 0xff));
          os << cbuf;
        }
      }
    }
    os << "\"";
    break;
  case SmeltArg_Symbol:
    assert (_argsymbol != nullptr);
    os << (_argsymbol->name());
    break;
  case SmeltArg_Vector: {
    int nb=0;
    assert (_argvector != nullptr);
    os << "(";
    for (auto v : *_argvector) {
      if (nb>0)
        os << " ";
      v.out(os);
      nb++;
    }
    os << ")";
  }
  break;
  }
}

class SmeltCommandSymbol : public SmeltSymbol {
public:
  typedef void (SmeltAppl::*cmdfun_t)(SmeltVector&);
  friend class SmeltAppl;
private:
  cmdfun_t _cmdfun;
public:
  SmeltCommandSymbol(const std::string&name, cmdfun_t fun)
    : SmeltSymbol(name), _cmdfun(fun) {
  };
  ~SmeltCommandSymbol() {
    _cmdfun = nullptr;
  };
  cmdfun_t fun() {
    return _cmdfun;
  };
  void call(SmeltAppl*, SmeltVector&);
};				// end class SmeltCommandSymbol


class SmeltAppl
  // when gtkmm3.4 is available, should inherit from Gtk::Application
    : public Gtk::Main {
  Glib::RefPtr<Gsv::LanguageManager> _app_langman;
  SmeltMainWindow _app_mainwin;	// main window
  std::unique_ptr<SmeltTraceWindow> _app_tracewin;
  bool _app_traced;
  Glib::RefPtr<Glib::IOChannel> _app_reqchan_to_melt; // channel for request to MELT
  Glib::RefPtr<Glib::IOChannel> _app_cmdchan_from_melt; // channel for commands from MELT
  sigc::connection _app_connreq_to_melt;
  sigc::connection _app_conncmd_from_melt;
  std::ostringstream _app_writestream_to_melt; // string buffer for requests to MELT
  std::string _app_cmdstr_from_melt;	       // the last command from MELT
  Glib::RefPtr<Gdk::Pixbuf> _app_indifferent_pixbuf;
  Glib::RefPtr<Gdk::Pixbuf> _app_key_7x11_pixbuf;
  Glib::RefPtr<Gdk::Pixbuf> _app_key_16x16_pixbuf;
protected:
  bool reqbuf_to_melt_cb(Glib::IOCondition);
  bool cmdbuf_from_melt_cb(Glib::IOCondition);
  void process_command_from_melt (std::string& str);
  std::ostringstream& outreq() {
    return _app_writestream_to_melt;
  };
  void sendreq (std::ostream& os) {
    os << std::endl << std::endl;
    if (os == _app_writestream_to_melt) {
      os << std::flush;
      if (!_app_connreq_to_melt) {
        SMELT_DEBUG("connecting requests");
        _app_connreq_to_melt
          = Glib::signal_io().connect(sigc::mem_fun(*this, &SmeltAppl::reqbuf_to_melt_cb),
                                      _app_reqchan_to_melt, Glib::IO_OUT);
      }
    }
  }
public:
  Glib::RefPtr<Gsv::LanguageManager> langman() const {
    return _app_langman;
  };
  Glib::RefPtr<Gdk::Pixbuf> indifferent_pixbuf() const {
    return _app_indifferent_pixbuf;
  }
  Glib::RefPtr<Gdk::Pixbuf> key_7x11_pixbuf() const {
    return _app_key_7x11_pixbuf;
  }
  Glib::RefPtr<Gdk::Pixbuf> key_16x16_pixbuf() const {
    return _app_key_16x16_pixbuf;
  }
  static SmeltAppl* instance() {
    return static_cast<SmeltAppl*>(Gtk::Main::instance());
  };
  SmeltAppl(int &argc, char**&argv)
    : Gtk::Main(argc, argv, smelt_options_context),
      _app_mainwin(),
      _app_traced(false) {
    Gsv::init(); /// initialize GtkSourceviewMM very early!
    _app_langman = Gsv::LanguageManager::get_default();
    _app_indifferent_pixbuf
      = Gdk::Pixbuf::create_from_xpm_data (smelt_indifferent_13x14_xpm);
    _app_key_16x16_pixbuf
      = Gdk::Pixbuf::create_from_xpm_data (smelt_key_16x16_xpm);
    _app_key_7x11_pixbuf
      = Gdk::Pixbuf::create_from_xpm_data (smelt_key_7x11_xpm);
  };
  void set_reqchan_to_melt(const std::string &reqname) {
    const char* reqcstr = reqname.c_str();
    char* endstr = nullptr;
    long reqfd = strtol(reqcstr, &endstr, 10);
    struct stat reqstat = {};
    if (reqfd > 0 && reqfd < sysconf(_SC_OPEN_MAX)
        && endstr && endstr[0] == (char)0
        && !fstat(reqfd, &reqstat)) {
      SMELT_DEBUG("request to MELT channel fd#" << reqfd);
      _app_reqchan_to_melt = Glib::IOChannel::create_from_fd(reqfd);
    } else if ((reqfd = open(reqcstr, O_RDONLY|O_CLOEXEC)) >= 0) {
      SMELT_DEBUG("request to MELT channel file " << reqcstr << " fd#" << reqfd);
      _app_reqchan_to_melt = Glib::IOChannel::create_from_fd(reqfd);
    } else
      SMELT_FATAL("cannot open request to MELT channel " << reqname);
    // don't connect now
  }
  void set_cmdchan_from_melt(const std::string &cmdname) {
    const char* cmdcstr = cmdname.c_str();
    char* endstr = nullptr;
    long cmdfd = strtol(cmdcstr, &endstr, 10);
    struct stat cmdstat = {};
    if (cmdfd >= 0 && cmdfd < sysconf(_SC_OPEN_MAX)
        && endstr && endstr[0] == (char)0
        && !fstat(cmdfd, &cmdstat)) {
      SMELT_DEBUG("command from MELT channel fd#" << cmdfd);
      _app_cmdchan_from_melt = Glib::IOChannel::create_from_fd(cmdfd);
    } else if ((cmdfd = open(cmdcstr, O_RDONLY|O_CLOEXEC)) >= 0) {
      SMELT_DEBUG("command from MELT channel file " << cmdcstr << " fd#" << cmdfd);
      _app_cmdchan_from_melt = Glib::IOChannel::create_from_fd(cmdfd);
    } else
      SMELT_FATAL("cannot open command from MELT channel " << cmdname);
    _app_conncmd_from_melt =
      Glib::signal_io().connect(sigc::mem_fun(*this, &SmeltAppl::cmdbuf_from_melt_cb),
                                _app_cmdchan_from_melt, Glib::IO_IN);
  }
  ~SmeltAppl() {};
  void run (void) {
    Gtk::Main::run(_app_mainwin);
  };
  void set_trace(bool =true);
  void on_trace_toggled(void);
  /// command handlers
  void tracemsg_cmd(SmeltVector&);
  void quit_cmd(SmeltVector&);
  void echo_cmd(SmeltVector&);
  void showfile_cmd(SmeltVector&);
  void marklocation_cmd(SmeltVector&);
};				// end class SmeltAppl


SmeltVector
SmeltArg::parse_string_vector(const std::string& s, int& pos) throw (std::exception)
{
  SmeltVector vec;
  size_t siz = s.size();
  if (pos<0 || pos>(int)siz)
    throw std::range_error("position out of range");
  while (pos < (int)siz) {
    while (pos < (int)siz && std::isspace(s[pos])) pos++;
    if (pos >= (int)siz || s[pos] == ')')
      break;
    SmeltArg arg = parse_string_arg(s, pos);
    vec.push_back(arg);
  }
  return vec;
}



SmeltArg SmeltArg::parse_string_arg(const std::string& s, int& pos) throw (std::exception)
{
  size_t siz = s.size();
  if (pos<0 || pos>(int)siz)
    throw std::range_error("position out of range");
  if (pos>=(int)siz) return SmeltArg();
  while (pos < (int) siz && std::isspace(s[pos])) pos++;
  if (pos>=(int) siz) return SmeltArg();
  char c = s[pos];
  if (std::isdigit(c) ||
      (pos<(int)siz-1 && (c=='+' || c=='-') && std::isdigit(s[pos+1]))) {
    // parse a number, choose between the double and the long the longest one
    char* endlng = nullptr;
    char* enddbl = nullptr;
    const char* cstr = s.c_str() + pos;
    long l = strtol(cstr, &endlng, 0);
    assert(endlng != nullptr);
    double d = strtod(cstr, &enddbl);
    if (enddbl && enddbl > endlng) {
      pos += endlng - cstr;
      return SmeltArg(d);
    } else {
      assert (endlng > cstr);
      pos += endlng - cstr;
      return SmeltArg(l);
    }
  } else if (std::isalpha(c) || c=='_') {
    // parse an existing symbol
    std::string name;
    name.reserve(30);
    int spos = pos;
    while (pos < (int) siz && (c=s[pos])!=(char)0 && (std::isalnum(c) || c=='$' || c=='_'))
      (name += toupper(c)), (pos++);

    SmeltSymbol* sym = SmeltSymbol::find(name);
    if (!sym)
      throw smelt_parse_error("unknown symbol", name, spos);
    return SmeltArg(sym);
  } else if (c == '"') {
    // parse a C encoded string
    std::string str;
    str.reserve(50);
    int spos = pos;
    pos++;
    SMELT_DEBUG("siz=" << siz << " pos=" << pos);
    while (pos < (int) siz && (c=s[pos])!=(char)0 && c!= '"') {
      if (c=='\\' && pos+1 < (int) siz) {
        const char nc = s[pos+1];
        switch (nc) {
        case '\\':
          str += "\\";
          pos += 2;
          break;
        case 'r':
          str += "\r";
          pos += 2;
          break;
        case 't':
          str += "\t";
          pos += 2;
          break;
        case 'n':
          str += "\n";
          pos += 2;
          break;
        case 'f':
          str += "\f";
          pos += 2;
          break;
        case 'v':
          str += "\v";
          pos += 2;
          break;
        case 'e':
          str += "\e";
          pos += 2;
          break;
        case '\"':
        case 'Q':
          str += "\"";
          pos += 2;
          break;
        case '_':
          str += " ";
          pos += 2;
          break;
        case '0' ... '7' : {
          const char *ds = s.c_str()+pos+1;
          char* end = nullptr;
          long l = strtol(ds, &end, 8);
          assert (end != nullptr);
          pos += 2 + end - ds;
          str += (char) l;
          break;
        }
        case 'x': {
          std::string xs;
          pos += 2;
          if (std::isxdigit(s[pos])) xs += s[pos++];
          if (std::isxdigit(s[pos])) xs += s[pos++];
          long l = strtol(xs.c_str(), nullptr, 16);
          str += (char) l;
          break;
        }
        default:
          throw smelt_parse_error("bad string backslash escape", str.substr(spos,4), pos);
        }
      } else if (std::isspace(c) || std::isprint(c)) {
        str += c;
        pos++;
      } else
        throw smelt_parse_error("bad character in string", str, pos);
    };
    SMELT_DEBUG("ending pos=" << pos << " c=" << c);
    if (c != '\"')
      throw smelt_parse_error("unterminated string", str, pos);
    pos++;
    SMELT_DEBUG("got str:" << str);
    return SmeltArg(str);
  } else if (c == '(') {
    int spos = pos;
    pos++;
    while (pos < (int)siz && std::isspace(s[pos])) pos++;
    if (pos >= (int)siz)
      throw smelt_parse_error("unbalanced paren for nil", "", pos);
    if (s[pos] == ')') {
      pos++;
      return SmeltArg(nullptr);
    } else {
      SmeltVector vect = parse_string_vector(s, pos);
      while (pos < (int) siz && std::isspace(s[pos])) pos++;
      if (pos >= (int) siz)
        throw smelt_parse_error("unbalanced paren in vector", s, spos);
      if (s[pos] == ')')
        pos++;
      else throw smelt_parse_error("unmatched paren in vector", s, spos);
      return SmeltArg(vect);
    }
  } else throw smelt_parse_error("unexpected char", s, pos);
}


////////////////////////////////////////////////////////////////
SmeltMainWindow::ShownFile::ShownFile(SmeltMainWindow*mwin,const std::string&filepath,long num)
  : _sfilnum(num), _sfilnblines(0), _sfilname(filepath), _sfilview()
{
  SMELT_DEBUG("filepath=" << filepath << " num=" << num);
  assert(mwin != nullptr);
  if (access(filepath.c_str(), R_OK))
    SMELT_FATAL("invalid filepath " << filepath);
  if (num == 0 || mainsfilemapnum_.find(num) != mainsfilemapnum_.end())
    throw smelt_domain_error("ShownFile: invalid shown file number",smelt_long_to_string(num));
  if (filepath.empty() || mainsfiledict_.find(filepath) != mainsfiledict_.end())
    throw smelt_domain_error("ShownFile: invalid shown file name",filepath);
  SMELT_DEBUG("guessing language for filepath=" << filepath);
  auto langman = SmeltAppl::instance()->langman();
  auto lang = langman->guess_language(filepath,std::string());
  assert (lang);
  SMELT_DEBUG("guessed lang id=" << (lang->get_id().c_str()) <<
              " name=" << (lang->get_name().c_str()));
  auto sbuf = _sfilview.get_source_buffer();
  sbuf->set_language(lang);
  _sfilview.set_editable(false);
#warning to improve set_mark_category_pixbuf
  /* look into gtksourceviewmm/gutterrenderer*.h & gtksourceviewmm/markattributes.h */
  //  _sfilview.set_mark_category_pixbuf
  //  ("keymark",
  //   SmeltAppl::instance()->key_16x16_pixbuf());
  {
    std::ifstream infil(filepath);
    int nblines = 0;
    while (!infil.eof()) {
      std::string linestr;
      std::getline(infil,linestr);
      linestr += "\n";
      sbuf->insert(sbuf->end(),linestr);
      nblines++;
    }
    _sfilnblines = nblines;
    SMELT_DEBUG("filepath=" << filepath << " got " << nblines << " lines");
    infil.close();
  }
  {
    auto scrowin = new Gtk::ScrolledWindow;
    scrowin->add (_sfilview);
    scrowin->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    auto labstr = Glib::ustring::compose("<span color='darkblue'>#%1</span>\n"
                                         "<tt>%2</tt>", num, Glib::path_get_basename(filepath));
    auto labw = new Gtk::Label;
    auto labtit = new Gtk::Label;
    {
      char* realfilpath = realpath(filepath.c_str(),nullptr);
      labtit->set_markup(Glib::ustring::compose("<span color='blue'>#%1</span>\n"
                         "<span size='small'><tt>%2</tt></span>",
                         num, realfilpath));
      labtit->set_justify(Gtk::JUSTIFY_CENTER);
      free(realfilpath);
    }
    auto vbox = new Gtk::VBox(false,2);
    vbox->pack_start(*labtit,Gtk::PACK_SHRINK);
    vbox->pack_start(*new Gtk::Separator(Gtk::ORIENTATION_HORIZONTAL),Gtk::PACK_SHRINK);
    vbox->pack_start(*scrowin,Gtk::PACK_EXPAND_WIDGET);
    labw->set_markup(labstr);
    mwin->_mainnotebook.append_page(*vbox,*labw);
    scrowin->show_all();
    labw->show_all();
    mwin->_mainnotebook.show_all();
  }
  mainsfilemapnum_[num] = this;
  mainsfiledict_[filepath] = this;
}

SmeltMainWindow::ShownFile::~ShownFile()
{
  assert (mainsfilemapnum_[_sfilnum] == this);
  assert (mainsfiledict_[_sfilname] == this);
  mainsfilemapnum_.erase (_sfilnum);
  mainsfiledict_.erase (_sfilname);
}

void
SmeltMainWindow::show_file(const std::string&path, long num)
{
  new ShownFile(this,path,num);
}


void
SmeltMainWindow::mark_location(long marknum,long filenum,int lineno, int col)
{
  SMELT_DEBUG("marknum=" << marknum << " filenum=" << filenum
              << " lineno=" << lineno << " col=" << col);
  ShownFile* sfil = shown_file_by_number (filenum);
  if (!sfil)
    throw smelt_domain_error("mark_location invalid file number",
                             smelt_long_to_string(filenum));
  if (lineno<=0 || lineno>sfil->nblines())
    throw smelt_domain_error("mark_location invalid line number",
                             smelt_long_to_string(lineno));
  Glib::RefPtr<Gsv::Gutter> gut = sfil->left_gutter();
  auto gutrenderer = new Gsv::GutterRendererPixbuf();
  gutrenderer->set_pixbuf(SmeltAppl::instance()->key_16x16_pixbuf());
  gut->insert(gutrenderer,lineno-1);
  auto tbuf = sfil->view().get_source_buffer();
  auto itlin = tbuf->get_iter_at_line (lineno-1);
  auto itendlin = itlin;
  itendlin.forward_line();
  itendlin.backward_char();
  int linwidth = itendlin.get_line_offset();
  if (col<=0 || linwidth==0)
    col = 0;
  else if (col>=linwidth)
    col = linwidth-1;
  SMELT_DEBUG ("linwidth=" << linwidth << " normalized col=" << col);
  auto itcur = itlin;
  if (col>0) itcur.forward_chars(col);
  Glib::RefPtr<Gtk::TextChildAnchor> chanch = tbuf->create_child_anchor(itcur);
  auto but = new Gtk::Button();
  but->add(*new Gtk::Image(SmeltAppl::instance()->key_7x11_pixbuf()));
  sfil->view().add_child_at_anchor(*but,chanch);
  but->show_all();
  SMELT_DEBUG("added but@" << (void*)but);
#warning incomplete SmeltMainWindow::mark_location
}
////////////////////////////////////////////////////////////////
void SmeltTraceWindow::add_title(const std::string &str)
{
  auto tbuf =  _tracetextview.get_buffer();
  tbuf->insert_with_tag(tbuf->end(), str, "title");
  if (str.empty() || str[str.size()-1] != '\n')
    tbuf->insert(tbuf->end(), "\n");
  add_date();
  tbuf->insert(tbuf->end(), "\n");
}


void SmeltTraceWindow::on_trace_clear(void)
{
  static long nbclear;
  nbclear++;
  auto tbuf =  _tracetextview.get_buffer();
  tbuf->erase(tbuf->begin(), tbuf->end());
  std::ostringstream out;
  out << "Trace Erased (" << nbclear << "th time)";
  add_title(out);
}

void SmeltTraceWindow::add_date(const std::string &s)
{
  struct timeval tv= {0,0};
  gettimeofday (&tv,nullptr);
  time_t now = tv.tv_sec;
  char timbuf[64];
  auto tbuf =  _tracetextview.get_buffer();
  strftime(timbuf, sizeof(timbuf), "%b %d %H:%M:%S", localtime(&now));
  tbuf->insert_with_tag(tbuf->end(), timbuf, "date");
  snprintf(timbuf, sizeof(timbuf), ".%02d", (int)(tv.tv_usec / 10000));
  tbuf->insert_with_tag(tbuf->end(), timbuf, "date");
  if (!s.empty()) {
    tbuf->insert_with_tag(tbuf->end(), " ", "date");
    tbuf->insert_with_tag(tbuf->end(), s, "date");
  }
  tbuf->insert(tbuf->end(), "\n");
}


SmeltTraceWindow::SmeltTraceWindow()
  :  _tracevbox(false, 3)
{
  set_border_width(5);
  set_default_size(380,280);
  add(_tracevbox);
  Glib::ustring labmarkstr = Glib::ustring::compose
                             ("<big><span color='darkred'>Trace Gcc MELT gtkmm-probe</span></big>\n"
                              "<small>pid %1 on <tt>%2</tt></small>",
                              (int)(getpid()), g_get_host_name());
  _tracelabel.set_markup(labmarkstr);
  _tracelabel.set_justify(Gtk::JUSTIFY_CENTER);
  /// create the action group
  {
    _traceactgroup = Gtk::ActionGroup::create();
    _traceactgroup->add(Gtk::Action::create("MenuFile", "_File"));
    _traceactgroup->add(Gtk::Action::create("MenuTrace", "Trace"),
                        sigc::mem_fun(*SmeltAppl::instance(), &SmeltAppl::on_trace_toggled));
    _traceactgroup->add(Gtk::Action::create("MenuClear", "Clear"),
                        sigc::mem_fun(*this,&SmeltTraceWindow::on_trace_clear));
    auto uimgr = Gtk::UIManager::create();
    uimgr->insert_action_group(_traceactgroup);
    Glib::ustring ui_info= R"*(
                           <ui>
                           <menubar name='MenuBar'>
                           <menu action='MenuFile'>
                           <menuitem name='MenuTrace' action='MenuTrace'/>
                           <menuitem action='MenuClear'/>
                           </menu>
                           </menubar>
                           </ui>
                           )*";
    try {
      uimgr->add_ui_from_string(ui_info);
    } catch (const Glib::Error& ex) {
      SMELT_FATAL("failed to build UI " << ex.what());
    }
    _tracevbox.pack_start(_tracelabel,Gtk::PACK_SHRINK);
    {
      auto menubarp = uimgr->get_widget("/MenuBar");
      if (menubarp)
        _tracevbox.pack_start(*menubarp,Gtk::PACK_SHRINK);
    }
    _traceitemcheck = Glib::RefPtr<Gtk::CheckMenuItem>(dynamic_cast<Gtk::CheckMenuItem*>(uimgr->get_widget("/MenuBar/MenuTrace")));
  }
  _tracevbox.pack_start(_tracescrollw,Gtk::PACK_EXPAND_WIDGET);
  _tracescrollw.add(_tracetextview);
  _tracescrollw.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
  _tracetextview.set_editable(false);
  {
    auto tbuf =  _tracetextview.get_buffer();
    _tracetitletag = tbuf->create_tag("title");
    _tracetitletag->property_foreground() = "darkred";
    _tracetitletag->property_scale() = 1.1;
    _tracetitletag->property_weight() = Pango::WEIGHT_BOLD;
    _tracetitletag->property_family() = "Verdana";
    _tracetitletag->property_justification() = Gtk::JUSTIFY_CENTER;
    _tracedatetag = tbuf->create_tag("date");
    _tracedatetag->property_foreground() = "brown";
    _tracedatetag->property_background() = "lavender";
    _tracedatetag->property_scale() = 0.8;
    _tracecommandtag = tbuf->create_tag("command");
    _tracecommandtag->property_foreground() = "darkblue";
    _tracecommandtag->property_family() = "Monospace";
    _tracecommandtag->property_style() = Pango::STYLE_ITALIC;
    _tracereplytag = tbuf->create_tag("reply");
    _tracereplytag->property_foreground() = "saddlebrown";
    _tracereplytag->property_family() = "Monospace";
    tbuf->insert_with_tag(tbuf->end(),
                          "Trace Window Gcc MELT gtkmm-prove\n",
                          "title");
    {
      time_t now = 0;
      time (&now);
      char nowbuf[80];
      strftime (nowbuf, sizeof(nowbuf), "%c\n", localtime(&now));
      tbuf->insert_with_tag(tbuf->end(),
                            Glib::ustring::compose
                            ("start at %1\n", nowbuf),
                            "date");
      tbuf->insert(tbuf->end(), "Commands from MELT are ");
      tbuf->insert_with_tag(tbuf->end(), "like this\n", "command");
      tbuf->insert(tbuf->end(), "Replies to MELT are ");
      tbuf->insert_with_tag(tbuf->end(), "like that\n", "reply");
      tbuf->insert(tbuf->end(), __FILE__ " compiled " __DATE__ "@" __TIME__ "\n");
    }
  }
  show_all();
}

void SmeltCommandSymbol::call(SmeltAppl* app, SmeltVector&v)
{
  ((*app).*(_cmdfun))(v);
}


void SmeltTraceWindow::add_command_from_melt(const std::string& str)
{
  auto tbuf =  _tracetextview.get_buffer();
  add_date();
  tbuf->insert_with_tag(tbuf->end(), str, "command");
  if (str.empty() || str[str.size()-1] != '\n')
    tbuf->insert(tbuf->end(), "\n");
}

void SmeltOptionGroup::setup_appl(SmeltAppl& app)
{
  if (!_file_to_melt.empty())
    app.set_reqchan_to_melt(_file_to_melt);
  if (!_file_from_melt.empty())
    app.set_cmdchan_from_melt(_file_from_melt);
  if (_trace_melt)
    app.set_trace(_trace_melt);
}

/* low level callback to write requests to MELT */
bool
SmeltAppl::reqbuf_to_melt_cb(Glib::IOCondition outcond)
{
  SMELT_DEBUG("start outcond=" << outcond);
  if ((outcond & Glib::IO_OUT) == 0)
    SMELT_FATAL ("error when writing requests to MELT");
  _app_writestream_to_melt.flush();
  std::string wstr = _app_writestream_to_melt.str();
  int wsiz = wstr.size();
  SMELT_DEBUG(" wsiz=" << wsiz << " wstr:" << wstr);
  if (wsiz == 0) {
    SMELT_DEBUG("disabling write requests");
    return false;
  }
  gsize wcnt = 0;
  int woff = 0;
  Glib::IOStatus wsta = Glib::IO_STATUS_EOF;
  const char* wdata = wstr.data();
  do {
    if (wsiz <= 0)
      break;
    wsta = _app_reqchan_to_melt->write(wdata+woff, wsiz, wcnt);
    switch (wsta) {
    case Glib::IO_STATUS_NORMAL:
    case Glib::IO_STATUS_AGAIN:
      if (wcnt > 0) {
        woff += wcnt;
        wsiz -= wcnt;
        continue;
      }
      break;
    case Glib::IO_STATUS_ERROR:
    case Glib::IO_STATUS_EOF:
      SMELT_FATAL("write error for requests to MELT");
    }
  } while (wsta == Glib::IO_STATUS_AGAIN && wcnt > 0);
  _app_writestream_to_melt.clear();
  if (wsiz > 0) {
    wstr.erase (0, wcnt);
    _app_writestream_to_melt.str(wstr);
  } else {
    _app_writestream_to_melt.str(std::string());
  }
  return true;
}

/* low-level callback to read commands from MELT */
bool
SmeltAppl::cmdbuf_from_melt_cb(Glib::IOCondition  incond)
{
  SMELT_DEBUG("start incond=" << incond);
  if ((incond & Glib::IO_IN) == 0)
    SMELT_FATAL ("error when reading commands from MELT");
  Glib::ustring buf;
  _app_cmdchan_from_melt->read_line (buf);
  SMELT_DEBUG("got buf:" << buf);
  if (!buf.empty()) {
    _app_cmdstr_from_melt.append (buf.c_str());
    unsigned cmdlen = _app_cmdstr_from_melt.size();
    if (cmdlen>2 && _app_cmdstr_from_melt[cmdlen-2]=='\n'
        &&  (_app_cmdstr_from_melt[cmdlen-1]=='\n' ||  _app_cmdstr_from_melt[cmdlen-1]=='\f')) {
      std::string curcmd = _app_cmdstr_from_melt;
      _app_cmdstr_from_melt.erase();
      process_command_from_melt(curcmd);
    }
  }
  return true;
}

void
SmeltAppl::set_trace(bool traced)
{
  SMELT_DEBUG(" traced=" << traced);
  if (traced) {
    if (!_app_tracewin)
      _app_tracewin.reset (new SmeltTraceWindow());
    else
      _app_tracewin->add_title("Tracing resumed");
    _app_tracewin->show_all();
    _app_traced = true;
  } else {
    if (_app_tracewin) {
      _app_tracewin->hide();
      _app_tracewin->add_title("Tracing stopped");
    }
    _app_traced = false;
  }
}


void
SmeltAppl::on_trace_toggled(void)
{
  if (_app_tracewin)
    set_trace(_app_tracewin->tracing());
  else
    set_trace(true);
}

void
SmeltAppl::process_command_from_melt(std::string& str)
{
  SMELT_DEBUG("str.siz=" << str.size() << " str:" << str);
  if (_app_traced) {
    _app_tracewin->add_command_from_melt(str);
  }
  int pos = 0;
  try {
    SMELT_DEBUG("parsing str:" << str);
    SmeltVector v = SmeltArg::parse_string_vector(str, pos);
    SMELT_DEBUG("v:" << v << " final pos:" << pos);
    SMELT_DEBUG("v[0] " << v.at(0));
    SmeltSymbol& sym = v.at(0).to_symbol();
    SmeltCommandSymbol* csym = dynamic_cast<SmeltCommandSymbol*>(&sym);
    SMELT_DEBUG("csym@" << (void*)csym);
    if (!csym)
      throw smelt_domain_error("process_command_from_melt: invalid command", sym.name());
    SMELT_DEBUG("*csym::" << csym->name());
    csym->call(this,v);
  } catch (std::exception ex) {
    std::clog << "Command error:" << ex.what() << std::endl;
    if (_app_traced)
      _app_tracewin->add_title(std::string("Command error:") + ex.what());
  }
}


/////////////// tracemsg_cmd
SmeltCommandSymbol smeltsymb_tracemsg_cmd("TRACEMSG_PCD",&SmeltAppl::tracemsg_cmd);

void
SmeltAppl::tracemsg_cmd(SmeltVector&v)
{
  auto& msg = v[1].to_string();
  SMELT_DEBUG("msg:" << msg);
  if (_app_traced && _app_tracewin)
    _app_tracewin->add_title(msg);
}

////////////// quit_cmd

SmeltCommandSymbol smeltsymb_quit_cmd("QUIT_PCD",&SmeltAppl::quit_cmd);


static void smelt_quit(void)
{
  Gtk::Main::quit();
}

void
SmeltAppl::quit_cmd(SmeltVector&v)
{
  long delay = (v.size()>0)?v.at(1).as_long():0L;
  SMELT_DEBUG("delay:" << delay);
  if (delay <= 0)
    Gtk::Main::quit();
  else  /* delayed quit; the delay is in milliseconds */
    Glib::signal_timeout().connect_once(sigc::ptr_fun(&smelt_quit),delay);
}



////////////// echo_cmd

SmeltCommandSymbol smeltsymb_echo_cmd("ECHO_PCD",&SmeltAppl::echo_cmd);



void
SmeltAppl::echo_cmd(SmeltVector&v)
{
  v.erase(v.begin());
  sendreq(outreq() << v);
}



//////////////// showfile_cmd

SmeltCommandSymbol smeltsymb_showfile_cmd("SHOWFILE_PCD",&SmeltAppl::showfile_cmd);

void
SmeltAppl::showfile_cmd(SmeltVector&v)
{
  auto filnam = v.at(1).to_string();
  auto num = v.at(2).to_long();
  SMELT_DEBUG("filnam=" << filnam << " num=" << num);
  _app_mainwin.show_file(filnam,num);
}

//////////////// showfile_cmd

SmeltCommandSymbol smeltsymb_marklocation_cmd("MARKLOCATION_PCD",&SmeltAppl::marklocation_cmd);

void
SmeltAppl::marklocation_cmd(SmeltVector&v)
{
  auto marknum = v.at(1).to_long();
  auto filnum = v.at(2).to_long();
  auto lineno = v.at(3).to_long();
  auto col = v.at(4).to_long();
  SMELT_DEBUG("filnum=" << filnum << " lineno=" << lineno << " col=" << col);
  _app_mainwin.mark_location(marknum,filnum,lineno,col);
}


////////////////////////////////////////////////////////////////
int main (int argc, char** argv)
{
  SmeltOptionGroup optgroup;
  smelt_options_context.set_main_group(optgroup);
  SmeltAppl app(argc, argv);
  optgroup.setup_appl(app);
  SMELT_DEBUG("running pid " << (int)getpid());
  app.run();
  return 0;
}

/**** for emacs
  ++ Local Variables: ++
  ++ compile-command: "g++ -std=gnu++0x -Wall -O -g $(pkg-config --cflags --libs gtksourceviewmm-3.0  gtkmm-3.0  gtk+-3.0) -o $HOME/bin/simplemelt-gtkmm-probe simplemelt-gtkmm-probe.cc" ++
  ++ End: ++
  Not needed compilation-directory: "."
 ****/
