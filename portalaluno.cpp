#include "portalaluno.h"
#include "ui_portalaluno.h"
#include <QMessageBox>

void alertaCamposEmBranco(PortalAluno *instancia) {
    QMessageBox::critical(instancia, "Campos obrigatórios em branco!", "Por favor, preencha todos os campos.");
}

void selecionarAba(Ui::PortalAluno *ui, int aba) {
    ui->tabWidget->setCurrentIndex(aba);
}

void limparCampos(Ui::PortalAluno *ui) {
    ui->txt_aluno->clear();
    ui->txt_curso->clear();
    ui->txt_disciplina->clear();
    ui->txt_professor->clear();
    ui->sb_n1->setValue(0);
    ui->sb_n2->setValue(0);
    ui->sb_n3->setValue(0);
}

float calculaMediaPonderada(float n1, float n2, float n3) {
    return (n1*5 + n2*3 + n3*2) / (5+3+2);
}

int quant_disciplinas = 0;
int quant_alunos = 0;
struct Disciplina {
    QString nome;
    QString professor;
    float notas[3];
    float AR;
    float mediaFinal;
    QString situacao;
};

struct Curso {
    QString nome;
    QString grau;
    QString periodo;
    struct Disciplina disciplina;
};

struct Aluno {
    QString nome;
    int RA;
    int idade;
    QString sexo;
    struct Curso curso;
};

struct Faculdade {
    QString nome;
    struct Aluno aluno;
};

struct Faculdade faculdade;

PortalAluno::PortalAluno(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PortalAluno)
{
    ui->setupUi(this);
    selecionarAba(ui, 0);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);
}

PortalAluno::~PortalAluno()
{
    delete ui;
}

void PortalAluno::on_pushButton_clicked()
{
    if (ui->txtFaculdade->text() == "") {
        alertaCamposEmBranco(this);
    } else {
        faculdade.nome = ui->txtFaculdade->text();
        ui->lbl_faculdade->setText(faculdade.nome);
        ui->tabWidget->setTabEnabled(1, true);
        selecionarAba(ui, 1);
    }
}

void PortalAluno::on_btn_avancarAbaAlunos_clicked()
{
    if (ui->txt_aluno->text() == "" || ui->txt_curso->text() == "") {
        alertaCamposEmBranco(this);
    } else {
        faculdade.aluno.nome = ui->txt_aluno->text();
        faculdade.aluno.RA = ui->sb_ra->value();
        faculdade.aluno.sexo = ui->combo_sexo->currentText();
        faculdade.aluno.curso.nome = ui->txt_curso->text();
        ui->tabWidget->setTabEnabled(2, true);
        selecionarAba(ui, 2);
    }
}

void PortalAluno::on_btn_avancarAbaDisciplina_clicked()
{
    if (ui->txt_disciplina->text() == "" || ui->txt_professor->text() == "") {
        alertaCamposEmBranco(this);
    } else {
        faculdade.aluno.curso.disciplina.nome = ui->txt_disciplina->text();
        faculdade.aluno.curso.disciplina.professor = ui->txt_professor->text();
        faculdade.aluno.curso.disciplina.notas[0] = ui->sb_n1->value();
        faculdade.aluno.curso.disciplina.notas[1] = ui->sb_n2->value();
        faculdade.aluno.curso.disciplina.notas[2] = ui->sb_n3->value();

        float nota1 = ui->sb_n1->value();
        float nota2 = ui->sb_n2->value();
        float nota3 = ui->sb_n3->value();

        faculdade.aluno.curso.disciplina.mediaFinal = calculaMediaPonderada(nota1, nota2, nota3);

        ui->tabWidget->setTabEnabled(3, true);
        selecionarAba(ui, 3);

        const int COL_ALUNO = 0;
        const int COL_CURSO = 1;
        const int COL_DISCIPLINA = 2;
        const int COL_N1= 3;
        const int COL_N2= 4;
        const int COL_N3= 5;
        const int COL_MEDIA= 6;

        ui->tableWidget->insertRow(0);
        QTableWidgetItem *aluno =       new QTableWidgetItem(faculdade.aluno.nome);
        QTableWidgetItem *curso =       new QTableWidgetItem(faculdade.aluno.curso.nome);
        QTableWidgetItem *disciplina =  new QTableWidgetItem(faculdade.aluno.curso.disciplina.nome);
        QTableWidgetItem *n1 =          new QTableWidgetItem(QString::number(faculdade.aluno.curso.disciplina.notas[0]));
        QTableWidgetItem *n2 =          new QTableWidgetItem(QString::number(faculdade.aluno.curso.disciplina.notas[1]));
        QTableWidgetItem *n3 =          new QTableWidgetItem(QString::number(faculdade.aluno.curso.disciplina.notas[2]));
        QTableWidgetItem *media =       new QTableWidgetItem("7");

        ui->tableWidget->setItem(0, COL_ALUNO, aluno);
        ui->tableWidget->setItem(0, COL_CURSO, curso);
        ui->tableWidget->setItem(0, COL_DISCIPLINA, disciplina);
        ui->tableWidget->setItem(0, COL_N1, n1);
        ui->tableWidget->setItem(0, COL_N2, n2);
        ui->tableWidget->setItem(0, COL_N3, n3);
        ui->tableWidget->setItem(0, COL_MEDIA, media);
    }
}

void PortalAluno::on_btn_home_clicked()
{
    selecionarAba(ui, 1);
    limparCampos(ui);
    ui->txt_aluno->setFocus();
}

void PortalAluno::on_btn_gerarPdf_clicked()
{
    QString msg = "Essa funcionalidade ainda não foi implementada, \naguarde as próximas versões do software!";
    QMessageBox::information(this, "Versão alfa", msg);
}

void PortalAluno::on_btnAbout_clicked()
{

}
