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

#include "SNSplash.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QFontMetrics>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QLinearGradient>


/*!
  \class SNSplash
  \brief Jednoducha startovacia obrazovka pouzitelna ako informacne okno

  \ingroup widgets

  Tato trieda sluzi na zobrazenie bud startovacej obrazovky alebo
  informacneho okna (s progressbarom). Startovacia obrazovka sa vyznacuje
  tym, ze sa da zatvorit kliknutim na jej plochu. Informacne okno sa da pouzit
  na zobrazovanie roznych casovo narocnych operacii s progressbarom.


  \section skins Podpora zmeny vzhladu

  Tato startovacia obrazovka podporuje zmenu vzhladu. Vzhlad musi byt ulozeny
  v adresari pod nazvom style.ini. Tento subor ma jedinu sekciu - \b General.

  <table>
    <tr><th>Polozka</th><th>Typ</th><th>Popis</th></tr>
    <tr>
      <td>background</td>
      <td>QString</td>
      <td>Lokalna cesta k suboru s pozadim. Pozadie moze obsahovat alfa kanal,
          v takom pripade je ale nutne respektovat, ze na niektorych platformach
          nemusi podpora priehladnosti fungovat a priehladne casti tam budu
          mat cierne pozadie.</td>
    </tr>
    <tr>
      <td>progressBarBorder</td>
      <td>QPen</td>
      <td>Styl okraja progress baru.</td>
    </tr>
    <tr>
      <td>progressBarBg</td>
      <td>QBrush</td>
      <td>Pozadie progress baru.</td>
    </tr>
    <tr>
      <td>progressBarActiveBg</td>
      <td>QBrush</td>
      <td>Pozadie stlpca vo vnutri progress baru.</td>
    </tr>
    <tr>
      <td>progressPosition</td>
      <td>QRect</td>
      <td>Oblast v ktorej sa nachadza progress bar.</td>
    </tr>
    <tr>
      <td>messagePosition</td>
      <td>QRect</td>
      <td>Oblast v ktorej sa nachadza sprava. Vyska tejto oblasti musi byt 0.
          Konecna vyska sa vypocitava z vysky fontu.</td>
    </tr>
    <tr>
      <td>messageColor</td>
      <td>QColor</td>
      <td>Farba fontu spravy.</td>
    </tr>
  </table>

  Priklad stylu:
  \verbatim
[General]
background=splash.png
progressBarBorder=@Variant(\0\0\0MQ?\xf0\0\0\0\0\0\0\x1\x1\xff\xff\f\f\v\vII\0\0@\0\0\0\0\0\0\0\0\0\0\0)
progressBarBg=@Variant(\0\0\0\x42\0\x1\xff\xff\0\0\0\0\0\0\0\0)
progressBarActiveBg=@Variant(\0\0\0\x42\x1\x1\xff\xff\xab\xab\xbc\xbc\xeb\xeb\0\0)
progressPosition=@Rect(148 135 202 8)
messagePosition=@Rect(148 130 200 0)
messageColor=@Variant(\0\0\0\x43\x1\xff\xff\0\0\0\0\0\0\0\0)\endverbatim

  Styl obsahuje niektore hodnoty ktore binarne hodnoty. Tieto hodnoty nie je
  trivialny zapisat rucne. Jednoduchsie riesenie je si subor so stylom
  vygenerovat. Kod generujuci styl by mohol vyzerat takto:

\code
QSettings settings("style.ini", QSettings::IniFormat);
settings.setValue("background",          QString("splash.png"));
settings.setValue("progressBarBorder",   QPen(Qt::blue));
settings.setValue("progressBarBg",       QBrush(Qt::white));
settings.setValue("progressBarActiveBg", QBrush(Qt::green));
settings.setValue("progressPosition",    QRect(10, 30, 210, 8));
settings.setValue("messagePosition",     QRect(10, 25, 210, 0));
settings.setValue("messageColor",        QColor(50, 50, 80));
\endcode
*/

/*!
  Vytvori startovaciu obrazovku.
*/

SNSplash::SNSplash(const QString &skinPath, QWidget *parent):
		QWidget(parent), m_steps(100), m_progress(0), m_infoMode(false), m_closeTimeout(1000),
		m_autoHide(false), m_displayed(false), m_timerId(0)
{
	setFixedSize(220, 50);
	m_skinPath = QString(skinPath);
	setAttribute(Qt::WA_AlwaysShowToolTips);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	if (skinPath.isNull())
	{
		loadDefaultSkin();
		return;
	}

	int ret = loadSkin();
	// nepodarilo sa nacitat skin
	if (ret)
	{
		qWarning("Using default skin");
		loadDefaultSkin();
	}
}

/*!
  Zrusi startovaciu obrazovku.
*/
SNSplash::~SNSplash()
{
}

/*!
  \reimp
 */
void SNSplash::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);
	QPainter painter(this);
	foreach(QRect rect, e->region().rects())
	{
		painter.drawPixmap(rect, m_background, rect);
	}
	drawProgress(painter);
	drawMessage(painter);
	painter.end();
}

/*!
  Vrati aktualnu hodnotu progress baru.

  Hodnota progress baru sa pohybuje medzi 0 a poctom krokov.

  \sa setProgress() steps()
*/
int SNSplash::progress() const
{
	return m_progress;
}

/*!
  Nastavenie hodnoty progress baru.

  \sa steps() progress() setAutoHide()
*/
void SNSplash::setProgress(int progress)
{
	if (progress >= 0 && progress <= m_steps)
	{
		m_progress = progress;
	}
	update();
	updateAutoHideTimer();
}

/*!
  Vrati pocet krokov progress baru.

  \sa setSteps()
*/
int SNSplash::steps() const
{
	return m_steps;
}

/*!
  Nastavenie poctu krokov progress baru.

  Progress bar ma standardne 100 krokov. Tento pocet je mozne zmenit volanim
  tejto metody.

  \sa steps() setAutoHide()
*/
void SNSplash::setSteps(int steps)
{
	if (steps > 0)
	{
		m_steps = steps;
		if (m_progress > m_steps)
		{
			m_progress = m_steps;
		}
	}
	update();
	updateAutoHideTimer();
}

/*!
  Vrati aktualne zobrazenu spravu.

  \sa setMessage()
*/
QString SNSplash::message() const
{
	return m_message;
}

/*!
  Nastavi text spravy.

  Po nastaveni textu spravy sa tato sprava zobrazi na startovacej obrazovke.
  Ak sa text nezmesti na startovaciu obrazovku bude automaticky skrateny
  a plna verzia sa zobrazi v tool tipe.

  \sa message() setAutoHide()
*/
void SNSplash::setMessage(const QString &message)
{
	m_message = message;
	update();
	updateAutoHideTimer();
}

/*!
  Oneskorene zatvorenie startovacej obrazovky.

  \param[in] delay Oneskorenie v milisekundach.

  Ak je oneskorenie kladne startovacia obrazovka bude v danom case uzatvorena.
  Pri zapornom oneskoreni sa zatvori po standardnej dobe zatvorenia.

  \sa setCloseTimeout()
*/
void SNSplash::closeDelayed(int delay)
{
	if (delay < 0)
	{
		delay = m_closeTimeout;
	}
	if (m_timerId != 0)
	{
		killTimer(m_timerId);
	}
	m_timerId = startTimer(delay);
}

/*!
  Vrati informaciu o tom, ci je startovacia obrazovka v info rezime.

  Startovacia obrazovka ktora ja v info rezime sa \b neda zatvorit jednoduchym
  kliknutim na jaj plochu.

  \sa setInfoMode()
*/
bool SNSplash::infoMode() const
{
	return m_infoMode;
}

/*!
  Nastavenie info rezimu startovacej obrazovky

  \sa infoMode()
*/
void SNSplash::setInfoMode(bool newInfoMode)
{
	m_infoMode = newInfoMode;
}

/*!
  Vrati standardny cas zatvorenia startovacej obrazovky v milisekundach.

  \sa setCloseTimeout()
*/
int SNSplash::closeTimeout() const
{
	return m_closeTimeout;
}


/*!
  Nastavenie starndardneho casu zatvorenia startovacej obrazovky.

  Tento cas je pouzity pri automatickom zatvarani alebo zatvarani so
  standardnym oneskorenim.

  \sa closeTimeout() closeDelayed() setAutoHide()
*/
void SNSplash::setCloseTimeout(int newCloseTimeout)
{
	m_closeTimeout = newCloseTimeout;
}

/*!
  Vracia nastavenie automatickeho skryvania.
*/
bool SNSplash::autoHide() const
{
	return m_autoHide;
}

/*!
  Nastavenie automatickeho skryvania startovacej obrazovky.

  Po zapnuti automatickeho skryvania sa startovacia obrazovka automaticky skryje
  po closeTimeout(). K zobrazeniu alebo vynulovaniu casovaca dojde pri pouziti
  metod setProgress(), setSteps(), setDisplayed() a setMessage().

  \sa setCloseTimeout()
*/
void SNSplash::setAutoHide(bool autoHide)
{
	m_autoHide = autoHide;
	if (autoHide && m_displayed && m_closeTimeout > 0)
	{
		setCloseTimer(m_closeTimeout);
	}
}

/*!
  Vracia to, ci je startovacia obrazovka zobrazena (alebo sa dokaze zobrazit
  automaticky).

  Tato funkcia vracia \b true ak bola startovacia obrazovka zobrazena a uzivatel
  ju neuzavrel (teda neklikol na jej plochu). Ak uzivatel uzavrel startovaciu
  obrazovku nemala by sa ani napriek nastavenemu automatickemu skryvaniu znovu
  zobrazovat.

  \sa setDisplayed() setAutoClsoe()
*/
bool SNSplash::displayed() const
{
	return m_displayed;
}

/*!
  Nastavenie hodnoty displayed.

  \attention Tato funkcia by sa mala pouzivat namiesto show().

  \sa displayed()
*/
void SNSplash::setDisplayed(bool displayed)
{
	m_displayed = displayed;
	if (displayed)
	{
		if (m_autoHide && m_closeTimeout > 0)
		{
			setCloseTimer(m_closeTimeout);
		}
		show();
	}
	else
	{
		close();
	}
}

/*!
  Vrati cestu k adresaru so skinom.

  \sa setSkinPath()
*/
QString SNSplash::skinPath() const
{
	return m_skinPath;
}

/*!
  Nastavi cestu k skinu a nacita tento skin. Cestou moze byt aj hodnota null,
  v tom pripade sa pouzuje standardny vstavany skin.
*/
void SNSplash::setSkinPath(const QString &filename)
{
	m_skinPath = filename;
	if (filename.isNull())
	{
		loadDefaultSkin();
		return;
	}

	int ret = loadSkin();
	if (ret)
	{
		qWarning("Using default skin");
		loadDefaultSkin();
	}
	update();
}

/*!
  Presun okna do stredu obrazovky.
*/
void SNSplash::moveToCenter()
{
	QRect geometry = QApplication::desktop()->screenGeometry();
	move(geometry.center() - QPoint(size().width(), size().height()) / 2);
}

void SNSplash::drawProgress(QPainter &painter)
{
	painter.setPen(m_progressBarBorder);
	painter.setBrush(m_progressBarBg);
	painter.drawRect(m_progressPosition);
	int width = (m_progressPosition.width() - 1) * m_progress / m_steps;
	QRect barRect(m_progressPosition.topLeft() + QPoint(1, 1), QSize(width, m_progressPosition.height() - 1));
	painter.setPen(Qt::NoPen);
	painter.setBrush(m_progressBarActiveBg);
	painter.drawRect(barRect);
}

void SNSplash::drawMessage(QPainter &painter)
{
	QFontMetrics m(font());
	int width = m.width(m_message);
	painter.setPen(m_messageColor);
	QString message;
	if (width > m_messagePosition.width())
	{
		message = m.elidedText(m_message, Qt::ElideRight, m_messagePosition.width());
		setToolTip(m_message);
	}
	else
	{
		message = m_message;
		setToolTip(QString());
	}
	painter.drawText(m_messagePosition, Qt::AlignRight, message);
}

/*!
  \reimp
*/
void SNSplash::mousePressEvent(QMouseEvent *e)
{
	Q_UNUSED(e);
	if (!m_infoMode)
	{
		m_displayed = false;
		close();
	}
}

/*!
  \reimp
 */
void SNSplash::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
}

/*!
  \reimp
 */
void SNSplash::closeEvent(QCloseEvent *e)
{
	QWidget::closeEvent(e);
	stopTimer();
}

/*!
  \reimp
 */
void SNSplash::timerEvent(QTimerEvent *e)
{
	close();
	killTimer(e->timerId());
	m_timerId = 0;
}

void SNSplash::stopTimer()
{
	if (m_timerId != 0)
	{
		killTimer(m_timerId);
		m_timerId = 0;
	}
}

void SNSplash::setCloseTimer(int delay)
{
	if (m_timerId != 0)
	{
		killTimer(m_timerId);
	}
	m_timerId = startTimer(delay);
}

void SNSplash::updateAutoHideTimer()
{
	if (m_autoHide && m_displayed)
	{
		if (!isVisible())
		{
			show();
		}
		if (m_closeTimeout > 0)
		{
			setCloseTimer(m_closeTimeout);
		}
	}
}

int SNSplash::loadSkin()
{
	if (m_skinPath.isNull())
	{
		return -4;
	}
	QDir dir(m_skinPath);
	if (!dir.exists())
	{
		qWarning("Directory %s not found", dir.absolutePath().toAscii().constData());
		return -1;
	}

	QString settingsFileName = dir.filePath("style.ini");
	if (!QFile::exists(settingsFileName))
	{
		qWarning("File %s not found", settingsFileName.toAscii().constData());
		return -2;
	}

	QSettings styleInfo(settingsFileName, QSettings::IniFormat);
	QVariant background = styleInfo.value("background");
	QVariant progressBarBorder = styleInfo.value("progressBarBorder");
	QVariant progressBarBg = styleInfo.value("progressBarBg");
	QVariant progressBarActiveBg = styleInfo.value("progressBarActiveBg");
	QVariant progressPosition = styleInfo.value("progressPosition");
	QVariant messagePosition = styleInfo.value("messagePosition");
	QVariant messageColor = styleInfo.value("messageColor");

	if (background.isNull())
	{
		qWarning("Variable '%s' not set", "background");
		return -3;
	}
	if (progressBarBorder.isNull())
	{
		qWarning("Variable '%s' not set", "progressBarBorder");
		return -3;
	}
	if (progressBarBg.isNull())
	{
		qWarning("Variable '%s' not set", "progressBarBg");
		return -3;
	}
	if (progressBarActiveBg.isNull())
	{
		qWarning("Variable '%s' not set", "progressBarActiveBg");
		return -3;
	}
	if (progressPosition.isNull())
	{
		qWarning("Variable '%s' not set", "progressPosition");
		return -3;
	}
	if (messagePosition.isNull())
	{
		qWarning("Variable '%s' not set", "messagePosition");
		return -3;
	}
	if (messageColor.isNull())
	{
		qWarning("Variable '%s' not set", "messageColor");
		return -3;
	}

	QString backgroundFilePath = dir.filePath(background.toString());
	m_background.load(backgroundFilePath);
	if (!m_background.isNull())
	{
		setFixedSize(m_background.size());
		resize(m_background.size());
	}

	m_progressBarBorder = progressBarBorder.value<QPen>();
	m_progressBarBg = progressBarBg.value<QBrush>();
	m_progressBarActiveBg = progressBarActiveBg.value<QBrush>();
	m_progressPosition = progressPosition.toRect();
	m_messagePosition = messagePosition.toRect();
	m_messageColor = messageColor.value<QColor>();

	QFontMetrics m(font());
	int height = m.height();
	m_messagePosition.setY(m_messagePosition.y() - height);
	m_messagePosition.setHeight(height);
	return 0;
}

void SNSplash::loadDefaultSkin()
{
	setFixedSize(220, 50);
	resize(220, 50);
	m_background = QPixmap(size());
	m_background.fill(QColor(100, 100, 100, 0));
	QLinearGradient gradient(QPointF(0, 0), QPointF(0, 50));
	/*gradient.setColorAt(0.0, QColor(240, 245, 255, 200));
	gradient.setColorAt(0.50, QColor(170, 180, 255, 150));
	gradient.setColorAt(0.51, QColor(100, 130, 200, 150));
	gradient.setColorAt(1.0, QColor(150, 170, 240, 150));*/
	gradient.setColorAt(0.0, QColor(240, 240, 240, 200));
	gradient.setColorAt(1.0, QColor(200, 200, 200, 100));
	QBrush bgBrush(gradient);
	QPainter painter(&m_background);
	painter.setBrush(bgBrush);
	painter.drawRect(0, 0, width() - 1, height() - 1);
	painter.end();
	m_progressBarBorder = QPen(QColor(12, 11, 73));
	m_progressBarBorder.setWidth(1);
	m_progressBarBg = Qt::NoBrush;
	m_progressBarActiveBg = QBrush(QColor(171, 188, 235));
	m_progressBarActiveBg.setStyle(Qt::SolidPattern);
	m_progressPosition = QRect(4, 30, 212, 8);
	m_messageColor = QColor(Qt::black);

	QFontMetrics m(font());
	int height = m.height();
	m_messagePosition = QRect(10, 25 - height, 200, height);

}

