#!/usr/bin/python
# -*- coding: utf-8 -*-
# vim: set ts=4 sw=4 et:

#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""
MELT probe in PyQt
"""

## See also git://git.mandriva.com/users/alissy/python-qt-melt-probe.git
## which contains the "upstream" version of this script.

import sys
import os
import argparse
import select
import pprint
import re
import logging
from datetime import datetime
from threading import Thread
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from PyQt4.Qsci import *

MELT_SIGNAL_UNHANDLED_COMMAND = SIGNAL("unhandledCommand(PyQt_PyObject)")
MELT_SIGNAL_DISPATCH_COMMAND = SIGNAL("dispatchCommand(PyQt_PyObject)")
MELT_SIGNAL_APPEND_TRACE_COMMAND = SIGNAL("appendCommand(PyQt_PyObject)")
MELT_SIGNAL_APPEND_TRACE_REQUEST = SIGNAL("appendRequest(PyQt_PyObject)")
MELT_SIGNAL_SOURCE_SHOWFILE = SIGNAL("sourceShowfile(PyQt_PyObject)")
MELT_SIGNAL_SOURCE_MARKLOCATION = SIGNAL("sourceMarklocation(PyQt_PyObject)")
MELT_SIGNAL_SOURCE_INFOLOCATION = SIGNAL("sourceInfoLocation(PyQt_PyObject)")
MELT_SIGNAL_ASK_INFOLOCATION = SIGNAL("askInfoLocation(PyQt_PyObject)")
MELT_SIGNAL_MOVE_TO_INDICATOR = SIGNAL("moveToIndicator(PyQt_PyObject)")

MELT_SIGNAL_SOURCE_STARTINFOLOC = SIGNAL("startInfoLocation(PyQt_PyObject)")
MELT_SIGNAL_SOURCE_ADDINFOLOC = SIGNAL("addInfoLocation(PyQt_PyObject)")

MELT_SIGNAL_SHOWFILE_COMPLETE = SIGNAL("showfileComplete(PyQt_PyObject)")

MELT_SIGNAL_INFOLOC_COMPLETE = SIGNAL("infolocComplete(PyQt_PyObject)")

MELT_SIGNAL_INFOLOC_QUIT = SIGNAL("quitInfoloc()")

MELT_SIGNAL_UPDATECOUNT = SIGNAL("updateCount(PyQt_PyObject)")
MELT_SIGNAL_UPDATECURRENT = SIGNAL("updateCurrent(PyQt_PyObject)")

MELT_SIGNAL_GETVERSION = SIGNAL("getVersion(PyQt_PyObject)")

logger = logging.getLogger('melt-probe')
console = logging.StreamHandler()
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
console.setFormatter(formatter)
logger.addHandler(console)

class MeltInfoLoc(QMainWindow):
    INFOLOC_IDENT_RE = re.compile(r"(\d+):(.*)")

    def __init__(self):
        QMainWindow.__init__(self)
        self.handled_marknums = {}
        self.initUI()

    def initUI(self):
        window = QWidget()
        self.vlayout = QVBoxLayout()
        self.tree = QTreeWidget()
        # {'marknum': 543, 'command': 'addinfoloc', 'filenum': 1, 'payload': [' "1:Basic Block #10 Gimple Seq', '[drivers/media/rc/imon.c : 1247:10] rel_x.11 = (signed char) rel_x;\\n[drivers/media/rc/imon.c : 1247:10] D.21223 = rel_x.11 | -16;\\n[drivers/media/rc/imon.c : 1247:10] rel_x = (char) D.21223;\\n"  ']}
        # columns:
        #  - "1" -> infolocid
        #  - "Basic Block #10 Gimple Seq" -> bb
        #  - ... -> content
        self.tree.setColumnCount(3)
        self.tree.setHeaderLabels(["ID", "Block", "Content"])
        self.vlayout.addWidget(self.tree)
        window.setLayout(self.vlayout)
        window.show()
        self.setCentralWidget(window)
        self.setGeometry(0, 0, 320, 240)
        self.setWindowTitle("MELT InfoLoc Window - PID:PPID=%(pid)d:%(ppid)d @%(host)s" % {'pid': os.getpid(), 'ppid': os.getppid(), 'host': os.uname()[1]})
        self.show()

    def push_infolocation(self, obj):
        logger.debug("push_infolocation(%(obj)s)" % {'obj': obj})
        ident = obj['payload'][0].replace('"', '')
        payload = obj['payload'][1].replace('"', '').split("\\n")

        getident = self.INFOLOC_IDENT_RE.search(ident)
        if getident:
            id = getident.group(1)
            marknum_key = str(obj['marknum']) + ":" + str(id)
            logger.debug("Checking for previously handled %(marknum_key)s ..." % {'marknum_key': marknum_key})
            if self.handled_marknums.has_key(marknum_key):
                logger.debug("Already handled %(marknum_key)s not duplicating." % {'marknum_key': marknum_key})
                return

            block = getident.group(2)
            cols = QStringList()
            cols.append(id)
            cols.append(block)
            item = QTreeWidgetItem(cols, QTreeWidgetItem.UserType)
            for line in payload:
                chcols = QStringList()
                chcols.append("")
                chcols.append("")
                chcols.append(line)
                child = QTreeWidgetItem(item, chcols, QTreeWidgetItem.UserType)
                item.addChild(child)
            self.tree.addTopLevelItem(item)
            self.handled_marknums[marknum_key] = True

    def closeEvent(self, ev):
        self.emit(MELT_SIGNAL_INFOLOC_QUIT)

class MeltSourceViewer(QsciScintilla):
    ARROW_MARKER_PENDING = 8
    ARROW_MARKER_SELECTED = 9

    def __init__(self, parent, obj):
        QsciScintilla.__init__(self, parent)

        self.infolocs = {}
        self.mil_to_marknum = {}
        self.indicators = {}
        self.marklocations = {}
        self.markers_counter = {}
        self.file = obj
        self.setReadOnly(True)
        self.setObjectName("MeltSourceViewer:" + self.file['filename'])
        self.indicatorPending = self.indicatorDefine(QsciScintilla.BoxIndicator)
        self.indicatorSelected = self.indicatorDefine(QsciScintilla.DotBoxIndicator)

        # Set the default font
        font = QFont()
        font.setFamily('Courier')
        font.setFixedPitch(True)
        font.setPointSize(10)
        ## self.setFont(font)
        self.setMarginsFont(font)

        # Margin 0 is used for line numbers
        fontmetrics = QFontMetrics(font)
        self.setMarginsFont(font)
        self.setMarginWidth(0, fontmetrics.width("00000") + 6)
        self.setMarginLineNumbers(0, True)
        self.setMarginsBackgroundColor(QColor("#cccccc"))

        # Clickable margin 1 for showing markers
        self.setMarginSensitivity(1, False)
        # self.connect(self,
        #    SIGNAL('marginClicked(int, int, Qt::KeyboardModifiers)'),
        #    self.on_margin_clicked)
        self.markerDefine(QsciScintilla.RightArrow,
            self.ARROW_MARKER_PENDING)
        self.setMarkerBackgroundColor(QColor("#ee1111"),
            self.ARROW_MARKER_PENDING)
        self.markerDefine(QsciScintilla.RightArrow,
            self.ARROW_MARKER_SELECTED)
        self.setMarkerBackgroundColor(QColor("#11ee11"),
            self.ARROW_MARKER_SELECTED)

        self.connect(self,
            SIGNAL('indicatorClicked(int, int, Qt::KeyboardModifiers)'),
            self.on_indicator_clicked)

        # Brace matching: enable for a brace immediately before or after
        # the current position
        #
        self.setBraceMatching(QsciScintilla.SloppyBraceMatch)

        # Current line visible with special background color
        self.setCaretLineVisible(True)
        self.setCaretLineBackgroundColor(QColor("#ffe4e4"))

        # Set lexer
        # Set style for Python comments (style number 1) to a fixed-width
        # courier.
        #
        ## lexer.setDefaultFont(font)
        self.setLexer(self.select_lexer(self.file['filename']))
        ## self.SendScintilla(QsciScintilla.SCI_STYLESETFONT, 1, 'Courier')

        # Don't want to see the horizontal scrollbar at all
        # Use raw message to Scintilla here (all messages are documented
        # here: http://www.scintilla.org/ScintillaDoc.html)
        self.SendScintilla(QsciScintilla.SCI_SETHSCROLLBAR, 0)

        # not too small
        self.setMinimumSize(600, 450)

        self.append(self.read_file(self.file['filename']))

    def get_filenum(self):
        return self.file['filenum']

    def select_lexer(self, filename):
        lexer = QsciLexerBash()
        fname, ext = os.path.splitext(filename)

        if ext == ".c" or ext == ".cpp" or ext == ".h" or ext == ".hpp":
            lexer = QsciLexerCPP()

        return lexer

    def read_file(self, filename):
        if (filename.startswith("<") and filename.endswith(">")):
            return "Pseudo file, built-in."

        content = ""
        with open(filename) as f:
            content = f.readlines()
        return "".join(content)

    def marknum_to_lineindex(self, marknum):
        if not self.marklocations.has_key(marknum):
            return None
        else:
            return self.marklocations[marknum]

    def lineindex_to_marknum(self, line, index):
        key = str(line) + ":" + str(index)
        key2 = str(line) + ":" + str(index + 1)
        if not self.indicators.has_key(key) and not self.indicators.has_key(key2):
            return None
        else:
            return self.marklocations[self.indicators[key]['marknum']]

    def set_marker(self, line, stateFrom, stateTo):
        self.markerDelete(line, stateFrom)
        self.markerAdd(line, stateTo)

    def set_marker_pending(self, line, init):
        logger.debug("entering set_marker_pending, line: %(line)d counter: %(counter)d" % {'line': line, 'counter': self.markers_counter[line]})

        change = init
        if not init:
            self.markers_counter[line] -= 1
            if self.markers_counter[line] <= 0:
                change = True

        if change:
            self.set_marker(line, self.ARROW_MARKER_SELECTED, self.ARROW_MARKER_PENDING)

        logger.debug("leaving set_marker_pending, line: %(line)d counter: %(counter)d" % {'line': line, 'counter': self.markers_counter[line]})

    def set_marker_selected(self, line):
        logger.debug("entering set_marker_selected, line: %(line)d counter: %(counter)d" % {'line': line, 'counter': self.markers_counter[line]})
        self.markers_counter[line] += 1
        self.set_marker(line, self.ARROW_MARKER_PENDING, self.ARROW_MARKER_SELECTED)
        logger.debug("leaving set_marker_selected, line: %(line)d counter: %(counter)d" % {'line': line, 'counter': self.markers_counter[line]})

    def switch_marklocation_pending(self, marknum, init = False):
        pos = self.marknum_to_lineindex(marknum)
        if pos is not None:
            line = pos['line']
            index = pos['index']
            self.clearIndicatorRange(line, index, line, index + 2, self.indicatorSelected)
            self.fillIndicatorRange(line, index, line, index + 2, self.indicatorPending)
            self.set_marker_pending(line, init)

    def switch_marklocation_selected(self, marknum):
        pos = self.marknum_to_lineindex(marknum)
        if pos is not None:
            line = pos['line']
            index = pos['index']
            self.clearIndicatorRange(line, index, line, index + 2, self.indicatorPending)
            self.fillIndicatorRange(line, index, line, index + 2, self.indicatorSelected)
            self.set_marker_selected(line)

    def mark_location(self, o):
        line = o['line']
        index = o['col']
        if not self.markers_counter.has_key(line):
            self.markers_counter[line] = 0
        self.marklocations[o['marknum']] = {'line': line, 'index': index}
        self.indicators[str(line) + ":" + str(index)] = o
        self.indicators[str(line) + ":" + str(index + 1)] = o
        self.switch_marklocation_pending(o['marknum'], True)
        logger.debug("Adding marker on line %(line)d of file %(file)s" % {'file': self.file['filename'], 'line': line})

    def on_margin_clicked(self, nmargin, nline, modifiers):
        # Toggle marker for the line the margin was clicked on
        if self.markersAtLine(nline) != 0:
            self.markerDelete(nline, self.ARROW_MARKER_NUM)
        else:
            self.markerAdd(nline, self.ARROW_MARKER_NUM)

    def on_indicator_clicked(self, line, index, state):
        logger.debug("on_indicator_clicked(%(line)d, %(index)d, %(state)s)" % {'line': line, 'index': index, 'state': state})
        indic = self.indicators[str(line) + ":" + str(index)]
        self.emit(MELT_SIGNAL_SOURCE_INFOLOCATION, indic)

    def slot_marklocation(self, o):
        if (self.file['filenum'] == o['filenum']):
            self.mark_location(o)

    def slot_startinfolocation(self, o):
        if (self.file['filenum'] == o['filenum']):
            logger.debug("slot_startinfolocation(%(o)s)" % {'o': o})
            try:
                w = self.infolocs[o['marknum']]
                w.raise_()
                w.setFocus(True)
            except KeyError as e:
                mil = MeltInfoLoc()
                QObject.connect(mil, MELT_SIGNAL_INFOLOC_QUIT, self.slot_infolocation_quit, Qt.QueuedConnection)
                self.infolocs[o['marknum']] = mil
                self.mil_to_marknum[mil] = o['marknum']
                self.switch_marklocation_selected(o['marknum'])
                self.emit(MELT_SIGNAL_INFOLOC_COMPLETE, o['marknum'])

    def slot_addinfolocation(self, o):
        if (self.file['filenum'] == o['filenum']):
            logger.debug("slot_addinfolocation(%(o)s)" % {'o': o})
            w = self.infolocs[o['marknum']]
            if w is not None:
                w.push_infolocation(o)

    def slot_infolocation_quit(self):
        marknum = self.mil_to_marknum[self.sender()]
        if marknum:
            self.infolocs.pop(marknum)
            self.switch_marklocation_pending(marknum)

    def slot_moveToIndicator(self, indic):
        if (self.file['filenum'] == indic['filenum']):
            logger.debug("Received a request to move to indicator: %(indic)s" % {'indic': indic})
            self.setCursorPosition(indic['line'], indic['col'])
            self.setCaretLineVisible(True)
            self.setCaretLineBackgroundColor(QColor("#ffe4e4"))

class MeltCommandDispatcher(QObject, Thread):
    FILES = {}
    MARKS = {}
    QUEUE_MARKLOCATION_MUTEX = QMutex()
    SHOWFILE_READY = {}
    QUEUE_MARKLOCATION = {}

    QUEUE_INFOLOC_MUTEX = QMutex()
    INFOLOC_READY = {}
    QUEUE_INFOLOC = {}

    def __init__(self):
        QObject.__init__(self)
        Thread.__init__(self)
        self.daemon = True
        self.start()

    def run(self):
        print "I'm", self.getName()

    def slot_unhandledCommand(self, cmd):
        logger.error("Unhandled command: %(comm)s" % {'comm': cmd})

    def slot_dispatchCommand(self, comm):
        logger.debug("Dispatcher receive: %(comm)s" % {'comm': comm})

        o = comm.split(" ")
        self.emit(MELT_SIGNAL_APPEND_TRACE_COMMAND, o)

        sig = MELT_SIGNAL_UNHANDLED_COMMAND
        obj = o
        cmd = o[0]
        if cmd == "SHOWFILE_PCD":
            fnum = int(o[4])
            p = o[2].strip('"')
            if not (p.startswith("<") and p.endswith(">")):
                p = os.path.abspath(p)
            obj = {'command': 'showfile', 'filename': p, 'filenum': fnum}
            sig = MELT_SIGNAL_SOURCE_SHOWFILE
            if not self.FILES.has_key(fnum):
                self.FILES[fnum] = {'file': obj, 'marks': {}}
        elif cmd == "MARKLOCATION_PCD":
            # -1 pour corriger l'affichage
            marknum = int(o[1])
            filenum = int(o[2])
            obj = {'command': 'marklocation', 'marknum': marknum, 'filenum': filenum, 'line': max(int(o[3]) - 1, 0), 'col': max(int(o[4]) - 1, 0)}
            sig = MELT_SIGNAL_SOURCE_MARKLOCATION
            if not self.MARKS.has_key(marknum):
                self.MARKS[marknum] = filenum
                self.FILES[filenum]['marks'][marknum] = obj
                self.QUEUE_INFOLOC[marknum] = []
            # If SHOWFILE interface has not been completed, enqueue, and we will dequeue
            # when the interface is ready
            self.QUEUE_MARKLOCATION_MUTEX.lock()
            if not self.SHOWFILE_READY.has_key(filenum):
                try:
                    self.QUEUE_MARKLOCATION[filenum] += [ obj ]
                except KeyError as e:
                    self.QUEUE_MARKLOCATION[filenum] = [ obj ]
                finally:
                    self.QUEUE_MARKLOCATION_MUTEX.unlock()
                return
            self.QUEUE_MARKLOCATION_MUTEX.unlock()
        elif cmd == "STARTINFOLOC_PCD":
            marknum = int(o[1])
            filenum = self.MARKS[marknum]
            obj = {'command': 'startinfoloc', 'marknum': marknum, 'filenum': filenum}
            sig = MELT_SIGNAL_SOURCE_STARTINFOLOC
        elif cmd == "ADDINFOLOC_PCD":
            marknum = int(o[1])
            filenum = self.MARKS[marknum]
            obj = {'command': 'addinfoloc', 'marknum': marknum, 'filenum': filenum, 'payload': " ".join(o[2:]).split('"   "')}
            sig = MELT_SIGNAL_SOURCE_ADDINFOLOC
            # If INFOLOC interface has not been completed, enqueue, and we will dequeue
            # when the interface is ready
            self.QUEUE_INFOLOC_MUTEX.lock()
            if not self.INFOLOC_READY.has_key(marknum):
                self.QUEUE_INFOLOC[marknum] += [ obj ]
                self.QUEUE_INFOLOC_MUTEX.unlock()
                return
            self.QUEUE_INFOLOC_MUTEX.unlock()
        elif cmd == "SETSTATUS_PCD":
            # ['SETSTATUS_PCD', '', '"MELT', 'version=0.9.6-d', '[melt-branch_revision_190124]"', '', '']
            version = o[3].split('=')[1]
            rev = o[4].replace('"', "").replace("[", "").replace("]", "")
            obj = {'command': 'setstatus', 'version': version, 'rev': rev}
            sig = MELT_SIGNAL_GETVERSION

        logger.debug("Dispatcher emit: %(sig)s %(obj)s" % {'sig': sig, 'obj': obj})

        self.emit(sig, obj)

    def slot_sendInfoLocation(self, obj):
        self.emit(MELT_SIGNAL_ASK_INFOLOCATION, "INFOLOCATION_prq " + str(obj['marknum']))

    def slot_showfileComplete(self, filenum):
        self.QUEUE_MARKLOCATION_MUTEX.lock()
        try:
            queue = self.QUEUE_MARKLOCATION[filenum]
            logger.debug("SHOWFILE has been completed for %(filenum)s QUEUED %(queue)s" % {'filenum': filenum, 'queue': queue})
            for obj in queue:
                self.emit(MELT_SIGNAL_SOURCE_MARKLOCATION, obj)
        except KeyError as e:
            # nothing has been put in queue, bypassing
            pass
        self.SHOWFILE_READY[filenum] = True
        self.QUEUE_MARKLOCATION_MUTEX.unlock()

    def slot_infolocComplete(self, marknum):
        self.QUEUE_INFOLOC_MUTEX.lock()
        queue = self.QUEUE_INFOLOC[marknum]
        logger.debug("INFOLOC has been completed for %(marknum)s QUEUED %(queue)s" % {'marknum': marknum, 'queue': queue})
        for obj in queue:
            self.emit(MELT_SIGNAL_SOURCE_ADDINFOLOC, obj)
        self.INFOLOC_READY[marknum] = True
        self.QUEUE_INFOLOC_MUTEX.unlock()

class MeltCommunication(QObject, Thread):
    def __init__(self, fdin, fdout):
        QObject.__init__(self)
        Thread.__init__(self)
        self.melt_stdout = fdin
        self.melt_stdin  = fdout

        self.epoll = select.epoll()
        self.epoll.register(self.melt_stdout, select.EPOLLIN)

        self.buf = ""
        self.daemon = True

    def run(self):
        print "I'm", self.getName()
        try:
            while True:
                events = self.epoll.poll(1)
                for fileno, event in events:
                    if event & select.EPOLLIN:
                        c = os.read(fileno, 1)
                        if c == '\n':
                            if len(self.buf) > 0:
                                self.command = self.buf
                                self.emit(MELT_SIGNAL_DISPATCH_COMMAND, self.command)
                            self.buf = ""
                        else:
                            self.buf += c
                    elif event & select.EPOLLOUT:
                        print "READY TO WRITE"
                    elif event & select.EPOLLHUP:
                        self.epoll.unregister(fileno)
        finally:
            self.epoll.unregister(self.melt_stdout)
            self.epoll.close()

    def send_melt_command(self, str):
        self.emit(MELT_SIGNAL_APPEND_TRACE_REQUEST, str)
        return os.write(self.melt_stdin, str + "\n\n")

    def slot_sendInfoLocation(self, cmd):
        self.send_melt_command(cmd)

class MeltTraceWindow(QMainWindow, Thread):
    def __init__(self):
        Thread.__init__(self)
        super(MeltTraceWindow, self).__init__()
        self.initUI()
        self.daemon = True
        self.start()

    def initUI(self):
        self.text = QTextEdit()
        self.setCentralWidget(self.text)
        self.setGeometry(0, 0, 640, 480)
        self.setWindowTitle("MELT Trace Window - PID:PPID=%(pid)d:%(ppid)d @%(host)s" % {'pid': os.getpid(), 'ppid': os.getppid(), 'host': os.uname()[1]})
        self.show()

    def run(self):
        print "I'm", self.getName()
        pass

    def slot_appendCommand(self, command):
        str = "<font color=\"gray\">%(date)s</font><br /><font color=\"blue\">%(command)s</font><br />" % {'date': datetime.isoformat(datetime.now()), 'command': " ".join(command)}
        self.text.append(str)

    def slot_appendRequest(self, command):
        str = "<font color=\"gray\">%(date)s</font><br /><font color=\"red\">%(command)s</font><br />" % {'date': datetime.isoformat(datetime.now()), 'command': command}
        self.text.append(str)

class MeltSourceWindow(QMainWindow, Thread):
    LBL_COUNT = "Count: %(cnt)d"
    COUNTS = {}
    LBL_VERSION = "Version: %(version)s"
    LBL_REVISION = "Revision: %(revision)s"
    INDICATORS = {}
    CURRENT_INDICATOR = {}
    LBL_CURRENT = "Current: %(cur)d"

    def __init__(self, dispatcher, comm):
        Thread.__init__(self)
        super(MeltSourceWindow, self).__init__()
        self.dispatcher = dispatcher
        self.comm = comm
        self.filemaps = {}
        self.filemaps_reverse = {}
        self.initUI()

        QObject.connect(self.dispatcher, MELT_SIGNAL_SOURCE_SHOWFILE, self.slot_showfile, Qt.QueuedConnection)
        QObject.connect(self.dispatcher, MELT_SIGNAL_GETVERSION, self.slot_getversion, Qt.QueuedConnection)
        QObject.connect(self, MELT_SIGNAL_UPDATECOUNT, self.slot_updateCount, Qt.QueuedConnection)
        QObject.connect(self, MELT_SIGNAL_UPDATECURRENT, self.slot_updateCurrent, Qt.QueuedConnection)
        self.daemon = True
        self.start()

    def initUI(self):
        window = QWidget()
        self.tabs = QTabWidget()
        self.tabs.setTabPosition(QTabWidget.West)
        self.header = QHBoxLayout()
        self.vlayout = QVBoxLayout()
        self.vlayout.addLayout(self.header)
        self.vlayout.addWidget(self.tabs)
        window.setLayout(self.vlayout)
        window.show()
        self.setCentralWidget(window)
        self.setGeometry(0, 0, 640, 480)
        self.setWindowTitle("MELT Source Window - PID:PPID=%(pid)d:%(ppid)d @%(host)s" % {'pid': os.getpid(), 'ppid': os.getppid(), 'host': os.uname()[1]})

        self.show()

        self.comm.send_melt_command("VERSION_prq")

    def run(self):
        print "I'm", self.getName()
        pass

    def get_filename(self, path):
        (dir, fname) = os.path.split(path)
        return fname

    def get_count(self, filenum):
        return self.LBL_COUNT % {'cnt': self.COUNTS[filenum]}

    def get_current(self, filenum):
        return self.LBL_CURRENT % {'cur': self.CURRENT_INDICATOR[filenum]}

    def slot_showfile(self, o):
        qw = QWidget()
        layout = QVBoxLayout()
        qw.setLayout(layout)

        if o['filename'] == "/dev/null":
            logger.error('Cannot open /dev/null, exiting.')
            sys.exit(0)

        if os.path.exists(o['filename']) or (o['filename'].startswith("<") and o['filename'].endswith(">")):
            txt = MeltSourceViewer(qw, o)
            lbl = QLabel(o['filename'])
            lbl.setObjectName("filename")
            self.COUNTS[o['filenum']] = 0
            cnt = QLabel(self.get_count(o['filenum']))
            cnt.setObjectName("count")
            self.CURRENT_INDICATOR[o['filenum']] = 0
            cur = QLabel(self.get_current(o['filenum']))
            cur.setObjectName("current")
            hlayout = QHBoxLayout()
            hlayout.addWidget(cnt)
            hlayout.addWidget(cur)
            searchBar = QToolBar()
            searchBar.setObjectName("search")
            prevIndic = searchBar.addAction("<", self.slot_prevIndicator)
            prevIndic.setToolTip("Go to previous GCC mark")
            nextIndic = searchBar.addAction(">", self.slot_nextIndicator)
            nextIndic.setToolTip("Go to next GCC mark")
            searchBar.addSeparator()
            searchLabel = QLabel("Search: ")
            searchText = QLineEdit()
            searchBar.addWidget(searchLabel)
            searchBar.addWidget(searchText)
            searchReset = searchBar.addAction("Reset", self.slot_searchReset)
            searchNext = searchBar.addAction("Next", self.slot_searchNext)
            QObject.connect(self.dispatcher, MELT_SIGNAL_SOURCE_MARKLOCATION, txt.slot_marklocation, Qt.QueuedConnection)
            QObject.connect(txt, MELT_SIGNAL_SOURCE_INFOLOCATION, self.dispatcher.slot_sendInfoLocation, Qt.QueuedConnection)
            QObject.connect(txt, MELT_SIGNAL_INFOLOC_COMPLETE, self.dispatcher.slot_infolocComplete, Qt.QueuedConnection)
            QObject.connect(self.dispatcher, MELT_SIGNAL_SOURCE_STARTINFOLOC, txt.slot_startinfolocation, Qt.QueuedConnection)
            QObject.connect(self.dispatcher, MELT_SIGNAL_SOURCE_ADDINFOLOC, txt.slot_addinfolocation, Qt.QueuedConnection)
            QObject.connect(self, MELT_SIGNAL_MOVE_TO_INDICATOR, txt.slot_moveToIndicator, Qt.QueuedConnection)
            layout.addWidget(lbl)
            layout.addWidget(txt)
            layout.addLayout(hlayout)
            layout.addWidget(searchBar)
            # searchBar.hide()
            self.tabs.addTab(qw, "[%(fnum)s] %(filename)s" % {'fnum': o['filenum'], 'filename': self.get_filename(o['filename'])})
            self.filemaps[o['filenum']] = qw
            self.filemaps_reverse[txt] = o['filenum']
            logger.debug("Mapping %(filenum)s with object %(object)s" % {'filenum': o['filenum'], 'object': txt.objectName()})
            self.emit(MELT_SIGNAL_SHOWFILE_COMPLETE, o['filenum'])
        else:
            logger.error("Unable to open '%(file)s'" % {'file': o['filename']})
            return
            err = QErrorMessage("Unable to open '%(file)s'" % {'file': o['filename']})
            err.showMessage()

    def slot_marklocation(self, obj):
        self.COUNTS[obj['filenum']] += 1
        try:
            self.INDICATORS[obj['filenum']] += [ obj ]
            self.INDICATORS[obj['filenum']].sort(key=lambda x: x['line'], reverse=False)
        except KeyError as e:
            self.INDICATORS[obj['filenum']] = [ obj ]
            self.CURRENT_INDICATOR[obj['filenum']] = 0
        self.emit(MELT_SIGNAL_UPDATECOUNT, obj['filenum'])

    def slot_updateCount(self, fnum):
        cnt = self.filemaps[fnum].findChild(QLabel, "count")
        if cnt:
            cnt.setText(self.get_count(fnum))

    def slot_updateCurrent(self, fnum):
        cur = self.filemaps[fnum].findChild(QLabel, "current")
        if cur:
            cur.setText(self.get_current(fnum))

    def keyReleaseEvent(self, ev):
        if (ev.modifiers() == Qt.ControlModifier and ev.key() == Qt.Key_F):
            ev.accept()
            searchBar = self.tabs.currentWidget().findChild(QToolBar, "search")
            if searchBar.isHidden():
                searchBar.show()
                self.tabs.currentWidget().findChild(QLineEdit).setFocus()
            else:
                searchBar.hide()
        else:
            if self.tabs.currentWidget().findChild(QLineEdit).hasFocus():
                self.start_search()
            else:
                logger.debug("Received key: %(key)s" % {'key': ev.key()})
                if ev.key() == Qt.Key_N:
                    if ev.modifiers() == Qt.ShiftModifier:
                        self.slot_prevIndicator()
                    else:
                        self.slot_nextIndicator()

    def slot_searchNext(self):
        searchText = self.tabs.currentWidget().findChild(MeltSourceViewer)
        if searchText:
            searchText.findNext()

    def slot_searchReset(self):
        searchText = self.tabs.currentWidget().findChild(MeltSourceViewer)
        if searchText:
            self.start_search()

    def start_search(self):
        searchText = self.tabs.currentWidget().findChild(MeltSourceViewer)
        txt = self.tabs.currentWidget().findChild(QLineEdit)
        findText = txt.text()
        if searchText and findText.length() > 1:
            searchText.setCursorPosition(0, 0)
            searchText.findFirst(findText, False, False, False, False)

    def slot_prevIndicator(self):
        self.move_indicator(-1)

    def slot_nextIndicator(self):
        self.move_indicator(1)

    def move_indicator(self, sens):
        searchText = self.tabs.currentWidget().findChild(MeltSourceViewer)
        if searchText:
            try:
                filenum = self.filemaps_reverse[searchText]
                newpos = (self.CURRENT_INDICATOR[filenum] + sens) % len(self.INDICATORS[filenum])
                self.CURRENT_INDICATOR[filenum] = newpos
                self.set_indicator(filenum, newpos)
            except KeyError as e:
                logger.error("Could not find associated file with %(obj)s" % {'obj': searchText})

    def set_indicator(self, file, id):
        if self.INDICATORS[file][id]:
            indic = self.INDICATORS[file][id]
            logger.debug("Moving indicator of %(file)s to %(pos)d at (%(line)d,%(col)d)" % {'file': file, 'pos': id, 'line': indic['line'], 'col': indic['col']})
            self.emit(MELT_SIGNAL_MOVE_TO_INDICATOR, indic)
            self.emit(MELT_SIGNAL_UPDATECURRENT, file)
        else:
            logger.error("No indicator %(id)d" % {'id': id})

    def slot_getversion(self, obj):
        logger.debug("Received version: %(version)s; revision: %(revision)s" % {'version': obj['version'], 'revision': obj['rev']})
        self.version = QLabel(self.LBL_VERSION % {'version': obj['version']})
        self.revision = QLabel(self.LBL_REVISION % {'revision': obj['rev']})
        self.header.addWidget(self.version)
        self.header.addWidget(self.revision)

class MeltProbeApplication(QApplication):
    TRACE_WINDOW = None
    SOURCE_WINDOW = None

    def __init__(self):
        self.app = QApplication(sys.argv)
        self.parse_args()

        logger.setLevel(logging.ERROR)
        console.setLevel(logging.ERROR)

        if (self.args.D):
            logger.setLevel(logging.DEBUG)
            console.setLevel(logging.DEBUG)

        self.main()

    def main(self):
        dispatcher = MeltCommandDispatcher()
        comm = MeltCommunication(self.args.command_from_MELT, self.args.request_to_MELT)
        if (self.args.T):
            self.TRACE_WINDOW = MeltTraceWindow()
        self.SOURCE_WINDOW = MeltSourceWindow(dispatcher, comm)

        QObject.connect(comm, MELT_SIGNAL_DISPATCH_COMMAND, dispatcher.slot_dispatchCommand, Qt.QueuedConnection)
        QObject.connect(dispatcher, MELT_SIGNAL_ASK_INFOLOCATION, comm.slot_sendInfoLocation, Qt.QueuedConnection)
        QObject.connect(dispatcher, MELT_SIGNAL_SOURCE_MARKLOCATION, self.SOURCE_WINDOW.slot_marklocation, Qt.QueuedConnection)
        QObject.connect(self.SOURCE_WINDOW, MELT_SIGNAL_SHOWFILE_COMPLETE, dispatcher.slot_showfileComplete, Qt.QueuedConnection)

        if (self.args.T):
            QObject.connect(dispatcher, MELT_SIGNAL_APPEND_TRACE_COMMAND, self.TRACE_WINDOW.slot_appendCommand, Qt.QueuedConnection)
            QObject.connect(comm, MELT_SIGNAL_APPEND_TRACE_REQUEST, self.TRACE_WINDOW.slot_appendRequest, Qt.QueuedConnection)

        QObject.connect(dispatcher, MELT_SIGNAL_UNHANDLED_COMMAND, dispatcher.slot_unhandledCommand, Qt.QueuedConnection)

        comm.start()
        sys.exit(self.app.exec_())

    def parse_args(self):
        self.parser = argparse.ArgumentParser(description="MELT probe")
        self.parser.add_argument("-T", action="store_true", required=False, help="Tracing mode")
        self.parser.add_argument("-D", action="store_true", required=False, help="Debug mode")
        self.parser.add_argument("--command-from-MELT", type=int, required=True, help="FD to read from")
        self.parser.add_argument("--request-to-MELT", type=int, required=True, help="FD to write to")
        self.args = self.parser.parse_args()

if __name__ == '__main__':
    mpa = MeltProbeApplication()
