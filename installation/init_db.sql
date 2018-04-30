-- -----------------------------------------------------
-- CREATING DATABASE IN CASE IT DOESN'T EXIST
-- ------------------------------------------------------

CREATE SCHEMA IF NOT EXISTS projeto1 DEFAULT CHARACTER SET utf8;

USE `projeto1`;

-- Creating table for Disciplinas
CREATE TABLE IF NOT EXISTS `projeto1`.`DISCIPLINAS` (
  `CODIGO_DISCIPLINA` VARCHAR(5) NOT NULL,
  `TITULO` VARCHAR(250) NULL,
  `EMENTA` VARCHAR(1000) NULL,
  `SALA` VARCHAR(5) NULL,
  `HORARIO` VARCHAR(45) NULL,
  `COMENTARIO` VARCHAR(1500) NULL,
  PRIMARY KEY (`CODIGO_DISCIPLINA`));

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
    ('MC833','Laboratorio de Redes','TCP/UDP','IC302','10:00','Projeto pra quinta!'),
    ('HZ834','Humanas coxa','Marx', 'CB09','08:00','Trabalhadores da Unixamps, uni-vos!!!'),
    ('MC536','Banco de Dados','MYSQL', 'CB10','10:00','Bora usar BD!!!');
