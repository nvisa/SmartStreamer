#include "algorithmcontrolwidget.h"
#include "ui_algorithmcontrolwidget.h"
#include "libsmartelement.h"
#include "proto/v2/AlgorithmCommunicationV2.pb.h"

#include <lmm/debug.h>
#include <lmm/qtvideooutput.h>

#include <QMenu>
#include <QRectF>
#include <QDebug>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>

static void myPaintHook(void *, void *priv, const RawBuffer &buf)
{
	auto cw = (AlgorithmControlWidget *)priv;
	cw->paintHook(buf);
}

AlgorithmControlWidget::AlgorithmControlWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AlgorithmControlWidget)
{
	ui->setupUi(this);
	QTimer::singleShot(3000, this, SLOT(updateSettings()));
	ui->frameVideo->setLayout(new QVBoxLayout());
	ui->frameVideo->layout()->setContentsMargins(0, 0, 0, 0);
}

AlgorithmControlWidget::~AlgorithmControlWidget()
{
	delete ui;
}

QWidget *AlgorithmControlWidget::getVideoParent()
{
	return ui->frameVideo;
}

void AlgorithmControlWidget::useVideoWidget(VideoWidget *w)
{
	videoWidget = w;
	videoWidget->setPaintHook(&myPaintHook, this);
	w->installEventFilter(this);
}

AlgorithmControlWidget *AlgorithmControlWidget::instance()
{
	static AlgorithmControlWidget *inst = nullptr;
	if (!inst)
		inst = new AlgorithmControlWidget();
	return inst;
}

void AlgorithmControlWidget::paintHook(const RawBuffer &buf)
{
	/* results and regions are already painted, we just create mouse region mapping here */
	if (buf.constPars()->metaDataRaw == nullptr)
		return;
	lastBuffer = buf;
}

bool AlgorithmControlWidget::eventFilter(QObject *obj, QEvent *ev)
{
	QWidget *widget = qobject_cast<QWidget *>(obj);
	if (!widget)
		return false;
	if (ev->type() != QEvent::MouseButtonPress)
		return false;
	QMouseEvent *mev = (QMouseEvent *)ev;

	auto res = std::static_pointer_cast<algorithm::v2::Alarm>(lastBuffer.constPars()->metaDataRaw);
	if (!res) {
		mDebug("Last buffer doesn't contain algorithm metadata");
		return false;
	}

	QPointF ptf;
	ptf.setX(mev->pos().x() / (float)ui->frameVideo->width());
	ptf.setY(mev->pos().y() / (float)ui->frameVideo->height());

	for (int i = 0; i < res->detected_object_size(); i++) {
		auto alarm = res->detected_object(i);
		auto bb = alarm.bounding_box();
		float x = bb.top_left().x();
		float y = bb.top_left().y();
		float w = bb.bottom_right().x() - x;
		float h = bb.bottom_right().y() - y;
		QRectF r(x, y, w, h);
		bool inrect = r.contains(ptf);
		//qDebug() << ptf << x << y << w << h << inrect;
		if (inrect) {
			int action = 0; //0: manuel track, 1: auto track
			if (mev->button() == Qt::RightButton) {
				QMenu menu;
				menu.addAction("Start manuel track");
				menu.addAction("Start auto track");
				QAction *a = menu.exec(widget->mapToGlobal(mev->pos()));
				if (!a)
					action = -1;
				else if (a->text() == "Start manuel track")
					action = 0;
				else if (a->text() == "Start auto track")
					action = 1;

			}

			if (action == -1)
				return false;

			auto el = LibSmartElement::instance();
			el->setTrackRegion(r);
			if (action == 0)
				el->setRunMode(algorithm::v2::AlgorithmParameters_RunMode_MANUEL_TRACK);
			else if (action == 1)
				el->setRunMode(algorithm::v2::AlgorithmParameters_RunMode_AUTO_TRACK);
		}
	}

	return true;
}

void AlgorithmControlWidget::updateSettings()
{
	auto el = LibSmartElement::instance();
	ui->checkVideoStab->setChecked(el->isVideoStabilizationEnabled());
	ui->checkTamper->setChecked(el->isTamperDetectionEnabled());
	ui->checkMasking->setChecked(el->isPrivacyMaskingEnabled());
	ui->comboRunMode->setCurrentIndex((int)el->runMode());

	QTimer::singleShot(3000, this, SLOT(updateSettings()));
}

void AlgorithmControlWidget::on_checkVideoStab_clicked()
{
	qDebug() << "video stab" << ui->checkVideoStab->isChecked();
	auto el = LibSmartElement::instance();
	el->setVideoStabilizationEnabled(ui->checkVideoStab->isChecked());
}

void AlgorithmControlWidget::on_checkTamper_clicked()
{
	qDebug() << "tamper" << ui->checkTamper->isChecked();
}

void AlgorithmControlWidget::on_checkMasking_clicked()
{
	qDebug() << "masking" << ui->checkMasking->isChecked();
}

void AlgorithmControlWidget::on_comboRunMode_activated(int index)
{
	qDebug() << "run mode" << index;
	auto el = LibSmartElement::instance();
	el->setRunMode(algorithm::v2::AlgorithmParameters_RunMode(index));
}

void AlgorithmControlWidget::on_radioPre_1_clicked()
{
	auto el = LibSmartElement::instance();
	el->setPreprocessingType(algorithm::v2::AlgorithmParameters_PreProcessing_NONE, ui->horizontalScrollBarPre->value() / 100.0);
}

void AlgorithmControlWidget::on_radioPre_2_clicked()
{
	auto el = LibSmartElement::instance();
	el->setPreprocessingType(algorithm::v2::AlgorithmParameters_PreProcessing_CONTRAST_ENHANCEMENT, ui->horizontalScrollBarPre->value() / 100.0);
}

void AlgorithmControlWidget::on_radioPre_3_clicked()
{
	auto el = LibSmartElement::instance();
	el->setPreprocessingType(algorithm::v2::AlgorithmParameters_PreProcessing_DEHAZING, ui->horizontalScrollBarPre->value() / 100.0);
}

void AlgorithmControlWidget::on_horizontalScrollBarPre_sliderMoved(int position)
{
	auto el = LibSmartElement::instance();
	algorithm::v2::AlgorithmParameters_PreProcessing pre = algorithm::v2::AlgorithmParameters_PreProcessing_NONE;
	if (ui->radioPre_1->isChecked())
		pre = algorithm::v2::AlgorithmParameters_PreProcessing_NONE;
	else if (ui->radioPre_2->isChecked())
		pre = algorithm::v2::AlgorithmParameters_PreProcessing_CONTRAST_ENHANCEMENT;
	else if (ui->radioPre_3->isChecked())
		pre = algorithm::v2::AlgorithmParameters_PreProcessing_DEHAZING;
	el->setPreprocessingType(pre, position / 100.0);
}
