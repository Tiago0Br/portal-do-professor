#ifndef PORTALALUNO_H
#define PORTALALUNO_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class PortalAluno; }
QT_END_NAMESPACE

class PortalAluno : public QMainWindow
{
    Q_OBJECT

public:
    PortalAluno(QWidget *parent = nullptr);
    ~PortalAluno();

private slots:
    void on_btn_avancarAbaFaculdade_clicked();

    void on_btn_avancarAbaAlunos_clicked();

    void on_btn_avancarAbaDisciplina_clicked();

    void on_btn_home_clicked();

    void on_btn_gerarPdf_clicked();

    void on_btnAbout_clicked();

private:
    Ui::PortalAluno *ui;
};
#endif // PORTALALUNO_H
