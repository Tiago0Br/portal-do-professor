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
#include <QDate>
#define ABA_INICIO 0
#define ABA_FACULDADE 1
#define ABA_CURSO 2
#define ABA_DISCIPLINA 3
#define ABA_ALUNO 4
#define ABA_NOTAS 5
#define ABA_TABELA 6
#define ABA_LISTAGEM 7

// Declaração das variáveis e Structs
QDate *date = new QDate();

struct Disciplina {
    QString nome;
    QString professor;
    float notas[3];
    float AR;
    float mediaFinal;
    QString situacao;
    int pertenceAoCursoID; // Variável para vincular a disciplina a um curso
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
struct Curso curso;
struct Disciplina disciplina;

// Vetores utilizados para popular campos do tipo ComboBox
QVector<Curso> cursosCadastrados;
QVector<Disciplina> disciplinasCadastradas;
QVector<Aluno> alunosCadastrados;

// Declaração dos métodos auxiliadores utilizados
void setup(Ui::PortalAluno *ui);
void alertaCamposEmBranco(PortalAluno *instancia);
void selecionarAba(Ui::PortalAluno *ui, int aba);
float calculaMediaPonderada(float n1, float n2, float n3);
void adicionarDadosNaTabela(Ui::PortalAluno *ui, Faculdade faculdade);
void gerarPDF(PortalAluno *instancia, Faculdade faculdade);
void acessarTelaInicial(Ui::PortalAluno *ui);
void adicionarCursosNaTabela(Ui::PortalAluno *ui, QVector<Curso> cursos);
void adicionarDisciplinasNaTabela(Ui::PortalAluno *ui, QVector<Disciplina> disciplinas);
void adicionarAlunosNaTabela(Ui::PortalAluno *ui, QVector<Aluno> alunos);
void gerarPDF_alunosCadastrados(PortalAluno *instancia, QVector<Aluno> alunos);
void gerarPDF_cursosCadastrados(PortalAluno *instancia, QVector<Curso> cursos);
void gerarPDF_disciplinasCadastradas(PortalAluno *instancia, QVector<Disciplina> disciplinas);

PortalAluno::PortalAluno(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PortalAluno)
{
    ui->setupUi(this);
    setup(ui);
}

PortalAluno::~PortalAluno()
{
    delete ui;
}

// Exibe tela de "Sobre" com informações do programa e dos criadores
void PortalAluno::on_btnAbout_clicked()
{
    Sobre modalSobre;
    modalSobre.exec();
}

//  CADASTRO / EDIÇÃO DA FACULDADE
void PortalAluno::on_btn_faculdade_clicked()
{
    selecionarAba(ui, ABA_FACULDADE);
}

void PortalAluno::on_btn_salvaFaculdade_clicked()
{
    // Se o campo estiver em branco exibe um alerta de erro
    if (ui->txtFaculdade->text() == "") {
        alertaCamposEmBranco(this);
    } else {
        faculdade.nome = ui->txtFaculdade->text();
        QMessageBox::information(this, "Sucesso", "Faculdade Salva com sucesso!");
        ui->btn_faculdade->setText("Editar faculdade");
        acessarTelaInicial(ui);
    }
}

//  CADASTRO DE CURSOS
void PortalAluno::on_btn_curso_clicked()
{
    if(faculdade.nome == "") {
        QMessageBox::warning(this, "Faculdade não informada", "Por favor, cadastre a faculdade primeiro.");
    } else {
        selecionarAba(ui, ABA_CURSO);
    }
}

void PortalAluno::on_btn_salvaCurso_clicked()
{
    if(ui->txt_curso->text() == "") {
        alertaCamposEmBranco(this);
    } else {
        // Lê as informações do curso
        curso.nome = ui->txt_curso->text();
        curso.grau = ui->combo_grauCurso->currentText();
        curso.periodo = ui->combo_periodo->currentText();
        cursosCadastrados.push_back(curso);

        // Adicionando o curso cadastrado nos combos de Cursos
        ui->combo_cursos->addItem(curso.nome);
        ui->combo_cursos_2->addItem(curso.nome);

        int res = QMessageBox::information(this, "Cadastro efetuado", "Deseja adicionar mais cursos?", "Sim", "Não");
        // 0 para "Sim" e 1 para "Não"
        if(res == 1) {
            acessarTelaInicial(ui);
        }
        // Limpa o campo do nome do curso após o cadastro
        ui->txt_curso->clear();
    }
}

// CADASTRO DE DISCIPLINAS
void PortalAluno::on_btn_disciplinas_clicked()
{
    if(faculdade.nome == "" || cursosCadastrados.empty()) {
        QMessageBox::warning(this, "Informações faltando", "Deve ser cadastrada a faculdade e pelo menos um curso.");
    } else {
        selecionarAba(ui, ABA_DISCIPLINA);
    }
}

void PortalAluno::on_btn_salvarDisciplina_clicked()
{
    if (ui->txt_disciplina->text() == "" || ui->txt_professor->text() == "") {
        alertaCamposEmBranco(this);
    } else {
        disciplina.pertenceAoCursoID = ui->combo_cursos_2->currentIndex();
        disciplina.nome = ui->txt_disciplina->text();
        disciplina.professor = ui->txt_professor->text();
        disciplinasCadastradas.push_back(disciplina);

        int res = QMessageBox::information(this, "Cadastro efetuado", "Deseja adicionar mais disciplinas?", "Sim", "Não");
        // 0 para "Sim" e 1 para "Não"
        if(res == 1) {
            acessarTelaInicial(ui);
        }
        ui->txt_disciplina->clear();
        ui->txt_professor->clear();
    }
}

// CADASTRO DE ALUNOS
void PortalAluno::on_btn_aluno_clicked()
{
    if(faculdade.nome == "" || cursosCadastrados.empty()) {
        QMessageBox::warning(this, "Informações faltando", "Deve ser cadastrada a faculdade e pelo menos um curso.");
    } else {
        selecionarAba(ui, ABA_ALUNO);
    }
}

void PortalAluno::on_combo_cursos_currentIndexChanged(int index)
{
    // Alterando as informações exibidas de acordo com o curso selecionado
    if(index == -1) return;

    ui->lbl_nomeCurso->setText(cursosCadastrados.at(index).nome);
    ui->lbl_grauCurso->setText(cursosCadastrados.at(index).grau);
    ui->lbl_periodoCurso->setText(cursosCadastrados.at(index).periodo);
}

void PortalAluno::on_btn_salvaAluno_clicked()
{
    if(ui->txt_aluno->text() == "") {
        alertaCamposEmBranco(this);
    } else {
        QDate dataNascimento = ui->data_nascimento->date();
        aluno.nome = ui->txt_aluno->text();
        aluno.RA = ui->sb_ra->value();
        aluno.idade = date->currentDate().year() - dataNascimento.year(); // Subtrai o ano atual pelo ano de nascimento do aluno
        aluno.sexo = ui->combo_sexo->currentText();

        int cursoSelecionadoID = ui->combo_cursos->currentIndex();
        aluno.curso = cursosCadastrados.at(cursoSelecionadoID);
        alunosCadastrados.push_back(aluno);
        // Adicionando o aluno cadastrado nos combos de Cursos
        ui->combo_alunos->addItem(aluno.nome);

        int res = QMessageBox::information(this, "Cadastro efetuado", "Deseja cadastrar mais alunos?", "Sim", "Não");
        // 0 para "Sim" e 1 para "Não"
        if(res == 1) {
            acessarTelaInicial(ui);
        }
        ui->txt_aluno->clear();
    }
}

// LANÇAMENTO DE NOTAS DOS ALUNOS
void PortalAluno::on_btn_notas_clicked()
{
    if(alunosCadastrados.empty()) {
        QMessageBox::warning(this, "Informações faltando", "Deve ser cadastrado pelo menos um aluno.");
    } else {
        selecionarAba(ui, ABA_NOTAS);
    }
}

void PortalAluno::on_combo_alunos_currentIndexChanged(int index)
{
    // Populando o combo de disciplinas de acordo com o curso do aluno selecionado
    if(index == -1) return;
    ui->combo_disciplinas->clear();
    int indiceDoCurso = 0;
    ui->lbl_curso->setText(alunosCadastrados.at(index).curso.nome);
    for (int i = 0; i < cursosCadastrados.length(); i++) {
        if (alunosCadastrados.at(index).curso.nome == cursosCadastrados.at(i).nome) {
            indiceDoCurso = i;
        }
    }
    for (int i = 0; i < disciplinasCadastradas.length(); i++) {
        if(disciplinasCadastradas.at(i).pertenceAoCursoID == indiceDoCurso) {
            ui->combo_disciplinas->addItem(disciplinasCadastradas.at(i).nome);
        }
    }
}

// Exibindo o professor da disciplina que for escolhida
void PortalAluno::on_combo_disciplinas_currentIndexChanged(QString value)
{
    if (value == "") return;
    QString professor;
    for (int i = 0; i < disciplinasCadastradas.length(); i++) {
        if (disciplinasCadastradas.at(i).nome == value) {
            professor = disciplinasCadastradas.at(i).professor;
        }
    }
    ui->lbl_professor->setText(professor);
}

void PortalAluno::on_btn_salvaNotas_clicked()
{
    if(ui->combo_disciplinas->count() == 0) {
        QMessageBox::warning(this, "Erro", "O curso desse aluno não possui disciplinas cadastradas");
        return;
    }
    int alunoID = ui->combo_alunos->currentIndex();
    aluno.nome = alunosCadastrados.at(alunoID).nome;
    aluno.RA = alunosCadastrados.at(alunoID).RA;
    aluno.idade = alunosCadastrados.at(alunoID).idade;
    aluno.sexo = alunosCadastrados.at(alunoID).sexo;
    aluno.curso = alunosCadastrados.at(alunoID).curso;

    QString disciplinaSelecionada = ui->combo_disciplinas->currentText();
    for (int i = 0; i < disciplinasCadastradas.length(); i++) {
        if(disciplinaSelecionada == disciplinasCadastradas.at(i).nome) {
            aluno.curso.disciplina = disciplinasCadastradas.at(i);
        }
    }
    aluno.curso.disciplina.notas[0] = ui->sb_n1->value();
    aluno.curso.disciplina.notas[1] = ui->sb_n2->value();
    aluno.curso.disciplina.notas[2] = ui->sb_n3->value();

    float nota1 = ui->sb_n1->value();
    float nota2 = ui->sb_n2->value();
    float nota3 = ui->sb_n3->value();

    aluno.curso.disciplina.mediaFinal = calculaMediaPonderada(nota1, nota2, nota3);
    aluno.curso.disciplina.situacao = aluno.curso.disciplina.mediaFinal >= 6 ? "Aprovado" : "Reprovado";
    faculdade.alunos.push_back(aluno);

    adicionarDadosNaTabela(ui, faculdade);

    int res = QMessageBox::information(this, "Notas lançadas", "Deseja lançar mais notas?", "Sim", "Não");
    // 0 para "Sim" e 1 para "Não"
    if(res == 1) {
        selecionarAba(ui, ABA_TABELA);
    }
    // Resetando os campos de notas
    ui->sb_n1->setValue(5);
    ui->sb_n2->setValue(5);
    ui->sb_n3->setValue(5);
}

void PortalAluno::on_btn_lancarMaisNotas_clicked()
{
    selecionarAba(ui, ABA_NOTAS);
}

void PortalAluno::on_btn_gerarPdf_clicked()
{
    gerarPDF(this, faculdade);
}

// LISTAGEM DOS CADASTROS
void PortalAluno::on_btn_visualizarCadastros_clicked()
{
    selecionarAba(ui, ABA_LISTAGEM);
}

void PortalAluno::on_btn_pesquisar_clicked()
{
    int tipoCadastro = ui->combo_cadastros->currentIndex(); // 0 - Cursos, 1- Disciplinas, 2 - Alunos
    ui->tabelaCadastros->clear();
    ui->tabelaCadastros->setRowCount(0);
    switch (tipoCadastro) {
        case 0:
            adicionarCursosNaTabela(ui, cursosCadastrados);
            break;
        case 1:
            adicionarDisciplinasNaTabela(ui, disciplinasCadastradas);
            break;
        case 2:
            adicionarAlunosNaTabela(ui, alunosCadastrados);
            break;
    }
}
// Exportação em PDF
void PortalAluno::on_btn_gerarPdfCadastros_clicked()
{
    int tipoCadastro = ui->combo_cadastros->currentIndex(); // 0 - Cursos, 1- Disciplinas, 2 - Alunos
    switch (tipoCadastro) {
        case 0:
            gerarPDF_cursosCadastrados(this, cursosCadastrados);
            break;
        case 1:
        gerarPDF_disciplinasCadastradas(this, disciplinasCadastradas);
            break;
        case 2:
            gerarPDF_alunosCadastrados(this, alunosCadastrados);
            break;
    }
}

// IMPLEMENTAÇÃO DOS MÉTODOS AUXILIADORES
void setup(Ui::PortalAluno *ui) {
    selecionarAba(ui, ABA_INICIO);
    ui->tabWidget->setTabEnabled(ABA_FACULDADE, false);
    ui->tabWidget->setTabEnabled(ABA_ALUNO, false);
    ui->tabWidget->setTabEnabled(ABA_CURSO, false);
    ui->tabWidget->setTabEnabled(ABA_DISCIPLINA, false);
    ui->tabWidget->setTabEnabled(ABA_NOTAS, false);
    ui->tabWidget->setTabEnabled(ABA_TABELA, false);
    ui->tabWidget->setTabEnabled(ABA_LISTAGEM, false);
}

void alertaCamposEmBranco(PortalAluno *instancia) {
    QMessageBox::critical(instancia, "Campos obrigatórios em branco!", "Por favor, preencha todos os campos.");
}

void selecionarAba(Ui::PortalAluno *ui, int aba) {
    ui->tabWidget->setTabEnabled(aba, true);
    ui->tabWidget->setCurrentIndex(aba);
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

    // Configurando tamanho das colunas
    ui->tableWidget->setColumnWidth(COL_CURSO, 200);
    ui->tableWidget->setColumnWidth(COL_DISCIPLINA, 170);
    ui->tableWidget->setColumnWidth(COL_N1, 10);
    ui->tableWidget->setColumnWidth(COL_N2, 10);
    ui->tableWidget->setColumnWidth(COL_N3, 10);
    ui->tableWidget->setColumnWidth(COL_MEDIA, 10);

    // Configuração para não permitir edição dos dados pela tabela
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Adiciona uma nova linha no final e preenche com os novos dados
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
    QPrinter printer;
    QPainter painter;
    QString nomeArquivo = QDir::currentPath() + "/relatorio_notas.pdf";
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

void acessarTelaInicial(Ui::PortalAluno *ui) {
    ui->tabWidget->setCurrentIndex(ABA_INICIO);
}

void adicionarCursosNaTabela(Ui::PortalAluno *ui, QVector<Curso> cursos) {
    QStringList cabecalhoCursos = {"Nome", "Grau", "Periodo"};
    QTableWidgetItem *nome;
    QTableWidgetItem *grau;
    QTableWidgetItem *periodo;

    const int COL_NOME = 0;
    const int COL_GRAU = 1;
    const int COL_PERIODO = 2;

    ui->tabelaCadastros->setColumnCount(3);
    ui->tabelaCadastros->setHorizontalHeaderLabels(cabecalhoCursos);
    ui->tabelaCadastros->setColumnWidth(COL_NOME, 230);
    ui->tabelaCadastros->setColumnWidth(COL_GRAU, 230);
    ui->tabelaCadastros->setColumnWidth(COL_PERIODO, 230);

    for (int i = 0; i < cursos.length(); i++) {
        nome = new QTableWidgetItem(cursos.at(i).nome);
        grau = new QTableWidgetItem(cursos.at(i).grau);
        periodo = new QTableWidgetItem(cursos.at(i).periodo);

        ui->tabelaCadastros->insertRow(i);
        ui->tabelaCadastros->setItem(i, COL_NOME, nome);
        ui->tabelaCadastros->setItem(i, COL_GRAU, grau);
        ui->tabelaCadastros->setItem(i, COL_PERIODO, periodo);
    }
}

void adicionarDisciplinasNaTabela(Ui::PortalAluno *ui, QVector<Disciplina> disciplinas) {
    QStringList cabecalhoDisciplinas = {"Nome", "Professor", "Curso"};
    QTableWidgetItem *nome;
    QTableWidgetItem *professor;
    QTableWidgetItem *curso;
    QString cursoDisciplina;

    const int COL_NOME = 0;
    const int COL_PROF = 1;
    const int COL_CURSO = 2;

    ui->tabelaCadastros->setColumnCount(3);
    ui->tabelaCadastros->setHorizontalHeaderLabels(cabecalhoDisciplinas);
    ui->tabelaCadastros->setColumnWidth(COL_NOME, 230);
    ui->tabelaCadastros->setColumnWidth(COL_PROF, 230);
    ui->tabelaCadastros->setColumnWidth(COL_CURSO, 230);

    for (int i = 0; i < disciplinas.length(); i++) {
        nome = new QTableWidgetItem(disciplinas.at(i).nome);
        professor = new QTableWidgetItem(disciplinas.at(i).professor);
        cursoDisciplina = cursosCadastrados.at(disciplinas.at(i).pertenceAoCursoID).nome;
        curso = new QTableWidgetItem(cursoDisciplina);

        ui->tabelaCadastros->insertRow(i);
        ui->tabelaCadastros->setItem(i, COL_NOME, nome);
        ui->tabelaCadastros->setItem(i, COL_PROF, professor);
        ui->tabelaCadastros->setItem(i, COL_CURSO, curso);
    }
}

void adicionarAlunosNaTabela(Ui::PortalAluno *ui, QVector<Aluno> alunos) {
    QStringList cabecalhoAlunos = {"Nome", "RA", "Sexo", "Idade", "Curso"};
    QTableWidgetItem *nome;
    QTableWidgetItem *ra;
    QTableWidgetItem *sexo;
    QTableWidgetItem *idade;
    QTableWidgetItem *curso;

    const int COL_NOME = 0;
    const int COL_RA = 1;
    const int COL_SEXO = 2;
    const int COL_IDADE = 3;
    const int COL_CURSO = 4;

    ui->tabelaCadastros->setColumnCount(5);
    ui->tabelaCadastros->setHorizontalHeaderLabels(cabecalhoAlunos);
    ui->tabelaCadastros->setColumnWidth(COL_NOME, 230);
    ui->tabelaCadastros->setColumnWidth(COL_RA, 90);
    ui->tabelaCadastros->setColumnWidth(COL_SEXO, 90);
    ui->tabelaCadastros->setColumnWidth(COL_IDADE, 50);
    ui->tabelaCadastros->setColumnWidth(COL_CURSO, 230);

    for (int i = 0; i < alunos.length(); i++) {
        nome = new QTableWidgetItem(alunos.at(i).nome);
        ra = new QTableWidgetItem(QString::number(alunos.at(i).RA));
        sexo = new QTableWidgetItem(alunos.at(i).sexo);
        idade = new QTableWidgetItem(QString::number(alunos.at(i).idade));
        curso = new QTableWidgetItem(alunos.at(i).curso.nome);

        ui->tabelaCadastros->insertRow(i);
        ui->tabelaCadastros->setItem(i, COL_NOME, nome);
        ui->tabelaCadastros->setItem(i, COL_RA, ra);
        ui->tabelaCadastros->setItem(i, COL_SEXO, sexo);
        ui->tabelaCadastros->setItem(i, COL_IDADE, idade);
        ui->tabelaCadastros->setItem(i, COL_CURSO, curso);
    }
}

void gerarPDF_cursosCadastrados(PortalAluno *instancia, QVector<Curso> cursos) {
    if(cursos.empty()) {
        QMessageBox::warning(instancia, "Arquivo não gerado", "Não tem cursos cadastrados.");
        return;
    }
    QPrinter printer;
    QPainter painter;
    QString nomeArquivo = QDir::currentPath() + "/relatorio_cursos.pdf";
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(nomeArquivo);

    if (!painter.begin(&printer)) {
        QMessageBox::critical(instancia, "Arquivo não gerado", "Desculpe, não foi possível gerar o PDF!");
        return;
    }
    int linha = 80;
    const int COL_NOME = 25;
    const int COL_GRAU = 240;
    const int COL_PERIODO = 380;

    painter.drawText(320, linha, faculdade.nome.toUpper());
    linha+=50;
    painter.drawText(320, linha, "CURSOS CADASTRADOS NO SISTEMA");
    linha+=30;

    painter.drawText(COL_NOME, linha, "NOME");
    painter.drawText(COL_GRAU, linha, "GRAU DE ENSINO");
    painter.drawText(COL_PERIODO, linha, "PERÍODO");
    linha+=30;

    for (int i = 0; i < cursos.length(); i++) {
        painter.drawText(COL_NOME, linha, cursos.at(i).nome);
        painter.drawText(COL_GRAU, linha, cursos.at(i).grau);
        painter.drawText(COL_PERIODO, linha, cursos.at(i).periodo);
        linha+=20;
    }

    painter.end();
    QDesktopServices::openUrl(QUrl("file:///" + nomeArquivo));
}

void gerarPDF_disciplinasCadastradas(PortalAluno *instancia, QVector<Disciplina> disciplinas) {
    if(disciplinas.empty()) {
        QMessageBox::warning(instancia, "Arquivo não gerado", "Não tem disciplinas cadastradas.");
        return;
    }
    QPrinter printer;
    QPainter painter;
    QString nomeArquivo = QDir::currentPath() + "/relatorio_disciplinas.pdf";
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(nomeArquivo);

    if (!painter.begin(&printer)) {
        QMessageBox::critical(instancia, "Arquivo não gerado", "Desculpe, não foi possível gerar o PDF!");
        return;
    }
    int linha = 80;
    const int COL_NOME = 25;
    const int COL_PROF = 240;
    const int COL_CURSO = 400;
    int cursoID;

    painter.drawText(320, linha, faculdade.nome.toUpper());
    linha+=50;
    painter.drawText(320, linha, "DISCIPLINAS CADASTRADAS NO SISTEMA");
    linha+=30;

    painter.drawText(COL_NOME, linha, "NOME");
    painter.drawText(COL_PROF, linha, "PROFESSOR");
    painter.drawText(COL_CURSO, linha, "CURSO");
    linha+=30;

    for (int i = 0; i < disciplinas.length(); i++) {
        painter.drawText(COL_NOME, linha, disciplinas.at(i).nome);
        painter.drawText(COL_PROF, linha, disciplinas.at(i).professor);
        cursoID = disciplinas.at(i).pertenceAoCursoID;
        painter.drawText(COL_CURSO, linha, cursosCadastrados.at(cursoID).nome);
        linha+=20;
    }

    painter.end();
    QDesktopServices::openUrl(QUrl("file:///" + nomeArquivo));
}

void gerarPDF_alunosCadastrados(PortalAluno *instancia, QVector<Aluno> alunos) {
    if(alunos.empty()) {
        QMessageBox::warning(instancia, "Arquivo não gerado", "Não tem alunos cadastrados.");
        return;
    }
    QPrinter printer;
    QPainter painter;
    QString nomeArquivo = QDir::currentPath() + "/relatorio_alunos.pdf";
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(nomeArquivo);

    if (!painter.begin(&printer)) {
        QMessageBox::critical(instancia, "Arquivo não gerado", "Desculpe, não foi possível gerar o PDF!");
        return;
    }
    int linha = 80;
    const int COL_NOME = 25;
    const int COL_RA = 175;
    const int COL_IDADE = 315;
    const int COL_SEXO = 425;
    const int COL_CURSO = 520;

    painter.drawText(320, linha, faculdade.nome.toUpper());
    linha+=50;
    painter.drawText(320, linha, "ALUNOS CADASTRADOS NO SISTEMA");
    linha+=30;

    painter.drawText(COL_NOME, linha, "NOME");
    painter.drawText(COL_RA, linha, "RA");
    painter.drawText(COL_IDADE, linha, "IDADE");
    painter.drawText(COL_SEXO, linha, "SEXO");
    painter.drawText(COL_CURSO, linha, "CURSO");
    linha+=30;

    for (int i = 0; i < alunos.length(); i++) {
        painter.drawText(COL_NOME, linha, alunos.at(i).nome);
        painter.drawText(COL_RA, linha, QString::number(alunos.at(i).RA));
        painter.drawText(COL_IDADE, linha, QString::number(alunos.at(i).idade));
        painter.drawText(COL_SEXO, linha, alunos.at(i).sexo);
        painter.drawText(COL_CURSO, linha, alunos.at(i).curso.nome);
        linha+=20;
    }

    painter.end();
    QDesktopServices::openUrl(QUrl("file:///" + nomeArquivo));
}
