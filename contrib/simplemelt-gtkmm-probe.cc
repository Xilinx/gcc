
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

/*
This standalone program is a simple compiler probe client - it is a
single C++ source file using gtksourceview & gtk; some of the code is
taken or inspired by the contrib/simple-probe.c file of MELT branch
svn revision 147544 from mid-may 2009; it is not compiled by the GCC
building process. The compilation command is given near the end of
file (as a local.var to emacs) */

#include <gtkmm.h>
#include <gtksourceviewmm.h>
#include <iostream>
#include <sstream>
#include <cerrno>
#include <map>
#include <libguile.h>

#define SMELT_FATAL(C) do { int er = errno;	\
  std::clog << __FILE__ << ":" << __LINE__	\
	    << "@" << __func__ << " " << C;	\
  if (er) std::clog << " ~" << strerror(er) ;	\
  std::clog << std::endl;			\
  abort();					\
} while(0)



class SmeltMainWindow : public Gtk::Window
{
  Gtk::VBox _mainvbox;
  Gtk::MenuBar _mainmenubar;
  Gtk::Label _mainlabel;
public:
  SmeltMainWindow() : 
    Gtk::Window() 
  {
    set_border_width(5);
    add(_mainvbox);
    Glib::ustring labmarkstr = Glib::ustring::compose 
      ("<big><span color='darkred'>Simple Gcc Melt gtkmm-probe</span></big>\n"
       "<small>pid %1 on <tt>%2</tt></small>",  
       (int)(getpid()), g_get_host_name());
    _mainlabel.set_markup(labmarkstr);
    _mainlabel.set_justify(Gtk::JUSTIFY_CENTER);
    _mainvbox.pack_start(_mainmenubar);
    _mainvbox.pack_start(_mainlabel);
    show_all();

  }
  virtual ~SmeltMainWindow() 
  {
  }
  virtual bool on_delete_event(GdkEventAny*ev)
  {
    // return false to accept the delete event, true to reject it..
    return Gtk::Window::on_delete_event(ev);
  }
};


static Glib::OptionContext smelt_options("smeltgtk");

class SmeltLowOptionGroup : public Glib::OptionGroup
{
  std::string _file_to_melt;
  std::string _file_from_melt;
public:
  SmeltLowOptionGroup() 
    : Glib::OptionGroup("smelt_low", "the low level options of smeltgtk",
			"the low-level options for smeltgtk"),
      _file_to_melt(), _file_from_melt()
  {
    ///
    Glib::OptionEntry entry_to_melt;
    entry_to_melt.set_long_name("request-to-MELT");
    entry_to_melt.set_short_name('R');
    entry_to_melt.set_description("the file name, or file descriptor number, for channel sending requests to MELT");
    add_entry_filename(entry_to_melt, _file_to_melt);
    ///
    Glib::OptionEntry entry_from_melt;
    entry_from_melt.set_long_name("command-from-MELT");
    entry_to_melt.set_short_name('C');
    entry_to_melt.set_description("the file name, or file descriptor number, for channel recieving commands from MELT");
    add_entry_filename(entry_from_melt, _file_from_melt);
  }
};				// end class SmeltLowOptionGroup

class SmeltGuileOptionGroup : public Glib::OptionGroup
{
public:
  SmeltGuileOptionGroup()
    : Glib::OptionGroup("smelt_guile", "the GUILE specific options of smeltgtk",
			"Options for smeltgtk related to GUILE")
  {
  }
};				// end class SmeltGuileOptionGroup

class SmeltAppl
  // when gtkmm3.4 is available, should inherit from Gtk::Application
  : public Gtk::Main { 
  SmeltMainWindow _app_mainwin;	// main window
  Glib::RefPtr<Glib::IOChannel> _app_reqchan_to_melt; // channel for request to MELT
  Glib::RefPtr<Glib::IOChannel> _app_cmdchan_from_melt; // channel for commands from MELT
  std::ostringstream _app_writestream_to_melt; // string buffer for requests to MELT
  std::string _app_cmdstr_from_melt;	       // the last command from MELT
protected:
  bool reqbuf_to_melt_cb(Glib::IOCondition);
  bool cmdbuf_from_melt_cb(Glib::IOCondition);
  void process_command_from_melt (std::string& str);
public:
  SmeltAppl(int &argc, char**&argv)
    : Gtk::Main(argc, argv, smelt_options),
      _app_mainwin()
  {
    _app_reqchan_to_melt = Glib::IOChannel::create_from_fd(STDIN_FILENO);
    _app_cmdchan_from_melt = Glib::IOChannel::create_from_fd(STDOUT_FILENO);
  };
  ~SmeltAppl() {};
  void run (void) { Gtk::Main::run(_app_mainwin); };
};				// end class SmeltAppl


/* low level callback to write requests to MELT */
bool 
SmeltAppl::reqbuf_to_melt_cb(Glib::IOCondition outcond)
{
  if ((outcond & Glib::IO_OUT) == 0) 
    SMELT_FATAL ("error when writing requests to MELT");
  _app_writestream_to_melt.flush();
  std::string wstr = _app_writestream_to_melt.str();
  int wsiz = wstr.size();
  gsize wcnt = 0;
  int woff = 0;
  Glib::IOStatus wsta = Glib::IO_STATUS_EOF;
  const char* wdata = wstr.data();
  do { 
    if (wsiz <= 0) 
      break;
    wsta = _app_reqchan_to_melt->write(wdata+woff, wsiz, wcnt);
  switch (wsta) 
    {
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
  if (wsiz > 0) 
    {
      wstr.erase (0, wcnt);
      _app_writestream_to_melt.str(wstr);
    }
  else 
    {
      _app_writestream_to_melt.str(std::string());
    }
  return true;
}

/* low-level callback to read commands from MELT */
bool
SmeltAppl::cmdbuf_from_melt_cb(Glib::IOCondition  incond)
{  
  if ((incond & Glib::IO_IN) == 0) 
    SMELT_FATAL ("error when reading commands from MELT");
  Glib::ustring buf;
  _app_cmdchan_from_melt->read_line (buf);
  if (!buf.empty()) {
    _app_cmdstr_from_melt.append (buf.c_str());
    unsigned cmdlen = _app_cmdstr_from_melt.size();
    if (cmdlen>2 && _app_cmdstr_from_melt[cmdlen-2]=='\n' 
	&&  (_app_cmdstr_from_melt[cmdlen-1]=='\n' ||  _app_cmdstr_from_melt[cmdlen-1]=='\f'))
      {
	std::string curcmd = _app_cmdstr_from_melt;
	_app_cmdstr_from_melt.erase();
	process_command_from_melt(curcmd);
      }
  }
  return true;
}

void
SmeltAppl::process_command_from_melt(std::string& str)
{
}

int main (int argc, char** argv)
{
  scm_init_guile ();
  SmeltAppl app(argc, argv);
  app.run();
}

/**** for emacs 
  ++ Local Variables: ++
  ++ compile-command: "g++ -std=gnu++0x -Wall -O -g $(pkg-config --cflags --libs gtksourceviewmm-3.0  gtkmm-3.0  gtk+-3.0 guile-2.0) -o $HOME/bin/simplemelt-gtkmm-probe simplemelt-gtkmm-probe.cc" ++
  ++ End: ++
  Not needed compilation-directory: "."
 ****/
