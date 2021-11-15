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
    void on_btn_gerarPdf_clicked();
    void on_btnAbout_clicked();
    void on_btn_faculdade_clicked();
    void on_btn_voltar_clicked();
    void on_btn_aluno_clicked();
    void on_btn_curso_clicked();
    void on_btn_salvaCurso_clicked();
    void on_btn_disciplinas_clicked();
    void on_btn_salvarDisciplina_clicked();
    void on_combo_cursos_currentIndexChanged(int index);
    void on_btn_salvaFaculdade_clicked();
    void on_btn_salvaAluno_clicked();
    void on_btn_notas_clicked();
    void on_combo_alunos_currentIndexChanged(int index);
    void on_btn_salvaNotas_clicked();

    void on_btn_lancarMaisNotas_clicked();

private:
    Ui::PortalAluno *ui;
};
#endif // PORTALALUNO_H
