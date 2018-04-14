-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema projeto1
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema projeto1
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `projeto1-mc833` DEFAULT CHARACTER SET utf8 ;
USE `projeto1-mc833` ;

-- -----------------------------------------------------
-- Table `projeto1`.`DISCIPLINA`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `projeto1-mc833`.`DISCIPLINA` (
  `idDISCIPLINA` VARCHAR(5) NOT NULL,
  `TITULO` VARCHAR(250) NULL,
  `EMENTA` VARCHAR(1000) NULL,
  `SALA` INT NULL,
  `HORARIO` VARCHAR(45) NULL,
  `COMENTARIO` VARCHAR(1500) NULL,
  PRIMARY KEY (`idDISCIPLINA`))
ENGINE = InnoDB;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
