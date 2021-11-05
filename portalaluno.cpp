#include "portalaluno.h"
#include "ui_portalaluno.h"
#include "sobre.h"
#include <QMessageBox>
#include <math.h>
#include <QPrinter>
#include <QPainter>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>

// Declaração das variáveis
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
    QVector<Aluno> alunos;
};

struct Faculdade faculdade;
struct Aluno aluno;

// Declaração dos métodos utilizados
void alertaCamposEmBranco(PortalAluno *instancia);
void selecionarAba(Ui::PortalAluno *ui, int aba);
void limparCampos(Ui::PortalAluno *ui);
float calculaMediaPonderada(float n1, float n2, float n3);
void adicionarDadosNaTabela(Ui::PortalAluno *ui, Faculdade faculdade);
void gerarPDF(PortalAluno *instancia, Faculdade faculdade);

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

void PortalAluno::on_btnAbout_clicked()
{
    Sobre modalSobre;
    modalSobre.exec();
}

void PortalAluno::on_btn_avancarAbaFaculdade_clicked()
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
        aluno.nome = ui->txt_aluno->text();
        aluno.RA = ui->sb_ra->value();
        aluno.sexo = ui->combo_sexo->currentText();
        aluno.curso.nome = ui->txt_curso->text();
        ui->tabWidget->setTabEnabled(2, true);
        selecionarAba(ui, 2);
    }
}

void PortalAluno::on_btn_avancarAbaDisciplina_clicked()
{
    if (ui->txt_disciplina->text() == "" || ui->txt_professor->text() == "") {
        alertaCamposEmBranco(this);
    } else {
        aluno.curso.disciplina.nome = ui->txt_disciplina->text();
        aluno.curso.disciplina.professor = ui->txt_professor->text();
        aluno.curso.disciplina.notas[0] = ui->sb_n1->value();
        aluno.curso.disciplina.notas[1] = ui->sb_n2->value();
        aluno.curso.disciplina.notas[2] = ui->sb_n3->value();

        float nota1 = ui->sb_n1->value();
        float nota2 = ui->sb_n2->value();
        float nota3 = ui->sb_n3->value();

        aluno.curso.disciplina.mediaFinal = calculaMediaPonderada(nota1, nota2, nota3);
        aluno.curso.disciplina.situacao = aluno.curso.disciplina.mediaFinal >= 6 ? "Aprovado" : "Reprovado";
        faculdade.alunos.push_back(aluno);

        ui->tabWidget->setTabEnabled(3, true);
        selecionarAba(ui, 3);

        adicionarDadosNaTabela(ui, faculdade);
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
    gerarPDF(this, faculdade);
}

// Implementação dos métodos auxiliadores
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
    ui->sb_n1->setValue(5);
    ui->sb_n2->setValue(5);
    ui->sb_n3->setValue(5);
}

float calculaMediaPonderada(float n1, float n2, float n3) {
    float media = (n1*5 + n2*3 + n3*2) / (5+3+2);
    return round(media);
}

void adicionarDadosNaTabela(Ui::PortalAluno *ui, Faculdade faculdade) {
    const int COL_ALUNO = 0;
    const int COL_CURSO = 1;
    const int COL_DISCIPLINA = 2;
    const int COL_N1 = 3;
    const int COL_N2 = 4;
    const int COL_N3 = 5;
    const int COL_MEDIA= 6;
    const int COL_SITUACAO= 7;

    QTableWidgetItem *aluno;
    QTableWidgetItem *curso;
    QTableWidgetItem *disciplina;
    QTableWidgetItem *n1;
    QTableWidgetItem *n2;
    QTableWidgetItem *n3;
    QTableWidgetItem *media;
    QTableWidgetItem *situacao;

    int i = faculdade.alunos.length()-1;

    ui->tableWidget->insertRow(i);
    aluno =       new QTableWidgetItem(faculdade.alunos.at(i).nome);
    curso =       new QTableWidgetItem(faculdade.alunos.at(i).curso.nome);
    disciplina =  new QTableWidgetItem(faculdade.alunos.at(i).curso.disciplina.nome);
    n1 =          new QTableWidgetItem(QString::number(faculdade.alunos.at(i).curso.disciplina.notas[0]));
    n2 =          new QTableWidgetItem(QString::number(faculdade.alunos.at(i).curso.disciplina.notas[1]));
    n3 =          new QTableWidgetItem(QString::number(faculdade.alunos.at(i).curso.disciplina.notas[2]));
    media =       new QTableWidgetItem(QString::number(faculdade.alunos.at(i).curso.disciplina.mediaFinal));
    situacao =    new QTableWidgetItem(faculdade.alunos.at(i).curso.disciplina.situacao);

    ui->tableWidget->setItem(i, COL_ALUNO, aluno);
    ui->tableWidget->setItem(i, COL_CURSO, curso);
    ui->tableWidget->setItem(i, COL_DISCIPLINA, disciplina);
    ui->tableWidget->setItem(i, COL_N1, n1);
    ui->tableWidget->setItem(i, COL_N2, n2);
    ui->tableWidget->setItem(i, COL_N3, n3);
    ui->tableWidget->setItem(i, COL_MEDIA, media);
    ui->tableWidget->setItem(i, COL_SITUACAO, situacao);
}

void gerarPDF(PortalAluno *instancia, Faculdade faculdade) {
    QString nomeArquivo = QDir::currentPath() + "/relatorio_alunos.pdf";
    QPrinter printer;
    QPainter painter;

    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(nomeArquivo);

    if (!painter.begin(&printer)) {
        QMessageBox::critical(instancia, "Arquivo não gerado", "Desculpe, não foi possível gerar o PDF!");
    } else {
        const int COL_ALUNO = 25;
        const int COL_CURSO = 175;
        const int COL_DISCIPLINA = 315;
        const int COL_N1 = 425;
        const int COL_N2 = 475;
        const int COL_N3 = 525;
        const int COL_MEDIA = 575;
        const int COL_SITUACAO = 610;

        int linha = 80;
        painter.drawText(320, linha, faculdade.nome.toUpper());

        linha = 120;
        painter.drawText(COL_ALUNO, linha, "NOME");
        painter.drawText(COL_CURSO, linha, "CURSO");
        painter.drawText(COL_DISCIPLINA, linha, "DISCIPLINA");
        painter.drawText(COL_N1, linha, "NOTA 1");
        painter.drawText(COL_N2, linha, "NOTA 2");
        painter.drawText(COL_N3, linha, "NOTA 3");
        painter.drawText(COL_MEDIA, linha, "MÉDIA");
        painter.drawText(COL_SITUACAO, linha, "SITUAÇÃO");

        linha = 160;
        for (int i = 0; i < faculdade.alunos.length(); i++) {
            painter.drawText(COL_ALUNO, linha, faculdade.alunos.at(i).nome);
            painter.drawText(COL_CURSO, linha, faculdade.alunos.at(i).curso.nome);
            painter.drawText(COL_DISCIPLINA, linha, faculdade.alunos.at(i).curso.disciplina.nome);
            painter.drawText(COL_N1, linha, QString::number(faculdade.alunos.at(i).curso.disciplina.notas[0]));
            painter.drawText(COL_N2, linha, QString::number(faculdade.alunos.at(i).curso.disciplina.notas[1]));
            painter.drawText(COL_N3, linha, QString::number(faculdade.alunos.at(i).curso.disciplina.notas[2]));
            painter.drawText(COL_MEDIA, linha, QString::number(faculdade.alunos.at(i).curso.disciplina.mediaFinal));
            painter.drawText(COL_SITUACAO, linha, faculdade.alunos.at(i).curso.disciplina.situacao);

            linha+=20;
        }

        painter.end();

        QDesktopServices::openUrl(QUrl("file:///" + nomeArquivo));
    }
}
