
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
#include <cerrno>
#include <map>

#define SMELT_FATAL(C) do { int er = errno;	\
  std::clog << __FILE__ << ":" << __LINE__	\
	    << "@" << __func__ << " " << C;	\
  if (er) std::clog << " ~" << strerror(er) ;	\
  std::clog << std::endl;			\
  abort();					\
} while(0)



static Glib::OptionContext smelt_options_context(" - a simple MELT Gtk probe");

class SmeltMainWindow : public Gtk::Window {
    Gtk::VBox _mainvbox;
    Gtk::MenuBar _mainmenubar;
    Gtk::Label _mainlabel;
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
        _mainvbox.pack_start(_mainmenubar);
        _mainvbox.pack_start(_mainlabel);
        show_all();

    }
    virtual ~SmeltMainWindow() {
    }
    virtual bool on_delete_event(GdkEventAny*ev) {
        // return false to accept the delete event, true to reject it..
        return Gtk::Window::on_delete_event(ev);
    }
};



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
    };
    void setup_appl(SmeltAppl&);
};				// end class SmeltOptionGroup

class SmeltAppl
    // when gtkmm3.4 is available, should inherit from Gtk::Application
        : public Gtk::Main {
    SmeltMainWindow _app_mainwin;	// main window
    std::unique_ptr<SmeltTraceWindow> _app_tracewin;
    bool _app_traced;
    Glib::RefPtr<Glib::IOChannel> _app_reqchan_to_melt; // channel for request to MELT
    Glib::RefPtr<Glib::IOChannel> _app_cmdchan_from_melt; // channel for commands from MELT
    std::ostringstream _app_writestream_to_melt; // string buffer for requests to MELT
    std::string _app_cmdstr_from_melt;	       // the last command from MELT
protected:
    bool reqbuf_to_melt_cb(Glib::IOCondition);
    bool cmdbuf_from_melt_cb(Glib::IOCondition);
    void process_command_from_melt (std::string& str);
public:
    static SmeltAppl* instance() {
        return static_cast<SmeltAppl*>(Gtk::Main::instance());
    };
    SmeltAppl(int &argc, char**&argv)
        : Gtk::Main(argc, argv, smelt_options_context),
          _app_mainwin(),
          _app_traced(false) {
    };
    void set_reqchan_to_melt(const std::string &reqname) {
        const char* reqcstr = reqname.c_str();
        char* endstr = nullptr;
        long reqfd = strtol(reqcstr, &endstr, 10);
        struct stat reqstat = {};
        if (reqfd > 0 && reqfd < sysconf(_SC_OPEN_MAX)
                && endstr && endstr[0] == (char)0
                && !fstat(reqfd, &reqstat))
            _app_reqchan_to_melt = Glib::IOChannel::create_from_fd(reqfd);
        else if ((reqfd = open(reqcstr, O_RDONLY|O_CLOEXEC)) >= 0) {
            _app_reqchan_to_melt = Glib::IOChannel::create_from_fd(reqfd);
        } else
            SMELT_FATAL("cannot open request to MELT channel " << reqname);

    }
    void set_cmdchan_from_melt(const std::string &cmdname) {
        const char* cmdcstr = cmdname.c_str();
        char* endstr = nullptr;
        long cmdfd = strtol(cmdcstr, &endstr, 10);
        struct stat cmdstat = {};
        if (cmdfd > 0 && cmdfd < sysconf(_SC_OPEN_MAX)
                && endstr && endstr[0] == (char)0
                && !fstat(cmdfd, &cmdstat))
            _app_cmdchan_from_melt = Glib::IOChannel::create_from_fd(cmdfd);
        else if ((cmdfd = open(cmdcstr, O_RDONLY|O_CLOEXEC)) >= 0) {
            _app_cmdchan_from_melt = Glib::IOChannel::create_from_fd(cmdfd);
        } else
            SMELT_FATAL("cannot open command from MELT channel " << cmdname);
    }
    ~SmeltAppl() {};
    void run (void) {
        Gtk::Main::run(_app_mainwin);
    };
    void set_trace(bool =true);
    void on_trace_toggled(void);
};				// end class SmeltAppl



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
        }
    }
    show_all();
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
    if ((incond & Glib::IO_IN) == 0)
        SMELT_FATAL ("error when reading commands from MELT");
    Glib::ustring buf;
    _app_cmdchan_from_melt->read_line (buf);
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
    if (_app_traced) {
        _app_tracewin->add_command_from_melt(str);
    }
}

int main (int argc, char** argv)
{
    SmeltOptionGroup optgroup;
    smelt_options_context.set_main_group(optgroup);
    SmeltAppl app(argc, argv);
    optgroup.setup_appl(app);
    app.run();
}

/**** for emacs
  ++ Local Variables: ++
  ++ compile-command: "g++ -std=gnu++0x -Wall -O -g $(pkg-config --cflags --libs gtksourceviewmm-3.0  gtkmm-3.0  gtk+-3.0) -o $HOME/bin/simplemelt-gtkmm-probe simplemelt-gtkmm-probe.cc" ++
  ++ End: ++
  Not needed compilation-directory: "."
 ****/
