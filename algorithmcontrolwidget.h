#ifndef ALGORITHMCONTROLWIDGET_H
#define ALGORITHMCONTROLWIDGET_H

#include <QWidget>

#include <lmm/rawbuffer.h>

class VideoWidget;

namespace Ui {
class AlgorithmControlWidget;
}

class AlgorithmControlWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AlgorithmControlWidget(QWidget *parent = 0);
	~AlgorithmControlWidget();

	QWidget * getVideoParent();
	void useVideoWidget(VideoWidget *w);

	static AlgorithmControlWidget * instance();

	void paintHook(const RawBuffer &buf);
protected:
	bool eventFilter(QObject *, QEvent *);
private slots:
	void updateSettings();
	void on_checkVideoStab_clicked();

	void on_checkTamper_clicked();

	void on_checkMasking_clicked();

	void on_comboRunMode_activated(int index);

	void on_radioPre_1_clicked();

	void on_radioPre_2_clicked();

	void on_radioPre_3_clicked();

	void on_horizontalScrollBarPre_sliderMoved(int position);

private:
	Ui::AlgorithmControlWidget *ui;
	VideoWidget *videoWidget;
	RawBuffer lastBuffer;
};

#endif // ALGORITHMCONTROLWIDGET_H
