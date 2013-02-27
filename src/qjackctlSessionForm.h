// qjackctlSessionForm.h
//
/****************************************************************************
   Copyright (C) 2003-2013, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#ifndef __qjackctlSessionForm_h
#define __qjackctlSessionForm_h

#include <QtGlobal>

#if QT_VERSION < 0x040200
#define setAllColumnsShowFocus(x) parent()
#endif

#include "ui_qjackctlSessionForm.h"

#include <QFileInfo>

// Forward declarations.
class qjackctlSession;
class qjackctlSetup;

class QMenu;
class QIcon;


#include <QItemDelegate>

class QLineEdit;
class QToolButton;


//-------------------------------------------------------------------------
// qjackctlSessionInfraClientItemEditor

class qjackctlSessionInfraClientItemEditor : public QWidget
{
	Q_OBJECT

public:

	// Constructor.
	qjackctlSessionInfraClientItemEditor(
		QWidget *pParent = NULL, const QModelIndex& index = QModelIndex());

	// Shortcut text accessors.
	void setText(const QString& sText);
	QString text() const;

	// Default (initial) shortcut text accessors.
	void setDefaultText(const QString& sDefaultText)
		{ m_sDefaultText = sDefaultText; }
	const QString& defaultText() const
		{ return m_sDefaultText; }

signals:

	void editingFinished();
	void editingCanceled();

public slots:

	void clear();

protected slots:

	void finish();
	void cancel();

private:

	// Instance variables.
	QLineEdit *m_pItemEdit;
	QToolButton *m_pResetButton;
	QString m_sDefaultText;
	QModelIndex m_index;
};


//-------------------------------------------------------------------------
// qjackctlSessionInfraClientItemDelegate

class qjackctlSessionInfraClientItemDelegate : public QItemDelegate
{
	Q_OBJECT

public:

	// Constructor.
	qjackctlSessionInfraClientItemDelegate(QObject *pParent = NULL);

protected:

	QWidget *createEditor(QWidget *pParent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

	void setEditorData(QWidget *pEditor,
		const QModelIndex &index) const;
	void setModelData(QWidget *pEditor,
		QAbstractItemModel *pModel,
		const QModelIndex& index) const;

protected slots:

	void commitEditor();
};


//----------------------------------------------------------------------------
// qjackctlSessionForm -- UI wrapper form.

class qjackctlSessionForm : public QWidget
{
	Q_OBJECT

public:

	// Constructor.
	qjackctlSessionForm(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
	~qjackctlSessionForm();

	// Global setup method.
	void setup(qjackctlSetup *pSetup);

	// Maybe ask whether we can close.
	bool queryClose();

	// Recent session directories and save type accessors.
	const QStringList& sessionDirs() const;
	bool isSaveSessionVersion() const;

	// Recent menu accessor.
	QMenu *recentMenu() const;

	// Save menu accessor.
	QMenu *saveMenu() const;

	void stabilizeForm(bool bEnabled);

public slots:

	void loadSession();

	void saveSessionSave();
	void saveSessionSaveAndQuit();
	void saveSessionSaveTemplate();

	void saveSessionVersion(bool);

	void updateSession();

protected slots:

	void recentSession();
	void updateRecentMenu();
	void clearRecentMenu();

	void addInfraClient();
	void editInfraClient();
	void editInfraClientCommit();
	void removeInfraClient();

	void selectInfraClient();

protected:

	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);
	void closeEvent(QCloseEvent *);

	void contextMenuEvent(QContextMenuEvent *);

	void keyPressEvent(QKeyEvent *);

	void saveSessionEx(int iSessionType = 0);

	void loadSessionDir(const QString& sSessionDir);
	void saveSessionDir(const QString& sSessionDir, int iSessionType = 0);

	void updateRecent(const QString& sSessionDir);

	void updateSessionView();
	void updateInfraClients();

	static QIcon iconStatus(const QIcon& icon, bool bStatus);

private:

	// The Qt-designer UI struct...
	Ui::qjackctlSessionForm m_ui;

	// Common (sigleton) session object.
	qjackctlSession *m_pSession;
	
	// Recent session menu.
	QMenu *m_pRecentMenu;

	// Save session menu.
	QMenu *m_pSaveMenu;

	// Setup options.
	qjackctlSetup *m_pSetup;

	// Session directory history.
	QStringList m_sessionDirs;
};


#endif	// __qjackctlSessionForm_h


// end of qjackctlSessionForm.h
