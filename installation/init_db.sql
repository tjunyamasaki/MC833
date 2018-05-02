-- -----------------------------------------------------
-- CREATING DATABASE IN CASE IT DOESN'T EXIST
-- ------------------------------------------------------

CREATE SCHEMA IF NOT EXISTS projeto1 DEFAULT CHARACTER SET utf8;

USE `projeto1`;

DROP TABLE IF EXISTS `projeto1`.`DISCIPLINAS`;
-- Creating table for Disciplinas
CREATE TABLE IF NOT EXISTS `projeto1`.`DISCIPLINAS` (
  `CODIGO_DISCIPLINA` VARCHAR(5) NOT NULL,
  `TITULO` VARCHAR(250) NULL,
  `EMENTA` VARCHAR(1000) NULL,
  `SALA` VARCHAR(5) NULL,
  `HORARIO` VARCHAR(45) NULL,
  `COMENTARIO` VARCHAR(1500) NULL,
  PRIMARY KEY (`CODIGO_DISCIPLINA`));

DROP TABLE IF EXISTS `projeto1`.`USUARIOS`;
-- Creating table for Usuarios
CREATE TABLE IF NOT EXISTS `projeto1`.`USUARIOS` (
  `USERNAME` VARCHAR(45) NOT NULL,
  `TIPO` VARCHAR(45) NULL,
  PRIMARY KEY (`USERNAME`));


-- Inserting root user into DB table
INSERT INTO `projeto1`.`USUARIOS`(USERNAME,TIPO) VALUES('root','admin');

-- Creating dummy professor for test
INSERT INTO `projeto1`.`USUARIOS`(USERNAME,TIPO) VALUES('professor','professor');
GRANT SELECT, UPDATE ON projeto1.* TO 'professor'@'localhost' IDENTIFIED BY 'senha123';
FLUSH PRIVILEGES;

-- Creating dummy student for test
INSERT INTO `projeto1`.`USUARIOS`(USERNAME,TIPO) VALUES('aluno','aluno');
GRANT SELECT ON projeto1.* TO 'aluno'@'localhost' IDENTIFIED BY 'senha123';
FLUSH PRIVILEGES;



-- ----------------------------------------------------
-- POPULATING DATABASE
-- ----------------------------------------------------

USE `projeto1`;

INSERT INTO `projeto1`.`DISCIPLINAS`(CODIGO_DISCIPLINA,TITULO,EMENTA,SALA,HORARIO,COMENTARIO)
VALUES
    ('MC102','Algoritmos e Programacao de Computadores','C Language','CB10','10:00','Aprendam recursao!'),
    ('MC202','Estruturas de Dados','Pilha', 'IC09','09:00','FIFO FILO!'),
    ('MC302','Programacao Orientada a Objetos','Java', 'CB09','08:00','Bora encapsular tudo!'),
    ('MC404','Linguagem de Montagem','ARM', 'CB09','08:00','Criar robo woly!'),
    ('CE738','Economia','Karl Marx', 'IE09','21:00','Trabalhadores da Unicamps, uni-vos!!!'),
    ('EA513','Circuitos Eletricos','Thevenin e Norton', 'CB09','08:00','U = R i !!!'),
    ('MC714','Sistema Distribuidos','Latencia', 'CB09','08:00','Qual a causa de todos os males? Latencia!!'),
    ('MC832','Redes','Network Layers', 'CB02','08:00',''),
    ('MC833','Laboratorio de Redes','TCP/UDP', 'CB02','08:00','10 pra todos no projeto!!'),
    ('MC536','Banco de Dados','MYSQL', 'CB10','10:00','Bora usar BD!!!');
