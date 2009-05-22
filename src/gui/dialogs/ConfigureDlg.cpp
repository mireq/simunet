/***************************************************************************
 *   Simunet - Computer Network Simulator                                  *
 *   http://simunet.eu/                                                    *
 *                                                                         *
 *   Copyright (C) 2008 by Miroslav Bendik                                 *
 *   miroslav.bendik@gmail.com                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "ConfigureDlg.h"

#include "SNTitleWidget.h"
#include "SNConfigPanel.h"
#include "SNIcon.h"

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QPushButton>
#include <QCloseEvent>
#include <QListWidget>

/*!
  \class ConfigureDlg
  \brief Konfiguracne okno.
  \ingroup dialogs

  Okno pomocou ktoreho sa konfiguruje SimuNet.
*/

/*!
  Vytvorenie konfiguracneho okna.
*/
ConfigureDlg::ConfigureDlg(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	setWindowTitle(tr("Configure - SimuNet"));

	m_currentItem = NULL;
	m_previousItem = NULL;

	m_titleWidget = new SNTitleWidget();
	m_panelsList = new QListWidget();
	m_panelsList->setFixedWidth(200);
	m_panelsList->setIconSize(QSize(32, 32));
	m_panelsList->setUniformItemSizes(true);
	m_panelsList->setWordWrap(true);
	m_panelsList->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(m_panelsList, SIGNAL(itemSelectionChanged()), SLOT(listSelectionChanged()));

	m_panel = new QStackedWidget();

	QVBoxLayout *layout = new QVBoxLayout();
	QHBoxLayout *centerLayout = new QHBoxLayout();
	QVBoxLayout *panelLayout = new QVBoxLayout();

	layout->addLayout(centerLayout, 1);
	centerLayout->addWidget(m_panelsList);
	centerLayout->addLayout(panelLayout);
	panelLayout->addWidget(m_panel, 1);
	panelLayout->addWidget(m_titleWidget);

	m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|QDialogButtonBox::Apply);
	layout->addWidget(m_buttons);
	setLayout(layout);

	m_titleWidget->hide();

	connect(m_buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(onOkClicked()));
	connect(m_buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(onCancelClicked()));
	connect(m_buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()), SLOT(onApplyClicked()));
}

/*!
  Zrusenie konfiguracneho dialogu.
*/
ConfigureDlg::~ConfigureDlg()
{
}

/*!
  Pridanie konfiguracneho panelu

  Do zoznamu konfiguracnych panelov sa prida novy panel.
*/
void ConfigureDlg::addPanel(SNConfigPanel *panel)
{
	QListWidgetItem *newItem = new QListWidgetItem();
	newItem->setData(Qt::DisplayRole, panel->panelName());
	newItem->setData(Qt::DecorationRole, panel->icon());
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	m_panelsList->addItem(newItem);
	m_panel->addWidget(panel);
	if (m_panelsList->model()->rowCount(QModelIndex()) == 1)
	{
		m_panelsList->setCurrentRow(0);
	}
}

/*!
  \overload
*/
void ConfigureDlg::showInfo(const QString &text, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::InfoMessage);
	showMsg(text, QString(), msecs);
}

/*!
  Tato metoda zobrazi informacnu spravu (ikona informacie).

  \sa showMessage() setMsgPixmap()
*/
void ConfigureDlg::showInfo(const QString &text, const QString &comment, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::InfoMessage);
	showMsg(text, comment, msecs);
}

/*!
  \overload
*/
void ConfigureDlg::showWarning(const QString &text, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::WarningMessage);
	showMsg(text, QString(), msecs);
}

/*!

  Tato metoda zobrazi varovnu spravu (ikona varovania).

  \sa showMessage() setMsgPixmap()
 */
void ConfigureDlg::showWarning(const QString &text, const QString &comment, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::WarningMessage);
	showMsg(text, comment, msecs);
}

/*!
  \overload
*/
void ConfigureDlg::showError(const QString &text, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::ErrorMessage);
	showMsg(text, QString(), msecs);
}

/*!

  Tato metoda zobrazi chybovu spravu (ikona chyby).

  \sa showMessage() setMsgPixmap()
 */
void ConfigureDlg::showError(const QString &text, const QString &comment, int msecs)
{
	m_titleWidget->setPixmap(SNTitleWidget::ErrorMessage);
	showMsg(text, comment, msecs);
}

/*!
  \reimp
*/
void ConfigureDlg::closeEvent(QCloseEvent *e)
{
	QWidget *w = m_panel->currentWidget();
	if (w == NULL)
	{
		return;
	}
	SNConfigPanel *panel = static_cast<SNConfigPanel *>(w);
	if (!panel->panelChanged())
	{
		e->ignore();
	}
}

/*!
  \reimp
*/
void ConfigureDlg::showEvent(QShowEvent *e)
{
	Q_UNUSED(e);
	m_currentItem = NULL;
	m_previousItem = NULL;
	m_titleWidget->setHideTimeout(0);
	m_titleWidget->hide();
	m_panelsList->setCurrentRow(0);
	listSelectionChanged();
}

/*!
  Zobrazenie spravy v informacnej casti okna.

  \param text Text ktory sa ma zobrazit.
  \param comment Komentar k danej sprave.
  \param msecs Cas v milisekundach po ktorom informacia zmzne.

  Sprava sa zobrazi v ramceku v dolnej casti okna. Text by mal byt strucny
  a vystizny. Ak nestaci kratke vysvetlenie je lepsie pouzit atribut
  \a comment. Komentar sa zobrazi pod spravou nezvyraznenym textom. Spravu
  je mozne automaticky po zvolenom casovom intervale skryt. Cas urcuje atribut
  \a msecs. Po nastaveni pixmapy sa vpravo zobrazi zvolena ikona.

  \sa setMsgPixmap() hideMsg()
 */
void ConfigureDlg::showMsg(const QString &text, const QString &comment, int msecs)
{
	m_titleWidget->setText(text);
	m_titleWidget->setComment(comment);
	m_titleWidget->show();
	m_titleWidget->setHideTimeout(msecs);
}

/*!
  Skrytie informacnej spravu.
*/
void ConfigureDlg::hideMsg()
{
}

/*!
  Nastavenie ikony pre informacnu spravu.
*/
void ConfigureDlg::setMsgPixmap(const QPixmap &pixmap)
{
	m_titleWidget->setPixmap(pixmap);
}

/*!
  \overload
*/
void ConfigureDlg::setMsgPixmap(const QString &name)
{
	m_titleWidget->setPixmap(name);
}

/*!
  \overload
*/
void ConfigureDlg::setMsgPixmap(const QIcon &icon)
{
	m_titleWidget->setPixmap(icon);
}

/*!
  \overload

  \sa SNTitleWidget::MessageType
*/
void ConfigureDlg::setMsgPixmap(SNTitleWidget::MessageType type)
{
	m_titleWidget->setPixmap(type);
}


void ConfigureDlg::connectPanel(SNConfigPanel *panel)
{
	connect(panel, SIGNAL(info(QString, QString, int)), SLOT(showInfo(QString, QString, int)));
	connect(panel, SIGNAL(warning(QString, QString, int)), SLOT(showWarning(QString, QString, int)));
	connect(panel, SIGNAL(error(QString, QString, int)), SLOT(showError(QString, QString, int)));
	connect(panel, SIGNAL(changed(bool)), m_buttons->button(QDialogButtonBox::Apply), SLOT(setEnabled(bool)));
}

void ConfigureDlg::disconnectPanel(SNConfigPanel *panel)
{
	panel->disconnect();
}

SNConfigPanel *ConfigureDlg::currentPanel()
{
	return static_cast<SNConfigPanel *>(m_panel->currentWidget());
}

void ConfigureDlg::listSelectionChanged()
{

	QListWidgetItem *pom = m_currentItem;
	m_currentItem = m_panelsList->currentItem();

	if (m_currentItem == NULL)
	{
		return;
	}

	bool changePanel = true;
	if (pom != NULL)
	{
		int prevRow = m_panelsList->row(pom);
		SNConfigPanel *panel = static_cast<SNConfigPanel *>(m_panel->widget(prevRow));
		changePanel = panel->panelChanged();
	}

	if (changePanel)
	{
		// odpojime signaly od predchadzajuceho panelu
		if (pom != NULL)
		{
			int prevRow = m_panelsList->row(pom);
			SNConfigPanel *prevPanel = static_cast<SNConfigPanel *>(m_panel->widget(prevRow));
			disconnectPanel(prevPanel);
		}

		int currRow = m_panelsList->row(m_currentItem);
		SNConfigPanel *panel = static_cast<SNConfigPanel *>(m_panel->widget(currRow));
		connectPanel(panel);
		panel->panelSelected();
		m_panel->setCurrentIndex(currRow);
		m_previousItem = m_currentItem;
	}
	else
	{
		m_panelsList->disconnect();
		if (pom != NULL)
		{
			m_panelsList->setCurrentItem(pom);
			m_previousItem->setSelected(true);
		}
		m_currentItem = m_previousItem;
		connect(m_panelsList, SIGNAL(itemSelectionChanged()), SLOT(listSelectionChanged()));
	}
}

void ConfigureDlg::onOkClicked()
{
	currentPanel()->saveChanges();
	accept();
}

void ConfigureDlg::onCancelClicked()
{
	if (currentPanel()->panelChanged())
	{
		reject();
	}
}

void ConfigureDlg::onApplyClicked()
{
	currentPanel()->saveChanges();
}



