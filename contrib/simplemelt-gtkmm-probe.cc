
/** Simple MELT GTKMM probe example (with GTKMM, GTK3,
    GTKSOURCEVIEWMM3, ...)
   Copyright (C) 2012 Free Software Foundation, Inc.
   Contributed by Basile Starynkevitch <basile@starynkevitch.net>


This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>. */



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

The MELT code of the probe is in file melt/xtramelt-probe.melt

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


Indent this file with
   astyle -v -s2 -gnu -c simplemelt-gtkmm-probe.cc

To debug this program in isolation, start it with e.g.
   simplemelt-gtkmm-probe -D -T --command-from-MELT 0 --request-to-MELT 1
then type (assuming you have simplemelt-gtkmm-probe.cc, replace it appropriately)
   SHOWFILE_PCD "simplemelt-gtkmm-probe.cc" 1
   MARKLOCATION_PCD 1 1 1002 3
   MARKLOCATION_PCD 2 1 1023 4
syntax is MARKLOCATION_PCD <marknum> <filenum> <line> <col>

Note to the reader; this C++ code used to be coded for C++2011 (up to
svn rev 188620) but has been backported to C++1998 in august 2012.

****/


// I have been bitten by the bug I reported at
// https://bugzilla.gnome.org/show_bug.cgi?id=672544 corrected in GTK 3.4
// so you need GTK 3.4 to run that.

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

#if GTK_VERSION_LT(3,4)
// we depend upon https://bugzilla.gnome.org/show_bug.cgi?id=672544  bug fixed
#error GTK version should be at least 3.4
#endif

#define SMELT_FATAL(C) do { int er = errno; \
  std::cerr << __FILE__ << ":" << __LINE__  \
      << "@" << __func__ << " " << C; \
  if (er) std::cerr << " ~" << strerror(er) ; \
  std::cerr << std::endl;     \
  abort();          \
} while(0)

bool smelt_debugging;

#define SMELT_DEBUG(C) do { if (smelt_debugging)  \
  std::cerr <<  __FILE__ << ":" << __LINE__   \
      << "@" << __func__ << " " << C << std::endl; } while(0)


#define SMELT_NULLPTR ((void*)0)

std::string smelt_long_to_string(long l)
{
  char buf[32];
  snprintf(buf, sizeof(buf), "%ld", l);
  return std::string(buf);
}

class SmeltParseErrorAt : public std::runtime_error {
public:
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
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
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
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

static void smelt_quit(void);
class SmeltArg;

#define SMELT_MARKLOC_CATEGORY "smeltmarkloc"
#define SMELT_MARKLOC_STOCKID GTK_STOCK_YES

class SmeltMainWindow;
class SmeltFile {
  friend class SmeltMainWindow;
  long _sfilnum;    // unique number in _mainsfilemapnum;
  int _sfilnblines;   // number of lines
  std::string _sfilname;  // file path
  Gsv::View _sfilview;  // source view
  static std::map<long,SmeltFile*> mainsfilemapnum_;
  static std::map<std::string,SmeltFile*> mainsfiledict_;
public:
  enum pseudofile_en {
    PseudoFile_None,
    PseudoFileBuiltin,
    PseudoFileCommand,
  };
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
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
  SmeltFile(SmeltMainWindow*,const std::string&filepath,long num);
  SmeltFile(SmeltMainWindow*,const enum pseudofile_en pseudo,long num);
  ~SmeltFile();
  Glib::RefPtr<Gsv::Gutter> left_gutter() {
    return _sfilview.get_gutter(Gtk::TEXT_WINDOW_LEFT);
  }
  static SmeltFile* by_path(const std::string& filepath) {
    if (filepath.empty()) return (SmeltFile*)SMELT_NULLPTR;
    std::map<std::string,SmeltFile*>::iterator itsfil = mainsfiledict_.find(filepath);
    if (itsfil == mainsfiledict_.end()) return (SmeltFile*)SMELT_NULLPTR;
    return itsfil->second;
  }
  static SmeltFile* by_number (long l) {
    if (l == 0) return (SmeltFile*)SMELT_NULLPTR;
    std::map<long,SmeltFile*>::iterator itsfil = mainsfilemapnum_.find(l);
    if (itsfil == mainsfilemapnum_.end()) return (SmeltFile*)SMELT_NULLPTR;
    return itsfil->second;
  }
};        // end class SmeltFile

class SmeltLocationInfo;

//////
class SmeltLocationDialog : public Gtk::MessageDialog {
  friend class SmeltLocationInfo;
  SmeltLocationInfo *_sld_info;
  Gtk::TextView _sld_view;
  Gtk::ScrolledWindow _sld_swin;
  std::vector<Glib::RefPtr<Gtk::TextTag> > _sld_tagvect;
public:
  SmeltLocationDialog(SmeltLocationInfo*sli);
  virtual ~SmeltLocationDialog();
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
  SmeltLocationInfo* info() const {
    return _sld_info;
  };
  Glib::RefPtr<Gtk::TextBuffer> tbuf() {
    return _sld_view.get_buffer();
  };
  Gtk::TextView& view() {
    return _sld_view;
  };
  Gtk::ScrolledWindow& scrwin() {
    return _sld_swin;
  };
  void clear_tag_vector (void) {
    _sld_tagvect.clear();
  };
  void grow_tag_vector (unsigned gap) {
    unsigned sz = _sld_tagvect.size();
    _sld_tagvect.reserve (sz + gap);
  }
  Glib::RefPtr<Gtk::TextTag> find_tag(const Glib::ustring &tagname) {
    g_assert (tbuf ());
    return tbuf()->get_tag_table()->lookup(tagname);
  }
  void push_tag (Glib::RefPtr<Gtk::TextTag> tgref) {
    _sld_tagvect.push_back (tgref);
  }
  void push_tag(const Glib::ustring &tagname) {
    g_assert (tbuf ());
    Glib::RefPtr<Gtk::TextTag> tgref = find_tag(tagname);
    push_tag (tgref);
  }
  void pop_tag (void) {
    _sld_tagvect.pop_back ();
  }
  void append_buffer (const Glib::ustring &str) {
    Glib::RefPtr<Gtk::TextBuffer> tb = tbuf();
    g_assert (tb);
    tb->insert_with_tags(tb->end(),str,_sld_tagvect);
  }
  void append_buffer (const std::string&s, const std::string &tagname);
  void append_buffer (const SmeltArg&a, const std::string &tagname="");
};        // end class SmeltLocationDialog

class SmeltLocationInfo : public sigc::trackable {
  SmeltFile* _sli_fil;
  long _sli_num;
  int _sli_lineno;
  int _sli_col;
  // this cannot be a smart pointer,
  // see https://mail.gnome.org/archives/gtkmm-list/2012-June/msg00036.html
  SmeltLocationDialog* _sli_dial;
  static Glib::RefPtr<Gtk::TextTagTable> sli_tagtbl_;
public:
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
  SmeltLocationInfo(long num, SmeltFile* fil, int lineno, int col)
    : _sli_fil(fil), _sli_num(num), _sli_lineno(lineno), _sli_col(col),
      _sli_dial((SmeltLocationDialog*)SMELT_NULLPTR) {
    SMELT_DEBUG("num=" << num << " this@" << (void*)this);
  }
  ~SmeltLocationInfo() {
    SMELT_DEBUG("destroying this@" << (void*)this
                << " with _sli_dial@" << (void*) _sli_dial);
    if (_sli_dial)
      delete _sli_dial;
    _sli_dial = (SmeltLocationDialog*)SMELT_NULLPTR;
    _sli_fil = (SmeltFile*)SMELT_NULLPTR;
    _sli_num = 0;
    _sli_lineno = 0, _sli_col = 0;
  }
  void on_update(void);
  void on_dialog_response(int);
  static Glib::RefPtr<Gtk::TextTagTable> the_tag_table() {
    return sli_tagtbl_;
  };
  SmeltFile* sfile() const {
    return _sli_fil;
  };
  SmeltLocationDialog* dialog() const {
    return _sli_dial;
  };
  long num() const {
    return _sli_num;
  };
  int lineno() const {
    return _sli_lineno;
  };
  int col() const {
    return _sli_col;
  };
  void destroy_dialog() {
    SMELT_DEBUG("destroy dialog " << (void*) _sli_dial
                << " in this locationinfo @" << (void*)this);
    if (!_sli_dial) return;
    {
      SmeltLocationDialog* dial = _sli_dial;
      _sli_dial = (SmeltLocationDialog*)SMELT_NULLPTR;
      delete dial;
    }
    SMELT_DEBUG("destroyed dialog this locationinfo @" <<  (void*)this);
  };
  static void initialize (void);
};        // end class SmeltLocationInfo

/* The SmeltMainWindow is our graphical interface; allmost all GUI
   code is inside. */
class SmeltMainWindow : public Gtk::ApplicationWindow {
  Gtk::VBox _mainvbox;
  Gtk::Label _mainlabel;
  Gtk::Notebook _mainnotebook;
  Gtk::Statusbar _mainstatusbar;
  Glib::RefPtr<Gtk::ActionGroup> _mainactgroup;
  static std::map<long,SmeltLocationInfo*> mainlocinfmapnum_;
  sigc::connection _mainshowconn;		// connection to show later all the main window
  int _mainshowfromline;
  static SmeltLocationInfo* shown_location_by_number (long l) {
    if (l == 0) return (SmeltLocationInfo*)SMELT_NULLPTR;
    std::map<long,SmeltLocationInfo*>::iterator itsloc = mainlocinfmapnum_.find(l);
    if (itsloc == mainlocinfmapnum_.end()) return (SmeltLocationInfo*)SMELT_NULLPTR;
    return itsloc->second;
  }
  ////
  bool postpone_show_all_cb (void);
public:
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
  SmeltMainWindow() :
    Gtk::ApplicationWindow(),
    _mainshowfromline(0) {
    SMELT_DEBUG ("constructing main window " << (void*)this);
    set_border_width (5);
    set_default_size (620,440);
    set_resizable (true);
    set_title (Glib::ustring::compose("MELT probe pid %1 main window", (int)getpid()));
    add(_mainvbox);
    Glib::ustring labmarkstr = Glib::ustring::compose
                               ("<big><span color='darkred'>Simple Gcc Melt gtkmm-probe</span></big>\n"
                                "<span color='blue'>See <span color='darkgreen'>gcc-melt.org</span></span>\n"
                                "<small>pid %1 on <tt>%2</tt></small>",
                                (int)(getpid()), g_get_host_name());
    _mainlabel.set_markup(labmarkstr);
    _mainlabel.set_justify(Gtk::JUSTIFY_CENTER);
    _mainvbox.pack_start(_mainlabel,Gtk::PACK_SHRINK);
    {
      _mainactgroup = Gtk::ActionGroup::create ();
      _mainactgroup->add(Gtk::Action::create("MainMenuFile", "_File"));
      _mainactgroup->add(Gtk::Action::create("MainQuit", "Quit"),
                         sigc::ptr_fun(&smelt_quit));
      _mainactgroup->add(Gtk::Action::create("MainShowVersion", "Version"),
                         sigc::mem_fun(*this,&SmeltMainWindow::on_version_show));
      Glib::RefPtr<Gtk::UIManager>  uimgr = Gtk::UIManager::create();
      uimgr->insert_action_group(_mainactgroup);
      Glib::ustring ui_info=
        "<ui>"
        "<menubar name='MainMenuBar'>"
        "<menu action='MainMenuFile'>"
        "<menuitem action='MainShowVersion'/>"
        "<menuitem action='MainQuit'/>"
        "</menu>"
        "</menubar>"
        "</ui>";
      try {
        uimgr->add_ui_from_string(ui_info);
        Gtk::Widget* menubar = uimgr->get_widget("/MainMenuBar");
        _mainvbox.pack_start(*menubar,Gtk::PACK_SHRINK);
      } catch (const Glib::Error& ex) {
        SMELT_FATAL("failed to build mainwin UI " << ex.what());
      }
    }
    _mainvbox.pack_start(_mainnotebook,Gtk::PACK_EXPAND_WIDGET);
    _mainvbox.pack_start(_mainstatusbar,Gtk::PACK_SHRINK);
  }
  void send_quit_req(void);
  virtual ~SmeltMainWindow() {
    send_quit_req();
  }
  virtual bool on_delete_event(GdkEventAny*ev) {
    send_quit_req();
    // return false to accept the delete event, true to reject it..
    return Gtk::Window::on_delete_event(ev);
  }
  void notebook_append_page(Gtk::Widget&child,const Glib::ustring&markup) {
    Gtk::Label lab;
    if (_mainnotebook.get_n_pages() == 8)
      _mainnotebook.set_scrollable(true);
    lab.set_markup(markup);
    _mainnotebook.append_page(child,lab);
    postpone_show_all_from(__LINE__);
  }
  void on_version_show(void);
  void show_file(const std::string&path, long num);
  void mark_location(long marknum,long filenum,int lineno, int col);
  void showinfo_location(long marknum);
  void addinfo_location(long marknum, const std::string& title, const SmeltArg& arg);
  guint push_status(const std::string&msg);
  void pop_status(void);
  void remove_status(guint msgid);
  void remove_all_status(void);
  void postpone_show_all_from (int lin);
};        // end SmeltMainWindow


std::map<long,SmeltFile*> SmeltFile::mainsfilemapnum_;
std::map<std::string,SmeltFile*> SmeltFile::mainsfiledict_;
std::map<long,SmeltLocationInfo*>  SmeltMainWindow::mainlocinfmapnum_;
Glib::RefPtr<Gtk::TextTagTable> SmeltLocationInfo::sli_tagtbl_;

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
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
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


class SmeltApplication;
class SmeltOptionGroup : public Glib::OptionGroup {
  std::string _file_to_melt;
  std::string _file_from_melt;
  bool _trace_melt;
public:
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
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
      entry_trace_melt.set_short_name('T');
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
  void setup_appl(SmeltApplication&);
};        // end class SmeltOptionGroup


class SmeltSymbol {
  const std::string _symname;
  void* _symdata;
  static std::map<std::string,SmeltSymbol*> dictsym_;
public:
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
  SmeltSymbol(const std::string& name, void* data=SMELT_NULLPTR)
    : _symname(name), _symdata(data) {
    SMELT_DEBUG("constructing SmeltSymbol name=" << name << " this@" << (void*)this);
    assert (!name.empty());
    {
      std::map<std::string,SmeltSymbol*>::iterator symit = dictsym_.find(_symname);
      if (symit == dictsym_.end())
        SMELT_DEBUG("symit at end");
      else
        SMELT_DEBUG("symit key=" << (symit->first)
                    << " value=" << (void*)(symit->second));
    }
    assert(dictsym_.find(_symname) == dictsym_.end());
    dictsym_[_symname] = this;
  };
  virtual ~SmeltSymbol() {
    dictsym_.erase(_symname);
    _symdata = SMELT_NULLPTR;
  }
  const std::string& name() const {
    return _symname;
  };
  const void* data() const {
    return _symdata;
  };
  static SmeltSymbol* find(const std::string& name) {
    if (name.empty()) return (SmeltSymbol*)SMELT_NULLPTR;
    std::map<std::string,SmeltSymbol*>::iterator symit = dictsym_.find (name);
    if (symit != dictsym_.end())
      return symit->second;
    return (SmeltSymbol*)SMELT_NULLPTR;
  };
  static SmeltSymbol* find(const char* s) {
    if (!s) return (SmeltSymbol*)SMELT_NULLPTR;
    std::string str(s);
    return find(str);
  }
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
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
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
    return (_argkind==SmeltArg_Symbol)?_argsymbol:(SmeltSymbol*)SMELT_NULLPTR;
  };
  SmeltVector* vector_ptr() const {
    return  (_argkind==SmeltArg_Vector)?_argvector:(SmeltVector*)SMELT_NULLPTR;
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
  SmeltArg() : _argkind(SmeltArg_None), _argptr(SMELT_NULLPTR) {};
  SmeltArg(const SmeltArg& a)
    : _argkind(a._argkind), _argptr(SMELT_NULLPTR) {
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
      assert (_argsymbol != SMELT_NULLPTR);
      break;
    case SmeltArg_Vector:
      _argvector = new SmeltVector(*a._argvector);
      break;
    }
  };
  SmeltArg(SmeltArg& a) : _argkind(a._argkind) {
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
      assert (_argsymbol != SMELT_NULLPTR);
      break;
    case SmeltArg_Vector:
      _argvector = a._argvector;
      break;
    }
    const_cast<SmeltArgKind&>(a._argkind) = SmeltArg_None;
    a._argptr = SMELT_NULLPTR;
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
      _argsymbol = (SmeltSymbol*)SMELT_NULLPTR;
      break;
    case SmeltArg_Vector:
      delete _argvector;
      break;
    }
    _argptr = SMELT_NULLPTR;
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
      assert (_argsymbol != SMELT_NULLPTR);
      break;
    case SmeltArg_Vector:
      _argvector = new SmeltVector(*a._argvector);
      break;
    }
    const_cast<SmeltArgKind&>(_argkind) = a._argkind;
    return *this;
  }
  explicit SmeltArg(void*) : _argkind(SmeltArg_None), _argptr(SMELT_NULLPTR) {};
  explicit SmeltArg(long l) : _argkind(SmeltArg_Long), _arglong(l) {};
  explicit SmeltArg(double d) : _argkind(SmeltArg_Double), _argdouble(d) {};
  explicit SmeltArg(const std::string& s)
    : _argkind(SmeltArg_String), _argptr(SMELT_NULLPTR) {
    _argstring = new std::string(s);
  }
  explicit SmeltArg(SmeltSymbol* s): _argkind(SmeltArg_Symbol), _argsymbol(s) {
    assert(s!=SMELT_NULLPTR);
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
};        // end class SmeltArg


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
  size_t ln = v.size();
  for (size_t ix=0; ix<ln; ix++) {
    if (ix>0) os << " ";
    os << v[ix];
  }
  os << ")";
  return os;
}


void
SmeltArg::out(std::ostream& os) const
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
    {
      size_t sln = _argstring->size();
      for (size_t six=0; six<sln; six++) {
        char c = _argstring->at(six);
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
    }
    os << "\"";
    break;
  case SmeltArg_Symbol:
    assert (_argsymbol != SMELT_NULLPTR);
    os << (_argsymbol->name());
    break;
  case SmeltArg_Vector: {
    size_t nb=0;
    assert (_argvector != SMELT_NULLPTR);
    size_t ln=_argvector->size();
    os << "(";
    for (nb=0; nb<ln; nb++) {
      if (nb>0)
        os << " ";
      _argvector->at(nb).out(os);
      nb++;
    }
    os << ")";
  }
  break;
  }
}

class SmeltCommandSymbol : public SmeltSymbol {
public:
  typedef void (SmeltApplication::*cmdfun_t)(SmeltVector&);
  friend class SmeltApplication;
private:
  cmdfun_t _cmdfun;
public:
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
  SmeltCommandSymbol(const std::string&name, cmdfun_t fun)
    : SmeltSymbol(name), _cmdfun(fun) {
  };
  ~SmeltCommandSymbol() {
    _cmdfun = 0;
  };
  cmdfun_t fun() {
    return _cmdfun;
  };
  void call(SmeltApplication*, SmeltVector&);
};        // end class SmeltCommandSymbol


class SmeltTagSymbol : public SmeltSymbol {
  SmeltTagSymbol(const std::string&name) : SmeltSymbol(name) {};
  virtual ~SmeltTagSymbol() {};
public:
  static SmeltTagSymbol* create(const std::string&name) {
    g_assert (SmeltSymbol::find(name) == SMELT_NULLPTR);
    SmeltTagSymbol *tgsym = new SmeltTagSymbol(name);
    return tgsym;
  };
  static SmeltTagSymbol* register_tag(Glib::RefPtr<Gtk::TextTag>tgref) {
    if (!tgref) return (SmeltTagSymbol*)SMELT_NULLPTR;
    const Glib::ustring& tgname = tgref->property_name().get_value();
    SMELT_DEBUG("tgname=" << tgname);
    if (!tgname.empty())
      return SmeltTagSymbol::create(tgname);
    return (SmeltTagSymbol*)SMELT_NULLPTR;
  }
};

class SmeltApplication
    : public Gtk::Application {
  static SmeltApplication* _application;
  Glib::RefPtr<Gsv::LanguageManager> _app_langman;
  Glib::RefPtr<SmeltMainWindow> _app_mainwinp;  // main window
  Glib::RefPtr<SmeltTraceWindow> _app_tracewin;
  bool _app_traced;
  Glib::RefPtr<Glib::IOChannel> _app_reqchan_to_melt; // channel for request to MELT
  Glib::RefPtr<Glib::IOChannel> _app_cmdchan_from_melt; // channel for commands from MELT
  std::string _app_reqname_to_melt;     // human-readable name for request channel to MELT
  std::string _app_cmdname_from_melt;     // human-readable name for command channel from MELT
  sigc::connection _app_connreq_to_melt;
  sigc::connection _app_conncmd_from_melt;
  std::ostringstream _app_writestream_to_melt; // string buffer for requests to MELT
  std::string _app_cmdstr_from_melt;         // the last command from MELT
  Glib::RefPtr<Gdk::Pixbuf> _app_indifferent_pixbuf;
  Glib::RefPtr<Gdk::Pixbuf> _app_key_7x11_pixbuf;
  Glib::RefPtr<Gdk::Pixbuf> _app_key_16x16_pixbuf;
protected:
  bool reqbuf_to_melt_cb(Glib::IOCondition);
  bool cmdbuf_from_melt_cb(Glib::IOCondition);
  void process_command_from_melt (std::string& str);
  void initialize(void) {
    SMELT_DEBUG("start of initialize");
    if (_app_mainwinp) return;
    {
      SmeltMainWindow* mainwin = new SmeltMainWindow();
      SMELT_DEBUG("mainwin=" << (void*)mainwin);
      add_window(*mainwin);
      _app_mainwinp = Glib::RefPtr<SmeltMainWindow>(mainwin);
      mainwin->show_all();
    }
    _app_langman = Gsv::LanguageManager::get_default ();
    SMELT_DEBUG("done");
  };
public:
  void* thisptr() const {
    return (void*) this;
  }; // for debugging
  std::ostringstream& outreq() {
    return _app_writestream_to_melt;
  };
  const std::string& reqname_to_melt() const {
    return _app_reqname_to_melt;
  };
  const std::string& cmdname_from_melt() const {
    return _app_cmdname_from_melt;
  };
  void sendreq (std::ostream& os) {
    os << std::endl << std::endl;
    if (os == _app_writestream_to_melt) {
      os << std::flush;
      SMELT_DEBUG("sendreq " << _app_writestream_to_melt.str());
      if (_app_traced && _app_tracewin)
        _app_tracewin->add_reply_to_melt (_app_writestream_to_melt.str());
      (void) reqbuf_to_melt_cb(Glib::IO_OUT);
      if (!_app_connreq_to_melt) {
        SMELT_DEBUG("connecting requests");
        _app_connreq_to_melt
        = Glib::signal_io().connect(sigc::mem_fun(*this, &SmeltApplication::reqbuf_to_melt_cb),
                                    _app_reqchan_to_melt, Glib::IO_OUT);
      }
    }
  }
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
  static SmeltApplication* instance() {
    return _application;
  };
  SmeltApplication()
    : Gtk::Application
    ("org.gcc-melt.simple-probe",
     Gio::ApplicationFlags(Gio::APPLICATION_HANDLES_COMMAND_LINE
                           | Gio::APPLICATION_NON_UNIQUE)),
    _app_traced(false) {
    Gsv::init(); /// initialize GtkSourceviewMM very early!
    _application = this;
  };
  static Glib::RefPtr<SmeltApplication> create() {
    g_assert (_application == SMELT_NULLPTR);
    return Glib::RefPtr<SmeltApplication> (new SmeltApplication());
  }
  void set_reqchan_to_melt(const std::string &reqname) {
    const char* reqcstr = reqname.c_str();
    SMELT_DEBUG("reqname=" << reqname);
    char* endstr = (char*)SMELT_NULLPTR;
    long reqfd = strtol(reqcstr, &endstr, 10);
    struct stat reqstat = {};
    if (reqfd > 0 && reqfd < sysconf(_SC_OPEN_MAX)
        && endstr && endstr[0] == (char)0
        && !fstat(reqfd, &reqstat)) {
      SMELT_DEBUG("request to MELT channel fd#" << reqfd);
      _app_reqchan_to_melt = Glib::IOChannel::create_from_fd(reqfd);
    } else if ((reqfd = ::open(reqcstr, O_RDONLY|O_CLOEXEC)) >= 0) {
      SMELT_DEBUG("request to MELT channel file " << reqcstr << " fd#" << reqfd);
      _app_reqchan_to_melt = Glib::IOChannel::create_from_fd(reqfd);
    } else
      SMELT_FATAL("cannot open request to MELT channel " << reqname);
    // don't connect now
    _app_reqname_to_melt = reqname;
  }
  void set_cmdchan_from_melt(const std::string &cmdname) {
    const char* cmdcstr = cmdname.c_str();
    SMELT_DEBUG("cmdname=" << cmdname);
    char* endstr = 0;
    long cmdfd = strtol(cmdcstr, &endstr, 10);
    struct stat cmdstat = {};
    if (cmdfd >= 0 && cmdfd < sysconf(_SC_OPEN_MAX)
        && endstr && endstr[0] == (char)0
        && !fstat(cmdfd, &cmdstat)) {
      SMELT_DEBUG("command from MELT channel fd#" << cmdfd);
      _app_cmdchan_from_melt = Glib::IOChannel::create_from_fd(cmdfd);
    } else if ((cmdfd = ::open(cmdcstr, O_RDONLY|O_CLOEXEC)) >= 0) {
      SMELT_DEBUG("command from MELT channel file " << cmdcstr << " fd#" << cmdfd);
      _app_cmdchan_from_melt = Glib::IOChannel::create_from_fd(cmdfd);
    } else
      SMELT_FATAL("cannot open command from MELT channel " << cmdname);
    _app_conncmd_from_melt =
      Glib::signal_io().connect(sigc::mem_fun(*this, &SmeltApplication::cmdbuf_from_melt_cb),
                                _app_cmdchan_from_melt, Glib::IO_IN);
    _app_cmdname_from_melt = cmdname;
  }
  ~SmeltApplication() {
  };
  //Overrides of default signal handlers
  virtual int
  on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& cmdline) {
    int argc = 0;
    char** argv = cmdline->get_arguments(argc);
    SMELT_DEBUG("on_command_line argc=" << argc << " argv=" << argv);
    Glib::OptionContext optcontext;
    SmeltOptionGroup optgroup;
    optcontext.set_main_group(optgroup);
    try {
      optcontext.parse(argc,argv);
      SMELT_DEBUG("on_command_line parsed _app_traced=" << _app_traced);
      optgroup.setup_appl(*this);
      SMELT_DEBUG("on_command_line after setup_appl _app_traced=" << _app_traced);
    } catch (const Glib::Error& ex) {
      std::cerr
          << argv[0]
          << " got exception when parsing command line:" << ex.what() << std::endl;
      std::cerr << optcontext.get_help() << std::endl;
      return EXIT_FAILURE;
    }
    SMELT_DEBUG ("on_command_line done");
    return EXIT_SUCCESS;
  }
  virtual void
  on_activate(void) {
    SMELT_DEBUG ("activated");
    initialize ();
    Gtk::Application::on_activate();
  }
  void set_trace(bool =true);
  void on_trace_toggled(void);
  /// command handlers
  void tracemsg_cmd(SmeltVector&);
  void quit_cmd(SmeltVector&);
  void echo_cmd(SmeltVector&);
  void showfile_cmd(SmeltVector&);
  void marklocation_cmd(SmeltVector&);
  void startinfoloc_cmd(SmeltVector&);
  void addinfoloc_cmd(SmeltVector&);
  void clearstatus_cmd(SmeltVector&);
  void pushstatus_cmd(SmeltVector&);
  void popstatus_cmd(SmeltVector&);
  void setstatus_cmd(SmeltVector&);
};        // end class SmeltApplication

// for debugging
const char* smelt_gobject_type_name (GObject* ob)
{
  if (!ob) return "*null*";
  return G_OBJECT_TYPE_NAME(ob);
}

SmeltApplication*  SmeltApplication::_application;

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
    char* endlng = 0;
    char* enddbl = 0;
    const char* cstr = s.c_str() + pos;
    long l = strtol(cstr, &endlng, 0);
    assert(endlng != SMELT_NULLPTR);
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
          char* end = 0;
          long l = strtol(ds, &end, 8);
          assert (end != 0);
          pos += 2 + end - ds;
          str += (char) l;
          break;
        }
        case 'x': {
          std::string xs;
          pos += 2;
          if (std::isxdigit(s[pos])) xs += s[pos++];
          if (std::isxdigit(s[pos])) xs += s[pos++];
          long l = strtol(xs.c_str(), 0, 16);
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
      return SmeltArg(SMELT_NULLPTR);
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
SmeltFile::SmeltFile(SmeltMainWindow*mwin,const std::string&filepath,long num)
  : _sfilnum(num), _sfilnblines(0), _sfilname(filepath), _sfilview()
{
  SMELT_DEBUG("constructing filepath=" << filepath << " num=" << num << " this@" << (void*)this);
  assert(mwin != SMELT_NULLPTR);
  if (access(filepath.c_str(), R_OK))
    SMELT_FATAL("invalid filepath " << filepath);
  if (num == 0 || mainsfilemapnum_.find(num) != mainsfilemapnum_.end())
    throw smelt_domain_error("SmeltFile: invalid shown file number",smelt_long_to_string(num));
  if (filepath.empty() || mainsfiledict_.find(filepath) != mainsfiledict_.end())
    throw smelt_domain_error("SmeltFile: invalid shown file name",filepath);
  Glib::RefPtr<Gsv::Buffer> sbuf = _sfilview.get_source_buffer();
  SMELT_DEBUG("guessing language for filepath=" << filepath);
  // sometimes no language is guessed, i.e. when #include-ing foo.def
  Glib::RefPtr<Gsv::LanguageManager> langman = SmeltApplication::instance()->langman();
  Glib::RefPtr<Gsv::Language> lang = langman->guess_language(filepath,std::string());
  if (lang) {
    SMELT_DEBUG("guessed lang id=" << (lang->get_id().c_str()) <<
		" name=" << (lang->get_name().c_str()));
    sbuf->set_language(lang);
  }
  _sfilview.set_editable(false);
  _sfilview.set_show_line_numbers(true);
  _sfilview.set_show_line_marks(true);
  {
    int markprio=1;
    Glib::RefPtr<Gsv::MarkAttributes> markattrs = Gsv::MarkAttributes::create();
    markattrs->set_stock_id(SMELT_MARKLOC_STOCKID);
    _sfilview.set_mark_attributes(SMELT_MARKLOC_CATEGORY,markattrs,markprio);
  }
  {
    std::ifstream infil(filepath.c_str());
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
    Gtk::ScrolledWindow* scrowin = new Gtk::ScrolledWindow;
    scrowin->add (_sfilview);
    scrowin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    const Glib::ustring& labstr = Glib::ustring::compose("<span color='darkblue'>#%1</span>\n"
                                  "<tt>%2</tt>", num, Glib::path_get_basename(filepath));
    Gtk::Label* labtit = new Gtk::Label;
    {
      char* realfilpath = realpath(filepath.c_str(),0);
      labtit->set_markup(Glib::ustring::compose("<span color='blue'>#%1</span>\n"
                         "<span size='small'><tt>%2</tt></span>",
                         num, realfilpath));
      labtit->set_justify(Gtk::JUSTIFY_CENTER);
      free(realfilpath);
    }
    Gtk::VBox* vbox = new Gtk::VBox(false,2);
    vbox->pack_start(*Gtk::manage(labtit),Gtk::PACK_SHRINK);
    vbox->pack_start(*Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_HORIZONTAL)),Gtk::PACK_SHRINK);
    vbox->pack_start(*Gtk::manage(scrowin),Gtk::PACK_EXPAND_WIDGET);
    mwin->notebook_append_page(*Gtk::manage(vbox),labstr);
  }
  mwin->postpone_show_all_from(__LINE__);
  mainsfilemapnum_[num] = this;
  mainsfiledict_[filepath] = this;
}


////////////////////////////////////////////////////////////////
SmeltFile::SmeltFile(SmeltMainWindow*mwin,SmeltFile::pseudofile_en pseudokind,long num)
  : _sfilnum(num), _sfilnblines(0), _sfilname(), _sfilview()
{
  SMELT_DEBUG("constructing pseudokind#" << (int)pseudokind << " num=" << num << " this@" << (void*)this);
  assert(mwin != SMELT_NULLPTR);
  std::string pseudopath; // no < inside, because it goes into some Pango Markup
  switch (pseudokind) {
  case PseudoFile_None:
    SMELT_FATAL("invalid pseudo file ");
    break;
  case PseudoFileBuiltin:
    pseudopath="**built-in**";
    break;
  case PseudoFileCommand:
    pseudopath="**command**";
    break;
  }
  _sfilname = pseudopath;
  if (num == 0 || mainsfilemapnum_.find(num) != mainsfilemapnum_.end())
    throw smelt_domain_error("SmeltFile: invalid shown file number",smelt_long_to_string(num));
  if (pseudopath.empty() || mainsfiledict_.find(pseudopath) != mainsfiledict_.end())
    throw smelt_domain_error("SmeltFile: invalid shown pseudo file",pseudopath);
  Glib::RefPtr<Gsv::Buffer> sbuf = _sfilview.get_source_buffer();
  sbuf->insert(sbuf->end(),"pseudo file ");
  sbuf->insert(sbuf->end(),pseudopath);
  sbuf->insert(sbuf->end(),"\n");
  SMELT_DEBUG("filled for " << pseudopath);
  _sfilview.set_editable(false);
  _sfilview.set_show_line_numbers(true);
  _sfilview.set_show_line_marks(true);
  {
    int markprio=1;
    Glib::RefPtr<Gsv::MarkAttributes> markattrs = Gsv::MarkAttributes::create();
    markattrs->set_stock_id(SMELT_MARKLOC_STOCKID);
    _sfilview.set_mark_attributes(SMELT_MARKLOC_CATEGORY,markattrs,markprio);
  }
  {
    Gtk::ScrolledWindow* scrowin = new Gtk::ScrolledWindow;
    scrowin->add (_sfilview);
    scrowin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    const Glib::ustring& labstr = Glib::ustring::compose("<span color='darkblue'>#%1</span>\n"
                                  "<b>%2</b>", num, pseudopath);
    Gtk::Label* labtit = new Gtk::Label;
    {
      labtit->set_markup(Glib::ustring::compose("<span color='blue'>#%1</span>\n"
                         "<span size='small' color='brown'><b>Pseudo File</b> <tt>%2</tt></span>",
                         num, pseudopath));
      labtit->set_justify(Gtk::JUSTIFY_CENTER);
    }
    Gtk::VBox* vbox = new Gtk::VBox(false,2);
    vbox->pack_start(*Gtk::manage(labtit),Gtk::PACK_SHRINK);
    vbox->pack_start(*Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_HORIZONTAL)),Gtk::PACK_SHRINK);
    vbox->pack_start(*Gtk::manage(scrowin),Gtk::PACK_EXPAND_WIDGET);
    scrowin->show_all();
    mwin->notebook_append_page(*Gtk::manage(vbox),labstr);
  }
  mainsfilemapnum_[num] = this;
  mainsfiledict_[pseudopath] = this;
  mwin->postpone_show_all_from(__LINE__);
  SMELT_DEBUG("done num=" << num << " for pseudopath" << pseudopath);
}




SmeltFile::~SmeltFile()
{
  SMELT_DEBUG("destructing this@" << (void*)this);
  assert (mainsfilemapnum_[_sfilnum] == this);
  assert (mainsfiledict_[_sfilname] == this);
  mainsfilemapnum_.erase (_sfilnum);
  mainsfiledict_.erase (_sfilname);
}



void
SmeltMainWindow::postpone_show_all_from(int lin)
{
  if (_mainshowconn) 
    return;
  SMELT_DEBUG("postponed show all from line " << lin);
  _mainshowconn = 
    Glib::signal_timeout().connect(sigc::mem_fun(*this,&SmeltMainWindow::postpone_show_all_cb),
				   200);
  _mainshowfromline = lin;
}

// callback for timeout...
bool 
SmeltMainWindow::postpone_show_all_cb (void)
{
  int fromlin =  _mainshowfromline;
  _mainshowfromline = 0;
  _mainshowconn.disconnect();
  SMELT_DEBUG("postponed show all from line " << fromlin);
  show_all();
  return false;
}

void
SmeltMainWindow::show_file(const std::string&path, long num)
{
  SMELT_DEBUG("show_file path:" << path << " num:" << num);
  if (path.at(0)=='<') {
    // handle specially pseudo file names like <built-in>
    if (!path.compare("<built-in>"))
      new SmeltFile(this,SmeltFile::PseudoFileBuiltin,num);
    else
      throw smelt_domain_error("invalid pseudo file to show", path);
  } else
    new SmeltFile(this,path,num);
  postpone_show_all_from(__LINE__);
}


void
SmeltMainWindow::mark_location(long marknum,long filenum,int lineno, int col)
{
  SMELT_DEBUG("marknum=" << marknum << " filenum=" << filenum
              << " lineno=" << lineno << " col=" << col);
  SmeltFile* sfil = SmeltFile::by_number (filenum);
  if (!sfil)
    throw smelt_domain_error("mark_location invalid file number",
                             smelt_long_to_string(filenum));
  if (lineno<0 || lineno>sfil->nblines())
    throw smelt_domain_error("mark_location invalid line number",
                             smelt_long_to_string(lineno));
  if (marknum==0)
    throw smelt_domain_error("mark_location invalid number",
                             smelt_long_to_string(marknum));
  if (shown_location_by_number(marknum))
    throw smelt_domain_error("mark_location duplicate number",
                             smelt_long_to_string(marknum));
  Glib::RefPtr<Gsv::Buffer> tbuf = sfil->view().get_source_buffer();
  Gtk::TextIter itlin = (lineno>0)?(tbuf->get_iter_at_line (lineno-1)):(tbuf->begin());
  Gtk::TextIter itendlin = itlin;
  itendlin.forward_line();
  itendlin.backward_char();
  int linwidth = itendlin.get_line_offset();
  if (col<=0 || linwidth==0)
    col = 1;
  else if (col>linwidth)
    col = linwidth;
  SMELT_DEBUG ("linwidth=" << linwidth << " normalized col=" << col);

  /* create_source_mark works only at the start of the line */
  Glib::RefPtr<Gsv::Mark> linemark = tbuf->create_source_mark(SMELT_MARKLOC_CATEGORY,itlin);
  /* add the button inside the line */
  itlin = tbuf->get_iter_at_line (lineno-1);
  Gtk::TextIter itcur = itlin;
  if (col>1)
    itcur.forward_chars(col-1);
  SMELT_DEBUG("itcur=" << itcur);
  SmeltLocationInfo* inf = new SmeltLocationInfo(marknum,sfil,lineno,col);
  mainlocinfmapnum_[marknum] = inf;

  Gtk::Button* but = new Gtk::Button("*");
  Glib::RefPtr<Gtk::TextChildAnchor> chanch = Gtk::TextChildAnchor::create ();
  tbuf->insert_child_anchor(itcur,chanch);
  sfil->view().add_child_at_anchor(*but,chanch);
  but->signal_clicked().connect(sigc::mem_fun(*inf,
                                &SmeltLocationInfo::on_update));
  but->show();
  sfil->view().queue_draw();
  postpone_show_all_from(__LINE__);
  SMELT_DEBUG("added mark but@" << (void*) but);
}



void
SmeltLocationInfo::initialize (void)
{
  typedef Glib::RefPtr<Gtk::TextTag> tagref_t;
  g_assert (!sli_tagtbl_);
  sli_tagtbl_ = Gtk::TextTagTable::create();
  SMELT_DEBUG("created tagtbl gobj@" << (void*)sli_tagtbl_->gobj());
  {
    tagref_t tagtitle = Gtk::TextTag::create ("title");
    tagtitle->property_weight() = Pango::WEIGHT_BOLD;
    tagtitle->property_scale() = Pango::SCALE_X_LARGE;
    tagtitle->property_foreground() = "FireBrick";
    SmeltTagSymbol::register_tag(tagtitle);
    sli_tagtbl_->add(tagtitle);
  }
  {
    tagref_t tagsubtitle = Gtk::TextTag::create ("subtitle");
    tagsubtitle->property_weight() = Pango::WEIGHT_BOLD;
    tagsubtitle->property_scale() = Pango::SCALE_LARGE;
    tagsubtitle->property_foreground() = "Navy";
    SmeltTagSymbol::register_tag(tagsubtitle);
    sli_tagtbl_->add(tagsubtitle);
  }
  {
    tagref_t tagbold = Gtk::TextTag::create ("bold");
    tagbold->property_weight() = Pango::WEIGHT_BOLD;
    SmeltTagSymbol::register_tag(tagbold);
    sli_tagtbl_->add(tagbold);
  }
  {
    tagref_t tagitalic = Gtk::TextTag::create ("italic");
    tagitalic->property_style() = Pango::STYLE_OBLIQUE;
    SmeltTagSymbol::register_tag(tagitalic);
    sli_tagtbl_->add(tagitalic);
  }
}

/* When the user press a location button, on_update is called. It
   should send an INFOLOCATION_prq request to MELT, which should
   respond with one STARTINFOLOC_pcd followed by zero, one or more
   ADDINFOLOC_pcd commands */
void
SmeltLocationInfo::on_update(void)
{
  SMELT_DEBUG("updating loc#" << _sli_num);
  SmeltApplication::instance()->sendreq(SmeltApplication::instance()->outreq()
                                        << "INFOLOCATION_prq " << _sli_num);
  g_assert (sli_tagtbl_);
  if (!_sli_dial)
    _sli_dial = new SmeltLocationDialog(this);
}




void
SmeltLocationInfo::on_dialog_response(int resp)
{
  SMELT_DEBUG("responding loc#" << _sli_num << " resp=" << resp);
  switch (resp) {
  case Gtk::RESPONSE_CLOSE:
    SMELT_DEBUG("RESPONSE_CLOSE");
    break;
  case Gtk::RESPONSE_DELETE_EVENT:
    SMELT_DEBUG("RESPONSE_DELETE_EVENT");
    break;
  default:
    break;
  }
  {
    _sli_dial->hide();
    // we cannot destroy the dial inside this event, we queue to
    // destroy it latter
    Glib::signal_idle().connect_once(sigc::mem_fun(this,&SmeltLocationInfo::destroy_dialog));
  }
}

SmeltLocationDialog::SmeltLocationDialog(SmeltLocationInfo*info)
  : Gtk::MessageDialog
  (Glib::ustring::compose("MELT info at <tt>%1</tt> <small>L%2C%3</small>",
                          basename(info->sfile()->name().c_str()), info->lineno(), info->col()),
 /*use_markup*/ true,
   Gtk::MESSAGE_INFO,
   Gtk::BUTTONS_CLOSE,
 /*modal*/ false),
  _sld_info(info),
  _sld_view(),
  _sld_swin()
{
  SMELT_DEBUG("constructing this@" << (void*)this);
  this->set_resizable (true);
  Glib::RefPtr<Gtk::TextBuffer> tbuf = Gtk::TextBuffer::create (SmeltLocationInfo::the_tag_table());
  _sld_view.set_buffer (tbuf);
  _sld_view.set_editable (false);
  _sld_swin.set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  _sld_swin.add(_sld_view);
  _sld_swin.set_min_content_height(200);
  set_title(Glib::ustring::compose("MELT probe info #%1", info->num()));
  set_secondary_text
  (Glib::ustring::compose
   ("<i>MELT</i> about <tt>%1</tt> [#%2]\n"
    "line <b>%3</b> column <i>%4</i>",
    info->sfile()->name(), info->sfile()->number(), info->lineno(), info->col()),
   /*use_markup:*/ true);
  get_content_area()->pack_start(_sld_swin,
                                 /*expand:*/ true,
                                 /*fill:*/ true,
                                 /*padding:*/ 2);
  SMELT_DEBUG("constructing location dialog for info#" << info->num() << " @" << (void*)this);
  signal_response().connect(sigc::mem_fun(*info,&SmeltLocationInfo::on_dialog_response));
}

SmeltLocationDialog::~SmeltLocationDialog()
{
  SMELT_DEBUG("destructing this@" << (void*)this << " for info #" << _sld_info->num());
  _sld_info = 0;
}


void
SmeltLocationDialog::append_buffer(const std::string& s, const std::string &tagname)
{
  Glib::RefPtr<Gtk::TextBuffer> tb = tbuf();
  SMELT_DEBUG ("s=" << s << " tagname=" << tagname << " this@" << (void*)this);
  if (!tb) return;
  bool withtag = !tagname.empty();
  if (withtag)
    push_tag (tagname);
  /* Explicit conversion to Glib::ustring to avoid infinite recursion.  */
  append_buffer (Glib::ustring(s));
  if (withtag)
    pop_tag ();
}

void
SmeltLocationDialog::append_buffer(const SmeltArg&a, const std::string &tagname)
{
  Glib::RefPtr<Gtk::TextBuffer> tb = tbuf();
  SMELT_DEBUG ("a=" << a << " tagname=" << tagname << " this@" << (void*)this);
  if (!tb) return;
  bool withtag = !tagname.empty();
  if (withtag)
    push_tag (tagname);
  switch (a.kind()) {
  case SmeltArg_None:
    break;
  case SmeltArg_Long: {
    long l = a.as_long();
    char str[48];
    snprintf(str, sizeof(str), "%ld", l);
    /* Explicit conversion to Glib::ustring to avoid infinite recursion.  */
    append_buffer(Glib::ustring(str));
  };
  case SmeltArg_Double: {
    double x = a.as_double();
    char str[64];
    snprintf(str, sizeof(str), "%g", x);
    /* Explicit conversion to Glib::ustring to avoid infinite recursion.  */
    append_buffer(Glib::ustring(str));
  };
  break;
  case SmeltArg_String: {
    const std::string& s = a.as_string();
    /* Explicit conversion to Glib::ustring to avoid infinite recursion.  */
    append_buffer (Glib::ustring(s));
  }
  break;
  case SmeltArg_Symbol: {
    SmeltSymbol* sy = a.symbol_ptr();
    if (sy)
      /* Explicit conversion to Glib::ustring to avoid infinite recursion.  */
      append_buffer (Glib::ustring(sy->name()));
  }
  break;
  case SmeltArg_Vector: {
    SmeltVector* vec = a.vector_ptr();
    Glib::RefPtr<Gtk::TextTag> tagfirstref;
    if (vec && vec->size() > 0) {
      SmeltSymbol* firstsy = vec->at(0).symbol_ptr();
      if (firstsy)
        tagfirstref = find_tag(Glib::ustring(firstsy->name()).lowercase());
      if (tagfirstref)
        push_tag(tagfirstref);
      for (unsigned ix = 1; ix < vec->size(); ix++)
        append_buffer (vec->at(ix));
      if (tagfirstref)
        pop_tag();
    }
  }
  break;
  }
  if (withtag)
    pop_tag ();
}

void
SmeltMainWindow::showinfo_location(long marknum)
{
  SMELT_DEBUG("start marknum=" << marknum << " this@" << (void*)this);
  SmeltLocationInfo* inf = shown_location_by_number(marknum);
  if (inf == SMELT_NULLPTR) {
    SMELT_DEBUG("invalid marknum=" << marknum);
    throw smelt_domain_error("showinfo_location invalid mark number",
                             smelt_long_to_string(marknum));
  }
  SMELT_DEBUG ("inf @" << ((void*)inf)
               << " with dialog@" << (void*) (inf->dialog()));
  g_assert (inf->dialog());
  inf->dialog()->show_all();
  SMELT_DEBUG ("inf did show all dialog@" << (void*) (inf->dialog()));
}

void
SmeltMainWindow::addinfo_location(long marknum, const std::string& title, const SmeltArg&arg)
{
  SMELT_DEBUG("start marknum=" << marknum << " this@" << (void*)this);
  SmeltLocationInfo* inf = shown_location_by_number(marknum);
  if (inf == SMELT_NULLPTR) {
    SMELT_DEBUG("invalid marknum=" << marknum);
    throw smelt_domain_error("addinfo_location invalid mark number",
                             smelt_long_to_string(marknum));
  }
  SmeltLocationDialog* dial = inf->dialog();
  g_assert (dial != SMELT_NULLPTR);
  if (!title.empty()) {
    dial->append_buffer(title,"title");
    dial->append_buffer(Glib::ustring("\n"));
  }
  dial->append_buffer(arg);
  dial->append_buffer(Glib::ustring("\n"));
}

////////////////////////////////////////////////////////////////
guint
SmeltMainWindow::push_status(const std::string&msg)
{
  postpone_show_all_from(__LINE__);
  return _mainstatusbar.push(msg);
}


void
SmeltMainWindow::pop_status(void)
{
  postpone_show_all_from(__LINE__);
  _mainstatusbar.pop();
}


void
SmeltMainWindow::remove_status(guint msgid)
{
  postpone_show_all_from(__LINE__);
  _mainstatusbar.remove_message(msgid);
}


void
SmeltMainWindow::remove_all_status(void)
{
  postpone_show_all_from(__LINE__);
  _mainstatusbar.remove_all_messages();
}


void
SmeltMainWindow::send_quit_req(void)
{
  SmeltApplication::instance()->sendreq(SmeltApplication::instance()->outreq() << "QUIT_prq");
  Glib::signal_timeout().connect_once(sigc::ptr_fun(&smelt_quit),250);
}


void
SmeltMainWindow::on_version_show(void)
{
  SMELT_DEBUG("on_version_show start");
  Gtk::AboutDialog dial;
  dial.set_program_name (basename(__FILE__));
  dial.set_version(std::string("$Id$")
                   + " built @" __DATE__);
  dial.set_copyright("Free Sofware Foundation");
  dial.set_comments("a simple Gtk probe communicating with MELT extension to GCC");
  dial.set_license("Gnu Public License version 3");
  dial.set_license_type(Gtk::LICENSE_GPL_3_0);
  dial.set_website("http://gcc-melt.org/");
  {
    std::vector<Glib::ustring> authorsvec;
    authorsvec.push_back ("Basile Starynkevitch");
    dial.set_authors (authorsvec);
  }
  SmeltApplication::instance()->sendreq(SmeltApplication::instance()->outreq() << "VERSION_prq");
  int res = dial.run();
  SMELT_DEBUG("got res=" << res);
  postpone_show_all_from(__LINE__);
}

////////////////////////////////////////////////////////////////
void
SmeltTraceWindow::add_title(const std::string &str)
{
  Glib::RefPtr<Gtk::TextBuffer> tbuf =  _tracetextview.get_buffer();
  tbuf->insert_with_tag(tbuf->end(), str, "title");
  if (str.empty() || str[str.size()-1] != '\n')
    tbuf->insert(tbuf->end(), "\n");
  add_date();
  tbuf->insert(tbuf->end(), "\n");
}



void
SmeltTraceWindow::on_trace_clear(void)
{
  static long nbclear;
  nbclear++;
  Glib::RefPtr<Gtk::TextBuffer>  tbuf =  _tracetextview.get_buffer();
  tbuf->erase(tbuf->begin(), tbuf->end());
  std::ostringstream out;
  out << "Trace Erased (" << nbclear << "th time)";
  add_title(out);
}


void
SmeltTraceWindow::add_date(const std::string &s)
{
  struct timeval tv= {0,0};
  gettimeofday (&tv,0);
  time_t now = tv.tv_sec;
  char timbuf[64];
  Glib::RefPtr<Gtk::TextBuffer>  tbuf =  _tracetextview.get_buffer();
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
  set_title (Glib::ustring::compose("MELT probe pid %1 trace", (int)(getpid())));
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
    _traceactgroup->add(Gtk::Action::create("TraceMenuFile", "_File"));
    _traceactgroup->add(Gtk::Action::create("TraceMenuTrace", "Trace"),
                        sigc::mem_fun(*SmeltApplication::instance(), &SmeltApplication::on_trace_toggled));
    _traceactgroup->add(Gtk::Action::create("TraceMenuClear", "Clear"),
                        sigc::mem_fun(*this,&SmeltTraceWindow::on_trace_clear));
    Glib::RefPtr<Gtk::UIManager> uimgr  = Gtk::UIManager::create();
    uimgr->insert_action_group(_traceactgroup);
    Glib::ustring ui_info=
      "<ui>"
      "<menubar name='TraceMenuBar'>"
      "<menu action='TraceMenuFile'>"
      "<menuitem action='TraceMenuTrace'/>"
      "<menuitem action='TraceMenuClear'/>"
      "</menu>"
      "</menubar>"
      "</ui>";
    try {
      uimgr->add_ui_from_string(ui_info);
    } catch (const Glib::Error& ex) {
      SMELT_FATAL("failed to build UI " << ex.what());
    }
    _tracevbox.pack_start(_tracelabel,Gtk::PACK_SHRINK);
    {
      Gtk::Widget* menubarp = uimgr->get_widget("/TraceMenuBar");
      if (menubarp)
        _tracevbox.pack_start(*menubarp,Gtk::PACK_SHRINK);
    }
    _traceitemcheck = Glib::RefPtr<Gtk::CheckMenuItem>(dynamic_cast<Gtk::CheckMenuItem*>(uimgr->get_widget("/MenuBar/MenuTrace")));
  }
  _tracevbox.pack_start(_tracescrollw,Gtk::PACK_EXPAND_WIDGET);
  _tracescrollw.add(_tracetextview);
  _tracescrollw.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  _tracetextview.set_editable(false);
  {
    Glib::RefPtr<Gtk::TextBuffer> tbuf =  _tracetextview.get_buffer();
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
                          "Trace Window Gcc MELT simplemelt-gtkmm-probe\n",
                          "title");
    {
      time_t now = 0;
      time (&now);
      char buf[80];
      strftime (buf, sizeof(buf), "%c\n", localtime(&now));
      tbuf->insert_with_tag(tbuf->end(),
                            Glib::ustring::compose
                            ("start at %1\n", buf),
                            "date");
      tbuf->insert(tbuf->end(), "Commands from MELT are ");
      tbuf->insert_with_tag(tbuf->end(), "like this", "command");
      tbuf->insert(tbuf->end(), Glib::ustring::compose (" on %1\n",
                   SmeltApplication::instance()->cmdname_from_melt()));
      tbuf->insert(tbuf->end(), "Requests to MELT are ");
      tbuf->insert_with_tag(tbuf->end(), "like that", "reply");
      tbuf->insert(tbuf->end(), Glib::ustring::compose (" on %1\n",
                   SmeltApplication::instance()->reqname_to_melt()));
      tbuf->insert(tbuf->end(), __FILE__ " compiled " __DATE__ "@" __TIME__ "\n");
    }
  }
  show_all();
}


void
SmeltCommandSymbol::call(SmeltApplication* app, SmeltVector&v)
{
  ((*app).*(_cmdfun))(v);
}


void
SmeltTraceWindow::add_command_from_melt(const std::string& str)
{
  Glib::RefPtr<Gtk::TextBuffer>  tbuf =  _tracetextview.get_buffer();
  add_date();
  tbuf->insert_with_tag(tbuf->end(), str, "command");
  if (str.empty() || str[str.size()-1] != '\n')
    tbuf->insert(tbuf->end(), "\n");
}


void
SmeltTraceWindow::add_reply_to_melt(const std::string& str)
{
  Glib::RefPtr<Gtk::TextBuffer> tbuf =  _tracetextview.get_buffer();
  add_date();
  tbuf->insert_with_tag(tbuf->end(), str, "reply");
  if (str.empty() || str[str.size()-1] != '\n')
    tbuf->insert(tbuf->end(), "\n");
}

void
SmeltOptionGroup::setup_appl(SmeltApplication& app)
{
  SMELT_DEBUG("setup_appl _file_to_melt=" << _file_to_melt
              << " _file_from_melt=" << _file_from_melt
              << " _trace_melt=" << _trace_melt);
  if (!_file_to_melt.empty())
    app.set_reqchan_to_melt(_file_to_melt);
  if (!_file_from_melt.empty())
    app.set_cmdchan_from_melt(_file_from_melt);
  if (_trace_melt)
    app.set_trace(_trace_melt);
}

/* low level callback to write requests to MELT */
bool
SmeltApplication::reqbuf_to_melt_cb(Glib::IOCondition outcond)
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
    wcnt = 0;
    SMELT_DEBUG("before write woff=" << woff << " wsiz=" << wsiz);
    wsta = _app_reqchan_to_melt->write(wdata+woff, wsiz, wcnt);
    SMELT_DEBUG("after write wsta#" << (int) wsta << " wcnt=" << wcnt);
    switch (wsta) {
    case Glib::IO_STATUS_AGAIN:
    case Glib::IO_STATUS_NORMAL:
      if (wcnt > 0) {
        woff += wcnt;
        wsiz -= wcnt;
        _app_reqchan_to_melt->flush();
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
SmeltApplication::cmdbuf_from_melt_cb(Glib::IOCondition  incond)
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
SmeltApplication::set_trace(bool traced)
{
  SMELT_DEBUG(" traced=" << traced);
  if (traced) {
    if (!_app_tracewin)
      _app_tracewin = Glib::RefPtr<SmeltTraceWindow>(new SmeltTraceWindow());
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
SmeltApplication::on_trace_toggled(void)
{
  if (_app_tracewin)
    set_trace(_app_tracewin->tracing());
  else
    set_trace(true);
}

void
SmeltApplication::process_command_from_melt(std::string& str)
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
    SmeltSymbol& sym = v.at(0).to_symbol();
    SmeltCommandSymbol* csym = dynamic_cast<SmeltCommandSymbol*>(&sym);
    SMELT_DEBUG("csym@" << (void*)csym << (csym?" named ":"") << (csym?csym->name():""));
    if (!csym)
      throw smelt_domain_error("process_command_from_melt: invalid command", sym.name());
    csym->call(this,v);
  } catch (SmeltDomainErrorAt derr) {
    std::cerr << "Smelt domain error:" << derr.what() << std::endl;
    if (_app_traced)
      _app_tracewin->add_title(std::string("Domain error: ") + derr.what());
  } catch (SmeltParseErrorAt perr) {
    std::cerr << "Smelt parse error:" << perr.what() << std::endl;
    if (_app_traced)
      _app_tracewin->add_title(std::string("Parse error: ") + perr.what());
  } catch (std::exception ex) {
    std::cerr << "Command error:" << ex.what() << std::endl;
    if (_app_traced)
      _app_tracewin->add_title(std::string("Command error: ") + ex.what());
  }
}


////////////////////////////////////////////////////////////////
int
main (int argc, char** argv)
{
  const char* progname = (argc>0)?(argv[0]):"*Simple-Melt-Probe*";
  if (argc>1 && !strcmp(argv[1],"-D"))
    smelt_debugging = true;
  SMELT_DEBUG("start " << progname << " pid#" << (int)getpid());
  try {
    Glib::RefPtr<SmeltApplication> appl =
      SmeltApplication::create();
    SMELT_DEBUG("before application registration");
    appl->register_application ();
    SMELT_DEBUG("before application activation");
    appl->activate();
    SMELT_DEBUG("before location info initialize");
    SmeltLocationInfo::initialize ();
    SMELT_DEBUG("before application run");
    appl->run(argc, argv);
    SMELT_DEBUG("after application run");
  } catch (std::exception ex) {
    std::cerr << progname << " got exception: " << ex.what() << std::endl;
    exit(EXIT_FAILURE);
  } catch (Glib::Error opterr) {
    std::cerr << progname << " got Glib error: " << opterr.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/////////////// tracemsg_cmd
SmeltCommandSymbol smeltsymb_tracemsg_cmd("TRACEMSG_PCD",&SmeltApplication::tracemsg_cmd);

void
SmeltApplication::tracemsg_cmd(SmeltVector&v)
{
  std::string msg = v[1].to_string();
  SMELT_DEBUG("msg:" << msg);
  if (_app_traced && _app_tracewin)
    _app_tracewin->add_title(msg);
}

////////////// quit_cmd

SmeltCommandSymbol smeltsymb_quit_cmd("QUIT_PCD",&SmeltApplication::quit_cmd);


static void smelt_quit(void)
{
  SMELT_DEBUG("delayed quit");
  SmeltApplication::instance()->quit();
}

void
SmeltApplication::quit_cmd(SmeltVector&v)
{
  long delay = (v.size()>0)?v.at(1).as_long():0L;
  SMELT_DEBUG("QUIT command delay:" << delay);
  if (delay <= 0)
    quit ();
  else  /* delayed quit; the delay is in milliseconds */
    Glib::signal_timeout().connect_once(sigc::ptr_fun(&smelt_quit),delay);
}



////////////// echo_pcd command for echoing

SmeltCommandSymbol smeltsymb_echo_cmd("ECHO_PCD",&SmeltApplication::echo_cmd);

void
SmeltApplication::echo_cmd(SmeltVector&v)
{
  v.erase(v.begin());
  sendreq(outreq() << v);
}



//////////////// showfile_pcd <filename> <number> command to show a file

SmeltCommandSymbol smeltsymb_showfile_cmd("SHOWFILE_PCD",&SmeltApplication::showfile_cmd);

void
SmeltApplication::showfile_cmd(SmeltVector&v)
{
  std::string filnam = v.at(1).to_string();
  long num = v.at(2).to_long();
  SMELT_DEBUG("filnam=" << filnam << " num=" << num);
  _app_mainwinp->show_file(filnam,num);
}

//////////////// marklocation_pcd <marknum> <filenum> <lineno> <col> to mark a location

SmeltCommandSymbol smeltsymb_marklocation_cmd("MARKLOCATION_PCD",&SmeltApplication::marklocation_cmd);

void
SmeltApplication::marklocation_cmd(SmeltVector&v)
{
  long marknum = v.at(1).to_long();
  long filnum = v.at(2).to_long();
  long lineno = v.at(3).to_long();
  long col = v.at(4).to_long();
  SMELT_DEBUG("filnum=" << filnum << " lineno=" << lineno << " col=" << col);
  _app_mainwinp->mark_location(marknum,filnum,lineno,col);
}


//////////////// startinfoloc_pcd <marknum> <titlestring> to start information for a location


SmeltCommandSymbol smeltsymb_startinfoloc_cmd("STARTINFOLOC_PCD",&SmeltApplication::startinfoloc_cmd);

void
SmeltApplication::startinfoloc_cmd(SmeltVector&v)
{
  long marknum = v.at(1).to_long();
  SMELT_DEBUG("STARTINFOLOC marknum=" << marknum);
  _app_mainwinp->showinfo_location(marknum);
}

//////////////// addinfoloc_pcd <marknum> <subtitlestring> <content> to add  information for a location

SmeltCommandSymbol smeltsymb_addinfoloc_cmd("ADDINFOLOC_PCD",&SmeltApplication::addinfoloc_cmd);

void
SmeltApplication::addinfoloc_cmd(SmeltVector&v)
{
  long marknum = v.at(1).to_long();
  const std::string& title = v.at(2).as_string();
  SMELT_DEBUG("ADDINFOLOC marknum=" << marknum << " title=" << title);
  _app_mainwinp->addinfo_location(marknum, title, v.at(3));
}

////////////////////////////////////////////////////////////////
//////////////// clearstatus_pcd to clear the status stack

SmeltCommandSymbol smeltsymb_clearstatus_cmd("CLEARSTATUS_PCD",&SmeltApplication::clearstatus_cmd);

void
SmeltApplication::clearstatus_cmd(SmeltVector&)
{
  SMELT_DEBUG("CLEARSTATUS");
  _app_mainwinp->remove_all_status ();
}


//////////////// pushstatus_pcd to push a message on the status stack

SmeltCommandSymbol smeltsymb_pushstatus_cmd("PUSHSTATUS_PCD",&SmeltApplication::pushstatus_cmd);

void
SmeltApplication::pushstatus_cmd(SmeltVector&v)
{
  std::string str = v.at(1).to_string();
  SMELT_DEBUG("PUSHSTATUS " << str);
  _app_mainwinp->push_status (str);
}


//////////////// popstatus_pcd to push a message on the status stack

SmeltCommandSymbol smeltsymb_popstatus_cmd("POPSTATUS_PCD",&SmeltApplication::popstatus_cmd);

void
SmeltApplication::popstatus_cmd(SmeltVector&)
{
  SMELT_DEBUG("POPSTATUS");
  _app_mainwinp->pop_status ();
}


//////////////// setstatus_pcd to set a message, that is pop then push, on status stack

SmeltCommandSymbol smeltsymb_setstatus_cmd("SETSTATUS_PCD",&SmeltApplication::setstatus_cmd);

void
SmeltApplication::setstatus_cmd(SmeltVector&v)
{
  std::string str = v.at(1).to_string();
  SMELT_DEBUG("SETSTATUS " << str);
  _app_mainwinp->pop_status (); // pop is valid on empty status
  _app_mainwinp->push_status (str);
}



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
/**** for emacs
  ++ Local Variables: ++
  ++ compile-command: "g++ -Wall -O -g $(pkg-config --cflags --libs gtksourceviewmm-3.0  gtkmm-3.0  gtk+-3.0) -o $HOME/bin/simplemelt-gtkmm-probe simplemelt-gtkmm-probe.cc" ++
  ++ End: ++

  To install, add a script or symlink called melt-probe somewhere in
  your $PATH to get that program started from MELT probe mode.
 ****/
