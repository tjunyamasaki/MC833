-- -----------------------------------------------------
-- CREATING DATABASE IN CASE IT DOESN'T EXIST
--------------------------------------------------------

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- Schema projeto1
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `projeto1` DEFAULT CHARACTER SET utf8 ;
USE `projeto1` ;

-- Table `projeto1`.`DISCIPLINA`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `projeto1`.`DISCIPLINAS` (
  `CODIGO-DISCIPLINA` VARCHAR(5) NOT NULL,
  `TITULO` VARCHAR(250) NULL,
  `EMENTA` VARCHAR(1000) NULL,
  `SALA` VARCHAR(5) NULL,
  `HORARIO` VARCHAR(45) NULL,
  `COMENTARIO` VARCHAR(1500) NULL,
  PRIMARY KEY (`CODIGO-DISCIPLINA`))
ENGINE = InnoDB;

-- Table `projeto1`.`USUARIO`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `projeto1`.`USUARIOS` (
  `idUSUARIO` INT NOT NULL,
  `LOGIN` VARCHAR(45) NOT NULL,
  `SENHA` VARCHAR(45) NOT NULL,
  `TIPO` VARCHAR(45) NULL,
  PRIMARY KEY (`idUSUARIO`))
ENGINE = InnoDB;

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;


-- ----------------------------------------------------
-- POPULATING DATABASE
-- ----------------------------------------------------

USE projeto1;

LOAD DATA INFILE '/Users/marcioivan/Dropbox/7ºSemestre/MC833/Projetos/projeto1/db/data/DISCIPLINAS.csv'
IGNORE INTO TABLE DISCIPLINAS
    CHARACTER SET utf8
    FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"'
    LINES TERMINATED BY '\r\n'
    IGNORE 1 LINES
    (CODIGO_DISCIPLINA,TITULO,EMENTA,SALA,HORARIO,COMENTARIO);

LOAD DATA INFILE '/Users/marcioivan/Dropbox/7ºSemestre/MC833/Projetos/projeto1/db/data/USUARIOS.csv' 
IGNORE INTO TABLE USUARIOS
    CHARACTER SET utf8
    FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"'
    LINES TERMINATED BY '\r\n'
    IGNORE 1 LINES
    (idUSUARIO,LOGIN,SENHA,TIPO);
