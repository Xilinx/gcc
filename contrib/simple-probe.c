/* Simple probe example (with GTK)
   Copyright (C) 2008 Free Software Foundation, Inc.
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
single source file using gtksourceview & gtk; it is not compiled by
the GCC building process. The compilation command is given near the
end of file (as a local.var to emacs)
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <ctype.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <gtk/gtktextbuffer.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagesmanager.h>

#define PROBE_PROTOCOL_NUMBER 200701

/* from /usr/share/xemacs21/xemacs-packages/etc/ediff/ediff-next.xpm */
/* XPM */
static const char *arrow_right_15x15_xpm[] = {
/* width height num_colors chars_per_pixel */
  "15 15 5 1",
  " 	c Gray75 s backgroundToolBarColor",
  ".	c black",
  "X	c white",
  "o	c black",
  "O	c black",
  "               ",
  "       .       ",
  "       ..      ",
  "       .X.     ",
  " .......XX.    ",
  " .XXXXXXXoX.   ",
  " .XooooooooX.  ",
  " .Xoooooooooo. ",
  " .XooooooooO.  ",
  " .oOOOOOOoO.   ",
  " .......OO.    ",
  "       .O.     ",
  "       ..      ",
  "       .       ",
  "               ",
};
GdkPixbuf *arrow_right_15x15_pixbuf;

/* from  /usr/share/xemacs21/xemacs-packages/etc/smilies/indifferent.xpm */
static const char *indifferent_13x14_xpm[] = {
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
GdkPixbuf *indifferent_13x14_pixbuf;

/* from /usr/share/xemacs21/xemacs-packages/lisp/speedbar/sb-info.xpm */
/* XPM */
static const char *sb_info_10x15_xpm[] = {
  "10 15 4 1",
  " 	c None",
  ".	c #BEBEBE",
  "+	c #0000FF",
  "@	c #FFFFFF",
  "    ..    ",
  "  ..+++.  ",
  " .+++@++. ",
  " .+++++++ ",
  " .+++++++ ",
  ".++@@@++++",
  ".++++@++++",
  ".++++@++++",
  ".++++@++++",
  " .+++@++++",
  " .+++@+++ ",
  " .+@@@@@+ ",
  " .+++++++ ",
  "  .+++++  ",
  "    ++    "
};
GdkPixbuf *sb_info_10x15_pixbuf;

/* from  /usr/share/xemacs21/xemacs-packages/etc/xwem/mini-info.xpm */
/* XPM */
static const char *mini_info_12x14_xpm[] = {
/* width height num_colors chars_per_pixel */
  "12 14 3 1",
/* colors */
  " 	c None",
  ".	c #cccc00",
  "#	c #dddd00",
/* pixels */
  "  .#.       ",
  "  ###       ",
  "  .#.       ",
  "            ",
  "  ...       ",
  ".###.       ",
  "..##.       ",
  " .##.       ",
  ".###        ",
  ".##.  .# .# ",
  ".##.  ######",
  "###..  #. #.",
  "####. ####. ",
  ".#..  .# ##."
};
GdkPixbuf *mini_info_12x14_pixbuf;


/* from /usr/lib/sourcenav/share/bitmaps/key.xpm */
/* XPM */
const static char *key_7x11_xpm[] = {
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
GdkPixbuf *key_7x11_pixbuf;

/* from  /usr/lib/sourcenav/share/bitmaps/tree.xpm */
/* XPM */
const static char *tree_24x24_xpm[] = {
/* width height ncolors cpp [x_hot y_hot] */
  "24 24 7 1 0 0",
/* colors */
  " 	s none	m none	c none",
  ".	s iconColor5	m black	c blue",
  "X	s iconColor2	m white	c white",
  "o	s iconColor4	m white	c green",
  "O	s iconColor1	m black	c black",
  "+	s iconColor6	m white	c yellow",
  "@	s iconColor3	m black	c red",
/* pixels */
  "                        ",
  "                 .....  ",
  "                ..XXX.  ",
  "         ooooo . .....  ",
  " OOOOOOOOoXXXo.         ",
  "         ooooo+         ",
  "               +        ",
  "                ++++++  ",
  "                 +XXX+  ",
  "                ++++++  ",
  "         @@@@@ +        ",
  "        O@XXX@+         ",
  " OOOOO O @@@@@          ",
  " OXXXOO                 ",
  " OOOOO O .....   ooooo  ",
  "        O.XXX.ooooXXXo  ",
  "         .....@  ooooo  ",
  "               @        ",
  "                @       ",
  "                 @@@@@  ",
  "                 @XXX@  ",
  "                 @@@@@  ",
  "                        ",
  "                        "
};
GdkPixbuf *tree_24x24_pixbuf;

#ifndef NDEBUG
FILE *dbgfile;
#define dbgprintf(Fmt, ...) do{if (dbgfile) { 				\
      fprintf(dbgfile,"+=simple-probe@%d:" Fmt "\n", (int)__LINE__, ##__VA_ARGS__); \
      fflush(dbgfile);}}while(0)
#else
#define dbgprintf(Fmt, ...) do{}while(0)
#endif

#define SIMPLE_GTK_TEXTBUFFER(B) GTK_TEXT_BUFFER(B)

GHashTable *action_table;

GtkWidget *window, *vbox, *notebook, *mainlabel, *stabar;
GtkWidget *menubar, *versionlab, *aboutdialog;
GtkTextBuffer *tractxtbuf;	/* the textbuffer for trace */
GtkWidget *tracwindow;		/* trace window */
GtkWidget *tracbox;		/* trace box */
GtkWidget *traccheck;		/* check button for scroll following */
GtkWidget *tracscroll;		/* the scrollbox for trace */
GtkWidget *tracview;		/* the textview for trace */
GtkTextTagTable *tractagtbl;	/* tag table for trace */
GtkTextTag *tractag_tim;	/* tag for time display & requests/commands counters */
GtkTextTag *tractag_title;	/* tag for title display */
GtkTextTag *tractag_imp;	/* tag for important display */
GtkTextTag *tractag_in;		/* tag for input display */
GtkTextTag *tractag_out;	/* tag for output display */

int trac_followout;		/* flag toggled by traccheck to scroll output */

GtkSourceLanguagesManager *lang_mgr;

struct fileinfo_st
{
  int fi_rank;			/* positive index inside fileinfo_array */
  char *fi_path;		/* strdup-ed file path */
  GtkWidget *fi_srcview;	/* main source view */
  GtkSourceBuffer *fi_srcbuf;	/* source buffer */
};
GPtrArray *fileinfo_array;


struct pointinfo_st
{
  int pi_rank;			/* rank of this pointinfo in pointinfo_array */
  int pi_filenum;		/* file number in fileinfo_array */
  int pi_line;			/* line number */
  GtkTextChildAnchor *pi_txanchor;	/* text anchor */
  GtkWidget *pi_txbutton;	/* button (in text) inside anchor */
};
GPtrArray *pointinfo_array;

struct dialogitem_st
{
  int di_rank;			/* rank of this item in the dialog menu */
  struct infodialog_st *di_dialog;	/* owning info dialog */
};

struct infodialog_st
{
  int id_rank;			/* rank of this infodialog in infodialog_array */
  int id_pinfrank;		/* originating point info rank */
  GtkWidget *id_dialog;		/* the dialog widget */
  GtkWidget *id_showcombo;	/* the combo widget to show */
  GtkWidget *id_menubar;	/* the mavigation menubar inside the widget */
  GtkWidget *id_infolab;	/* the information label inside the dialog */
  GPtrArray *id_showitems;	/* array of dialogitem-s for show combo */
  GPtrArray *id_navitems;	/* array dialogitem-s for navigation */
  GtkWidget *id_navtitle;	/* the navigation title item in menubar */
  GtkWidget *id_navmenu;	/* the navigation menu */
  GtkTooltips *id_tooltips;	/* the dialog tooltips */
};
GPtrArray *infodialog_array;


guint stid_pass;

typedef void action_handler_t (GString * act, void *data);

struct action_entry_st
{
  action_handler_t *handler;
  void *data;
};

/* requests are from probe to compiler, single line */
static void requestprintf (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));

static void
register_action (const char *action, action_handler_t * handler, void *data)
{
  struct action_entry_st *ae = g_malloc0 (sizeof (struct action_entry_st));
  ae->handler = handler;
  ae->data = data;
  g_assert (action_table != 0);
  g_hash_table_insert (action_table, g_strdup (action), ae);
}

/***
 * decode an encoded string, return the malloc-ed string and fil *PLEN
 * with its length and *PEND with the ending pointer
 ***/
static char *
decode_string (const char *s, int *plen, char **pend)
{
  char *res = 0;
  int len = 0, pos = 0, ix = 0;
  if (!s)
    return (char *) 0;
  if (sscanf (s, " STR%d'%n", &len, &pos) > 0 && pos > 0)
    {
      res = g_malloc0 (len + 1);
      s += pos;
      for (ix = 0; ix < len; ix++)
	{
	  char c = *s;
	  if (c == '%')
	    {
	      int d = 0;
	      char c1, c2;
	      if ((c1 = s[1]) && isxdigit (c1)
		  && (c2 = s[2]) && isxdigit (c2))
		{
		  d = (((c1 >= '0' && c1 <= '9') ? (c1 - '0')
			: (c1 >= 'A' && c1 <= 'F') ? (10 + c1 - 'A')
			: (c1 >= 'a' && c1 <= 'f') ? (10 + c1 - 'a')
			: 0) << 4)
		    + ((c2 >= '0' && c2 <= '9') ? (c2 - '0')
		       : (c2 >= 'A' && c2 <= 'F') ? (10 + c2 - 'A')
		       : (c2 >= 'a' && c2 <= 'f') ? (10 + c2 - 'a') : 0);
		  res[ix] = (char) d;
		  s += 3;
		}
	      else
		goto error;
	    }
	  else if (c == '+')
	    {
	      res[ix] = ' ';
	      s++;
	    }
	  else if (c > ' ' && c != '\'' && c != '\"')
	    {
	      res[ix] = c;
	      s++;
	    }
	  else
	    goto error;
	};
      if (*s != '\'')
	goto error;
      s++;
      if (plen)
	*plen = len;
      if (pend)
	*pend = (char *) s;
      return res;
    }
error:
  if (res)
    g_free (res);
  return 0;
}


/*** follow the trace window by scrolling to end ***/
void
trac_follow_end ()
{
  if (trac_followout && tracview)
    {
      GtkTextIter titer;
      gtk_text_buffer_get_end_iter (tractxtbuf, &titer);
      gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (tracview), &titer,
				    /*margin */ 0.05,
				    /*usalign */ FALSE,
				    /*xalign */ 0.0,
				    /* yalign */ 0.9);
    }
}

gboolean
delayed_follow_end_oncecb (gpointer data)
{
  g_assert (data == NULL);
  trac_follow_end ();
  if (tractxtbuf && tracview)
    gtk_widget_show (tracview);
  return FALSE;			/* remove this idle callback immediately */
}

/**************************** actions **************************/


static void
message_act (GString * s, void *d)
{
  char *msg = 0, *end = 0;
  int pos = 0, len = 0;
  g_assert (d != s);		/* just to use the arguments */
  dbgprintf ("message action %s", s->str);
  if (sscanf (s->str, " PROB_message msg: %n", &pos) >= 0 && pos > 0)
    msg = decode_string (s->str + pos, &len, &end);
  if (msg)
    {
      gtk_statusbar_pop (GTK_STATUSBAR (stabar), stid_pass);
      gtk_statusbar_push (GTK_STATUSBAR (stabar), stid_pass, msg);
      gtk_widget_show (stabar);
      g_free (msg);
    }
  else
    dbgprintf ("invalid message action %s", s->str);
}

static void
version_act (GString * s, void *d)
{
  char *msg = 0, *markup = 0, *end = 0;
  int pos = -1, len = 0, protonum = 0;
  g_assert (d != s);		/* just to use the arguments */
  dbgprintf ("version action %s", s->str);
  if (sscanf (s->str, " PROB_version proto: %d msg:%n", &protonum, &pos) >= 0
      && pos > 0) 
    msg = decode_string (s->str + pos, &len, &end);
  if (protonum != PROBE_PROTOCOL_NUMBER)
    {
      dbgprintf ("invalid protocol number %d expecting %d", protonum,
		 PROBE_PROTOCOL_NUMBER);
      exit (1);
    }
  if (msg)
    {
      markup =
	g_markup_printf_escaped
	("<small>(protocol %d)</small> - GCC "
	 "<span style='italic' foreground='darkgreen'>" "%s" "</span>",
	 protonum, msg);
      gtk_label_set_markup (GTK_LABEL (versionlab), markup);
      gtk_widget_show_all (window);
      g_free (markup);
    }
  else
    dbgprintf ("invalid version action (pos%d protonum%d no msg): %s",
	       pos, protonum, s->str);
  
}


static void
file_act (GString * s, void *d)
{
  int filerank = -1, pos = -1, len = 0;
  int fd = -1;
  char *file_path = 0, *end = 0;
  gchar *basename = 0;
  gchar *mime_type = 0;
  char *suffix = 0, *markup = 0;
  GtkSourceLanguage *language = NULL;
  GtkSourceBuffer *srcbuf = NULL;
  GtkWidget *srcview = NULL;
  GtkWidget *scrolwin = NULL;
  GtkWidget *label = NULL;
  GtkWidget *tablab = NULL;
  GtkWidget *box = NULL;
  struct stat filestat;
  struct fileinfo_st *filinf = NULL;
  const gchar *fcontent = 0;
  size_t filesize = 0;
  size_t mapsize = 0;
  static size_t pgsiz;
  g_assert (d != s);		/* just to use the arguments */
  memset (&filestat, 0, sizeof (filestat));
  if (sscanf (s->str, " PROB_file rank: %d fpath: %n", &filerank, &pos)
      > 0 && filerank >= 0 && pos > 0)
    {
      file_path = decode_string (s->str + pos, &len, &end);
      if ((fd = open (file_path, O_RDONLY)) < 0)
	{
	  dbgprintf ("failed to open %s : %m", file_path);
	  return;
	}
      basename = g_path_get_basename ((const gchar *) file_path);
      suffix = g_strrstr (basename, (const gchar *) ".");
      if (!strcmp (suffix, ".cc")
	  || !strcmp (suffix, ".cxx")
	  || !strcmp (suffix, ".cpp")
	  || !strcmp (suffix, ".cp")
	  || !strcmp (suffix, ".ii")
	  || !strcmp (suffix, ".CPP")
	  || !strcmp (suffix, ".hh")
	  || !strcmp (suffix, ".hxx")
	  || !strcmp (suffix, ".hpp")
	  || !strcmp (suffix, ".C") || !strcmp (suffix, ".H"))
	mime_type = "text/x-c++src";
      else if (!strcmp (suffix, ".c")
	       || !strcmp (suffix, ".i") || !strcmp (suffix, ".h"))
	mime_type = "text/x-csrc";
      else if (!strcmp (suffix, ".f")
	       || !strcmp (suffix, ".F")
	       || !strcmp (suffix, ".FOR")
	       || !strcmp (suffix, ".F77")
	       || !strcmp (suffix, ".f77")
	       || !strcmp (suffix, ".F95")
	       || !strcmp (suffix, ".f95")
	       || !strcmp (suffix, ".F90")
	       || !strcmp (suffix, ".f90") || !strcmp (suffix, ".for"))
	mime_type = "text/x-fortran";
      else if (!strcmp (suffix, ".adb")
	       || !strcmp (suffix, ".ads") || !strcmp (suffix, ".ada"))
	mime_type = "text/x-ada";
      if (!mime_type)
	mime_type = "text/x-c++src";
      dbgprintf ("file %s mimetype %s", file_path, mime_type);
      if (!fstat (fd, &filestat))
	filesize = filestat.st_size;
      language =
	gtk_source_languages_manager_get_language_from_mime_type (lang_mgr,
								  mime_type);
      g_assert (language != NULL);
      srcbuf = gtk_source_buffer_new_with_language (language);
      srcview = gtk_source_view_new_with_buffer (srcbuf);
      g_object_set (G_OBJECT (srcview), "editable", FALSE, NULL);
      gtk_source_buffer_set_highlight (srcbuf, TRUE);
      gtk_source_buffer_begin_not_undoable_action (srcbuf);
      if (!pgsiz)
	pgsiz = getpagesize ();
      if (filesize > (off_t) 0)
	{
	  gchar *convcont = 0;
	  gsize convsize = 0;
	  mapsize = filesize;
	  if (mapsize % pgsiz)
	    mapsize = (filesize | (pgsiz - 1)) + 1;
	  fcontent = (const gchar *) mmap ((void *) 0, mapsize, PROT_READ,
					   MAP_SHARED, fd, (off_t) 0);
	  if (fcontent != MAP_FAILED)
	    {
	      convcont =
		g_locale_to_utf8 (fcontent, filesize, NULL, &convsize,
				  (GError **) 0);
	      g_assert (convcont);
	      if (convcont)
		gtk_text_buffer_set_text (SIMPLE_GTK_TEXTBUFFER (srcbuf),
					  convcont, convsize);
	      g_free (convcont);
	      munmap ((char *) fcontent, mapsize);
	      fcontent = 0;
	    }
	  else
	    fprintf (stderr, "mmap file %s size %ld failed: %m\n",
		     file_path, filesize);
	};
      close (fd);
      gtk_source_buffer_end_not_undoable_action (srcbuf);
      gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW (srcview), TRUE);
      gtk_source_view_set_show_line_markers (GTK_SOURCE_VIEW (srcview), TRUE);
      markup = g_markup_printf_escaped
	("<span weight=\"bold\" size=\"larger\">%d</span>\n"
	 "<small><tt>%s</tt></small>", filerank, basename);
      tablab = gtk_label_new ((char *) 0);
      gtk_label_set_markup (GTK_LABEL (tablab), markup);
      g_free (markup);
      label = gtk_label_new ((char *) 0);
      markup = g_markup_printf_escaped
	("<span weight=\"bold\" size=\"larger\">#%d</span>\n"
	 "<span color='navy' style='italic'>file "
	 "<small><tt>%s</tt></small>\n"
	 "of %ld bytes</span>", filerank, file_path, (long) filesize);
      gtk_label_set_markup (GTK_LABEL (label), markup);
      g_free (markup);
      g_free (basename);
      basename = suffix = markup = NULL;
      scrolwin = gtk_scrolled_window_new (NULL, NULL);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolwin),
				      GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
      gtk_container_add (GTK_CONTAINER (scrolwin), srcview);
      box = gtk_vbox_new (FALSE, 1);
      gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 1);
      gtk_box_pack_start (GTK_BOX (box), scrolwin, TRUE, TRUE, 1);
      gtk_notebook_insert_page (GTK_NOTEBOOK (notebook), box, tablab,
				(gint) filerank);
      if (fileinfo_array->len <= filerank)
	g_ptr_array_set_size (fileinfo_array, 5 * filerank / 4 + 16);
      filinf = g_malloc0 (sizeof (*filinf));
      g_assert (g_ptr_array_index (fileinfo_array, filerank) == NULL);
      g_ptr_array_index (fileinfo_array, filerank) = filinf;
      filinf->fi_rank = filerank;
      filinf->fi_path = g_strdup (file_path);
      filinf->fi_srcview = srcview;
      filinf->fi_srcbuf = srcbuf;
      gtk_source_view_set_marker_pixbuf (GTK_SOURCE_VIEW (srcview), "info",
					 sb_info_10x15_pixbuf);
      gtk_widget_show_all (window);
      g_free (file_path);
    }
}

/* GTK callback called when an info dialog is responded */
static void
infodialog_cb (GtkWidget * widget, int respid, gpointer data)
{
  struct infodialog_st *dia = data;
  g_assert (dia && dia->id_dialog == widget);
  switch (respid)
    {
    case GTK_RESPONSE_ACCEPT:
      requestprintf ("prob_UPDATEINFODIALOG dia:%d\n", dia->id_rank);
      break;
    case GTK_RESPONSE_CLOSE:
    default:
      requestprintf ("prob_REMOVEINFODIALOG dia:%d\n", dia->id_rank);
      gtk_widget_hide (dia->id_dialog);
      break;
    }
}

/* internal routine to create a new info dialog (still empty and not
   displayed) */
static struct infodialog_st *
make_infodialog (struct pointinfo_st *pi)
{
  int ix = 0, k;
  struct infodialog_st *dia = NULL;
  struct fileinfo_st *fi = NULL;
  GtkWidget *dialog = NULL, *hbox = NULL, *menubar = NULL, *combo = NULL,
    *showlabel = NULL, *pointlabel = NULL, *infolabel = NULL,
    *infoscroll = NULL;
  GtkTooltips *tooltips = NULL;
  char titbuf[64];
  char *pointmarkup = 0;
  g_assert (pi != NULL && pi->pi_rank >= 0
	    && pi->pi_rank < pointinfo_array->len);
  g_assert (g_ptr_array_index (pointinfo_array, pi->pi_rank) == pi);
  g_assert (pi->pi_filenum >= 0 && fileinfo_array
	    && pi->pi_filenum < fileinfo_array->len);
  fi = g_ptr_array_index (fileinfo_array, pi->pi_filenum);
  dia = g_malloc0 (sizeof (*dia));
  memset (titbuf, 0, sizeof (titbuf));
  ix = -1;
  if (infodialog_array)
    {
      for (k = 0; k < infodialog_array->len; k++)
	if (!g_ptr_array_index (infodialog_array, k))
	  {
	    ix = k;
	    break;
	  }
    }
  if (ix >= 0)
    {
      dia->id_rank = ix;
      g_ptr_array_index (infodialog_array, ix) = dia;
    }
  else
    {
      dia->id_rank = infodialog_array->len;
      g_ptr_array_add (infodialog_array, dia);
    }
  dbgprintf ("make_infodialog dia %p rank %d", dia, dia->id_rank);
  dia->id_pinfrank = pi->pi_rank;
  snprintf (titbuf, sizeof (titbuf) - 1, "InfoPt#%d", dia->id_rank);
  dialog = dia->id_dialog
    = gtk_dialog_new_with_buttons
    (titbuf,
     GTK_WINDOW (window),
     GTK_DIALOG_DESTROY_WITH_PARENT,
     GTK_STOCK_HOME, GTK_RESPONSE_ACCEPT,
     GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
  tooltips = dia->id_tooltips = gtk_tooltips_new ();
  pointlabel = gtk_label_new ((char *) 0);
  pointmarkup = g_markup_printf_escaped
    ("<span size='large' foreground='darkgreen'>"
     "info point #%d" "</span>\n"
     "<b>file #%d</b> <tt>%s</tt> <i>line %d</i>",
     dia->id_rank, pi->pi_filenum, fi->fi_path, pi->pi_line);
  gtk_label_set_markup (GTK_LABEL (pointlabel), pointmarkup);
  g_free (pointmarkup);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
		      pointlabel, /*expand: */ FALSE, /*fill: */ FALSE,
		      1);
  hbox = gtk_hbox_new (FALSE, 3);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
		      hbox, /*expand: */ FALSE, /*fill: */ FALSE,
		      1);
  showlabel = gtk_label_new ((char *) 0);
  gtk_label_set_markup (GTK_LABEL (showlabel),
			"<span foreground='navy' weight='bold'>"
			"show:" "</span>");
  gtk_box_pack_start (GTK_BOX (hbox), showlabel,
		      /*expand: */ FALSE, /*fill: */ FALSE,
		      1);
  combo = dia->id_showcombo = gtk_combo_box_new_text ();
  gtk_tooltips_set_tip (tooltips, combo,
			"Select information to show",
			"Choose the information to show in this dialog\n"
			"for this info point");
  gtk_box_pack_start (GTK_BOX (hbox), combo,
		      /*expand: */ TRUE, /*fill: */ TRUE,
		      1);
  menubar = dia->id_menubar = gtk_menu_bar_new ();
  gtk_tooltips_set_tip (tooltips, menubar,
			"Navigation menu",
			"Choose where to go in this dialog\n"
			"for this info point");
  gtk_box_pack_start (GTK_BOX (hbox), menubar,
		      /*expand: */ FALSE, /*fill: */ FALSE,
		      2);
  infoscroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (infoscroll),
				  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  dia->id_infolab = infolabel = gtk_label_new ((char *) 0);
  gtk_label_set_markup (GTK_LABEL (infolabel),
			"<span size='large' foreground='darkred'>"
			"* select info to show *" "</span>\n");
  gtk_label_set_selectable (GTK_LABEL (infolabel), TRUE);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (infoscroll),
					 infolabel);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
		      infoscroll, /*expand: */ TRUE, /*fill: */ TRUE, 1);
  dia->id_showitems = g_ptr_array_sized_new (6);
  g_signal_connect (G_OBJECT (dialog),
		    "response", G_CALLBACK (infodialog_cb), dia);
  dbgprintf ("make_infodialog dia %p rank %d", dia, dia->id_rank);
  return dia;
}


/* GTK callback called when an info button is clicked; should create a
   dialog and ask it to be filled */
static void
txinfobutton_cb (GtkWidget * widget, gpointer data)
{
  struct pointinfo_st *pi = data;
  struct infodialog_st *dia = NULL;
  g_assert (pi && pi->pi_txbutton == widget);
  dia = make_infodialog (pi);
  requestprintf ("prob_NEWINFODIALOG pt:%d dia:%d\n",
		 pi->pi_rank, dia->id_rank);
}


/* GTK callback called when about information has been asked */
static void
aboutwin_cb (GtkWidget * widget, gpointer data)
{
  if (!aboutdialog)
    {
      aboutdialog = gtk_about_dialog_new ();
      gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (aboutdialog),
				 "GCC simple compiler probe");
      gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (aboutdialog),
				      "Copyright (C) 2007 Free Software Foundation, Inc");
      gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (aboutdialog),
				    "GNU General Public License version 2 or later");
      gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (aboutdialog),
				    "http://gcc.gnu.org/");
      gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (aboutdialog),
				     "A simple compiler probe to be used with GCC\n"
				     " [Gnu Compiler Collection] \n"
				     "with its -fcompiler-probe option\n"
				     "(simple-probe built " __DATE__ "@"
				     __TIME__ ")");
    };
  gtk_dialog_run (GTK_DIALOG (aboutdialog));
}


static void
infopoint_act (GString * s, void *d)
{
  int filerk = 0;
  int lineno = 0;
  int infonum = 0;
  struct fileinfo_st *filinf = NULL;
  struct pointinfo_st *pi = NULL;
  char *filepath = NULL;
  GtkTextIter txiter;
  GtkSourceBuffer *srcbuf = NULL;
  GtkWidget *srcview = NULL;
  GtkWidget *txbutton = NULL;
  GtkTextChildAnchor *anch = NULL;
  g_assert (s != d);
  if (sscanf
      (s->str, " PROB_infopoint fil:%d lin:%d rk:%d", &filerk, &lineno,
       &infonum) > 0 && infonum >= 0)
    {
      dbgprintf ("infopoint act filerk %d lineno %d infonum %d",
		 filerk, lineno, infonum);
      memset (&txiter, 0, sizeof (txiter));
      if (filerk >= 0 && filerk < fileinfo_array->len)
	filinf = g_ptr_array_index (fileinfo_array, filerk);
      if (!filinf)
	return;
      if (pointinfo_array->len <= infonum)
	g_ptr_array_set_size (pointinfo_array, 5 * infonum / 4 + 16);
      if (g_ptr_array_index (pointinfo_array, infonum) != NULL)
	return;
      filepath = filinf->fi_path;
      g_assert (filepath);
      srcbuf = filinf->fi_srcbuf;
      srcview = filinf->fi_srcview;
      g_assert (srcbuf);
      pi = g_malloc0 (sizeof (struct pointinfo_st));
      g_ptr_array_index (pointinfo_array, infonum) = pi;
      pi->pi_rank = infonum;
      pi->pi_filenum = filerk;
      pi->pi_line = lineno;
      gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER (srcbuf), &txiter,
					lineno - 1);
      anch =
	gtk_text_buffer_create_child_anchor (GTK_TEXT_BUFFER (srcbuf),
					     &txiter);
      txbutton = gtk_button_new ();
      gtk_button_set_image (GTK_BUTTON (txbutton),
			    gtk_image_new_from_pixbuf (key_7x11_pixbuf));
      gtk_text_view_add_child_at_anchor (GTK_TEXT_VIEW (srcview), txbutton,
					 anch);
      gtk_widget_show_all (txbutton);
      gtk_widget_show_all (srcview);
      gtk_widget_show_all (window);
      pi->pi_txanchor = anch;
      pi->pi_txbutton = txbutton;
      g_signal_connect (G_OBJECT (txbutton), "clicked",
			G_CALLBACK (txinfobutton_cb), pi);
    }
  else
    dbgprintf ("invalid infopoint_act %s", s->str);
}

/* GTK callback of show items */
static void
showcombochanged_cb (GtkComboBox * combo, struct infodialog_st *dia)
{
  gint rk = -1;
  g_assert (combo && dia && dia->id_showcombo == GTK_WIDGET (combo));
  rk = gtk_combo_box_get_active (combo);
  if (rk >= 0)
    requestprintf ("prob_SHOWINFODIALOG dia:%d ch:%d\n", dia->id_rank, rk);
}

static void
dialogchoice_act (GString * s, void *d)
{
  int diark = -1, pos = -1, len = 0, chrk = -1;
  char *end = 0, *msg = 0;
  struct infodialog_st *dia = NULL;
  struct dialogitem_st *itm = NULL;
  g_assert (s != d);
  dbgprintf ("dialogchoice_act start %s", s->str);
  if (sscanf
      (s->str, " PROB_dialogchoice dia: %d msg: %n", &diark, &pos) > 0
      && diark >= 0 && pos > 0)
    {
      dbgprintf ("dialogchoice_act diark%d", diark);
      if (!infodialog_array || diark >= infodialog_array->len)
	return;
      dia = g_ptr_array_index (infodialog_array, diark);
      if (!dia || dia->id_rank != diark)
	return;
      dbgprintf ("dialogchoice_act dia %p", dia);
      msg = decode_string (s->str + pos, &len, &end);
      if (sscanf (end, " ch: %d", &chrk) <= 0 || chrk < 0)
	{
	  g_free (msg);
	  return;
	}
      itm = g_malloc0 (sizeof (*itm));
      itm->di_rank = chrk;
      itm->di_dialog = dia;
      g_ptr_array_add (dia->id_showitems, itm);
      gtk_combo_box_insert_text (GTK_COMBO_BOX (dia->id_showcombo), chrk,
				 msg);
      g_signal_connect (G_OBJECT (dia->id_showcombo), "changed",
			G_CALLBACK (showcombochanged_cb), (gpointer) dia);
      g_free (msg);
      dbgprintf ("dialogchoice_act done dia %p", dia);
    }
}


static void
dialogcontent_act (GString * s, void *d)
{
  int diark = -1, pos = -1;
  struct infodialog_st *dia = NULL;
  char *ps = 0;
  g_assert (s != d);
  if (sscanf (s->str, " PROB_dialogcontent dia: %d %n", &diark, &pos) > 0
      && diark >= 0 && pos > 0)
    {
      dbgprintf ("dialogcontent_act diark%d", diark);
      if (!infodialog_array || diark >= infodialog_array->len)
	return;
      dia = g_ptr_array_index (infodialog_array, diark);
      if (!dia || dia->id_rank != diark)
	return;
      ps = s->str + pos;
      dbgprintf ("dialogcontent_act dia %p", dia);
      gtk_label_set_text (GTK_LABEL (dia->id_infolab), ps);
      /* destroy the navigation items & menu */
      if (dia->id_navitems)
	{
	  g_ptr_array_free (dia->id_navitems, TRUE);
	  dia->id_navitems = NULL;
	}
      if (dia->id_navmenu)
	{
	  gtk_object_destroy (GTK_OBJECT (dia->id_navmenu));
	  dia->id_navmenu = NULL;
	}
      if (dia->id_navtitle)
	{
	  gtk_object_destroy (GTK_OBJECT (dia->id_navtitle));
	  dia->id_navtitle = NULL;
	}
    }
}

static void
showdialog_act (GString * s, void *d)
{
  int diark = -1;
  struct infodialog_st *dia = NULL;
  g_assert (s != d);
  if (sscanf (s->str, " PROB_showdialog dia:%d", &diark) > 0 && diark >= 0)
    {
      dbgprintf ("showdialog_act diark%d", diark);
      if (!infodialog_array || diark >= infodialog_array->len)
	return;
      dia = g_ptr_array_index (infodialog_array, diark);
      if (!dia || dia->id_rank != diark)
	return;
      dbgprintf ("showdialog_act dia %p", dia);
      gtk_widget_show_all (dia->id_dialog);
    }
}


/*GTK callback for dialog navigation items */
static void
navigitem_cb (GtkMenuItem * menuitem, gpointer data)
{
  struct dialogitem_st *itm = data;
  g_assert (itm != 0 && itm->di_dialog);
  requestprintf ("prob_NAVIGINFODIALOG dia:%d nav:%d\n",
		 itm->di_dialog->id_rank, itm->di_rank);

}


static void
dialognavig_act (GString * s, void *d)
{
  int diark = -1, pos = -1, len = 0, navrk = -1;
  char *end = 0, *msg = 0;
  struct infodialog_st *dia = NULL;
  struct dialogitem_st *itm = NULL;
  GtkWidget *menuitem = NULL, *sepitem = NULL, *navigitem = NULL;
  g_assert (s != d);
  dbgprintf ("dialognavig_act start %s", s->str);
  if (sscanf
      (s->str, " PROB_dialognavig dia: %d msg: %n", &diark, &pos) > 0
      && diark >= 0 && pos > 0)
    {
      dbgprintf ("dialognavig_act diark%d", diark);
      if (!infodialog_array || diark >= infodialog_array->len)
	return;
      dia = g_ptr_array_index (infodialog_array, diark);
      if (!dia || dia->id_rank != diark)
	return;
      dbgprintf ("dialognavig_act dia %p", dia);
      msg = decode_string (s->str + pos, &len, &end);
      if (sscanf (end, " nav: %d", &navrk) <= 0 || navrk < 0)
	{
	  dbgprintf ("dialognavig_act bad end %s", end);
	  g_free (msg);
	  return;
	}
      itm = g_malloc0 (sizeof (*itm));
      dbgprintf ("dialognavig_act navrk %d msg %s", navrk, msg);
      itm->di_rank = navrk;
      itm->di_dialog = dia;
      if (!dia->id_navitems)
	dia->id_navitems = g_ptr_array_sized_new (6);
      g_ptr_array_add (dia->id_navitems, itm);
      menuitem = gtk_menu_item_new_with_label (msg);
      if (!dia->id_navmenu)
	{
	  dia->id_navmenu = gtk_menu_new ();
	  dbgprintf ("dialognavig_act navmenu %p", dia->id_navmenu);
	  sepitem = gtk_separator_menu_item_new ();
	  gtk_menu_shell_append (GTK_MENU_SHELL (dia->id_navmenu), sepitem);
	  g_assert (GTK_IS_MENU_SHELL (dia->id_menubar));
	  navigitem = dia->id_navtitle =
	    gtk_menu_item_new_with_label ("navigation");
	  gtk_menu_shell_append (GTK_MENU_SHELL (dia->id_menubar), navigitem);
	  gtk_menu_item_set_submenu (GTK_MENU_ITEM (navigitem),
				     dia->id_navmenu);
	  gtk_widget_show (dia->id_navmenu);
	}
      gtk_menu_shell_append (GTK_MENU_SHELL (dia->id_navmenu), menuitem);
      gtk_widget_show (menuitem);
      g_free (msg);
      msg = NULL;
      g_signal_connect (G_OBJECT (menuitem),
			"activate", G_CALLBACK (navigitem_cb), itm);
      dbgprintf ("dialognavig_act done dia %p", dia);
    }
}


static void
destroydialog_act (GString * s, void *d)
{
  int diark = -1;
  struct infodialog_st *dia = NULL;
  g_assert (s != d);
  if (sscanf (s->str, " PROB_destroydialog dia: %d", &diark) > 0
      && diark >= 0)
    {
      dbgprintf ("destroydialog_act diark%d", diark);
      if (!infodialog_array || diark >= infodialog_array->len)
	return;
      dia = g_ptr_array_index (infodialog_array, diark);
      if (!dia || dia->id_rank != diark)
	return;
      dbgprintf ("destroydialog_act dia %p", dia);
      if (dia->id_dialog)
	gtk_widget_hide (dia->id_dialog);
      if (dia->id_navitems)
	{
	  g_ptr_array_free (dia->id_navitems, TRUE);
	  dia->id_navitems = NULL;
	}
      if (dia->id_navmenu)
	{
	  gtk_object_destroy (GTK_OBJECT (dia->id_navmenu));
	  dia->id_navmenu = NULL;
	}
      if (dia->id_navtitle)
	{
	  gtk_object_destroy (GTK_OBJECT (dia->id_navtitle));
	  dia->id_navtitle = NULL;
	}
      gtk_object_destroy (GTK_OBJECT (dia->id_dialog));
      g_ptr_array_free (dia->id_showitems, TRUE);
      memset (dia, 0, sizeof (dia));
      g_ptr_array_index (infodialog_array, diark) = NULL;
      g_free (dia);
      dbgprintf ("destroydialog_act done diark%d", diark);
    }
}

/*********************** request & trace ***********************/
static void
insert_trace_time (int dated, const char *buf)
{
  struct
  {
    char buf[200];
    GTimeVal tv;
    struct tm tm;
    char sec[10];
  } t;
  time_t tim;
  GtkTextIter itend;
  memset (&t, 0, sizeof (t));
  memset (&itend, 0, sizeof (itend));
  g_get_current_time (&t.tv);
  tim = t.tv.tv_sec;
  localtime_r (&tim, &t.tm);
  strftime (t.buf, sizeof (t.buf) - 10, dated ? " %G %b %d @ %T" : " %T",
	    &t.tm);
  sprintf (t.sec, ".%03d ", (int) t.tv.tv_usec / 1000);
  strcat (t.buf, t.sec);
  gtk_text_buffer_get_end_iter (SIMPLE_GTK_TEXTBUFFER (tractxtbuf), &itend);
  gtk_text_buffer_insert_with_tags (tractxtbuf, &itend, "\n", -1,
				    tractag_tim, (void *) 0);
  gtk_text_buffer_get_end_iter (SIMPLE_GTK_TEXTBUFFER (tractxtbuf), &itend);
  if (buf)
    gtk_text_buffer_insert_with_tags (tractxtbuf, &itend, buf, -1,
				      tractag_tim, (void *) 0);
  gtk_text_buffer_get_end_iter (SIMPLE_GTK_TEXTBUFFER (tractxtbuf), &itend);
  gtk_text_buffer_insert_with_tags (tractxtbuf, &itend, t.buf, -1,
				    tractag_tim, (void *) 0);
}


static void
requestprintf (const char *fmt, ...)
{
  va_list ar;
  gchar *buf = 0;
  gint len = 0;
  static int nbreq;
  GtkTextIter itend;
  char bufn[64];
  va_start (ar, fmt);
  nbreq++;
  len = g_vasprintf (&buf, (const gchar *) fmt, ar);
  va_end (ar);
  dbgprintf ("begin requestprintf %.30s", fmt);
  if (tractxtbuf)
    {
      memset (&itend, 0, sizeof (itend));
      gtk_text_buffer_get_end_iter (SIMPLE_GTK_TEXTBUFFER (tractxtbuf),
				    &itend);
      memset (bufn, 0, sizeof (bufn));
      snprintf (bufn, sizeof (bufn) - 1, "!request %d:", nbreq);
      insert_trace_time (0, bufn);
      gtk_text_buffer_get_end_iter (SIMPLE_GTK_TEXTBUFFER (tractxtbuf),
				    &itend);
      gtk_text_buffer_insert_with_tags (tractxtbuf, &itend, buf, len,
					tractag_out, (void *) 0);
      gtk_text_buffer_get_end_iter (SIMPLE_GTK_TEXTBUFFER (tractxtbuf),
				    &itend);
    };
  fputs (buf, stdout);
  if (len <= 0 || buf[len - 1] != '\n')
    {
      putchar ('\n');
      if (tractxtbuf)
	gtk_text_buffer_insert (tractxtbuf, &itend, "\n", 1);
    };
  fflush (stdout);
  if (tractxtbuf)
    {
      trac_follow_end ();
      gtk_widget_show_all (tracwindow);
    }
  dbgprintf ("request #%d: %s\n", nbreq, buf);
  g_free (buf);
  if (tractxtbuf && trac_followout)
    g_idle_add (delayed_follow_end_oncecb, (void *) 0);
  buf = 0;
}

static gboolean
ioreader (GIOChannel * chan, GIOCondition cond, gpointer data)
{
  gchar *end = 0, *line = 0;
  gsize len = 0;
  gsize eolpos = 0;
  GIOStatus stat = 0;
  GError *err = 0;
  GString *str = 0;
  int leftmagic = 0, rightmagic = 0, pos = 0;
  char verb[64];
  char bufn[48];
  static int nbcmd;
  g_assert (cond == G_IO_IN);
  line = end = 0;
  len = eolpos = 0;
  dbgprintf ("ioreader begin");
  stat = g_io_channel_read_line (chan, &line, &len, &eolpos, &err);
  dbgprintf ("ioreader stat %d", stat);
  if (stat == G_IO_STATUS_NORMAL)
    {
      if (line[0] == '!'
	  && sscanf (line, "!#%x/%X[%n", &leftmagic, &rightmagic, &pos) >= 2
	  && rightmagic != 0 && pos > 0)
	{			/* multi-line command */
	  str = g_string_sized_new (1000 + eolpos);
	  str = g_string_append (str, line + pos);
	  g_free (line);
	  line = 0;
	  while ((stat =
		  g_io_channel_read_line (chan, &line, &len, &eolpos,
					  &err)) == G_IO_STATUS_NORMAL)
	    {
	      int left, right;
	      left = right = pos = 0;
	      if (line[0] == '!'
		  && sscanf (line, "!#%x/%X] %n", &left, &right, &pos) >= 2
		  && pos > 0 && left == leftmagic && right == rightmagic
		  && line[pos] == '\0')
		{
		  g_free (line);
		  line = 0;
		  break;
		};
	      str = g_string_append (str, line);
	      g_free (line);
	      line = 0;

	    };
	}
      else
	{			/* ordinary single line command */
	  str = g_string_sized_new (1000 + eolpos);
	  str = g_string_append (str, line);
	  g_free (line);
	  line = 0;
	}
    }
  if (str && str->len == 1 && str->str[0] == '\n')
    return TRUE;
  nbcmd++;
  dbgprintf ("command #%d: %s\n", nbcmd, str->str);
  if (tractxtbuf)
    {
      GtkTextIter itend;
      memset (&itend, 0, sizeof (itend));
      gtk_text_buffer_get_end_iter (SIMPLE_GTK_TEXTBUFFER (tractxtbuf),
				    &itend);
      memset (bufn, 0, sizeof (bufn));
      snprintf (bufn, sizeof (bufn) - 1, "?command %d:", nbcmd);
      insert_trace_time (0, bufn);
      gtk_text_buffer_get_end_iter (SIMPLE_GTK_TEXTBUFFER (tractxtbuf),
				    &itend);
      gtk_text_buffer_insert_with_tags (tractxtbuf, &itend, str->str,
					str->len, tractag_in, (void *) 0);
      gtk_text_buffer_get_end_iter (SIMPLE_GTK_TEXTBUFFER (tractxtbuf),
				    &itend);
      if (str->len > 0 && str->str[str->len - 1] != '\n')
	gtk_text_buffer_insert (tractxtbuf, &itend, "\n", 1);
      trac_follow_end ();
    };
  memset (verb, 0, sizeof (verb));
  if (str && sscanf (str->str, " %62[a-zA-Z0-9_] ", verb) > 0)
    {
      struct action_entry_st *ae = g_hash_table_lookup (action_table, verb);
      dbgprintf ("command verb %s", verb);
      if (ae && ae->handler)
	{
	  (*ae->handler) (str, ae->data);
	  if (tractxtbuf)
	    trac_follow_end ();
	}
      else if (tractxtbuf)
	{
	  static char unknownmsg[200];
	  GtkTextIter itend;
	  memset (&itend, 0, sizeof (itend));
	  gtk_text_buffer_get_end_iter (SIMPLE_GTK_TEXTBUFFER (tractxtbuf),
					&itend);
	  memset (unknownmsg, 0, sizeof (unknownmsg));
	  snprintf (unknownmsg, sizeof (unknownmsg) - 1,
		    "*?* unknown command verb '%s'\n", verb);
	  dbgprintf ("*? unknown command verb '%s'", verb);
	  gtk_text_buffer_insert_with_tags (tractxtbuf, &itend,
					    unknownmsg, strlen (unknownmsg),
					    tractag_tim, (void *) 0);
	  trac_follow_end ();
	}
      if (tractxtbuf)
	gtk_widget_show_all (tracwindow);
    }
  else
    dbgprintf ("invalid command string %s", str->str);
  if (str)
    g_string_free (str, TRUE);
  if (tractxtbuf && trac_followout)
    g_idle_add (delayed_follow_end_oncecb, (void *) 0);
  str = 0;
  /* remove the handler on eof */
  dbgprintf ("stat=%d isnormal=%d", stat, stat != G_IO_STATUS_EOF);
  /* the function should return FALSE if the event source should be removed. */
  return stat != G_IO_STATUS_EOF;
}

static void
destroy_cb (GtkWidget * widget, gpointer data)
{
  requestprintf ("prob_STOP\n");
  gtk_main_quit ();
}

static void
tracdestroy_cb (GtkWidget * widget, gpointer data)
{
  g_assert (widget == tracwindow);
  tracwindow = (void *) 0;
  tracbox = (void *) 0;
  tracscroll = (void *) 0;
  tracview = (void *) 0;
  tractagtbl = (void *) 0;
  tractag_tim = tractag_title = tractag_imp = tractag_in = tractag_out = 0;
  tractxtbuf = 0;
}


static GtkItemFactoryEntry menu_items[] = {
  {"/_File", NULL, NULL, 0, "<Branch>"},
  {"/File/_Quit", "<CTRL>Q", gtk_main_quit, 0, "<StockItem>", GTK_STOCK_QUIT},
  {"/_Help", NULL, NULL, 0, "<LastBranch>"},
  {"/_Help/About", NULL, aboutwin_cb, 0, "<Item>"},
};

/* Returns a menubar widget made from the above menu */
static GtkWidget *
get_menubar_menu (GtkWidget * window)
{
  GtkItemFactory *item_factory;
  GtkAccelGroup *accel_group;

  /* Make an accelerator group (shortcut keys) */
  accel_group = gtk_accel_group_new ();

  /* Make an ItemFactory (that makes a menubar) */
  item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>",
				       accel_group);

  /* This function generates the menu items. Pass the item factory,
     the number of items in the array, the array itself, and any
     callback data for the the menu items. */
  gtk_item_factory_create_items (item_factory,
				 sizeof (menu_items) / sizeof (menu_items[0]),
				 menu_items, NULL);

  /* Attach the new accelerator group to the window. */
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

  /* Finally, return the actual menu bar created by the item factory. */
  return gtk_item_factory_get_widget (item_factory, "<main>");
}

static void
trac_toggled_cb (GtkWidget * w, void *data)
{
  trac_followout = !trac_followout;
  trac_follow_end ();
}

int
main (int argc, char *argv[])
{
  GIOChannel *chan = 0;
  GtkSourceLanguagesManager *lm;
  guint inputio;
  int traced = 0, ix;
  GError *err = 0;
  char buf[200];
  char hn[64];
  /* initialization */
  gtk_init (&argc, &argv);
  for (ix = 1; ix < argc; ix++)
    {
      if (!strcmp (argv[ix], "--traced") || !strcmp (argv[ix], "-T"))
	traced = 1;
#ifndef NDEBUG
      if (!strcmp (argv[ix], "--debug") || !strcmp (argv[ix], "-D"))
	dbgfile = stderr;
#endif
    }
  arrow_right_15x15_pixbuf =
    gdk_pixbuf_new_from_xpm_data (arrow_right_15x15_xpm);
  indifferent_13x14_pixbuf =
    gdk_pixbuf_new_from_xpm_data (indifferent_13x14_xpm);
  sb_info_10x15_pixbuf = gdk_pixbuf_new_from_xpm_data (sb_info_10x15_xpm);
  mini_info_12x14_pixbuf = gdk_pixbuf_new_from_xpm_data (mini_info_12x14_xpm);
  key_7x11_pixbuf = gdk_pixbuf_new_from_xpm_data (key_7x11_xpm);
  tree_24x24_pixbuf = gdk_pixbuf_new_from_xpm_data (tree_24x24_xpm);
  action_table = g_hash_table_new (g_str_hash, g_str_equal);
  lm = gtk_source_languages_manager_new ();
  chan = g_io_channel_unix_new (STDIN_FILENO);
  g_io_channel_set_encoding (chan, (const gchar *) "latin1", &err);
  inputio = g_io_add_watch (chan, G_IO_IN, ioreader, (gpointer) 0);
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (window), 1);
  gtk_window_set_default_size (GTK_WINDOW (window), 450, 300);
  gtk_window_set_title (GTK_WINDOW (window), "simple GCC probe");
  vbox = gtk_vbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  menubar = get_menubar_menu (window);
  gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, TRUE, 1);
  gethostname (hn, sizeof (hn));
  snprintf (buf, sizeof (buf), "GCC simple probe pid %d on %s",
	    (int) getpid (), hn);
  buf[sizeof (buf) - 1] = 0;
  fileinfo_array = g_ptr_array_sized_new (200);
  pointinfo_array = g_ptr_array_sized_new (400);
  infodialog_array = g_ptr_array_sized_new (300);
  mainlabel = gtk_label_new (buf);
  gtk_box_pack_start (GTK_BOX (vbox), mainlabel, FALSE, FALSE, 1);
  versionlab = gtk_label_new ((char *) 0);
  gtk_label_set_selectable (GTK_LABEL (versionlab), TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), versionlab, FALSE, FALSE, 1);
  notebook = gtk_notebook_new ();
  gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 1);
  stabar = gtk_statusbar_new ();
  stid_pass =
    gtk_statusbar_get_context_id (GTK_STATUSBAR (stabar), "passctx");
  gtk_statusbar_push (GTK_STATUSBAR (stabar), stid_pass, "no pass");
  gtk_box_pack_start (GTK_BOX (vbox), stabar, FALSE, FALSE, 1);
  lang_mgr = gtk_source_languages_manager_new ();
  g_signal_connect (G_OBJECT (window), "destroy",
		    G_CALLBACK (destroy_cb), NULL);
  register_action ("PROB_destroydialog", destroydialog_act, (void *) 0);
  register_action ("PROB_dialogchoice", dialogchoice_act, (void *) 0);
  register_action ("PROB_dialogcontent", dialogcontent_act, (void *) 0);
  register_action ("PROB_dialognavig", dialognavig_act, (void *) 0);
  register_action ("PROB_file", file_act, (void *) 0);
  register_action ("PROB_infopoint", infopoint_act, (void *) 0);
  register_action ("PROB_message", message_act, (void *) 0);
  register_action ("PROB_showdialog", showdialog_act, (void *) 0);
  register_action ("PROB_version", version_act, (void *) 0);
  if (traced)
    {
      char buf[100];
      memset (buf, 0, sizeof (buf));
      snprintf (buf, sizeof (buf) - 1, "GCC simple probe trace pid %ld",
		(long) getpid ());
      tracwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_default_size (GTK_WINDOW (tracwindow), 500, 400);
      g_signal_connect (G_OBJECT (tracwindow), "destroy",
			G_CALLBACK (tracdestroy_cb), NULL);
      gtk_container_set_border_width (GTK_CONTAINER (tracwindow), 1);
      gtk_window_set_title (GTK_WINDOW (tracwindow), "simple GCC trace");
      tracbox = gtk_vbox_new (FALSE, 2);
      dbgprintf ("tracbox %p", tracbox);
      traccheck =
	gtk_check_button_new_with_label ("autoscroll follow output");
      g_signal_connect (traccheck, "toggled", G_CALLBACK (trac_toggled_cb),
			NULL);
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (traccheck), 1);
      tracscroll = gtk_scrolled_window_new (0, 0);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (tracscroll),
				      GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
      gtk_container_add (GTK_CONTAINER (tracwindow), tracbox);
      gtk_box_pack_start (GTK_BOX (tracbox), gtk_label_new (buf), FALSE,
			  FALSE, 1);
      gtk_box_pack_start (GTK_BOX (tracbox), traccheck, FALSE, FALSE, 1);
      tractagtbl = gtk_text_tag_table_new ();
      dbgprintf ("tractagtbl %p", tractagtbl);
      tractxtbuf = gtk_text_buffer_new (tractagtbl);
      dbgprintf ("tractxtbuf %p", tractxtbuf);
      tractag_tim = gtk_text_buffer_create_tag (tractxtbuf, "tim",
						"weight", PANGO_WEIGHT_BOLD,
						"scale", PANGO_SCALE_SMALL,
						"foreground",
						"DarkGoldenrod4", (void *) 0);
      tractag_title =
	gtk_text_buffer_create_tag (tractxtbuf, "title", "scale",
				    PANGO_SCALE_X_LARGE, "foreground", "red",
				    (void *) 0);
      tractag_imp =
	gtk_text_buffer_create_tag (tractxtbuf, "imp", "scale",
				    PANGO_SCALE_LARGE, "weight",
				    PANGO_WEIGHT_BOLD, "foreground", "red",
				    (void *) 0);
      tractag_in =
	gtk_text_buffer_create_tag (tractxtbuf, "in", "foreground", "blue",
				    (void *) 0);
      tractag_out =
	gtk_text_buffer_create_tag (tractxtbuf, "out", "style",
				    PANGO_STYLE_ITALIC, "foreground",
				    "darkgreen", (void *) 0);
      tracview = gtk_text_view_new_with_buffer (tractxtbuf);
      dbgprintf ("tracview %p", tracview);
      gtk_text_view_set_editable (GTK_TEXT_VIEW (tracview), FALSE);
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tracview), GTK_WRAP_CHAR);
      dbgprintf ("tracscroll %p", tracscroll);
      gtk_container_add (GTK_CONTAINER (tracscroll), tracview);
      dbgprintf ("tracbox %p", tracbox);
      gtk_box_pack_start (GTK_BOX (tracbox), tracscroll, TRUE, TRUE, 1);
      insert_trace_time (1, "TRACE [compiled " __DATE__ "@" __TIME__ "]: ");
      gtk_widget_show_all (tracwindow);
    };
  gtk_widget_show_all (window);
  gtk_main ();
  return 0;
}

/**** for emacs 
  ++ Local Variables: ++
  ++ compilation-directory: "." ++
  ++ compile-command: "gcc -Wall -O -g $(pkg-config --cflags --libs gtksourceview-1.0 gtk+-2.0) -o $HOME/bin/simple-probe simple-probe.c" ++
  ++ End: ++
 ****/

/* eof simple-probe.c */
